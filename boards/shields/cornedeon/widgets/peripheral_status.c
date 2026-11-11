#include <zephyr/kernel.h>
#include <zmk/display.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/ble.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zmk/events/split_peripheral_status_changed.h>
#include <zmk/events/battery_state_changed.h>

#include "peripheral_status.h"

#define ZMK_LED_NUMLOCK_BIT BIT(0)
#define ZMK_LED_CAPSLOCK_BIT BIT(1)
#define ZMK_LED_SCROLLLOCK_BIT BIT(2)

// static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static struct zmk_widget_peripheral_status *_widget;

// Update peripheral state labels
static void update_labels(void) {
    if (_widget == NULL) {
        return;
    }
    const int BUF_SIZE = 16;
    char buf[BUF_SIZE];
    for (int i = 0; i < 2; i++) {
        if (_widget->is_connected[i]) {
            snprintf(buf, BUF_SIZE, "%d%%", _widget->battery_levels[i]);
        } else {
            snprintf(buf, BUF_SIZE, " X ");
        }
        lv_label_set_text(_widget->labels[i], buf);
    }
}

static int reserve_slot_for_conn(struct bt_conn *conn) {
    const bt_addr_le_t *addr = bt_conn_get_dst(conn);
    int idx = zmk_ble_put_peripheral_addr(addr);
    if (idx < 0 || idx >= 2) {
        return -1;
    }
    _widget->conns[idx] = conn;
    _widget->is_connected[idx] = true;
    return idx;
}

static int release_slot_for_conn(struct bt_conn *conn) {
    for (int i = 0; i < 2; i++) {
        if (_widget->conns[i] == conn) {
            _widget->conns[i] = NULL;
            _widget->is_connected[i] = false;
            return i;
        }
    }
    return -1;
}

static void split_central_connected(struct bt_conn *conn, uint8_t conn_err) {
    struct bt_conn_info info;
    bt_conn_get_info(conn, &info);
    // Look central event only
    if (info.role != BT_CONN_ROLE_CENTRAL) {
        return;
    }
    if (conn_err) {
        release_slot_for_conn(conn);
        return;
    }
    int slot_idx = reserve_slot_for_conn(conn);
    if (slot_idx < 0) {
        return;
    }
    update_labels();
}

static void split_central_disconnected(struct bt_conn *conn, uint8_t reason) {
    int slot_idx = release_slot_for_conn(conn);
    if (slot_idx >= 0) {
        update_labels();
    }
}

// Connection callbacks
static struct bt_conn_cb conn_callbacks = {
    .connected = split_central_connected,
    .disconnected = split_central_disconnected,
};

// Battery listener
static int battery_listener_cb(const zmk_event_t *eh) {
    if (eh == NULL || _widget == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }
    const struct zmk_peripheral_battery_state_changed *ev =
        as_zmk_peripheral_battery_state_changed(eh);
    if (ev != NULL) {
        if (ev->source == 0 || ev->source == 1) {
            _widget->battery_levels[ev->source] = ev->state_of_charge;
            update_labels();
        }
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(battery_listener, battery_listener_cb);
ZMK_SUBSCRIPTION(battery_listener, zmk_peripheral_battery_state_changed);

int zmk_widget_peripheral_status_init(struct zmk_widget_peripheral_status *widget,
                                      lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_width(widget->obj, lv_pct(100));
    lv_obj_set_height(widget->obj, 24);
    lv_obj_align(widget->obj, LV_ALIGN_TOP_MID, 0, 0);

    for (int i = 0; i < 2; i++) {
        widget->labels[i] = lv_label_create(widget->obj);
        lv_label_set_text(widget->labels[i], "---");
        if (i == 0) {
            lv_obj_align(widget->labels[i], LV_ALIGN_LEFT_MID, 2, 0);
        } else {
            lv_obj_align(widget->labels[i], LV_ALIGN_RIGHT_MID, -2, 0);
        }
    }

    bt_conn_cb_register(&conn_callbacks);

    _widget = widget;
    return 0;
}

lv_obj_t *zmk_widget_peripheral_status_obj(struct zmk_widget_peripheral_status *widget) {
    return widget->obj;
}

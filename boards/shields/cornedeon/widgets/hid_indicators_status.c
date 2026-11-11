
#include <zephyr/kernel.h>
#include <zmk/display.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/hid.h>
#include <zmk/hid_indicators.h>
#include <string.h>
#include <dt-bindings/zmk/modifiers.h>
#include "hid_indicators_status.h"

#define ZMK_LED_NUMLOCK_BIT BIT(0)
#define ZMK_LED_CAPSLOCK_BIT BIT(1)
#define ZMK_LED_SCROLLLOCK_BIT BIT(2)

// static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static struct zmk_widget_hid_indicators_status *_widget;

struct hid_indicators_status_state {
    zmk_hid_indicators_t indicators;
};

// Callback draw HID indicators (Num, Caps, Scroll)
static void hid_indicators_status_update_cb(struct hid_indicators_status_state state) {
    char buf[32] = "";
    bool has_active = false;
    if (state.indicators & ZMK_LED_NUMLOCK_BIT) {
        strcat(buf, "NUM ");
        has_active = true;
    }
    if (state.indicators & ZMK_LED_CAPSLOCK_BIT) {
        strcat(buf, "CAPS ");
        has_active = true;
    }
    if (state.indicators & ZMK_LED_SCROLLLOCK_BIT) {
        strcat(buf, "SCRL ");
        has_active = true;
    }

    if (has_active) {
        buf[strlen(buf) - 1] = 0;
        lv_label_set_text(_widget->label, buf);
    } else {
        lv_label_set_text(_widget->label, "");
    }
}

static struct hid_indicators_status_state hid_indicators_status_get_state(const zmk_event_t *eh) {
    return (struct hid_indicators_status_state){.indicators =
                                                    zmk_hid_indicators_get_current_profile()};
}

// Indicator listener
ZMK_DISPLAY_WIDGET_LISTENER(widget_hid_indicators_status, struct hid_indicators_status_state,
                            hid_indicators_status_update_cb, hid_indicators_status_get_state);
ZMK_SUBSCRIPTION(widget_hid_indicators_status, zmk_hid_indicators_changed);

int zmk_widget_hid_indicators_status_init(struct zmk_widget_hid_indicators_status *widget,
                                          lv_obj_t *parent) {
    widget->label = lv_label_create(parent);

    //  sys_slist_append(&widgets, &widget->node);
    _widget = widget;
    widget_hid_indicators_status_init();
    return 0;
}

lv_obj_t *zmk_widget_hid_indicators_status_obj(struct zmk_widget_hid_indicators_status *widget) {
    return widget->label;
}

#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>
#include <zmk/display.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/display/widgets/output_status.h>
#include "widgets/hid_indicators_status.h"
#include "widgets/peripheral_status.h"

// Patch zephyr/modules/lvgl/lvgl_display_mono.c
// for disable blanking on update some widgets
// void lvgl_flush_cb_mono(
// ...
// if (is_epd && !data->blanking_on && !is_last) {
// ...
// if (is_epd && is_last && data->blanking_on) {
// ...

// Standard ZMK widgets
#ifdef CONFIG_ZMK_WIDGET_BATTERY_STATUS
static struct zmk_widget_battery_status battery_widget;
#endif
static struct zmk_widget_layer_status layer_widget;
static struct zmk_widget_output_status output_widget;

// Custom widgets
static struct zmk_widget_hid_indicators_status indicators_status_widget;
static struct zmk_widget_peripheral_status peripheral_status_widget;

#define EDGE_OFFSET_X 2
#define EDGE_OFFSET_Y 2

lv_obj_t *zmk_display_status_screen(void) {
    // White screen
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    // Connection widget
    zmk_widget_output_status_init(&output_widget, screen);
    lv_obj_t *output_obj = zmk_widget_output_status_obj(&output_widget);
    lv_obj_align(output_obj, LV_ALIGN_TOP_LEFT, EDGE_OFFSET_X, EDGE_OFFSET_Y);
    lv_obj_set_style_text_font(output_obj, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_style_text_color(output_obj, lv_color_black(), LV_PART_MAIN);

    // Battery widget
#ifdef CONFIG_ZMK_WIDGET_BATTERY_STATUS
    zmk_widget_battery_status_init(&battery_widget, screen);
    lv_obj_t *batt_obj = zmk_widget_battery_status_obj(&battery_widget);
    lv_obj_align(batt_obj, LV_ALIGN_TOP_RIGHT, -EDGE_OFFSET_X, EDGE_OFFSET_Y);
    lv_obj_set_style_text_font(batt_obj, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_style_text_color(batt_obj, lv_color_black(), LV_PART_MAIN);
#endif

    // Layer widget
    zmk_widget_layer_status_init(&layer_widget, screen);
    lv_obj_t *layer_obj = zmk_widget_layer_status_obj(&layer_widget);
    lv_obj_align(layer_obj, LV_ALIGN_CENTER, 0, -28);
    lv_obj_set_style_text_font(layer_obj, &lv_font_montserrat_26, LV_PART_MAIN);
    lv_obj_set_style_text_color(layer_obj, lv_color_black(), LV_PART_MAIN);

    // HID widget - NUM, CAPS, SCROLL
    zmk_widget_hid_indicators_status_init(&indicators_status_widget, screen);
    lv_obj_t *indicators_obj = zmk_widget_hid_indicators_status_obj(&indicators_status_widget);
    lv_obj_align(indicators_obj, LV_ALIGN_CENTER, 0, 12);
    lv_obj_set_style_text_font(indicators_obj, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(indicators_obj, lv_color_black(), LV_PART_MAIN);

    // Pripheral status widget
    zmk_widget_peripheral_status_init(&peripheral_status_widget, screen);
    lv_obj_t *peripheral_obj = zmk_widget_peripheral_status_obj(&peripheral_status_widget);
    lv_obj_align(peripheral_obj, LV_IMAGE_ALIGN_BOTTOM_MID, 0, -EDGE_OFFSET_Y);
    lv_obj_set_style_text_font(peripheral_obj, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_style_text_color(peripheral_obj, lv_color_black(), LV_PART_MAIN);

    return screen;
}
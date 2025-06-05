#include "wpm_ind.h"

#include <ctype.h>
#include <zmk/display.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <fonts.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static lv_meter_indicator_t * indic;

struct wpm_ind_state 
{
    lv_meter_indicator_t * indic;
    uint8_t wpm;
};

static void wpm_ind_set_sel(lv_obj_t *ind, struct wpm_ind_state state) 
{
    lv_meter_set_indicator_value(ind, indic, state.wpm);
}

static void wpm_ind_update_cb(struct wpm_ind_state state) 
{ 
    struct zmk_widget_wpm_ind *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) 
    {
        wpm_ind_set_sel(widget->obj, state);
    }
}

static struct wpm_ind_state wpm_ind_get_state(const zmk_event_t *eh) {
    struct zmk_widget_wpm_ind *ev = as_zmk_wpm_state_changed(eh);
    LOG_INF("wpm is  %d", ev->state);
    return (struct wpm_ind_state) 
    {
        .wpm = ev->state
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_wpm_ind, struct wpm_ind_state, wpm_ind_update_cb, wpm_ind_get_state)
ZMK_SUBSCRIPTION(widget_wpm_ind, zmk_wpm_state_changed);

int zmk_widget_wpm_ind_init(struct zmk_widget_wpm_ind *widget, lv_obj_t *parent)
{
    widget->obj = lv_meter_create(parent);
    lv_obj_set_size(widget->obj, 125, 125);
    lv_meter_scale_t * scale = lv_meter_add_scale(widget->obj);
    
    lv_meter_set_scale_ticks(widget->obj, scale, 41, 1, 3, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(widget->obj, scale, 8, 3, 6, lv_color_black(), 10);
    lv_meter_set_scale_range(widget->obj, scale, 0, 200, 270, 0);

    /*Add a red arc to the end*/
    indic = lv_meter_add_arc(widget->obj, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(widget->obj, indic, 100);
    lv_meter_set_indicator_end_value(widget->obj, indic, 200);

    /*Make the tick lines red at the end of the scale*/
    indic = lv_meter_add_scale_lines(widget->obj, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false, 0);
    lv_meter_set_indicator_start_value(widget->obj, indic, 100);
    lv_meter_set_indicator_end_value(widget->obj, indic, 200);

    indic = lv_meter_add_needle_line(idget->obj, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

    sys_slist_append(&widgets, &widget->node);

    widget_wpm_ind_init();
    return 0;
}

lv_obj_t *zmk_widget_wpm_ind_obj(struct zmk_widget_wpm_ind *widget) {return widget->obj;}
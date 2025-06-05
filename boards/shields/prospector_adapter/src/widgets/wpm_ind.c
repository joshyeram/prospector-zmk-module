#include "wpm_ind.h"

#include <ctype.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/wpm.h>
#include <fonts.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static lv_meter_indicator_t * indic;

struct wpm_ind_state 
{
    uint8_t wpm;
};

static void wpm_ind_set_sel(lv_obj_t *ind, struct wpm_ind_state state) 
{
    lv_meter_set_indicator_value(ind, indic, state.wpm);
    if(state.wpm < 90)
    {
        lv_obj_set_style_line_color(ind, lv_palette_main(LV_PALETTE_GREY), LV_PART_INDICATOR);
    }
    else
    {
        lv_obj_set_style_line_color(ind, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
    }
}

static void wpm_ind_update_cb(struct wpm_ind_state state) 
{ 
    struct zmk_widget_wpm_ind *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) 
    {
        wpm_ind_set_sel(widget->obj, state);
    }
}

static struct wpm_ind_state wpm_ind_get_state(const zmk_event_t *eh) 
{
    struct zmk_wpm_state_changed *ev = as_zmk_wpm_state_changed(eh);
    LOG_INF("wpm is  %d", ev->state);
    return (struct wpm_ind_state){.wpm = ev->state};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_wpm_ind, struct wpm_ind_state, wpm_ind_update_cb, wpm_ind_get_state)
ZMK_SUBSCRIPTION(widget_wpm_ind, zmk_wpm_state_changed);

int zmk_widget_wpm_ind_init(struct zmk_widget_wpm_ind *widget, lv_obj_t *parent)
{
    widget->obj = lv_meter_create(parent);
    lv_obj_set_size(widget->obj, 175, 175);
    lv_obj_align(widget->obj, LV_ALIGN_CENTER, 0, -25);
    lv_meter_scale_t * scale = lv_meter_add_scale(widget->obj);

    lv_obj_set_style_text_color(widget->obj, lv_color_white(), LV_PART_TICKS);
    lv_obj_set_style_text_font(widget->obj, &lv_font_montserrat_12, LV_PART_TICKS);
    lv_meter_set_scale_ticks(widget->obj, scale, 16, 4, 8, lv_color_white());
    lv_meter_set_scale_major_ticks(widget->obj, scale, 2, 6, 12, lv_color_white(), 10);
    lv_meter_set_scale_range(widget->obj, scale, 0, 150, 240, 150);

    /*Add a white arc to the red*/
    indic = lv_meter_add_arc(widget->obj, scale, 3, lv_color_white(), 0);
    lv_meter_set_indicator_start_value(widget->obj, indic, 0);
    lv_meter_set_indicator_end_value(widget->obj, indic, 90);

    /*Add a red arc to the end*/
    indic = lv_meter_add_arc(widget->obj, scale, 5, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(widget->obj, indic, 90);
    lv_meter_set_indicator_end_value(widget->obj, indic, 150);

    /*Make the tick lines red at the end of the scale*/
    indic = lv_meter_add_scale_lines(widget->obj, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false, 0);
    lv_meter_set_indicator_start_value(widget->obj, indic, 90);
    lv_meter_set_indicator_end_value(widget->obj, indic, 150);

    indic = lv_meter_add_needle_line(widget->obj, scale, 4, lv_palette_main(LV_PALETTE_GREY), -15);

    sys_slist_append(&widgets, &widget->node);
    widget_wpm_ind_init();
    return 0;
}

lv_obj_t *zmk_widget_wpm_ind_obj(struct zmk_widget_wpm_ind *widget) {return widget->obj;}
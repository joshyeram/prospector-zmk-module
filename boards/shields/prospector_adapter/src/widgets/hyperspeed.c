#include "hyperspeed.h"

#include <ctype.h>
#include <zephyr/kernel.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/activity.h>
#include <zmk/events/activity_state_changed.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct hyperspeed_visual_state 
{
    enum zmk_activity_state act;
};

static void hyperspeed_toggle_sel(lv_obj_t *canvas, struct hyperspeed_visual_state state) 
{   
    if(state.act == ZMK_ACTIVITY_IDLE)
    {
        LOG_INF("hyperspeed remain hide idle");
        lv_obj_add_flag(canvas, LV_OBJ_FLAG_HIDDEN);
    }
    else if (state.act == ZMK_ACTIVITY_PAST_IDLE)
    {
        LOG_INF("hyperspeed unhide sleep");
        lv_obj_clear_flag(canvas, LV_OBJ_FLAG_HIDDEN);
    }   
    else if (state.act == ZMK_ACTIVITY_ACTIVE)
    {
        LOG_INF("hyperspeed hide active");
        lv_obj_add_flag(canvas, LV_OBJ_FLAG_HIDDEN);
    }
}

static void hyperspeed_vis_toggle_update_cb(struct hyperspeed_visual_state state) 
{ 
    struct zmk_widget_hyperspeed *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) 
    {
        hyperspeed_toggle_sel(widget->obj, state);
    }
}

static struct hyperspeed_visual_state hyperspeed_vis_toggle_get_state(const zmk_event_t *eh) 
{
    struct zmk_activity_state_changed *ev = as_zmk_activity_state_changed(eh);
    return (struct hyperspeed_visual_state){.act = ev->state};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_hyperspeed_vis_toggle, struct hyperspeed_visual_state, hyperspeed_vis_toggle_update_cb,    hyperspeed_vis_toggle_get_state)
ZMK_SUBSCRIPTION(widget_hyperspeed_vis_toggle, zmk_activity_state_changed);

int zmk_widget_hyperspeed_init(struct zmk_widget_hyperspeed *widget, lv_obj_t *parent)
{    
    widget->obj = lv_canvas_create(parent);
    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(240, 280)];

    lv_canvas_set_buffer(widget->obj, cbuf, 240, 280, LV_IMG_CF_INDEXED_1BIT);
    lv_canvas_set_palette(widget->obj, 0, lv_color_white());
    lv_canvas_set_palette(widget->obj, 1, lv_color_black());
    lv_obj_center(widget->obj);

    lv_color_t c0;
    lv_color_t c1;

    c0.full = 0;
    c1.full = 1;

    lv_canvas_fill_bg(widget->obj, c1, LV_OPA_COVER);

    uint32_t x;
    uint32_t y;
    for( y = 10; y < 30; y++) {
        for( x = 5; x < 20; x++) {
            lv_canvas_set_px(widget->obj, x, y, c0);
        }
    }

    lv_obj_add_flag(widget->obj, LV_OBJ_FLAG_HIDDEN);
    sys_slist_append(&widgets, &widget->node);
    widget_hyperspeed_vis_toggle_init();
    return 0;
}

lv_obj_t *zmk_widget_hyperspeed_obj(struct zmk_widget_hyperspeed *widget) {return widget->obj;}
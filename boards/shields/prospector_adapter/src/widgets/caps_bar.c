#include "caps_bar.h"

#include <ctype.h>
#include <zephyr/kernel.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/activity.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/hid_indicators_types.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static bool lastActive = false;

#define LED_NLCK 0x01
#define LED_CLCK 0x02
#define LED_SLCK 0x04

struct caps_bar_state 
{    
    zmk_hid_indicators_t ind;
};

// struct caps_bar_visual_state 
// {
//     enum zmk_activity_state act;
// };

static void caps_bar_set(lv_obj_t *bar, struct caps_bar_state state) 
{
    if (state.ind & LED_CLCK) 
    {
        LOG_INF("clck");
        lv_obj_clear_flag(bar, LV_OBJ_FLAG_HIDDEN);
        lastActive = true;
    }
    else
    {
        LOG_INF("no clck");
        lv_obj_add_flag(bar, LV_OBJ_FLAG_HIDDEN);
        lastActive = false;
    }
}

static void caps_bar_update_cb(struct caps_bar_state state) 
{ 
    LOG_INF("updating clck");
    struct zmk_widget_caps_bar *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) 
    {
        caps_bar_set(widget->obj, state);
    }
}

static struct caps_bar_state caps_bar_get_state(const zmk_event_t *eh) 
{
    struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);
    return (struct caps_bar_state) {.ind = ev->indicators};
}


// static void caps_bar_toggle_sel(lv_obj_t *meter, struct caps_bar_visual_state state) 
// {   
//     if(state.act == ZMK_ACTIVITY_IDLE || state.act == ZMK_ACTIVITY_SLEEP)
//     {
//         LOG_INF("hide from inact");
//         lv_obj_add_flag(meter, LV_OBJ_FLAG_HIDDEN);
//     }   
//     else if (state.act == ZMK_ACTIVITY_ACTIVE && lastActive)
//     {
//         LOG_INF("unhide from inact");
//         lv_obj_clear_flag(meter, LV_OBJ_FLAG_HIDDEN);
//     }
// }

// static void caps_bar_vis_toggle_update_cb(struct caps_bar_visual_state state) 
// { 
//     struct zmk_widget_caps_bar *widget;
//     SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) 
//     {
//         caps_bar_toggle_sel(widget->obj, state);
//     }
// }

// static struct caps_bar_visual_state caps_bar_vis_toggle_get_state(const zmk_event_t *eh) 
// {
//     struct zmk_activity_state_changed *ev = as_zmk_activity_state_changed(eh);
//     return (struct caps_bar_visual_state){.act = ev->state};
// }
            
ZMK_DISPLAY_WIDGET_LISTENER(widget_caps_bar,            struct caps_bar_state,        caps_bar_update_cb,               caps_bar_get_state)
ZMK_SUBSCRIPTION(widget_caps_bar,            zmk_hid_indicators_changed);

// ZMK_DISPLAY_WIDGET_LISTENER(widget_caps_bar_vis_toggle, struct caps_bar_visual_state, caps_bar_vis_toggle_update_cb,    caps_bar_vis_toggle_get_state)
// ZMK_SUBSCRIPTION(widget_caps_bar_vis_toggle, zmk_activity_state_changed);

int zmk_widget_caps_bar_init(struct zmk_widget_caps_bar *widget, lv_obj_t *parent)
{
    widget->obj = lv_bar_create(parent);
    lv_obj_set_style_bg_color(widget->obj, lv_color_hex(0x1E90FF), LV_PART_MAIN);
    lv_obj_set_size(widget->obj, 150, 20);
    lv_bar_set_value(widget->obj, 100, LV_ANIM_OFF);
    lv_obj_align(widget->obj, LV_ALIGN_CENTER, 0, 40);  

    sys_slist_append(&widgets, &widget->node);
    widget_caps_bar_init();
    //widget_caps_bar_vis_toggle_init();
    return 0;
}

lv_obj_t *zmk_widget_caps_bar_obj(struct zmk_widget_caps_bar *widget) {return widget->obj;}
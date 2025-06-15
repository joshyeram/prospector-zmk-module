#include "caps_bar.h"
#include <ctype.h>
#include <zmk/display.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/events/caps_word_state_changed.h>
#include <zmk/activity.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct caps_bar_state 
{
    bool active;
};

struct caps_bar_visual_state 
{
    enum caps_bar_state act;
};

static void caps_bar_set_sel(lv_obj_t *bar, struct caps_bar_state state) 
{
    if (state.active) 
    {
        lv_obj_add_flag(bar, LV_OBJ_FLAG_HIDDEN);
    } 
    else 
    {
        lv_obj_clear_flag(bar, LV_OBJ_FLAG_HIDDEN);
    }
}

static void caps_bar_update_cb(struct caps_bar_state state) 
{ 
    struct zmk_widget_caps_bar *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) 
    {
        caps_bar_set_sel(widget->obj, state);
    }
}

static struct caps_bar_state lcaps_bar_get_state(const zmk_event_t *eh) {
    const struct zmk_caps_word_state_changed *ev = as_zmk_caps_word_state_changed(eh);
    LOG_INF("DISP | Caps Word State Changed: %d", ev->active);
    return (struct caps_bar_state){.active = ev->active};
}


static void caps_bar_toggle_sel(lv_obj_t *bar, struct caps_bar_visual_state state) 
{   
    if(state.act == ZMK_ACTIVITY_IDLE || state.act == ZMK_ACTIVITY_SLEEP)
    {
        lv_obj_add_flag(bar, LV_OBJ_FLAG_HIDDEN);
    }   
    else if (state.act == ZMK_ACTIVITY_ACTIVE)
    {
        lv_obj_clear_flag(bar, LV_OBJ_FLAG_HIDDEN);
    }
}

static void caps_bar_vis_toggle_update_cb(struct lcaps_bar_visual_state state) 
{ 
    struct zmk_widget_caps_bar *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) 
    {
        caps_bar_toggle_sel(widget->obj, state);
    }
}

static struct caps_bar_visual_state caps_bar_vis_toggle_get_state(const zmk_event_t *eh) 
{
    struct zmk_activity_state_changed *ev = as_zmk_activity_state_changed(eh);
    return (struct caps_bar_visual_state){.act = ev->state};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_caps_bar, struct caps_bar_state, caps_bar_update_cb, caps_bar_get_state)
ZMK_SUBSCRIPTION(widget_caps_bar, zmk_layer_state_changed);

ZMK_DISPLAY_WIDGET_LISTENER(widget_caps_bar_vis_toggle, struct caps_bar_visual_state, caps_bar_vis_toggle_update_cb, caps_bar_vis_toggle_get_state)
ZMK_SUBSCRIPTION(widget_caps_bar_vis_toggle, zmk_activity_state_changed);

int zmk_widget_caps_bar_init(struct zmk_widget_caps_bar *widget, lv_obj_t *parent) 
{
    widget->obj = lv_bar_create(parent);
    lv_obj_set_size(widget->obj, 150, 7);
    lv_obj_align(widget->obj, LV_ALIGN_CENTER, 0, 50);  

    struct caps_bar_state temp;
    temp.active = false;
    caps_bar_set_sel(widget->obj, temp);
    
    sys_slist_append(&widgets, &widget->node);
    widget_caps_bar_init();
    widget_caps_bar_vis_toggle_init();
    return 0;
}

lv_obj_t *zmk_widget_caps_bar_obj(struct zmk_widget_caps_bar *widget) {return widget->obj;}
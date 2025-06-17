#include "layer_prndl.h"

#include <ctype.h>
#include <zmk/display.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/activity.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <fonts.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define DEFAULT 0
#define MEDIA 1
#define SYMBOL 6

#define NUMBER 2
#define ARROW 3
#define MOUSE 4
#define FUNC 5


static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct layer_prndl_state 
{
    uint8_t index;
};

struct layer_prndl_visual_state 
{
    enum zmk_activity_state act;
};

static void layer_prndl_set_sel(lv_obj_t *prndl, struct layer_prndl_state state) 
{
    if (state.index == DEFAULT)
    {
        lv_label_set_text(prndl, "#FF5C00 B # #808080 M # #808080 S # #808080 N # #808080 A # #808080 M+ # #808080 F#");
    }
    else if (state.index == MEDIA)
    {
        lv_label_set_text(prndl, "#808080 B # #FF5C00 M # #808080 S # #808080 N # #808080 A # #808080 M+ # #808080 F#");
    }
    else if (state.index == SYMBOL)
    {
        lv_label_set_text(prndl, "#808080 B # #808080 M # #FF5C00 S # #808080 N # #808080 A # #808080 M+ # #808080 F#");
    }
    else if (state.index == NUMBER)
    {
        lv_label_set_text(prndl, "#808080 B # #808080 M # #808080 S # #FF5C00 N # #808080 A # #808080 M+ # #808080 F#");
    }
    else if (state.index == ARROW)
    {
        lv_label_set_text(prndl, "#808080 B # #808080 M # #808080 S # #808080 N # #FF5C00 A # #808080 M+ # #808080 F#");
    }
    else if (state.index == MOUSE)
    {
        lv_label_set_text(prndl, "#808080 B # #808080 M # #808080 S # #808080 N # #808080 A # #FF5C00 M+ # #808080 F#");
    }
    else if (state.index == FUNC)
    {
        lv_label_set_text(prndl, "#808080 B # #808080 M # #808080 S # #808080 N # #808080 A # #808080 M+ # #FF5C00 F#");
    }
    else
    {
        lv_label_set_text(prndl, "#808080 B # #808080 M # #808080 S # #808080 N # #808080 A # #808080 M+ # #808080 F#");
    }
}

static void layer_prndl_update_cb(struct layer_prndl_state state) 
{ 
    struct zmk_widget_layer_prndl *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) 
    {
        layer_prndl_set_sel(widget->obj, state);
    }
}

static struct layer_prndl_state layer_prndl_get_state(const zmk_event_t *eh) {
    uint8_t index = zmk_keymap_highest_layer_active();
    LOG_INF("prndl set to: %d", index);
    return (struct layer_prndl_state)
    {
        .index = index,
    };
}



static void layer_prndl_toggle_sel(lv_obj_t *meter, struct layer_prndl_visual_state state) 
{   
    if(state.act == ZMK_ACTIVITY_IDLE || state.act == ZMK_ACTIVITY_PAST_IDLE)
    {
        lv_obj_add_flag(meter, LV_OBJ_FLAG_HIDDEN);
    }   
    else if (state.act == ZMK_ACTIVITY_ACTIVE)
    {
        lv_obj_clear_flag(meter, LV_OBJ_FLAG_HIDDEN);
    }
}

static void layer_prndl_vis_toggle_update_cb(struct layer_prndl_visual_state state) 
{ 
    struct zmk_widget_layer_prndl *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) 
    {
        layer_prndl_toggle_sel(widget->obj, state);
    }
}

static struct layer_prndl_visual_state layer_prndl_vis_toggle_get_state(const zmk_event_t *eh) 
{
    struct zmk_activity_state_changed *ev = as_zmk_activity_state_changed(eh);
    return (struct layer_prndl_visual_state){.act = ev->state};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_prndl, struct layer_prndl_state, layer_prndl_update_cb, layer_prndl_get_state)
ZMK_SUBSCRIPTION(widget_layer_prndl, zmk_layer_state_changed);

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_prndl_vis_toggle, struct layer_prndl_visual_state, layer_prndl_vis_toggle_update_cb, layer_prndl_vis_toggle_get_state)
ZMK_SUBSCRIPTION(widget_layer_prndl_vis_toggle, zmk_activity_state_changed);

int zmk_widget_layer_prndl_init(struct zmk_widget_layer_prndl *widget, lv_obj_t *parent) 
{
    widget->obj = lv_label_create(parent);
    lv_label_set_long_mode(widget->obj, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(widget->obj, &lv_font_montserrat_20, 0);
    lv_label_set_recolor(widget->obj, true);
    lv_obj_set_width(widget->obj, 270);
    lv_obj_set_style_text_align(widget->obj, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(widget->obj, LV_ALIGN_CENTER, 0, 60);    
    struct layer_prndl_state temp;
    temp.index = 0;
    layer_prndl_set_sel(widget->obj, temp);
    sys_slist_append(&widgets, &widget->node);
    widget_layer_prndl_init();
    widget_layer_prndl_vis_toggle_init();
    return 0;
}

lv_obj_t *zmk_widget_layer_prndl_obj(struct zmk_widget_layer_prndl *widget) {return widget->obj;}
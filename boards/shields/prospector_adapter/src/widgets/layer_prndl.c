#include "layer_prndl.h"

#include <ctype.h>
#include <zmk/display.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <fonts.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

const char * layer_map[] = {"base", "num", "sym",  "arr", "\n", "media", "mouse", "funct", NULL};
static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct layer_prndl_state 
{
    uint8_t index;
};

static void layer_prndl_set_sel(lv_obj_t *prndl, struct layer_prndl_state state) 
{
    lv_label_set_text(prndl, "#FF5C00 B# #808080 N# #808080 S# #FF5C00 A#  #808080 M# #FF5C00 M+# #808080 F");
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
    return (struct layer_prndl_state){
        .index = index,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_prndl, struct layer_prndl_state, layer_prndl_update_cb, layer_prndl_get_state)
ZMK_SUBSCRIPTION(widget_layer_prndl, zmk_layer_state_changed);

int zmk_widget_layer_prndl_init(struct zmk_widget_layer_prndl *widget, lv_obj_t *parent) 
{
    widget->obj = lv_label_create(parent);
    lv_label_set_long_mode(widget->obj, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(widget->obj, &lv_font_unscii_8);
    lv_label_set_recolor(widget->obj, true);
    lv_obj_set_width(widget->obj, 200);
    lv_obj_set_style_text_align(widget->obj, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(widget->obj, LV_ALIGN_CENTER, 0, -50);
    lv_label_set_text(widget->obj, "#FF5C00 B# #808080 N# #808080 S# #808080 A#  #808080 M# #808080 M+# #808080 F");

    //lv_obj_add_event_cb(widget->obj, mask_event_cb, LV_EVENT_ALL, NULL);
    sys_slist_append(&widgets, &widget->node);

    widget_layer_prndl_init();
    return 0;
}

lv_obj_t *zmk_widget_layer_prndl_obj(struct zmk_widget_layer_prndl *widget) {return widget->obj;}
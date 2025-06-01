#include "layer_matrix.h"

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

struct layer_matrix_state 
{
    uint8_t index;
};

static void layer_matrix_set_sel(lv_obj_t *buttonMatrix, struct layer_matrix_state state) 
{
    lv_buttonmatrix_clear_button_ctrl_all(buttonMatrix, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_buttonmatrix_set_button_ctrl(buttonMatrix, state.index, LV_BUTTONMATRIX_CTRL_CHECKED);
}


static void layer_matrix_update_cb(struct layer_matrix_state state) 
{ 
    struct zmk_widget_layer_matrix *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) 
    {
        layer_matrix_set_sel(widget->obj, state);
    }
}

static struct layer_matrix_state layer_matrix_get_state(const zmk_event_t *eh) {
    uint8_t index = zmk_keymap_highest_layer_active();
    LOG_INF("matrix set to: %d", index);
    return (struct layer_matrix_state){
        .index = index,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_matrix, struct layer_matrix_state, layer_matrix_update_cb, layer_matrix_get_state)
ZMK_SUBSCRIPTION(widget_layer_matrix, zmk_layer_state_changed);

int zmk_widget_layer_matrix_init(struct zmk_widget_layer_matrix *widget, lv_obj_t *parent) 
{
    widget->obj = lv_buttonmatrix_create(parent);
    lv_buttonmatrix_set_map(widget->obj, layer_map);
    lv_buttonmatrix_set_button_ctrl(widget->obj, 0, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(widget->obj, 1, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(widget->obj, 2, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(widget->obj, 3, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(widget->obj, 4, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(widget->obj, 5, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(widget->obj, 6, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_one_checked(widget->obj, true);
    lv_buttonmatrix_set_button_ctrl(widget->obj, 0, LV_BUTTONMATRIX_CTRL_CHECKED);

    lv_obj_set_style_text_font(widget->obj, &LV_FONT_UNSCII_8, LV_PART_MAIN);

    lv_obj_align(widget->obj, LV_ALIGN_CENTER, 0, 0);

    //lv_obj_add_event_cb(widget->obj, mask_event_cb, LV_EVENT_ALL, NULL);
    sys_slist_append(&widgets, &widget->node);
    return 0;
}

lv_obj_t *zmk_widget_layer_matrix_obj(struct zmk_widget_layer_matrix *widget) {return widget->obj;}
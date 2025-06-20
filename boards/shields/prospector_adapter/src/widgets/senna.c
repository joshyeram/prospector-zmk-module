#include "senna.h"

#include <ctype.h>
#include <zephyr/kernel.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/activity.h>
#include <zmk/events/activity_state_changed.h>
#include <zephyr/logging/log.h>

#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/led.h>
#include <zephyr/device.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static const struct device *pwm_leds_dev = DEVICE_DT_GET_ONE(pwm_leds);
#define DISP_BL DT_NODE_CHILD_IDX(DT_NODELABEL(disp_bl))

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
LV_IMG_DECLARE(frame_001);
struct senna_visual_state 
{
    enum zmk_activity_state act;
};

static void senna_toggle_sel(lv_obj_t *image, struct senna_visual_state state) 
{   
    if(state.act == ZMK_ACTIVITY_IDLE)
    {
        led_set_brightness(pwm_leds_dev, DISP_BL, 150);
        LOG_INF("senna unhide idle");
        lv_obj_clear_flag(image, LV_OBJ_FLAG_HIDDEN);
    }
    else if (state.act == ZMK_ACTIVITY_PAST_IDLE)
    {
        led_set_brightness(pwm_leds_dev, DISP_BL, 50);
        LOG_INF("senna hide sleep");
        //lv_obj_add_flag(image, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(image, LV_OBJ_FLAG_HIDDEN);
    }   
    else if (state.act == ZMK_ACTIVITY_ACTIVE)
    {
        led_set_brightness(pwm_leds_dev, DISP_BL, 200);
        LOG_INF("senna hide act");
        lv_obj_add_flag(image, LV_OBJ_FLAG_HIDDEN);
    }
}

static void senna_vis_toggle_update_cb(struct senna_visual_state state) 
{ 
    struct zmk_widget_senna *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) 
    {
        senna_toggle_sel(widget->obj, state);
    }
}

static struct senna_visual_state senna_vis_toggle_get_state(const zmk_event_t *eh) 
{
    struct zmk_activity_state_changed *ev = as_zmk_activity_state_changed(eh);
    return (struct senna_visual_state){.act = ev->state};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_senna_vis_toggle, struct senna_visual_state, senna_vis_toggle_update_cb,    senna_vis_toggle_get_state)
ZMK_SUBSCRIPTION(widget_senna_vis_toggle, zmk_activity_state_changed);

int zmk_widget_senna_init(struct zmk_widget_senna *widget, lv_obj_t *parent)
{
    widget->obj = lv_img_create(parent);
    lv_obj_center(widget->obj);
    //lv_img_set_src(widget->obj, "senna_pic/frame_001565a565saend.bin");
    lv_img_set_src(widget->obj, &frame_001);
    lv_img_set_zoom(widget->obj, 512);
    
    lv_obj_clear_flag(widget->obj, LV_OBJ_FLAG_HIDDEN);
    sys_slist_append(&widgets, &widget->node);
    widget_senna_vis_toggle_init();
    return 0;
}

lv_obj_t *zmk_widget_senna_obj(struct zmk_widget_senna *widget) {return widget->obj;}
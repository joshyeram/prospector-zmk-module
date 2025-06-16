#include "senna.h"

#include <ctype.h>
#include <zephyr/kernel.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/activity.h>
#include <zmk/events/activity_state_changed.h>
#include <zephyr/logging/log.h>

#define FRAME_COUNT 15
#define VIDEO_FPS   6

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static lv_obj_t *video_img;
static lv_timer_t *video_timer;
static lv_obj_t *screen;
static uint8_t frame_idx;

// struct uint8_t_state 
// {    
//     uint8_t ind;
// };

struct senna_visual_state 
{
    enum zmk_activity_state act;
};

static void next_frame_cb(lv_timer_t *t)
{
    char path[64];
    snprintf(path, sizeof(path), "senna_pic/frame_%03d.bin", frame_idx);  // Adjust FS prefix!
    lv_img_set_src(video_img, path);
    frame_idx = (frame_idx + 1) % FRAME_COUNT;
}

void video_start(void)
{
    if (video_timer) return;

    video_img = lv_img_create(lv_scr_act());
    lv_obj_center(video_img);
    lv_obj_clear_flag(video_img, LV_OBJ_FLAG_HIDDEN);

    frame_idx = 0;
    lv_img_set_src(video_img, "senna_pic/frame_001.bin");

    video_timer = lv_timer_create(next_frame_cb, 1000 / VIDEO_FPS, NULL);
}

void video_stop_and_hide(void)
{
    if (!video_timer) return;
    lv_timer_del(video_timer);
    video_timer = NULL;
    lv_obj_add_flag(video_img, LV_OBJ_FLAG_HIDDEN);
}

static void senna_toggle_sel(lv_obj_t *meter, struct senna_visual_state state) 
{   
    if(state.act == ZMK_ACTIVITY_IDLE || state.act == ZMK_ACTIVITY_SLEEP)
    {
        sleep = true;
        LOG_INF("hide from inact");
        video_start(void);
    }   
    else if (state.act == ZMK_ACTIVITY_ACTIVE && lastActive)
    {
        sleep = false;
        LOG_INF("unhide from inact");
        video_stop_and_hide();
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

int zmk_widget_senna_init(lv_obj_t *parent)
{
    screen = parent;
    sys_slist_append(&widgets, &widget->node);
    widget_senna_init();
    widget_senna_vis_toggle_init();
    return 0;
}
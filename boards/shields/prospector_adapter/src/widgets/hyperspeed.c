#include "hyperspeed.h"
#include <math.h>
#include <ctype.h>
#include <zephyr/kernel.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/activity.h>
#include <zmk/events/activity_state_changed.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static lv_color_t white;
static lv_color_t black;
static bool draw = false;

static uint16_t degree[10] = {0, 36, 72, 108, 144, 180, 216, 252, 288, 324};
struct hyperspeed_visual_state 
{
    enum zmk_activity_state act;
};

static void hyperspeed_toggle_sel(lv_obj_t *canvas, struct hyperspeed_visual_state state) 
{   
    if(state.act == ZMK_ACTIVITY_IDLE)
    {
        //LOG_INF("hyperspeed remain hide idle");
        lv_obj_add_flag(canvas, LV_OBJ_FLAG_HIDDEN);
    }
    else if (state.act == ZMK_ACTIVITY_PAST_IDLE)
    {
        //LOG_INF("hyperspeed unhide sleep");
        lv_obj_clear_flag(canvas, LV_OBJ_FLAG_HIDDEN);
        draw = true;
    }   
    else if (state.act == ZMK_ACTIVITY_ACTIVE)
    {
        //LOG_INF("hyperspeed hide active");
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

static void anim_hyperspeed(lv_obj_t *canvas, uint32_t count)
{
    if(!draw)
    {
        return;
    } 
    lv_canvas_fill_bg(canvas, black, LV_OPA_COVER);
    
    float far   = exp(.007 * count) + 5;
    float close = exp(.0052 * count) + 5;

    uint32_t farX =   cos(degree[0] * (3.1415926/180.0)) * far + 120;
    uint32_t farY =   sin(degree[0] * (3.1415926/180.0)) * far + 140;
    uint32_t closeX = cos(degree[0] * (3.1415926/180.0)) * close + 120;
    uint32_t closeY = sin(degree[0] * (3.1415926/180.0)) * close + 140;
    
    farX = (farX > 240) ? 240 : farX;
    farY = (farY > 280) ? 280 : farY; 
    closeX = (closeX > 240) ? 240 : closeX; 
    closeY = (closeY > 280) ? 280 : closeY; 

    farX = (farX < 0) ? 0 : farX;
    farY = (farY < 0) ? 0 : farY; 
    closeX = (closeX < 0) ? 0 : closeX; 
    closeY = (closeY < 0) ? 0 : closeY; 

    uint32_t dx = farX - closeX;
    uint32_t dy = farY - closeY;

    uint32_t adx = (abs(dx) + 1) << 1;
    uint32_t ady = (abs(dy) + 1) << 1;

    uint32_t sx = dx > 0 ? 1 : -1;
    uint32_t sy = dy > 0 ? 1 : -1;

    if(adx > ady)
    {
        uint32_t eps = (ady - adx) >> 1;
        for(uint32_t x = closeX, y = closeY; sx < 0 ? x >= farX: x <= farX; x += sx)
        {
            if (x > 240 || y > 280 || x < 0 || y < 0)
            {

            }
            else
            {
                lv_canvas_set_px(canvas, x, y, white);
            }
            eps += ady;
            if (eps << 1 >= adx)
            {
                y += sy;
                eps -= adx;
            }
        }
    }
    else
    {
        uint32_t eps = (adx - ady) >> 1;
        for(uint32_t x = closeX, y = closeY; sy < 0 ? y >= farY: y <= farY; y += sy)
        {
            if (x > 240 || y > 280 || x < 0 || y < 0)
            {

            }
            else
            {
                lv_canvas_set_px(canvas, x, y, white);
            }
            eps += adx;
            if (eps << 1 >= ady)
            {
                x += sx;
                eps -= ady;
            }
        }
    }
}

int zmk_widget_hyperspeed_init(struct zmk_widget_hyperspeed *widget, lv_obj_t *parent)
{    
    widget->obj = lv_canvas_create(parent);
    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(240, 280)];
    lv_canvas_set_buffer(widget->obj, cbuf, 240, 280, LV_IMG_CF_INDEXED_1BIT);
    lv_canvas_set_palette(widget->obj, 0, lv_color_white());
    lv_canvas_set_palette(widget->obj, 1, lv_color_black());
    lv_obj_center(widget->obj);

    white.full = 0;
    black.full = 1;

    lv_canvas_fill_bg(widget->obj, black, LV_OPA_COVER);
    lv_obj_add_flag(widget->obj, LV_OBJ_FLAG_HIDDEN);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, widget->obj);
    lv_anim_set_values(&a, 0, 1000);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_repeat_delay(&a, 500);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, anim_hyperspeed);
    lv_anim_start(&a);
    
    sys_slist_append(&widgets, &widget->node);
    widget_hyperspeed_vis_toggle_init();
    return 0;
}

lv_obj_t *zmk_widget_hyperspeed_obj(struct zmk_widget_hyperspeed *widget) {return widget->obj;}
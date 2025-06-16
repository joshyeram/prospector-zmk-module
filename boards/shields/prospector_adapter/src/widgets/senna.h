#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_senna {
    sys_snode_t node;
    lv_obj_t *obj;
};

int zmk_widget_senna_init(lv_obj_t *parent);
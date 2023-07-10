#pragma once

#include <stdint.h>

typedef struct{
    float vx;   // 横向,右边正方向, mm/s
    float vy;   // 纵向,前边正方向, mm/s
    float vw;   // 旋转,逆时针正方形, deg/s
} bsp_chasis_state;

void bsp_chasis_set(bsp_chasis_state *target);
void bsp_chasis_get(bsp_chasis_state *result);
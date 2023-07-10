#include "bsp_chasis.h"
#include "bsp_motor.h"
#include "bsp_task.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>

volatile uint32_t last_input = 0;
static bsp_chasis_state state;

static void stop_task(void *param){
    while(1){
        if(HAL_GetTick() - last_input >= 250){
            state.vx = state.vy = state.vw = 0;
            bsp_chasis_set(&state);
        }
        osDelay(100);
    }
}

#define line_spd 400
#define rot_spd 100

void report_task(void *param){
    while(1){
        printf("%f %f %f %f\n", bsp_motor1->current_speed, bsp_motor2->current_speed, bsp_motor3->current_speed, bsp_motor4->current_speed);
        osDelay(20);
    }
    
}

void app_idle_task(){
    bsp_task_create("rm_stop", stop_task, NULL, osPriorityAboveNormal6, 128);
    bsp_task_create("spd rpt", report_task, NULL, osPriorityAboveNormal1, 256);
    bsp_motor_set_ctrl_mode_all(BSP_MOTOR_CTRL_MODE_SPEED);
    while(1){
        char c = getchar();
        state.vx = state.vy = state.vw = 0;
        switch(c){
            case 'w':
                state.vy = line_spd;
                break;
            case 's':
                state.vy = -line_spd;
                break;
            case 'a':
                state.vx = -line_spd;
                break;
            case 'd':
                state.vx = line_spd;
                break;
            case 'q':
                state.vw = rot_spd;
                break;
            case 'e':
                state.vw = -rot_spd;
                break;
            default:
                break;
        }
        bsp_chasis_set(&state);
        last_input = HAL_GetTick();
    }
}
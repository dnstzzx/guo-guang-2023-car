#include "line.h"
#include "bsp_motor.h"
#include "bsp_scan.h"
#include <stdio.h>

static void print_sensor(){
    for(int i=0;i<BSP_SCANNER_COUNT;i++){
        printf("%d, ", bsp_scan_get(i));
    }
    printf("\n");
}

static void sensor_test(){
    while(1){
        print_sensor();
        osDelay(50);
    }
}

#define set_l(speed) bsp_motor_set_speed(bsp_motor1, speed);bsp_motor_set_speed(bsp_motor3, speed);
#define set_r(speed) bsp_motor_set_speed(bsp_motor2, speed);bsp_motor_set_speed(bsp_motor4, speed);
#define set_all(speed) set_l(speed);set_r(speed);

void app_idle_task(){
    //sensor_test();
    for(int i=0;i<4;i++){
        bsp_motor_set_speed(&bsp_motors[i], 0);
        bsp_motor_set_ctrl_mode(&bsp_motors[i], BSP_MOTOR_CTRL_MODE_SPEED);
    }
    while(1){
        char c;
        scanf("%c", &c);
        switch(c){
            case 'f':
                line_straight_distance(400);
                break;
            case 'b':
                set_all(-1);
                break;
            case 's':
                line_stop();
                osDelay(500);
                break;
            case 'r':
                line_straight_r(1);
                line_stop();
                break;
            case 'R':
                line_turn_right(1);
                line_stop();
                break;
        }
        
        osDelay(50);
    }
}
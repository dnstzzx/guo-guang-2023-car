#include "line.h"
#include "bsp_motor.h"
#include "bsp_scan.h"
#include "cmsis_os.h"
#include <stdio.h>


#define _BASE_SPEED (1.2f)
float base_speed = _BASE_SPEED;
#define BASE_SPEED base_speed
#define set_base_speed(s) base_speed=s;
#define reset_base_speed(s) base_speed=_BASE_SPEED;

#define _TURN_SPEED (4.0f)
float turn_speed = _TURN_SPEED;
#define TURN_SPEED turn_speed
#define set_turn_speed(s) turn_speed=(s);
#define reset_turn_speed() turn_speed=_TURN_SPEED;

#define set_l(speed) bsp_motor_set_speed(bsp_motor1, speed);bsp_motor_set_speed(bsp_motor3, speed);
#define set_r(speed) bsp_motor_set_speed(bsp_motor2, speed);bsp_motor_set_speed(bsp_motor4, speed);
#define set_all(speed) set_l(speed);set_r(speed);

bool sensors[BSP_SCANNER_COUNT] = {};
#define updt_sensors() bsp_scan_get_all(sensors);
#define sensor1 sensors[0]
#define sensor2 sensors[1]
#define sensor3 sensors[2]
#define sensor4 sensors[3]
#define sensor5 sensors[4]
#define sensor6 sensors[5]
#define sensor7 sensors[6]

#define wait_until(what) \ 
    do{ \
        osDelay(10); \
        updt_sensors(); \
    }while(!(what));

#define keep_in_line_until(what) \ 
    do{ \
        keep_in_line(); \
        osDelay(10); \
        updt_sensors(); \
    }while(!(what));

static void print_sensor(){
    for(int i=0;i<BSP_SCANNER_COUNT;i++){
        printf("%d, ", sensors[i]);
    }
    printf("\n");
}


void keep_in_line(){
    float spd_l = BASE_SPEED, spd_r =BASE_SPEED;
    float delta = BASE_SPEED * 0.3;
    if(!sensor4){
        if(sensor3 && !sensor1){    // 右偏
            spd_r += delta;
            spd_l -= delta;
            printf("left correct\n");
        }
        if(sensor5 && !sensor7){    // 左偏
            spd_r -= delta;
            spd_l += delta;
            printf("right correct\n");
        }
        if(sensor2 && !sensor1){    // 右偏
            spd_r += delta * 1;
            spd_l -= delta * 1;
            printf("left correct\n");
        }
        if(sensor6 && !sensor7){    // 左偏
            spd_r -= delta * 1;
            spd_l += delta * 1;
            printf("right correct\n");
        }
    }
    set_l(spd_l);
    set_r(spd_r);
}

void line_stop(){
    set_all(0);
}

void line_go(int mm){
    set_all(BASE_SPEED);
    int32_t stop = bsp_motor_get_pos_mm(bsp_motor1) + mm;
    while(bsp_motor_get_pos_mm(bsp_motor1) < stop){
        // if(bsp_motor_get_pos_mm(bsp_motor1) - stop <= 100){
        //     set_all(BASE_SPEED)
        // }
        osDelay(20);
    }
    line_stop();
    
}
void line_straight_l(uint8_t count){
    keep_in_line_until(sensor1);
    keep_in_line_until(!sensor1);
    line_stop();
    osDelay(500);
}

void line_straight_r(uint8_t count){
    keep_in_line_until(sensor7);
    //set_base_speed(1);
    keep_in_line_until(!sensor7);
    line_stop();
    osDelay(500);
    //int32_t stop = bsp_motor_get_pos_mm(bsp_motor1) + 150;
    //keep_in_line_until(bsp_motor_get_pos_mm(bsp_motor1) >= stop);
}


void line_straight_distance(uint32_t mm){
    int32_t stop = bsp_motor_get_pos_mm(bsp_motor1) + mm;
    while(bsp_motor_get_pos_mm(bsp_motor1) < stop){
        updt_sensors();
        print_sensor();
        keep_in_line();
        osDelay(20);
    }
}

void line_turn_left(uint8_t count){
    set_l(-TURN_SPEED);
    set_r(TURN_SPEED);
    wait_until(sensor1);
    print_sensor();
    wait_until(!sensor4);
    print_sensor();
    wait_until(sensor3);
    set_l(-TURN_SPEED*0.7);
    set_r(TURN_SPEED*0.7);
    wait_until(sensor4);
    set_l(-TURN_SPEED*0.4);
    set_r(TURN_SPEED*0.4);
    print_sensor();
    wait_until(!sensor3);
    line_stop();
    print_sensor();
}

void line_turn_right(uint8_t count){
    set_l(TURN_SPEED);
    set_r(-TURN_SPEED);
    wait_until(sensor7);
    print_sensor();
    wait_until(!sensor4);
    print_sensor();
    wait_until(sensor5);
    set_l(TURN_SPEED*0.7);
    set_r(-TURN_SPEED*0.7);
    wait_until(sensor4);
    set_l(TURN_SPEED*0.4);
    set_r(-TURN_SPEED*0.4);
    print_sensor();
    wait_until(!sensor5);
    line_stop();
    print_sensor();
}
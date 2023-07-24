#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "bsp_scan.h"
#include "bsp_chasis.h"
#include "line.h"
#include "line_macros.h"
#include "pid.h"
#include "bsp_led.h"

#define TICK_MS (1)

#define BASE_SPEED (400.0f)
#define TURN_SPEED (100.0f)

static dn_pid_param_t line_delta_param = {
    .kp = 15.0f,
    .ki = 0.0f,
    .kd = 0.0f,
    .err_limit = 10,
    .integral_limit = 10,
    .out_limit = 100.0f
};

static dn_pid_param_t line_bias_param = {
    .kp = 60.0f,
    .ki = 0.0f,
    .kd = 0.0f,
    .err_limit = 10,
    .integral_limit = 10,
    .out_limit = 200
};

static bool line_inverse_mode = 0;  // 反向模式
static bsp_scanner_group_t *front_group = bsp_scanner_group_front; // 向后巡线时交换逻辑前传感器
static bsp_scanner_group_t *back_group = bsp_scanner_group_back;
static dn_pid_t line_delta_pid, line_bias_pid;


static float delta, bias;   // pid输入：误差 delat为方向误差,bias为横向偏移误差

void keep_in_line_reset(bool inverse_mode){
    line_inverse_mode = inverse_mode;
    front_group = inverse_mode ? bsp_scanner_group_back : bsp_scanner_group_front;
    back_group = inverse_mode ? bsp_scanner_group_front : bsp_scanner_group_back;
    pid_init(&line_delta_pid, &line_delta_param);
    pid_init(&line_bias_pid, &line_bias_param);
}

void keep_in_line(float speed){
    float spd_rate = 1.0f;
    static float front_pos, back_pos;
    static float dx, dy, dw;    // pid输出：速度,dx为横向纠正,dw为方向纠正

    bool front_valid = check_valid_to_line(front_group);
    bool back_valid = check_valid_to_line(back_group);
    if(front_valid) front_pos = group_pos(front_group);
    if(back_valid) back_pos = group_pos(back_group);

    static uint32_t fail_count = 0;
    if(!front_valid && !back_valid){
        printf("line failed: all on side or white\n");
        fail_count ++;
        if(fail_count >= 10){
            bsp_chasis_set_speed(0, 0, 0);
            printf("ERR: OUT OF LINE\n");
            while(1) osDelay(1);
        }
        return;
    }else{
        fail_count = 0;
    }

    if(front_valid && back_valid){
        // 双巡线模式
        delta = (front_pos + back_pos) / 2;
        bias = (front_pos - back_pos) / 2;
    }else{
        // 单巡线模式
        bsp_scanner_group_t *single_sensor = front_valid ? front_group : back_group;
        float single_pos = front_valid ? front_pos : -1 * back_pos;
        delta = single_pos;
        bias = 0;
    }

    dx = pid_update(&line_bias_pid, 0, bias);
    dy = speed;
    dw = -1 * pid_update(&line_delta_pid, 0, delta);
    dx *= spd_rate;dy *= spd_rate;dw *= spd_rate;
    bsp_chasis_set_speed(dx, dy, dw);
    
    // static uint32_t t = 0;
    // uint32_t curr = HAL_GetTick();
    // if(t != 0)
    //     printf("time: %d\n",curr - t);
    // t = curr;
    printf("%f,%f\n", delta, bias);
    //printf("delta: %f,%f\n", delta, dw);
    
}

void line_stop(){
    bsp_chasis_set_speed(0, 0, 0);
}

void line_go(int mm){
    // set_all(BASE_SPEED);
    // int32_t stop = bsp_motor_get_pos_mm(bsp_motor1) + mm;
    // while(bsp_motor_get_pos_mm(bsp_motor1) < stop){
    //     // if(bsp_motor_get_pos_mm(bsp_motor1) - stop <= 100){
    //     //     set_all(BASE_SPEED)
    //     // }
    //     osDelay(20);
    // }
    // line_stop();
    
}

static inline void line_half(float spd){
    float s = spd / 2;
    float dis = 100.0;
    keep_in_line_in_ms(1000 * dis / s, s);
}

#define calc_spd() \
    ((bias == 0.0f && delta == 0.0f) ?  BASE_SPEED * 2 : BASE_SPEED)

#define line_straight_def(enter, exit)\
    keep_in_line_reset(false);\
    bsp_led_set(BSP_LED1, true);\
    for(int i=0;i<count-1;i++){\
        keep_in_line_until(enter, calc_spd());\
        keep_in_line_until(exit, calc_spd());\
    }\
    keep_in_line_until(enter, BASE_SPEED);\
    line_half(BASE_SPEED);\
    line_stop();\
    bsp_led_set(BSP_LED1, false);\

void line_straight_l(uint8_t count){
    line_straight_def(check_left(front_group), !check_left(front_group));
}

void line_straight_r(uint8_t count){
    line_straight_def(check_right(front_group), !check_right(front_group));
}


void line_straight_distance(uint32_t mm){
    // int32_t stop = bsp_motor_get_pos_mm(bsp_motor1) + mm;
    // while(bsp_motor_get_pos_mm(bsp_motor1) < stop){
    //     updt_sensors();
    //     print_sensor();
    //     keep_in_line();
    //     osDelay(20);
    // }
}

void line_turn_left(uint8_t count){
    bsp_chasis_set_speed(0,0,TURN_SPEED);
    bsp_scanner_data_t *datas = front_group->datas;
    for(int i=0;i<count-1;i++){
        wait_until(!datas[3].digital);
        wait_until(check_left(front_group));
        wait_until(!check_left(front_group));
        wait_until(datas[3].digital);
        wait_until(!datas[3].digital);
        wait_until(check_right(front_group));
        wait_until(!check_right(front_group));
    }
    wait_until(!datas[3].digital);
    wait_until(check_left(front_group));
    bsp_chasis_set_speed(0,0,TURN_SPEED / 2);
    wait_until(datas[3].digital);
    bsp_chasis_set_speed(0,0,TURN_SPEED / 4);
    wait_until(datas[4].digital)
    bsp_chasis_set_speed(0,0,0);
}

void line_turn_right(uint8_t count){
    bsp_chasis_set_speed(0,0,-TURN_SPEED);
    bsp_scanner_data_t *datas = front_group->datas;
    for(int i=0;i<count-1;i++){
        wait_until(!datas[4].digital);
        wait_until(check_right(front_group));
        wait_until(!check_right(front_group));
        wait_until(datas[4].digital);
        wait_until(!datas[4].digital);
        wait_until(check_left(front_group));
        wait_until(!check_left(front_group));
    }
    wait_until(!datas[4].digital);
    wait_until(check_right(front_group));
    bsp_chasis_set_speed(0,0,-TURN_SPEED / 2);
    wait_until(datas[4].digital);
    bsp_chasis_set_speed(0,0,-TURN_SPEED / 4);
    wait_until(datas[3].digital)
    bsp_chasis_set_speed(0,0,0);
}
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "bsp_scan.h"
#include "bsp_chasis.h"
#include "line.h"
#include "line_macros.h"
#include "pid.h"
#include "bsp_led.h"
#include "report.h"

#define TICK_MS (1)

#define BASE_SPEED (460.0f)
#define TURN_SPEED (200.0f)

static dn_pid_param_t line_delta_param = {
    .kp = 20.0f,
    .ki = 0.0f,
    .kd = 0.0f,
    .err_limit = 10,
    .integral_limit = 10,
    .out_limit = 100.0f
};

static dn_pid_param_t line_bias_param = {
    .kp = 50.0f,
    .ki = 0.0f,
    .kd = 0.0f,
    .err_limit = 10,
    .integral_limit = 10,
    .out_limit = 200
};

static bool line_inverse_mode = 0;  // 反向模式
static bsp_scanner_group_t *staight_front_group = bsp_scanner_group_front; // 向后巡线时交换逻辑前传感器
static bsp_scanner_group_t *staight_back_group = bsp_scanner_group_back;
static dn_pid_t line_delta_pid, line_bias_pid;


static float delta, bias;   // pid输入：误差 delat为方向误差,bias为横向偏移误差

void keep_in_line_reset(bool inverse_mode){
    line_inverse_mode = inverse_mode;
    staight_front_group = inverse_mode ? bsp_scanner_group_back : bsp_scanner_group_front;
    staight_back_group = inverse_mode ? bsp_scanner_group_front : bsp_scanner_group_back;
    pid_init(&line_delta_pid, &line_delta_param);
    pid_init(&line_bias_pid, &line_bias_param);
}

void keep_in_line(float speed){
    float spd_rate = 1.4f;
    static float front_pos, back_pos;
    static float dx, dy, dw;    // pid输出：速度,dx为横向纠正,dw为方向纠正

    bool front_valid = check_valid_to_line(staight_front_group);
    bool back_valid = check_valid_to_line(staight_back_group);
    if(front_valid) front_pos = group_pos(staight_front_group);
    if(back_valid) back_pos = group_pos(staight_back_group);

    static uint32_t fail_count = 0;
    if(!front_valid && !back_valid){
        //printf("line failed: all on side or white\n");
        fail_count ++;
        if(fail_count >= 100){
            logical_set_speed(0, 0, 0);
            report_send_wrapper("ERR", "OUT OF LINE");
            print_scanner_group("front", bsp_scanner_group_front);
            print_scanner_group("back", bsp_scanner_group_back);
            report_send_wrapper("LOCKED");
            osDelay(3000);
            __set_FAULTMASK(1);
           HAL_NVIC_SystemReset(); 
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
        bsp_scanner_group_t *single_sensor = front_valid ? staight_front_group : staight_back_group;
        float single_pos = front_valid ? front_pos : back_pos;
        delta = single_pos;
        bias = 0;
    }

    dx = pid_update(&line_bias_pid, 0, bias);
    dy = speed;
    dw = -1 * pid_update(&line_delta_pid, 0, delta);
    dx *= spd_rate;dy *= spd_rate;dw *= spd_rate;
    logical_set_speed(dx, dy, dw);
    
    // static uint32_t t = 0;
    // uint32_t curr = HAL_GetTick();
    // if(t != 0)
    //     printf("time: %d\n",curr - t);
    // t = curr;
    printf("%f,%f\n", delta, bias);
    //printf("delta: %f,%f\n", delta, dw);
    
}

void line_stop(){
    logical_set_speed(0, 0, 0);
}


static inline void line_half(float spd){
    //float s = spd * 0.7;
    //float dis = 90.0;
    float s = spd * 0.5;
    //float dis = 36.0;
    float dis = 48.2;
    keep_in_line_in_ms(1000 * dis / s, s);
}

void line_straight_l(uint8_t count){
    line_straight_def(check_left(staight_front_group), !check_left(staight_front_group), false);
}

void line_straight_r(uint8_t count){
    line_straight_def(check_right(staight_front_group), !check_right(staight_front_group), false);
}

void line_backward_straight_l(uint8_t count){
    line_straight_def(check_left(staight_front_group), !check_left(staight_front_group), true);
}

void line_backward_straight_r(uint8_t count){
    line_straight_def(check_right(staight_front_group), !check_right(staight_front_group), true);
}

void line_backward_straight_distance(uint32_t mm){
    
}

void line_straight_distance(uint32_t mm){
    mm *= 0.76;
    float spd = BASE_SPEED;
    float slow_down_point = 100 * 0.7, slow_down_spd = BASE_SPEED * 0.7;
    keep_in_line_reset(false);
    bsp_led_set(BSP_LED1, true);
    keep_in_line_in_ms((mm - slow_down_point)/spd * 1000, spd);
    keep_in_line_in_ms(slow_down_point / slow_down_spd * 1000, slow_down_spd);
    line_stop();
    bsp_led_set(BSP_LED1, false);
}




static bsp_scanner_data_t turn_scanner_datas[BSP_SCANNER_COUNT];
static void turn_scanner_data_updt(bsp_scanner_group_t *group, bool inverse){
    if(inverse){
        for(int i=0;i<BSP_SCANNER_COUNT;i++){
            turn_scanner_datas[BSP_SCANNER_COUNT - i - 1] = group->datas[i];
        }
    }else{
        memcpy(turn_scanner_datas, group->datas, sizeof(turn_scanner_datas));
    }
}



// 逻辑左转
#define turn_wait_until(what) \
    do{ \
        osDelay(TICK_MS); \
        bsp_scanner_group_update_all(); \
        turn_scanner_data_updt(group, inverse); \
    }while(!(what));


static void _line_turn(uint8_t count, float base_speed, bsp_scanner_group_t *group, bool inverse){
    logical_set_speed(0,0, base_speed);
    bsp_scanner_data_t *datas = turn_scanner_datas;
    for(int i=0;i<count-1;i++){
        turn_wait_until(!datas[3].digital);
        turn_wait_until(datas[0].digital);
        turn_wait_until(!datas[0].digital);
        turn_wait_until(datas[3].digital);
        turn_wait_until(!datas[3].digital);
        turn_wait_until(datas[7].digital);
        turn_wait_until(!datas[7].digital);
    }
    turn_wait_until(!datas[3].digital);
    turn_wait_until(datas[0].digital);
    logical_set_speed(0,0,base_speed * 0.8);
    turn_wait_until(datas[3].digital);
    logical_set_speed(0,0,base_speed * 0.5);
    turn_wait_until(datas[4].digital)
    osDelay(2);
    logical_set_speed(0,0,0);
}

void line_turn_left(uint8_t count){
    _line_turn(count, TURN_SPEED, bsp_scanner_group_front, false);
}

void line_turn_right(uint8_t count){
    _line_turn(count, -TURN_SPEED, bsp_scanner_group_front, true);
}

void line_backward_turn_left(uint8_t count){
    _line_turn(count, TURN_SPEED, bsp_scanner_group_back, false);
}

void line_backward_turn_right(uint8_t count){
    _line_turn(count, -TURN_SPEED, bsp_scanner_group_back, true);
}
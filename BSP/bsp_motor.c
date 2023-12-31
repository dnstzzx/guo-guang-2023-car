#include "bsp_motor.h"
#include "tim.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "bsp_task.h"
#include "basic_algs.h"

#define MOTOR1_PWM1_TTM     (&htim1)
#define MOTOR1_PWM1_CHANNEL (TIM_CHANNEL_1)
#define MOTOR1_PWM2_TTM     (&htim8)
#define MOTOR1_PWM2_CHANNEL (TIM_CHANNEL_1)
#define MOTOR2_PWM1_TTM     (&htim1)
#define MOTOR2_PWM1_CHANNEL (TIM_CHANNEL_2)
#define MOTOR2_PWM2_TTM     (&htim8)
#define MOTOR2_PWM2_CHANNEL (TIM_CHANNEL_2)
#define MOTOR3_PWM1_TTM     (&htim1)
#define MOTOR3_PWM1_CHANNEL (TIM_CHANNEL_3)
#define MOTOR3_PWM2_TTM     (&htim8)
#define MOTOR3_PWM2_CHANNEL (TIM_CHANNEL_3)
#define MOTOR4_PWM1_TTM     (&htim1)
#define MOTOR4_PWM1_CHANNEL (TIM_CHANNEL_4)
#define MOTOR4_PWM2_TTM     (&htim8)
#define MOTOR4_PWM2_CHANNEL (TIM_CHANNEL_4)

#define MOTOR1_ENCODER_TIM      (&htim2)
#define MOTOR2_ENCODER_TIM      (&htim3)
#define MOTOR3_ENCODER_TIM      (&htim4)
#define MOTOR4_ENCODER_TIM      (&htim5)

//TIMER PERIOD = ARR + 1
#define MOTOR_PWM_PERIOD        (1125)
#define MOTOR_ENCODER_PERIOD    (65536)

bsp_motor_t bsp_motors[BSP_MOTOR_COUNT] = {
    {
        .inverse = true,
        .swap_pwm = false,
        .target_speed = 0.0f,
        .target_pos = 0,
        .pwm1_channel = MOTOR1_PWM1_CHANNEL,
        .pwm2_channel = MOTOR1_PWM2_CHANNEL,
        .pwm1_tim = MOTOR1_PWM1_TTM,
        .pwm2_tim = MOTOR1_PWM2_TTM,
        .encoder_tim = MOTOR1_ENCODER_TIM,
        .encoder_state = {},
        .last_encoder_state = {},
        .current_speed = 0,
        .current_pos = 0,
        .ctrl_mode = BSP_MOTOR_CTRL_MODE_NONE,
        .speed_pid = {}
    },
    {
        .inverse = false,
        .swap_pwm = true,
        .target_speed = 0.0f,
        .target_pos = 0,
        .pwm1_channel = MOTOR2_PWM1_CHANNEL,
        .pwm2_channel = MOTOR2_PWM2_CHANNEL,
        .pwm1_tim = MOTOR2_PWM1_TTM,
        .pwm2_tim = MOTOR2_PWM2_TTM,
        .encoder_tim = MOTOR2_ENCODER_TIM,
        .encoder_state = {},
        .last_encoder_state = {},
        .current_speed = 0,
        .current_pos = 0,
        .ctrl_mode = BSP_MOTOR_CTRL_MODE_NONE,
        .speed_pid = {}
    },
    {
        .inverse = true,
        .swap_pwm = false,
        .target_speed = 0.0f,
        .target_pos = 0,
        .pwm1_channel = MOTOR3_PWM1_CHANNEL,
        .pwm2_channel = MOTOR3_PWM2_CHANNEL,
        .pwm1_tim = MOTOR3_PWM1_TTM,
        .pwm2_tim = MOTOR3_PWM2_TTM,
        .encoder_tim = MOTOR3_ENCODER_TIM,
        .encoder_state = {},
        .last_encoder_state = {},
        .current_speed = 0,
        .current_pos = 0,
        .ctrl_mode = BSP_MOTOR_CTRL_MODE_NONE,
        .speed_pid = {}
    },
    {
        .inverse = false,
        .swap_pwm = true,
        .target_speed = 0.0f,
        .target_pos = 0,
        .pwm1_channel = MOTOR4_PWM1_CHANNEL,
        .pwm2_channel = MOTOR4_PWM2_CHANNEL,
        .pwm1_tim = MOTOR4_PWM1_TTM,
        .pwm2_tim = MOTOR4_PWM2_TTM,
        .encoder_tim = MOTOR4_ENCODER_TIM,
        .encoder_state = {},
        .last_encoder_state = {},
        .current_speed = 0,
        .current_pos = 0,
        .ctrl_mode = BSP_MOTOR_CTRL_MODE_NONE,
        .speed_pid = {}
    },
};

// static const dn_pid_param_t default_speed_pid = {
//     .kp = 0.18f,
//     .ki = 0.008f,
//     .kd = 0.000f,
//     .err_limit = 10,
//     .integral_limit = 1,
//     .out_limit = 0.45
// };
static const dn_pid_param_t default_speed_pid = {
    .kp = 0.008f,
    .ki = 0.005f,
    .kd = 0.01f,
    .err_limit = 10,
    .integral_limit = 10,
    .out_limit = 0.95
};

static const dn_pid_param_t default_pos_pid = {
    .kp = 8.0f,
    .ki = 0.002f,
    .kd = 0.2f,
    .err_limit = 2,
    .integral_limit = 1,
    .out_limit = 5
};

#define _for_each_dev(opr) \
    for(int _i=0;_i<BSP_MOTOR_COUNT;_i++){ \
        bsp_motor_t *dev = &bsp_motors[_i]; opr; \
    }

inline static uint32_t bsp_motor_read_encoder(bsp_motor_t *dev){
    return __HAL_TIM_GET_COUNTER(dev->encoder_tim);
}

void bsp_motor_set_pwm(bsp_motor_t *motor, float pwm){
    if(motor->inverse)  pwm = -pwm;
    if(pwm >= 0){
        __HAL_TIM_SET_COMPARE(motor->pwm1_tim, motor->pwm1_channel, pwm * MOTOR_PWM_PERIOD);
        __HAL_TIM_SET_COMPARE(motor->pwm2_tim, motor->pwm2_channel, 0);
    }else{
        pwm = 0 - pwm;
        __HAL_TIM_SET_COMPARE(motor->pwm1_tim, motor->pwm1_channel, 0);
        __HAL_TIM_SET_COMPARE(motor->pwm2_tim, motor->pwm2_channel, pwm * MOTOR_PWM_PERIOD);
    }
}

#include "stdio.h"
/* returns: delta of counter */
static int32_t bsp_motor_update(bsp_motor_t *dev){
    __disable_irq();
    dev->encoder_state.encoder_count = bsp_motor_read_encoder(dev);
    int32_t delta = 
        MOTOR_ENCODER_PERIOD * (dev->encoder_state.encoder_ovf_count - dev->last_encoder_state.encoder_ovf_count) 
        + dev->encoder_state.encoder_count
        - dev->last_encoder_state.encoder_count;
    
    if(dev->inverse)    delta *= -1;

    dev->current_pos += delta;
    dev->current_speed = delta * (1.0f * BSP_MOTOR_TASK_HZ * 60 / BSP_MOTOR_RATE );
    
    memcpy(&dev->last_encoder_state, &dev->encoder_state, sizeof(bsp_motor_encoder_state_t));
    __enable_irq();
    return delta;
}

static inline void motor_ctrl(bsp_motor_t *motor){
    bsp_motor_update(motor);
    switch(motor->ctrl_mode){
        case BSP_MOTOR_CTRL_MODE_POS:
            motor->target_speed = pid_update(&motor->pos_pid, motor->target_pos, 1.0 * motor->current_pos / BSP_MOTOR_RATE);
            if(fabs(motor->target_speed) <= 0.05)  motor->target_speed = 0.0f;
        // no break here
        case BSP_MOTOR_CTRL_MODE_SPEED:
            do{
                float pwm = pid_update(&motor->speed_pid, motor->target_speed, motor->current_speed);
                if(fabs(pwm) <= 0.05) pwm = 0.0f;
                bsp_motor_set_pwm(motor, pwm);
            }while(0);
            break;
    }

}

void bsp_motor_task(void *param){
    // static uint32_t last_t = 0;
    // printf("time: %d\n", HAL_GetTick() - last_t);
    // last_t = HAL_GetTick();
    _for_each_dev(motor_ctrl(dev));
}

void bsp_motor_init(){
    const bool debug_pid = false;
    _for_each_dev(
        dev->target_speed = 0;
        dev->ctrl_mode = BSP_MOTOR_CTRL_MODE_NONE;
        dev->current_speed = 0;
        dev->current_pos = 0;

        if(dev->swap_pwm){
            swap(dev->pwm1_tim, dev->pwm2_tim);
            swap(dev->pwm1_channel, dev->pwm2_channel);
        }

        HAL_TIM_PWM_Start(dev->pwm1_tim, dev->pwm1_channel);
        HAL_TIM_PWM_Start(dev->pwm2_tim, dev->pwm2_channel);

        memset(&dev->encoder_state, 0, sizeof(bsp_motor_encoder_state_t));
        memset(&dev->last_encoder_state, 0, sizeof(bsp_motor_encoder_state_t));
        TIM_HandleTypeDef *encoder_tim = dev->encoder_tim;
        __HAL_TIM_SET_COUNTER(encoder_tim, 0);
        __HAL_TIM_CLEAR_IT(encoder_tim,TIM_IT_UPDATE);
        __HAL_TIM_ENABLE_IT(encoder_tim,TIM_IT_UPDATE);
        __HAL_TIM_URS_ENABLE(encoder_tim);
        HAL_TIM_Encoder_Start(dev->encoder_tim, TIM_CHANNEL_ALL);

        bsp_motor_set_pwm(dev, 0.0f);
        
        bsp_motor_config_speed_pid(dev, &default_speed_pid);
        bsp_motor_config_pos_pid(dev, &default_pos_pid);
    )
    if(debug_pid){
        static char *names[4] = {
            "m1", "m2", "m3", "m4"
        };
        _for_each_dev(
            dev->speed_pid.tracer_callback = pid_print;
            dev->speed_pid.tracer_cbk_param = names[_i];
        );
    }

    bsp_motor_t *dev;
    osTimerAttr_t attr = {
        .name = "motor task"
    };
    osTimerId_t timer_id = osTimerNew(bsp_motor_task, osTimerPeriodic, NULL, &attr);
    osTimerStart(timer_id, 1000 / BSP_MOTOR_TASK_HZ);
    
}

void HAL_TIM_PeriodElapsedCallback_motor(TIM_HandleTypeDef *htim){
    _for_each_dev(
        if(htim == dev->encoder_tim){
            if(__HAL_TIM_IS_TIM_COUNTING_DOWN(htim)){
                dev->encoder_state.encoder_ovf_count --;
            }else{
                dev->encoder_state.encoder_ovf_count ++;
            }
        }
    )
}



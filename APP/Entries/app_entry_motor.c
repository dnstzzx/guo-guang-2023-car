#include "bsp_motor.h"
#include "bsp_uart.h"
#include "bsp_task.h"
#include "cmsis_os.h"
#include "math.h"
void report_task(void *param){
    while(1){
        printf("%f %f %f %f\n", bsp_motor1->current_speed, bsp_motor2->current_speed, bsp_motor3->current_speed, bsp_motor4->current_speed);
        osDelay(20);
    }
    
}

static dn_pid_param_t speed_pid = {
    .kp = 0.008f,
    .ki = 0.005f,
    .kd = 0.01f,
    .err_limit = 10,
    .integral_limit = 10,
    .out_limit = 0.7
};

static dn_pid_param_t pos_pid = {
    .kp = 8.0f,
    .ki = 0.002f,
    .kd = 0.2f,
    .err_limit = 2,
    .integral_limit = 1,
    .out_limit = 5
};

static void print_pid(void *cbk_param, float target, float measurement, float output){
    printf("%f %f %f\n", measurement, target - measurement, output);
}

static void speed_pid_test(){
    bsp_motor_t *testing_motor = bsp_motor1;
    //bsp_motor_config_speed_pid(BSP_MOTOR_L, &speed_pid);
    //bsp_motor_set_ctrl_mode(BSP_MOTOR_L, BSP_MOTOR_CTRL_MODE_SPEED);
    rcfg:
    bsp_motor_config_speed_pid(testing_motor, &speed_pid);
    bsp_motor_set_ctrl_mode(testing_motor, BSP_MOTOR_CTRL_MODE_SPEED);
    //BSP_MOTOR_L->speed_pid.tracer_cbk_param = NULL;
    //BSP_MOTOR_L->speed_pid.tracer_callback = print_pid;
    testing_motor->speed_pid.tracer_cbk_param = NULL;
    testing_motor->speed_pid.tracer_callback = print_pid;

    while(1){
        //printf("input new pwm\n");
        float s1, s2;
        //scanf("%f %f", &s1, &s2);   
        char c;
        scanf("%c", &c);
        switch(c){
            case 'p':
                scanf("%f", &s1);
                speed_pid.kp = s1;
                bsp_motor_set_speed(testing_motor, 0.0f);
                goto rcfg;
            case 'i':
                scanf("%f", &s1);
                speed_pid.ki = s1;
                bsp_motor_set_speed(testing_motor, 0.0f);
                goto rcfg;
            case 'd':
                scanf("%f", &s1);
                speed_pid.kd = s1;
                bsp_motor_set_speed(testing_motor, 0.0f);
                goto rcfg;
            case 'P':
                printf("pid is: %f, %f, %f\n", speed_pid.kp, speed_pid.ki, speed_pid.kd);
                break;
            case 's':
                scanf("%f", &s2);
                //printf("set pwms to %f\n", pwm);
                //bsp_motor_set_speed(BSP_MOTOR_L, s1);
                bsp_motor_set_speed(testing_motor, s2);
                osDelay(50);
                break;
            case 'm':
                do{
                    int motor_id;
                    scanf("%d", &motor_id);
                    if(motor_id > 0 && motor_id <= BSP_MOTOR_COUNT){
                        bsp_motor_set_speed(testing_motor, 0);
                        bsp_motor_set_ctrl_mode(testing_motor, BSP_MOTOR_CTRL_MODE_NONE);
                        bsp_motor_set_pwm(testing_motor, 0);
                        testing_motor->speed_pid.tracer_cbk_param = NULL;
                        testing_motor->speed_pid.tracer_callback = NULL;
                        testing_motor = &bsp_motors[motor_id - 1];
                        goto rcfg;
                    }

                }while(0);
                break;
                
        }
        
    }
    count_up_forever();
}


static void pwm_test(){
    bsp_task_create("spd rpt", report_task, NULL, osPriorityAboveNormal1, 256);

    for(int i=0;i<4;i++){
        bsp_motor_set_ctrl_mode(&bsp_motors[i], BSP_MOTOR_CTRL_MODE_NONE);
    }
    while(1){
        printf("input new pwm\n");
        float s1, s2, s3, s4;
        //printf("input\n");
        scanf("%f %f %f %f", &s1, &s2, &s3, &s4);   
        printf("set pwms to %f %f %f %f\n", &s1, &s2, &s3, &s4);
        bsp_motor_set_pwm(bsp_motor1, s1);
        bsp_motor_set_pwm(bsp_motor2, s2);
        bsp_motor_set_pwm(bsp_motor3, s3);
        bsp_motor_set_pwm(bsp_motor4, s4);

        osDelay(50);
    }
    count_up_forever();
}

// static void pos_pid_test(){
//     bsp_motor_init();
//     bsp_motor_config_speed_pid(BSP_MOTOR_L, &speed_pid);
//     bsp_motor_config_pos_pid(BSP_MOTOR_L, &pos_pid);
//     bsp_motor_set_ctrl_mode(BSP_MOTOR_L, BSP_MOTOR_CTRL_MODE_POS);
//     bsp_motor_config_speed_pid(BSP_MOTOR_R, &speed_pid);
//     bsp_motor_config_pos_pid(BSP_MOTOR_R, &pos_pid);
//     bsp_motor_set_ctrl_mode(BSP_MOTOR_R, BSP_MOTOR_CTRL_MODE_POS);
//     BSP_MOTOR_L->pos_pid.tracer_cbk_param = NULL;
//     BSP_MOTOR_L->pos_pid.tracer_callback = print_pid;
//     BSP_MOTOR_R->pos_pid.tracer_cbk_param = NULL;
//     BSP_MOTOR_R->pos_pid.tracer_callback = print_pid;

//     while(1){
//         //printf("input new pwm\n");
//         float s1, s2;
//         static float last1 = 0.0f, last2 = 0.0f;
//         scanf("%f %f", &s1, &s2);
//         s1 += last1;
//         s2 += last2;
    
//         //printf("set pwms to %f\n", pwm);
//         bsp_motor_set_pos(BSP_MOTOR_L, s1);
//         bsp_motor_set_pos(BSP_MOTOR_R, s2);
//         last1 = s1;last2 = s2;
//         //bsp_motor_set_pwm(BSP_MOTOR_R, pwm);
//         osDelay(50);
//     }
//     count_up_forever();
// }

// #define LPOS ( BSP_MOTOR_L->current_pos)
// #define RPOS ( BSP_MOTOR_R->current_pos)
// #define POS_D (LPOS - RPOS)
// // 转90编码器理论变化值 待测
// #define TURN_90_ENCODER_VAL (1976)
// static move_test(){
//     bsp_motor_init();
    
//     bsp_task_create("spd rpt", report_task, NULL, osPriorityAboveNormal1, 256);
//     while(1){
//         char c;
//         printf("input new command\n");
//         scanf("%c", &c);
        
//         int32_t pos;
//         float spdl, spdr;
//         switch(c){
//             case 'L':
//             case 'l':
//                 pos = -2 * TURN_90_ENCODER_VAL;
//                 spdl = -1;
//                 spdr = 1;
//                 break;
//             case 'R':
//             case 'r':
//                 pos = 2 * TURN_90_ENCODER_VAL;
//                 spdl = 1;
//                 spdr = -1;
//                 break;
//             case 's':
//             case 'S':
//                 bsp_motor_set_speed(BSP_MOTOR_L, 0);
//                 bsp_motor_set_speed(BSP_MOTOR_R, 0);
//                 BSP_MOTOR_L->current_pos = 0;
//                 BSP_MOTOR_R->current_pos = 0;
//                 continue;
//                 break;
//             default:
//                 continue;
//         }
//         spdl *= 0.5;
//         spdr *= 0.5;
//         BSP_MOTOR_L->current_pos = 0;
//         BSP_MOTOR_R->current_pos = 0;
//         bsp_motor_set_pwm(BSP_MOTOR_L, spdl);
//         bsp_motor_set_pwm(BSP_MOTOR_R, spdr);
//         while(1){
//             if(abs(POS_D - pos) <= 2000){
//                 spdl = spdl > 0 ? 0.25 : -0.25;
//                 spdr = spdr > 0 ? 0.25 : -0.25;
//                 bsp_motor_set_pwm(BSP_MOTOR_L, spdl);
//                 bsp_motor_set_pwm(BSP_MOTOR_R, spdr);
//             }
//             if(abs(POS_D) >= abs(pos)){
//                 bsp_motor_set_pwm(BSP_MOTOR_L, 0);
//                 bsp_motor_set_pwm(BSP_MOTOR_R, 0);
//                 break;
//             }
//             osDelay(2);
//         }
//     }
// }

#define set_all(speed) for(int i=0;i<4;i++){\
        bsp_motor_set_speed(&bsp_motors[i], speed); \
    }

#define stop_motor(m) bsp_motor_set_ctrl_mode(m, BSP_MOTOR_CTRL_MODE_NONE);bsp_motor_set_pwm(m, 0);
#define BASE_SPEED 1.5
static void go_test(){
    //start:
    for(int i=0;i<4;i++){
        bsp_motor_set_speed(&bsp_motors[i], 0);
        bsp_motor_set_ctrl_mode(&bsp_motors[i], BSP_MOTOR_CTRL_MODE_SPEED);
    }
    while(1){
        char c;
        scanf("%c", &c);
        switch(c){
            case 'f':
                set_all(BASE_SPEED);
                break;
            case 's':
                set_all(0);
                break;
            case 'b':
                set_all(-BASE_SPEED);
                break;
            case 'r':
                bsp_motor_set_speed(bsp_motor1, BASE_SPEED);
                bsp_motor_set_speed(bsp_motor3, BASE_SPEED);
                bsp_motor_set_speed(bsp_motor2, -BASE_SPEED);
                bsp_motor_set_speed(bsp_motor4, -BASE_SPEED);
                break;
            case 'l':
                bsp_motor_set_speed(bsp_motor1, -BASE_SPEED);
                bsp_motor_set_speed(bsp_motor3, -BASE_SPEED);
                bsp_motor_set_speed(bsp_motor2, BASE_SPEED);
                bsp_motor_set_speed(bsp_motor4, BASE_SPEED);
                break;
        }
    }
}




void app_idle_task(){
    speed_pid_test();
    //pwm_test();
    //bsp_task_create("spd", report_task, NULL, osPriorityLow2, 512);
    //go_test();
    //bsp_motor_init();
    // bsp_motor_set_pwm(bsp_motor4, -0.8);
    // while(1){
    //     printf("%f\n", bsp_motor4->current_speed);
    //     osDelay(20);
    // }
}
#include "bsp_chasis.h"
#include "bsp_motor.h"

// 轮距
#define BSP_CHASIS_WHEEL_TRACK (218)
// 轴距
#define BSP_CHASIS_WHEEL_BASE  (184)
// 轮直径
#define BSP_CHASIS_WHEEL_DIAMETER BSP_MOTOR_DIAMETER
// 轮半径
#define BSP_CHASIS_WHEEL_RADIUS   (BSP_CHASIS_WHEEL_DIAMETER / 2.0f)
// 轮周长
#define BSP_CHASIS_WHEEL_PERIMETER BSP_MOTOR_PERIMETER

#define deg_to_rad(x)   (x * PI / 180)
#define rpm_to_mmps(rpm)    (rpm * BSP_CHASIS_WHEEL_PERIMETER / 60)
#define rpm_from_mmps(mmps) (mmps * 60 / BSP_CHASIS_WHEEL_PERIMETER)

#define motor_lb (bsp_motor1)
#define motor_rb (bsp_motor2)
#define motor_lf (bsp_motor3)
#define motor_rf (bsp_motor4)

const static bsp_motor_t *reordered_motors[] = {motor_rf, motor_lf, motor_lb, motor_rb};

void bsp_chasis_set(bsp_chasis_state *target){
    float t1 = target->vy;
    float t2 = target->vx;
    float t3 = deg_to_rad(target->vw) * (BSP_CHASIS_WHEEL_TRACK + BSP_CHASIS_WHEEL_BASE) / 2;
    
    float spd0 = t1 - t2 + t3;
    float spd1 = t1 + t2 - t3; 
    float spd2 = t1 - t2 - t3;
    float spd3 = t1 + t2 + t3;

    bsp_motor_set_speed(reordered_motors[0], rpm_from_mmps(spd0));
    bsp_motor_set_speed(reordered_motors[1], rpm_from_mmps(spd1));
    bsp_motor_set_speed(reordered_motors[2], rpm_from_mmps(spd2)); 
    bsp_motor_set_speed(reordered_motors[3], rpm_from_mmps(spd3));
    
}

void bsp_chasis_get(bsp_chasis_state *result){
    // TODO
}
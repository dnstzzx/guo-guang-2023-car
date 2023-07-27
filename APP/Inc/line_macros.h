#pragma once
#include "bsp_scan.h"

#define wait_until(what) \
    do{ \
        osDelay(TICK_MS); \
        bsp_scanner_group_update_all(); \
    }while(!(what));

#define keep_in_line_until(what, spd) \
    bsp_scanner_group_update_all(); \
    do{ \
        keep_in_line(spd); \
        osDelay(TICK_MS); \
        bsp_scanner_group_update_all(); \
    }while(!(what));

#define keep_in_line_in_ms(ms, spd) \
    bsp_scanner_group_update_all(); \
    for(int i=0;i<ms/TICK_MS;i++){  \
        keep_in_line(spd); \
        osDelay(TICK_MS); \
        bsp_scanner_group_update_all(); \
    }

#define calc_spd() \
    ((delta == 0.0f && bias == 0.0f) ?  BASE_SPEED * 1.3 : BASE_SPEED)

#define line_straight_def(enter, exit, backward_mode)\
    keep_in_line_reset(backward_mode);\
    bsp_led_set(BSP_LED1, true);\
    for(int i=0;i<count-1;i++){\
        keep_in_line_until(enter, calc_spd());\
        keep_in_line_until(exit, calc_spd());\
    }\
    keep_in_line_until(enter, BASE_SPEED);\
    line_half(BASE_SPEED);\
    line_stop();\
    bsp_led_set(BSP_LED1, false);\


#define logical_set_speed(dx, dy, dw) \
    if(line_inverse_mode){ \
        bsp_chasis_set_speed(-1 * (dx), -1 * (dy), dw); \
    }else{ \
        bsp_chasis_set_speed(dx, dy, dw); \
    }


static inline bool check_left(bsp_scanner_group_t *group){
    return group->datas[0].digital;
}

static inline bool check_right(bsp_scanner_group_t *group){
    return group->datas[BSP_SCANNER_COUNT - 1].digital;
}

static inline bool check_all_white(bsp_scanner_group_t *group){
    return group->digital_byte == 0;
}

static inline bool check_valid_to_line(bsp_scanner_group_t *group){
    return !check_left(group) && !check_right(group) && !check_all_white(group);
}

static inline float group_pos(bsp_scanner_group_t *group){
    static uint8_t masks[] = {0b00011000, 0b00110000, 0b00001100, 0b00010000, 0b00001000, 0b01100000, 0b00000110, 0b00100000, 0b00000100, 0b01000000, 0b00000010};
    static float result[] = {3.5f, 2.5f, 4.5f, 3.0f, 4.0f, 1.5f, 5.5f, 2.0f, 5.0f, 1.0f, 6.0f};
    for(int i=0;i<sizeof(masks) / sizeof(uint8_t);i++)
        if((group->digital_byte & masks[i]) == masks[i])
            return result[i] - ((BSP_SCANNER_COUNT - 1) / 2.0f);
    
    printf("error: no matching group pos mask\n");
    return 0;
}

static inline void print_scanner_group(const char *preffix, bsp_scanner_group_t *group){
    printf("%s:", preffix);
    for(int i=0;i<BSP_SCANNER_COUNT;i++){
        printf(" %d", group->datas[i].digital);
    }
    printf("\n");
}
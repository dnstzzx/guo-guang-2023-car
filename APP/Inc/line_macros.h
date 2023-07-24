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
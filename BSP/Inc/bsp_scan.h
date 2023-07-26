#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "bsp_i2c.h"

#define BSP_SCANNER_GROUP_COUNT (2)
#define BSP_SCANNER_COUNT (8)
#define BSP_SCANNER_ENABLE_READ_ANALOG  (0)

typedef struct{
    bool digital;   // 黑1白0
    //uint8_t analog;
} bsp_scanner_data_t;

typedef struct{
    bsp_i2c_dev_t *dev;
    uint8_t i2c_addr;
    uint8_t digital_byte;
    bsp_scanner_data_t datas[BSP_SCANNER_COUNT];
} bsp_scanner_group_t;

extern bsp_scanner_group_t bsp_scanner_groups[BSP_SCANNER_GROUP_COUNT];
#define bsp_scanner_group_front (&bsp_scanner_groups[0])
#define bsp_scanner_group_back (&bsp_scanner_groups[1])

bool bsp_scanner_group_update(bsp_scanner_group_t *group);

static inline bool bsp_scanner_group_update_all(){
    for(int i=0;i<BSP_SCANNER_GROUP_COUNT;i++){
        if(!bsp_scanner_group_update(&bsp_scanner_groups[i]))
            return false;
    }
    return true;
}
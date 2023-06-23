#include "bsp_scan.h"
#include "main.h"

typedef struct{
    GPIO_TypeDef *port;
    uint16_t      pin;
} bsp_scanner_t;

// static bsp_scanner_t scanners[] = {
//     {.port=SCANNER1_GPIO_Port, .pin=SCANNER1_Pin},
//     {.port=SCANNER2_GPIO_Port, .pin=SCANNER2_Pin},
//     {.port=SCANNER3_GPIO_Port, .pin=SCANNER3_Pin},
//     {.port=SCANNER4_GPIO_Port, .pin=SCANNER4_Pin},
// };

static bsp_scanner_t scanners[] = {
    {.port=SCANNER7_GPIO_Port, .pin=SCANNER7_Pin},
    {.port=SCANNER6_GPIO_Port, .pin=SCANNER6_Pin},
    {.port=SCANNER5_GPIO_Port, .pin=SCANNER5_Pin},
    {.port=SCANNER4_GPIO_Port, .pin=SCANNER4_Pin},
    {.port=SCANNER3_GPIO_Port, .pin=SCANNER3_Pin},
    {.port=SCANNER2_GPIO_Port, .pin=SCANNER2_Pin},
    {.port=SCANNER1_GPIO_Port, .pin=SCANNER1_Pin},

};

bool bsp_scan_get(uint8_t id){
    return HAL_GPIO_ReadPin(scanners[id].port, scanners[id].pin);
}


void bsp_scan_get_all(bool *rtn){
    for(uint8_t i=0;i<BSP_SCANNER_COUNT;i++){
        rtn[i] = bsp_scan_get(i);
    }
}
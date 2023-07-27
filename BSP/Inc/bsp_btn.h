#pragma once
#include <stdbool.h>
#include "stm32f4xx_hal.h"

#define BSP_BTN_COUNT (4)
#define bsp_btn_dev_left (&bsp_btn_devs[0])
#define bsp_btn_dev_mid (&bsp_btn_devs[1])
#define bsp_btn_dev_set (&bsp_btn_devs[2])
#define bsp_btn_dev_res (&bsp_btn_devs[3])

typedef enum {
    BSP_BTN_EVENT_RELEASED = 0,
    BSP_BTN_EVENT_PRESSED = 1,
    
} bsp_btn_event_type_t;

#define bsp_btn_event_type_t uint8_t

typedef struct bsp_btn_dev_t bsp_btn_dev_t;
typedef void(*bsp_btn_callback_t)(bsp_btn_dev_t *dev, bsp_btn_event_type_t event_type);

struct bsp_btn_dev_t{
    GPIO_TypeDef *port;
    uint16_t pin;
    bool pressed;
    bool _last_detect;
    bsp_btn_callback_t pressed_callback;
    bsp_btn_callback_t released_callback;
} ;

extern bsp_btn_dev_t bsp_btn_devs[BSP_BTN_COUNT];

void bsp_btn_init();
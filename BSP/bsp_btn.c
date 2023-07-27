#include "bsp_btn.h"
#include "bsp_task.h"
#include "main.h"

bsp_btn_dev_t bsp_btn_devs[BSP_BTN_COUNT] = {
    {
        .port = BTN_LEFT_GPIO_Port,
        .pin = BTN_LEFT_Pin,
        .pressed = false,
        .pressed_callback = NULL,
        .released_callback = NULL
    },
    {
        .port = BTN_MID_GPIO_Port,
        .pin = BTN_MID_Pin,
        .pressed = false,
        .pressed_callback = NULL,
        .released_callback = NULL
    },
    {
        .port = BTN_SET_GPIO_Port,
        .pin = BTN_SET_Pin,
        .pressed = false,
        .pressed_callback = NULL,
        .released_callback = NULL
    },
    {
        .port = BTN_RES_GPIO_Port,
        .pin = BTN_RES_Pin,
        .pressed = false,
        .pressed_callback = NULL,
        .released_callback = NULL
    }
};

static void bsp_btn_detect_task(void *param){
    while(1){
        for(int i=0;i<BSP_BTN_COUNT;i++){
            bsp_btn_dev_t *btn = &bsp_btn_devs[i];
            bool curr = HAL_GPIO_ReadPin(btn->port, btn->pin) == GPIO_PIN_RESET;
            if(curr == btn->_last_detect && curr != btn->pressed){
                btn->pressed = curr;
                bsp_btn_callback_t callback = curr ? btn->pressed_callback : btn->released_callback;
                if(callback != NULL){
                    callback(btn, curr ? BSP_BTN_EVENT_PRESSED : BSP_BTN_EVENT_RELEASED);
                }
            }
            btn->_last_detect = curr;
        }
        osDelay(20);
    }
}

void bsp_btn_init(){
    bsp_task_create("bsp_btn", bsp_btn_detect_task, NULL, osPriorityLow3, 192);
}

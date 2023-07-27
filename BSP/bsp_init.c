#include "bsp_init.h"
#include "bsp_uart.h"
#include "bsp_task.h"
#include "bsp_motor.h"
#include "app_entry.h"
#include "bsp_btn.h"
#include "cmsis_os.h"

void bsp_init(){
    #ifdef APP_ENTRY_ENABLE_MOTOR
        bsp_motor_init();
    #endif
    osDelay(1000);
    bsp_uart_init();
    bsp_btn_init();
}

void bsp_init_task(void *p){
    bsp_init();
    
    bsp_task_create("main", bsp_idle_task, NULL, osPriorityNormal, 512);
    osThreadTerminate(osThreadGetId());
}

void bsp_idle_task(void *p){
    app_idle_task();
    /*
    static uint8_t data[120];
    sprintf(data, "Received:");
    while(1){
        if(bsp_uart1_received_data_count() != 0){
            uint32_t read_size = bsp_uart1_read_data(&data[9], 100 - 9);
            bsp_uart1_send_data(data, read_size + 9);
        }
        osDelay(1);
    }*/
    while(1) osDelay(1);
}
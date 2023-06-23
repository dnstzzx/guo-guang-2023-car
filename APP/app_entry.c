#include "app_entry.h"
#include "stdio.h"
#include "stdbool.h"
#include "cmsis_os.h"
#include "bsp_uart.h"

void app_idle_task();
void count_up_forever(){
    size_t aaa = 0;
    while(1){
        printf("%d\n", aaa);
        osDelay(1000);
        aaa ++;
    }
}

//#include "app_entry_motor.c"
#include "app_entry_line.c"
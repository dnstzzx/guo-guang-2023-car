#include <stdio.h>
#include "bsp_scan.h"


void app_idle_task(){
    while(1){
        bsp_scanner_group_update_all();
        for(int i=0;i<BSP_SCANNER_GROUP_COUNT;i++){
            printf("group %d:", i);
            for(int j=0;j<BSP_SCANNER_COUNT;j++){
                printf(" %d", bsp_scanner_groups[i].datas[j].digital);
            }
            printf("\nanalog:");
            for(int j=0;j<BSP_SCANNER_COUNT;j++){
                //printf(" %d", bsp_scanner_groups[i].datas[j].analog);
            }
            printf("\n");
        }
        printf("\n");
        osDelay(200);
    }
}
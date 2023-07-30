#include "bsp_btn.h"
#include "report.h"

static bsp_btn_dev_t *report_btns[] = {
    bsp_btn_dev_mid,
    bsp_btn_dev_set,
    bsp_btn_dev_res
};

static void callback(bsp_btn_dev_t *dev, bsp_btn_event_type_t event_type){
    const char *btn_name = NULL;
    if(dev == bsp_btn_dev_mid){
        btn_name = "BTN_MID";
    }else if(dev == bsp_btn_dev_set){
        btn_name = "BTN_SET";
    }else if(dev == bsp_btn_dev_res){
        btn_name = "BTN_RES";
    }

    if(btn_name == NULL)
        return;

    report_send_wrapper(btn_name, "RELEASED") 
}

void btn_report_init(){
    for(int i=0;i<sizeof(report_btns) / sizeof(bsp_btn_dev_t *);i++){
        report_btns[i]->released_callback = callback;
    }
}
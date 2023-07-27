#include "bsp_motor.h"
#include "command.h"
#include "cmds.h"

static cmd_hdler_set *enabled_cmd_sets[] = {
    &cmd_set_line, 
    &cmd_set_ctrl
};

static uint8_t enabled_cmd_set_count = sizeof(enabled_cmd_sets) / sizeof(cmd_hdler_set *);

void btn_report_init();
void app_idle_task(){
    btn_report_init();
    bsp_motor_set_ctrl_mode_all(BSP_MOTOR_CTRL_MODE_SPEED);
    command_main(enabled_cmd_sets, enabled_cmd_set_count);
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cmsis_os.h"
#include "line.h"
#include "bsp_motor.h"
#include "bsp_chasis.h"
#include "command.h"
#include "report.h"

// ctrl cmd set

static void hdlr_echo(command_t *cmd){
    report_t rpt = {
        .name = "OHCE",
        .args_count = cmd->args_count,
        .args_str = cmd->args_str
    };
    report_send(&rpt);
}

static command_handler_t ctrl_handlers[] = {
    {.cmd_name = "ECHO", .func = hdlr_echo}
};

cmd_hdler_set cmd_set_ctrl = {
    .handlers = ctrl_handlers,
    .hdler_count = sizeof(ctrl_handlers) / sizeof(command_handler_t)
};


// line cmd set

static void hdlr_straight(command_t *cmd){
    if(cmd->args_count != 1){
        printf("err:args count != 1\n");
        return;
    }
    int arg0 = atoi(cmd->args_str[0]);
    if(strcmp(cmd->cmd, "l") == 0){
        line_straight_l(arg0);
    }else if(strcmp(cmd->cmd, "r") == 0){
        line_straight_r(arg0);
    }else if(strcmp(cmd->cmd, "bl") == 0){
        line_backward_straight_l(arg0);
    }else if(strcmp(cmd->cmd, "br") == 0){
        line_backward_straight_r(arg0);
    }else if(strcmp(cmd->cmd, "f") == 0){
        line_straight_distance(arg0);
    }
}

static void hdlr_turn(command_t *cmd){
    uint8_t count = 1;
    if(cmd->args_count == 1){
        count = atoi(cmd->args_str[0]);
    }
    if(strcmp(cmd->cmd, "L") == 0){
        line_turn_left(count);
    }else if(strcmp(cmd->cmd, "R") == 0){
        line_turn_right(count);
    }else if(strcmp(cmd->cmd, "bL") == 0){
        line_backward_turn_left(count);
    }else if(strcmp(cmd->cmd, "bR") == 0){
        line_backward_turn_right(count);
    }
}

static void hdlr_stop(command_t *cmd){
    bsp_chasis_set_speed(0, 0, 0);
    osDelay(2000);
}

static command_handler_t line_handlers[] = {
    {.cmd_name="l", .func=hdlr_straight},
    {.cmd_name="r", .func=hdlr_straight},
    {.cmd_name="bl", .func=hdlr_straight},
    {.cmd_name="br", .func=hdlr_straight},
    {.cmd_name="f", .func=hdlr_straight},
    {.cmd_name="s", .func=hdlr_stop},
    {.cmd_name="L", .func=hdlr_turn},
    {.cmd_name="R", .func=hdlr_turn},
    {.cmd_name="bL", .func=hdlr_turn},
    {.cmd_name="bR", .func=hdlr_turn}
};

cmd_hdler_set cmd_set_line = {
    .handlers = line_handlers,
    .hdler_count = sizeof(line_handlers) / sizeof(command_handler_t)
};
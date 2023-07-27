#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "command.h"
#include "report.h"

#define BUFFER_SIZE 256

static char buffer[BUFFER_SIZE + 1];
static uint8_t length;

void command_read_cmd(command_t *cmd){
    static char c;
    start_read:
    length = 0;
    cmd->args_count = 0;
    
    bool last_blank = false;

    do{
        c = getchar();
        if(c == '\n' || c == '\r')
            continue;
        
        if(last_blank && c != ';'){
            if(c == ' ')
                continue;

            if(cmd->args_count < CMD_MAX_ARG_COUNT)
                cmd->args_str[cmd->args_count] = &buffer[length];
            cmd->args_count ++;
        }

        last_blank = c == ' ';
        buffer[length++] = c == ' ' ||  c == ';' ? '\0' : c;

    }while(c != ';' && length != BUFFER_SIZE);
    if(c != ';'){
        while(getchar()!=';');
        report_send_wrapper("READ");
        report_send_wrapper("ERR", "cmd too long");
        goto start_read;
    }
    cmd->cmd = buffer;

    report_send_wrapper("READ");
    // printf("name: %s\narg count:%d\n", cmd->cmd, cmd->args_count);
    // printf("args:");
    // for(int i=0;i<cmd->args_count;i++){
    //     printf(" %s,", cmd->args_str[i]);
    // }
    // printf("\n");
}

bool command_process_cmd(command_t *cmd, cmd_hdler_set *hdler_set){
    for(int i=0;i<hdler_set->hdler_count;i++){
        command_handler_t *hdler = &hdler_set->handlers[i];
        if(strcmp(cmd->cmd, hdler->cmd_name) == 0){
            hdler->func(cmd);
            report_send_wrapper("OK");
            return true;
        }
    }
    return false;
}

void command_main(cmd_hdler_set *using_cmd_hdler_set[], uint32_t cmd_hdler_set_count){
    static command_t cmd;
    while(1){
        command_read_cmd(&cmd);
        bool matched = false;
        for(int i=0;i<cmd_hdler_set_count;i++){
            if(command_process_cmd(&cmd, using_cmd_hdler_set[i])){
                matched = true;
                break;
            }
        }
        if(!matched){
            report_send_wrapper("ERR", "no matching cmd", cmd.cmd);
        }
    }
}
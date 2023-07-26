#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "command.h"

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
        printf("READ\n");
        printf("ERR:cmd too long\n");
        goto start_read;
    }
    cmd->cmd = buffer;

    printf("READ\n");
    // printf("name: %s\narg count:%d\n", cmd->cmd, cmd->args_count);
    // printf("args:");
    // for(int i=0;i<cmd->args_count;i++){
    //     printf(" %s,", cmd->args_str[i]);
    // }
    // printf("\n");
}

bool command_process_cmd(command_t *cmd, command_handler_t handlers[], uint32_t handlers_count){
    for(int i=0;i<handlers_count;i++){
        command_handler_t *hdler = &handlers[i];
        if(strcmp(cmd->cmd, hdler->cmd_name) == 0){
            hdler->func(cmd);
            printf("OK\n");
            return true;
        }
    }
    printf("ERR:no matching cmd\n");
    return false;
}
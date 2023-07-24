#pragma once
#include <stdint.h>

#define CMD_MAX_ARG_COUNT 16


typedef struct{
    char *cmd;
    uint8_t args_count;
    char *args_str[CMD_MAX_ARG_COUNT];
} command_t;

typedef void(*cmd_handler_func_t)(command_t *cmd);

typedef struct{
    const char *cmd_name;
    cmd_handler_func_t func;
} command_handler_t;

void command_read_cmd(command_t *cmd);
bool command_process_cmd(command_t *cmd, command_handler_t handlers[], uint32_t handlers_count);

static inline void command_main(command_handler_t handlers[], uint32_t handlers_count){
    static command_t cmd;
    while(1){
        command_read_cmd(&cmd);
        command_process_cmd(&cmd, handlers, handlers_count);
    }
}
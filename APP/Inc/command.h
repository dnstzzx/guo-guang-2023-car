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

typedef struct{
    command_handler_t *handlers;
    uint32_t hdler_count;
} cmd_hdler_set;

void command_read_cmd(command_t *cmd);

// returns: whether any command hdler matched
bool command_process_cmd(command_t *cmd, cmd_hdler_set *hdler_set);


void command_main(cmd_hdler_set *using_cmd_hdler_set[], uint32_t cmd_hdler_set_count);
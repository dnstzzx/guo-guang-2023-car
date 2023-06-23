#pragma once

typedef enum{
    CMD_STOP,
    CMD_LINE_STRAIGHT,
    CMD_LINE_TURN,
    
} command_type_t;

#pragma pcak(1)
typedef struct{
    command_type_t type;
    union{
        struct {
            
        } line_straight_param;
    };

} command_t;
#pragma pcak()
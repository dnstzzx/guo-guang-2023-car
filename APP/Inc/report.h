#pragma once
#include <stdint.h>
#include <stdio.h>

#define RPT_MAX_ARG_COUNT 16
typedef struct{
    char *name;
    uint8_t args_count;
    char **args_str;
} report_t;

static inline void report_send(report_t *report){
    printf("<<<%s", report->name);
    for(int i=0;i<report->args_count;i++){
        printf(":%s", report->args_str[i]);
    }
    printf(">>>\n");
}

#define report_send_wrapper(name, ...)                          \
    do {                                                        \
        char *args_list[] = { __VA_ARGS__ };                    \
        uint8_t args_count = sizeof(args_list) / sizeof(char*); \
        report_t report = { name, args_count, args_list };      \
        report_send(&report);                                   \
    } while(0);

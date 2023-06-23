#pragma once
#include <stdint.h>
#include <stdbool.h>

#define BSP_SCANNER_COUNT (7)

bool bsp_scan_get(uint8_t id);
void bsp_scan_get_all(bool *rtn);
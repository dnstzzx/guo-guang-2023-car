#pragma once
#include <stdint.h>

void line_stop();
void line_go(int mm);
void line_straight_l(uint8_t count);
void line_straight_r(uint8_t count);
void line_straight_distance(uint32_t mm);

void line_turn_left(uint8_t count);
void line_turn_right(uint8_t count);

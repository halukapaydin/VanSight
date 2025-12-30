//
// Created by haluk on 12/29/25.
//

#ifndef DISPLAY_CLIENT_CONFIG_H
#define DISPLAY_CLIENT_CONFIG_H
#include "misc/lv_color.h"

const lv_color_t COLOR_BUTTON_ACTIVE = lv_color_hex(0xd4b400);
const lv_color_t COLOR_BUTTON_INACTIVE = lv_color_hex(0x525252);
const lv_color_t COLOR_CONNECTION_ACTIVE = lv_color_hex(0xd4b400);
const lv_color_t COLOR_CONNECTION_LOST = lv_color_hex(0xED457E);

constexpr uint8_t SENSOR_CLEAN_WATER = 0;
constexpr uint8_t SENSOR_GRAY_WATER = 1;
constexpr uint8_t SENSOR_BLACK_WATER = 2;


#endif //DISPLAY_CLIENT_CONFIG_H
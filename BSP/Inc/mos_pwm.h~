/************************************************************************
 * LED智能控制器BSP包LED驱动文件
 * Copyright (C) 2025  Xu Ruijun
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ************************************************************************/
#ifndef LED_H
#define LED_H

#include <stdint.h>

#define LED(x) (x-1)
#define LED1  LED(1)
#define LED2  LED(2)
#define LED3  LED(3)
#define LED4  LED(4)
#define LED_COUNT (4)

void     LED_Init();
void     LED_SetOutputEnable(int LEDx, int isEnable);
int      LED_GetOutputEnable(int LEDx);
void     LED_SetOutputCompare(int LEDx, uint16_t compare);
uint32_t LED_GetOutputCompare(int LEDx);
void     LED_StopAll();

#endif

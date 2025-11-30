/************************************************************************
 * ADC驱动文件
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
*************************************************************************/
#ifndef ADC_H
#define ADC_H

#define GPIO_PORT_ADC_VBUS    GPIOA
#define LL_GPIO_PIN_ADC_VBUS  LL_GPIO_PIN_1
#define LL_ADC_CHANNEL_VBUS   LL_ADC_CHANNEL_1
#define GPIO_PORT_ADC_IBUS    GPIOA
#define LL_GPIO_PIN_ADC_IBUS  LL_GPIO_PIN_2
#define LL_ADC_CHANNEL_IBUS   LL_ADC_CHANNEL_2

#define GPIO_PORT_ADC_ISLR    GPIOA
#define LL_GPIO_PIN_ADC_ISLR  LL_GPIO_PIN_3
#define LL_ADC_CHANNEL_ISLR   LL_ADC_CHANNEL_3
#define GPIO_PORT_ADC_VSLR    GPIOA
#define LL_GPIO_PIN_ADC_VSLR  LL_GPIO_PIN_4
#define LL_ADC_CHANNEL_VSLR   LL_ADC_CHANNEL_4

#define LL_DMA_CHANNEL_ADC    LL_DMA_CHANNEL_3

#define ADC_BUFFSAMPS         (2)
#define ADC_BUFFSIZE          (4*2)

void ADC_Init();
void ADC_DMA_TC_Callback();
void ADC_DMA_HT_Callback();

#endif

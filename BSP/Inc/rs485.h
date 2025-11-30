/************************************************************************
 * RS485驱动文件
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
#ifndef RS485_H
#define RS485_H

#include <stdint.h>
#include "py32f0xx.h"

#define RS485_ADDR  (0xAC)

typedef enum{
  RS485_On_IdleORMute,
  RS485_On_Trasmit,
  RS485_On_Recevice,
}RS485_StatusType;

void RS485_Init();
int RS485_Send(uint8_t *p, uint16_t size);
void USART_TX_DMA_TC_Callback();
void USART_RX_DMA_TC_Callback();

#endif

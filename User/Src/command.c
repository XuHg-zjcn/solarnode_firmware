/************************************************************************
 * 命令解析
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
#include "command.h"
#include <stdint.h>
#include <string.h>
#include "modbus.h"
#include "adc.h"

uint8_t resp_buff[MAXSIZE_RESP];
extern volatile ADCSamp_t last_samp;


int MB_ReadInputCB_single(uint16_t addr)
{
  switch(addr){
  case 0x0010:
    return last_samp.vbus;
  case 0x0011:
    return last_samp.ibus;
  case 0x0012:
    return last_samp.vslr;
  case 0x0013:
    return last_samp.islr;
  default:
    return MB_ERR_ILL_ADDR;
  }
}

uint32_t process_cmd(uint8_t *p, int32_t size)
{
  return MB_ProcessRecvWithCRC(p, size, resp_buff);
}

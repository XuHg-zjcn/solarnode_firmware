/************************************************************************
 * 曼彻斯特编码库
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
#include <stdint.h>

static const uint8_t tab_enc[16] = {
  0xaa, 0xa9, 0xa6, 0xa5,
  0x9a, 0x99, 0x96, 0x95,
  0x6a, 0x69, 0x66, 0x65,
  0x5a, 0x59, 0x56, 0x55
};

//使用uint32_t作为变量，避免编译器输出uxth,uxtb等指令
void Manchester_encode(const uint8_t *pIn, uint8_t *pOut, uint32_t size)
{
  while(size--){
    uint8_t byte = *pIn++;
    *pOut++ = tab_enc[byte&0x0f];
    *pOut++ = tab_enc[(byte>>4)&0x0f];
  }
}

//TODO: 使用查表法加速
uint32_t Manchester_decode(const uint8_t *pIn, uint8_t *pOut, uint32_t size)
{
  uint32_t errcount = 0;
  while(size--){
    uint32_t byte = 0;
    uint16_t indata = (*pIn++) | ((*pIn++)<<8);
    if((indata ^ (indata>>1))&0x5555 != 0x5555){
      errcount++;
    }
    for(int i=0;i<8;i++){
      if(indata & 0x0001){
	byte |= (1U<<i);
      }
      indata >>= 2;
    }
    *pOut++ = byte;
  }
  return errcount;
}

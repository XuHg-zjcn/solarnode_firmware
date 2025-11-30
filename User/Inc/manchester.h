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
#ifndef MANCHESTER_H
#define MANCHESTER_H

#include <stdint.h>

void Manchester_encode(const uint8_t *pIn, uint8_t *pOut, uint32_t size);
void Manchester_decode(const uint8_t *pIn, uint8_t *pOut, uint32_t size);

#endif

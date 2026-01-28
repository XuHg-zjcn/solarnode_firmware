/************************************************************************
 * PI控制
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
#ifndef PI_CONTROLL_H
#define PI_CONTROLL_H

#include <stdint.h>

typedef struct{
  uint32_t k_p;
  uint32_t k_i;
  uint32_t target;
  uint32_t out_max;
  int32_t s;
}PI_data;

uint32_t PI_update(PI_data *data, uint32_t curr);
uint32_t PI_set_s(PI_data *data, uint32_t curr, uint32_t out);

#endif

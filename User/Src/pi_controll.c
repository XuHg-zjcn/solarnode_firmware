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
#include "pi_controll.h"


uint32_t PI_update(PI_data *data, uint32_t curr)
{
  int32_t err = data->target - curr;
  int32_t out = (((int32_t)data->s) >> 16) + (((int32_t)(err * data->k_p)) >> 16);
  data->s += (((int32_t)(err * data->k_i)) >> 8);
  if(data->s < 0){
    data->s = 0;
  }else if(data->s > (data->out_max << 16)){
    data->s = (data->out_max << 16);
  }
  if(out < 0){
    return 0;
  }else if(out > data->out_max){
    return data->out_max;
  }else{
    return out;
  }
}

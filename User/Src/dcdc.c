/************************************************************************
 * 软件DC-DC驱动文件
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
#include "dcdc.h"
#include "adc.h"
#include "pi_controll.h"
#include "mos_pwm.h"

static DCDC_Mode_t mode; //目前只支持CV（恒压）
PI_data pi_cv;
uint32_t i = 0;

void DCDC_Init()
{
  mode = DCDC_Mode_CV;
  pi_cv.k_p = 4534;    // 0.02 / 55.5 * 192 * 2**16
  pi_cv.k_i = 5804;    // 1e-4 / 55.5 * 192 * 2**24
  pi_cv.target = 2664; // 55.5 * 48
  pi_cv.out_max = 128; // 2/3 * 192
  pi_cv.s = 0;
}

void DCDC_ADC_update_callback(ADCSamp_t *data)
{
  uint16_t pwm_value;
  if(mode == DCDC_Mode_CV){
    pwm_value = PI_update(&pi_cv, data->vbus);
  }else{
    pwm_value = 0;
  }
  MOSPWM_SetOutputCompare(pwm_value);
}

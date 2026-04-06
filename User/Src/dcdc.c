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
#include "py32f0xx_hal.h"

#define PROT_EN  0
#define CV_EN    1
#define CC_EN    0
#define MPPT_EN  0

#define TH_LOW_VOLT_ADC  (256)

static DCDC_Mode_t mode; //目前只支持CV（恒压）
static DCDC_Param_t param;
PI_data pi_cv;
PI_data pi_cc;
volatile uint32_t update_count = 0;
extern uint16_t adc_buff[ADC_BUFFSIZE];

void DCDC_Init()
{
  mode = DCDC_Mode_Stop;
  // 22/(470+22)/3.3*4096 = 55.5
  // 以下数据通过多次实验调整得到
#if CV_EN
  pi_cv.k_p = 3000;  // k_p / 55.5 * 192 * 2**16
  pi_cv.k_i = 50;    // k_i / 55.5 * 192 * 2**24
  pi_cv.target = 0; // 55.5 * vout
  pi_cv.out_max = 100; // maxduty * 192
  pi_cv.s = 0;
#endif

#if CC_EN
  pi_cc.k_p = 1000;
  pi_cc.k_i = 10;
  pi_cc.target = 0;
  pi_cc.out_max = 100;
  pi_cc.s = 0;
#endif

  param.cv_targ = 800;
  param.cc_targ = 300;
  param.v_prot = 1000;
  param.i_prot = 500;
}

void DCDC_Soft_Start()
{
  mode = DCDC_Mode_Stop;
  uint32_t c0 = update_count;
  HAL_Delay(10);
  if(update_count == c0){
    //等了10ms了，还没有更新ADC数据，失败
    return;
  }
  if((adc_buff[0] < TH_LOW_VOLT_ADC) && (adc_buff[2] < TH_LOW_VOLT_ADC)){
    //VBUS和VSLR电压都很低，MCU电源不是来自板载降压器，连接了其他电源用于调试
    //或电压采样电路发生故障
    return;
  }
  pi_cv.target = (adc_buff[0]+adc_buff[4])/2;
  mode = DCDC_Mode_CV;
  while(pi_cv.target < 800){//param.cv_targ){
    HAL_Delay(1);
    pi_cv.target += 1;
  }
}

__attribute__((section(".fast_text_ram")))
void DCDC_ADC_update_callback(ADCSamp_t *data)
{
  int32_t pwm_value = 0;
  //TODO: STOP模式添加电压监控，从STOP模式启动
  //VBUS持续供电MCU一直处于运行，但DCDC没有电源输入不运行，日出光伏板接受光照时需要自动启动
#if PROT_EN
  if((data->vbus > param.v_prot) || (data->ibus > param.i_prot)){
    mode = DCDC_Mode_Stop;
  }else
#endif

#if CV_EN
  if(mode == DCDC_Mode_CV){
#if CC_EN
    if((data->ibus > param.cc_targ) && (data->vbus < param.cv_targ)){
      mode = DCDC_Mode_CC;
      uint32_t old_pwm = MOSPWM_GetOutputCompare();
      PI_set_s(&pi_cc, data->ibus, old_pwm);
      pwm_value = old_pwm;
    }else{
      pwm_value = PI_update(&pi_cv, data->vbus);
    }
#else
    pwm_value = PI_update(&pi_cv, data->vbus);
#endif
  }else
#endif

#if CC_EN
  if(mode == DCDC_Mode_CC){
#if CV_EN
    if((data->ibus < param.cc_targ) && (data->vbus > param.cv_targ)){
      mode = DCDC_Mode_CV;
      uint32_t old_pwm = MOSPWM_GetOutputCompare();
      PI_set_s(&pi_cv, data->vbus, old_pwm);
      pwm_value = old_pwm;
    }else{
      pwm_value = PI_update(&pi_cc, data->ibus);
    }
#else
    pwm_value = PI_update(&pi_cc, data->ibus);
#endif
  }else
#endif

  {
  //  pwm_value = 0;
  }
  MOSPWM_SetOutputCompare(192-pwm_value);
  update_count++;
}

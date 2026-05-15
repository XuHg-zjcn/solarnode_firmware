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
#include <stdlib.h>
#include "py32f0xx_hal.h"

#define PROT_EN  0
#define CV_EN    1
#define CC_EN    0
#define MPPT_EN  1

#define TH_LOW_VOLT_ADC  (256)

DCDC_Mode_t mode; //目前只支持CV（恒压）
static DCDC_Param_t param = {
	.cv_targ = 800,
	.cc_targ = 300,
	.v_prot = 1000,
	.i_prot = 500,
};
// 22/(470+22)/3.3*4096 = 55.5
// 以下数据通过多次实验调整得到
PI_data pi_cv = {
  .k_p = 3000,  // k_p / 55.5 * 192 * 2**16
  .k_i = 50,    // k_i / 55.5 * 192 * 2**24
  .target = 0,  // 55.5 * vout
  .out_max = 100, // maxduty * 192
  .s = 0,
};
PI_data pi_cc = {
  .k_p = 1000,
  .k_i = 10,
  .target = 0,
  .out_max = 100,
  .s = 0,
};
PI_data pi_mppt = {
  .k_p = 3000,
  .k_i = 50,
  .target = 0,
  .out_max = 100,
  .s = 0,
};
volatile uint32_t update_count = 0;
uint32_t slr_pvdown_cont = 0; //用于恒流或恒压模式下偏差过大计数
extern uint16_t adc_buff[ADC_BUFFSIZE];
int32_t mppt_step = 10;
uint32_t mppt_pout_prev;

void DCDC_Init()
{
  mode = DCDC_Mode_Stop;
}

#if CV_EN
static uint32_t DCDC_Entry_CV_Mode(uint16_t vbus)
{
  mode = DCDC_Mode_CV;
  uint32_t old_pwm = MOSPWM_GetOutputCompare();
  PI_set_s(&pi_cv, vbus, 192-old_pwm);
  return old_pwm;
}
#endif

#if CC_EN
static uint32_t DCDC_Entry_CC_Mode(uint16_t ibus)
{
  mode = DCDC_Mode_CC;
  uint32_t old_pwm = MOSPWM_GetOutputCompare();
  PI_set_s(&pi_cc, ibus, 192-old_pwm);
  return old_pwm;
}
#endif

void DCDC_Soft_Start()
{
  mode = DCDC_Mode_Stop;
  uint32_t c0 = update_count;
  HAL_Delay(10);
  if(update_count == c0){
    //等了10ms了，还没有更新ADC数据，失败
    return;
  }
  if((adc_buff[0] < TH_LOW_VOLT_ADC) && (adc_buff[3] < TH_LOW_VOLT_ADC)){
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

#if MPPT_EN
static void MPPT_Update()
{
  if(labs(((adc_buff[3]+adc_buff[7])/2) - pi_mppt.target) > 5){
    return;
  }
  uint32_t power = ((adc_buff[0]+adc_buff[4])*(adc_buff[1]+adc_buff[5]));
  if(power < 1000){
    DCDC_Entry_CV_Mode((adc_buff[0]+adc_buff[4])/2);
  }
  if(power < mppt_pout_prev){
    mppt_step = -mppt_step;
  }
  pi_mppt.target += mppt_step;
  mppt_pout_prev = power;
}
#endif

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
    if(data->ibus > param.cc_targ){
      pwm_value = DCDC_Entry_CC_Mode(data->ibus);
    }else{
      pwm_value = 192-PI_update(&pi_cv, data->vbus);
    }
#else
    pwm_value = 192-PI_update(&pi_cv, data->vbus);
#endif
  }else
#endif

#if CC_EN
  if(mode == DCDC_Mode_CC){
#if CV_EN
    if(data->vbus > param.cv_targ){
      pwm_value = DCDC_Entry_CV_Mode(data->vbus);
    }else{
      pwm_value = 192-PI_update(&pi_cc, data->ibus);
    }
#else
    pwm_value = 192-PI_update(&pi_cc, data->ibus);
#endif
  }else
#endif

#if MPPT_EN
  if(mode == DCDC_Mode_MPPT){
#if CC_EN
    if(data->ibus > param.cc_targ){
      DCDC_Entry_CC_Mode(data->ibus);
    }else
#endif
#if CV_EN
    if(data->vbus > param.cv_targ){
      DCDC_Entry_CV_Mode(data->vbus);
    }else
#endif
    {
      pwm_value = 92+PI_update(&pi_mppt, data->vslr);
    }
  }else
#endif

  {
    pwm_value = 192;
  }
  MOSPWM_SetOutputCompare(pwm_value);
  update_count++;
#if MPPT_EN
  //观测到光伏板的电压和功率同时下降，需要进入MPPT模式
  if(mode != DCDC_Mode_MPPT){
    if((
#if CV_EN
       ((mode == DCDC_Mode_CV) && (data->vbus < pi_cv.target - 10)) ||
#endif
#if CC_EN
       ((mode == DCDC_Mode_CC) && (data->ibus < pi_cc.target - 10)) ||
#endif
       (0)) && ((data->ibus*data->vbus) > 1000)){
      slr_pvdown_cont++;
      if(slr_pvdown_cont > 50){
	pi_mppt.target = data->vslr;
        uint32_t old_pwm = MOSPWM_GetOutputCompare();
        PI_set_s(&pi_mppt, data->vslr, old_pwm-92);
        mode = DCDC_Mode_MPPT;
        slr_pvdown_cont=0;
      }
    }else{
      slr_pvdown_cont=0;
    }
  }

  if((mode == DCDC_Mode_MPPT) && ((update_count % 1024) == 0)){
     MPPT_Update();
  }
#endif
}

/************************************************************************
 * 太阳能节点BSP包MOS PWM驱动文件
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
#include "mos_pwm.h"
#include "py32f0xx_ll_tim.h"
#include "py32f0xx.h"

#define PWM_GPIO_PORT	GPIOA
#define PWM_GPIO_PIN	GPIO_PIN_0

#define PWM_TIM_CHANNEL  TIM_CHANNEL_3

#define PERIOD          (192)
#define PULSE_DEFAULT   (0)

TIM_HandleTypeDef   htim_led;
TIM_OC_InitTypeDef  sPWMConfig = {0};

//TODO: 此文件全部代码改为LL库
void MOSPWM_Init()
{
    GPIO_InitTypeDef    GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_TIM1_CLK_ENABLE();

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;              /* 推免输出 */
    GPIO_InitStruct.Pull = GPIO_NOPULL;                      /* 不上下拉 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;            /* GPIO速度 */
    GPIO_InitStruct.Alternate = GPIO_AF13_TIM1;

    GPIO_InitStruct.Pin = PWM_GPIO_PIN;                     /* MOS PWM: PA0  TIM1_CH3 */
    HAL_GPIO_Init(PWM_GPIO_PORT, &GPIO_InitStruct);

    htim_led.Instance = TIM1;
    htim_led.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;/* 时钟不分频 */
    htim_led.Init.Prescaler         = 1 - 1;                 /* 不预分频 */
    htim_led.Init.Period            = PERIOD - 1;            /* 计数器周期 */
    htim_led.Init.CounterMode       = TIM_COUNTERMODE_UP;    /* 计数器模式 */
    htim_led.Init.RepetitionCounter = 1 - 1;                 /* 不重复计数 */
    htim_led.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /* 关闭预载 */
    HAL_TIM_PWM_Init(&htim_led);

    sPWMConfig.OCMode       = TIM_OCMODE_PWM1;               /* 配置为PWM1模式 */
    sPWMConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;           /* 高电平有效 */
    sPWMConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;          /* 高电平有效(此摄影不使用) */
    sPWMConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;         /* OCx空闲输出状态(此设计不使用) */
    sPWMConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;        /* OCxN空闲输出状态(此设计不使用) */
    sPWMConfig.OCFastMode   = TIM_OCFAST_DISABLE;            /* 关闭快速模式 */
    sPWMConfig.Pulse = PULSE_DEFAULT;                        /* 默认占空比 */

    HAL_TIM_PWM_ConfigChannel(&htim_led, &sPWMConfig, PWM_TIM_CHANNEL);
}

void MOSPWM_Start()
{
  LL_TIM_OC_SetMode(TIM1, PWM_TIM_CHANNEL, LL_TIM_OCMODE_PWM1);
  HAL_TIM_PWM_Start(&htim_led, PWM_TIM_CHANNEL);
}

void MOSPWM_Stop()
{
  LL_TIM_OC_SetMode(TIM1, PWM_TIM_CHANNEL, LL_TIM_OCMODE_INACTIVE);
  HAL_TIM_PWM_Stop(&htim_led, PWM_TIM_CHANNEL);
}


int MOSPWM_IsEnable(int LEDx)
{
  uint32_t mode = LL_TIM_OC_GetMode(TIM1, PWM_TIM_CHANNEL);
  if(mode == LL_TIM_OCMODE_PWM1){
    return 1;
  }else{
    return 0;
  }
}

void MOSPWM_SetOutputCompare(uint16_t compare)
{
  __HAL_TIM_SET_COMPARE(&htim_led, PWM_TIM_CHANNEL, compare);
}

uint32_t MOSPWM_GetOutputCompare()
{
  return __HAL_TIM_GET_COMPARE(&htim_led, PWM_TIM_CHANNEL);
}


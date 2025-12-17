/************************************************************************
 * ADC驱动文件
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
********************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by Puya under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/*
 * 此文件部分代码来源于以下文件
 * PY32F0xx_Firmware/Projects/PY32F003-STK/Example_LL/ADC/ADC_MultiChannelSingleConversion_TriggerSW_DMA/Src/main.c
 * PY32F0xx_Firmware/Projects/PY32F003-STK/Example_LL/ADC/ADC_SingleConversion_TriggerTimer_DMA/Src/main.c
 */
#include "adc.h"
#include "py32f0xx_ll_bus.h"
#include "py32f0xx_ll_system.h"
#include "py32f0xx_ll_gpio.h"
#include "py32f0xx_ll_adc.h"
#include "py32f0xx_ll_tim.h"
#include "py32f0xx_ll_dma.h"
#include "py32f0xx_ll_utils.h"
#include "dcdc.h"

uint16_t adc_buff[ADC_BUFFSIZE];
uint16_t adc_vbus = 0;
uint16_t adc_ibus = 0;
uint16_t adc_vslr = 0;
uint16_t adc_islr = 0;

/**
  * @brief  ADC configuration function
  * @param  None
  * @retval None
  */
static void ADC_AdcConfig(void)
{
  /* Enable GPIOA clock */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

  /* Configure pin 4 as analog input */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_ADC_VBUS|LL_GPIO_PIN_ADC_IBUS|LL_GPIO_PIN_ADC_VSLR|LL_GPIO_PIN_ADC_ISLR, LL_GPIO_MODE_ANALOG);

  /* ADC channel and clock source should be configured when ADEN=0, others should be configured when ADSTART=0 */
  /* Configure internal conversion channel */
  LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_NONE);

  /* Set ADC clock */
  LL_ADC_SetClock(ADC1, LL_ADC_CLOCK_SYNC_PCLK_DIV2);

  /* Set 12-bit resolution */
  LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);

  /* Right-alignment for converted data */
  LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);

  /* Set low power mode to none */
  LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_MODE_NONE);

  /* Set channel conversion time */
  LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_7CYCLES_5);

  /* Set the trigger source as TIM1 TRGO */
  LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_EXT_TIM1_TRGO);

  /* Set trigger edge as rising edge */
  LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);

  /* Single sampling */
  LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);

  /* Set DMA mode to circular mode and enable it */
  LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);

  /* Set overrun management mode to data overwritten */
  LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

  /* Set scan direction to forward */
  LL_ADC_REG_SetSequencerScanDirection(ADC1,LL_ADC_REG_SEQ_SCAN_DIR_FORWARD);

  /* Set discontinuous mode to disabled */
  LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_DISABLE);

  /* Set conversion channel */
  LL_ADC_REG_SetSequencerChannels(ADC1,
				  LL_ADC_CHANNEL_VBUS |
				  LL_ADC_CHANNEL_IBUS |
				  LL_ADC_CHANNEL_VSLR |
				  LL_ADC_CHANNEL_ISLR);
}

/**
  * @brief  ADC calibration
  * @param  None
  * @retval None
  */
static void ADC_AdcCalibrate(void)
{
  __IO uint32_t backup_setting_adc_dma_transfer = 0;
#if (USE_TIMEOUT == 1)
  uint32_t Timeout = 0;
#endif

  if (LL_ADC_IsEnabled(ADC1) == 0)
  {
    /* Disable ADC DMA transfer during calibration */
    backup_setting_adc_dma_transfer = LL_ADC_REG_GetDMATransfer(ADC1);
    LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_NONE);
    /* Enable calibration */
    LL_ADC_StartCalibration(ADC1);

#if (USE_TIMEOUT == 1)
    Timeout = ADC_CALIBRATION_TIMEOUT_MS;
#endif

    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
    {
#if (USE_TIMEOUT == 1)
      /* Check if calibration is timeout */
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        if(Timeout-- == 0)
        {

        }
      }
#endif
    }

    /* Delay between ADC calibration end and ADC enable: minimum 4 ADC Clock cycles */
    LL_mDelay(1);

    /* Restore ADC DMA configuration */
    LL_ADC_REG_SetDMATransfer(ADC1, backup_setting_adc_dma_transfer);
  }
}

/**
  * @brief  ADC enable function
  * @param  None
  * @retval None
  */
static void ADC_AdcEnable(void)
{
  /* Enable ADC */
  LL_ADC_Enable(ADC1);

  /* ADC stabilization time, minimum 8 ADC Clock cycles */
  LL_mDelay(1);
}

static void ADC_TimerInit(void)
{
  //已在其他代码中开启TIM1定时器
  LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_UPDATE);
}

/**
  * @brief  DMA configuration function
  * @param  None
  * @retval None
  */
static void ADC_DmaConfig(void)
{
  /* Enable DMA1 clock */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

  /* Enable syscfg clock */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);

  /* ADC corresponds to channel LL_DMA_CHANNEL_3 */
  LL_SYSCFG_SetDMARemap_CH3(LL_SYSCFG_DMA_MAP_ADC);

  /* Configure DMA data transfer direction as peripheral to memory */
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_ADC, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  /* Configure DMA priority as high */
  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_ADC, LL_DMA_PRIORITY_HIGH);

  /* Configure DMA in circular mode */
  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_ADC, LL_DMA_MODE_CIRCULAR);

  /* Configure DMA peripheral increment mode as no increment */
  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_ADC, LL_DMA_PERIPH_NOINCREMENT);

  /* Configure DMA memory increment mode as increment */
  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_ADC, LL_DMA_MEMORY_INCREMENT);

  /* Configure DMA peripheral data size as word */
  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_ADC, LL_DMA_PDATAALIGN_HALFWORD);

  /* Configure DMA memory data size as word */
  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_ADC, LL_DMA_MDATAALIGN_HALFWORD);

  /* Configure DMA transfer length */
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_ADC, ADC_BUFFSIZE);

  /* Configure DMA peripheral and memory addresses */
  LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_ADC, (uint32_t)&ADC1->DR, (uint32_t)&adc_buff, LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_ADC));

  /* Enable DMA transfer complete interrupt */
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_ADC);

  /* Enable DMA half transfer interrupt */
  LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_ADC);

  //已经在其他代码中开启此中断
  //NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0);
  //NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

  /* DMA interrupt configuration */
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_ADC);
}

void ADC_Init()
{
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);
  ADC_AdcCalibrate();
  ADC_DmaConfig();
  ADC_AdcConfig();
  ADC_AdcEnable();
  ADC_TimerInit();
  LL_ADC_REG_StartConversion(ADC1);
}

void ADC_DMA_TC_Callback()
{
  DCDC_ADC_update_callback((ADCSamp_t *)(&adc_buff[4]));
}

void ADC_DMA_HT_Callback()
{
  DCDC_ADC_update_callback((ADCSamp_t *)(&adc_buff[0]));
}

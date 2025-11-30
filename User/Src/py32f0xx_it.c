/**
  ******************************************************************************
  * @file    py32f0xx_it.c
  * @author  MCU Application Team
  * @brief   Interrupt Service Routines.
  ******************************************************************************
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "py32f0xx_it.h"
#include "py32f0xx_ll_usart.h"
#include "py32f0xx_ll_dma.h"
#include "py32f0xx_ll_gpio.h"
#include "py32f0xx_ll_exti.h"
#include "rs485.h"
#include "adc.h"

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/* PY32F0xx Peripheral Interrupt Handlers                                     */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file.                                          */
/******************************************************************************/
/*void USART1_IRQHandler(void)
{
}*/

void DMA1_Channel1_IRQHandler(void)
{
  if(LL_DMA_IsActiveFlag_TC1(DMA1)){
    LL_DMA_ClearFlag_TC1(DMA1);
    USART_TX_DMA_TC_Callback();
  }
}

void DMA1_Channel2_3_IRQHandler(void)
{
  if(LL_DMA_IsActiveFlag_TC2(DMA1)){
    LL_DMA_ClearFlag_TC2(DMA1);
    USART_RX_DMA_TC_Callback();
  }
  if(LL_DMA_IsActiveFlag_TC3(DMA1)){
    LL_DMA_ClearFlag_TC3(DMA1);
    ADC_DMA_TC_Callback();
  }
  if(LL_DMA_IsActiveFlag_HT3(DMA1)){
    LL_DMA_ClearFlag_HT3(DMA1);
    ADC_DMA_HT_Callback();
  }
}

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/

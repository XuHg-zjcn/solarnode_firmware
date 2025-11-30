/************************************************************************
 * RS485驱动文件
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
#include "rs485.h"
#include "py32f0xx_ll_system.h"
#include "py32f0xx_ll_bus.h"
#include "py32f0xx_ll_gpio.h"
#include "py32f0xx_ll_usart.h"
#include "py32f0xx_ll_dma.h"

#define TR_LL_GPIO_PIN    LL_GPIO_PIN_5
#define TR_GPIO_PORT      GPIOB

#define RXD_LL_GPIO_PIN   LL_GPIO_PIN_7
#define RXD_GPIO_PORT     GPIOB
#define RXD_LL_GPIO_AF    LL_GPIO_AF_0

#define TXD_LL_GPIO_PIN   LL_GPIO_PIN_14
#define TXD_GPIO_PORT     GPIOA
#define TXD_LL_GPIO_AF    LL_GPIO_AF_1
#define USARTx            USART1
#define BAUDRATE          (500000)

#define LL_DMA_CHANNEL_TX LL_DMA_CHANNEL_1
#define LL_DMA_CHANNEL_RX LL_DMA_CHANNEL_2

RS485_StatusType rs485_stat = RS485_On_IdleORMute;
uint8_t buff_rx[64];     //接收缓存区
volatile uint32_t buff_rxlen = 0; //接收完成时存放接收长度,缓存区内容不再使用时置0

/******************************************
 * 参考代码:
 * PY32F0xx_Firmware/Projects/PY32F003-STK/Example_LL/USART/USART_HyperTerminal_DMA_Init/Src/main.c
 * STM32CubeF0/Projects/STM32F072RB-Nucleo/Examples_LL/USART/USART_Communication_TxRx_DMA/Src/main.c
 ******************************************/

void RS485_Init()
{
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);

  LL_GPIO_ResetOutputPin(TR_GPIO_PORT, TR_LL_GPIO_PIN); //先设置电平，防止毛刺
  LL_GPIO_SetPinMode(TR_GPIO_PORT, TR_LL_GPIO_PIN, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinSpeed(TR_GPIO_PORT, TR_LL_GPIO_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(TR_GPIO_PORT, TR_LL_GPIO_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(TR_GPIO_PORT, TR_LL_GPIO_PIN, LL_GPIO_PULL_NO);

  LL_GPIO_SetPinMode(RXD_GPIO_PORT, RXD_LL_GPIO_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetAFPin_0_7(RXD_GPIO_PORT, RXD_LL_GPIO_PIN, RXD_LL_GPIO_AF);
  LL_GPIO_SetPinSpeed(RXD_GPIO_PORT, RXD_LL_GPIO_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(RXD_GPIO_PORT, RXD_LL_GPIO_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(RXD_GPIO_PORT, RXD_LL_GPIO_PIN, LL_GPIO_PULL_UP);

  LL_GPIO_SetPinMode(TXD_GPIO_PORT, TXD_LL_GPIO_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetAFPin_8_15(TXD_GPIO_PORT, TXD_LL_GPIO_PIN, TXD_LL_GPIO_AF);
  LL_GPIO_SetPinSpeed(TXD_GPIO_PORT, TXD_LL_GPIO_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(TXD_GPIO_PORT, TXD_LL_GPIO_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(TXD_GPIO_PORT, TXD_LL_GPIO_PIN, LL_GPIO_PULL_UP);

  //配置USART
  //设置1位停止位时示波器看TXD引脚会有2个周期的高电平，设置2位则有3个周期
  LL_USART_SetTransferDirection(USARTx, LL_USART_DIRECTION_TX_RX);
  LL_USART_ConfigCharacter(USARTx, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);
  LL_USART_SetHWFlowCtrl(USARTx, LL_USART_HWCONTROL_NONE);
  LL_USART_SetOverSampling(USARTx, LL_USART_OVERSAMPLING_16);
  //LL_USART_SetBaudRate(USARTx, 8000000, 115200, LL_USART_OVERSAMPLING_16); //该函数计算波特率有误
  USARTx->BRR = (SystemCoreClock + BAUDRATE/2)/BAUDRATE;
  LL_USART_ConfigAsyncMode(USARTx);
  LL_USART_EnableIT_RXNE(USARTx);
  LL_USART_Enable(USARTx);

  //配置DMA通道1, 用于USART发送
  LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_TX,
			LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
			LL_DMA_PRIORITY_HIGH              |
			LL_DMA_MODE_NORMAL                |
			LL_DMA_PERIPH_NOINCREMENT         |
			LL_DMA_MEMORY_INCREMENT           |
			LL_DMA_PDATAALIGN_BYTE            |
			LL_DMA_MDATAALIGN_BYTE);

  //配置DMA通道2, 用于USART接收
  LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_RX,
			LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
			LL_DMA_PRIORITY_MEDIUM            |
			LL_DMA_MODE_NORMAL                |
			LL_DMA_PERIPH_NOINCREMENT         |
			LL_DMA_MEMORY_INCREMENT           |
			LL_DMA_PDATAALIGN_BYTE            |
			LL_DMA_MDATAALIGN_BYTE);
  LL_SYSCFG_SetDMARemap_CH1(LL_SYSCFG_DMA_MAP_USART1_TX);
  LL_SYSCFG_SetDMARemap_CH2(LL_SYSCFG_DMA_MAP_USART1_RX);

  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_TX, LL_USART_DMA_GetRegAddr(USARTx));
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_TX);

  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_RX, (uint32_t)buff_rx);
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_RX, LL_USART_DMA_GetRegAddr(USARTx));
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_RX, sizeof(buff_rx));
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_RX);

  NVIC_SetPriority(DMA1_Channel1_IRQn, 1);
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);

  NVIC_SetPriority(DMA1_Channel2_3_IRQn, 1);
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

  NVIC_SetPriority(USART1_IRQn, 0);
  NVIC_EnableIRQ(USART1_IRQn);
}

int RS485_Send(uint8_t *p, uint16_t size)
{
  if(rs485_stat != RS485_On_IdleORMute){
    return 1;
  }
  rs485_stat = RS485_On_Trasmit;
  LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_TX);
  LL_DMA_ClearFlag_GI1(DMA1);
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_TX, (uint32_t)p);
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_TX, LL_USART_DMA_GetRegAddr(USARTx));
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_TX, size);
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_TX);

  LL_USART_ClearFlag_TC(USARTx);
  LL_GPIO_SetOutputPin(TR_GPIO_PORT, TR_LL_GPIO_PIN);
  LL_USART_EnableDMAReq_TX(USARTx);
  //完成发送后在`DMA1_Channel1_IRQHandler`拉低TR_GPIO
  return 0;
}

//TODO: 此处应该改为回调函数，中断函数在py32f0xx_it.c中定义
void USART1_IRQHandler()
{
  if(LL_USART_IsActiveFlag_RXNE(USARTx)){
    uint8_t byte = LL_USART_ReceiveData8(USARTx);
    if((rs485_stat == RS485_On_IdleORMute) && (byte == RS485_ADDR) && (buff_rxlen == 0)){
      rs485_stat = RS485_On_Recevice;
      LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_RX);
      LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_RX, sizeof(buff_rx));
      LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_RX);
      LL_USART_EnableDMAReq_RX(USARTx);
      LL_USART_DisableIT_RXNE(USARTx);
      LL_USART_EnableIT_IDLE(USARTx);
    }else{
      LL_USART_RequestEnterMuteMode(USARTx);
    }
  }
  if(LL_USART_IsActiveFlag_IDLE(USARTx)){
    uint8_t byte = LL_USART_ReceiveData8(USARTx); //清除IDLE位
    LL_USART_DisableDMAReq_RX(USARTx);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_RX);
    buff_rxlen = sizeof(buff_rx) - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_RX);
    rs485_stat = RS485_On_IdleORMute;
    LL_USART_EnableIT_RXNE(USARTx);
  }
}

void USART_TX_DMA_TC_Callback()
{
  LL_USART_ClearFlag_TC(USARTx);
  int count = 200;
  while(!LL_USART_IsActiveFlag_TC(USARTx) && count--);
  LL_GPIO_ResetOutputPin(TR_GPIO_PORT, TR_LL_GPIO_PIN);

  LL_USART_DisableDMAReq_TX(USARTx);
  rs485_stat = RS485_On_IdleORMute;
}

void USART_RX_DMA_TC_Callback()
{
  //接收缓存区已满
  LL_USART_DisableDMAReq_RX(USARTx);
  LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_RX);
  buff_rxlen = sizeof(buff_rx);
  rs485_stat = RS485_On_IdleORMute;
  LL_USART_RequestEnterMuteMode(USARTx);
  LL_USART_EnableIT_RXNE(USARTx);
}

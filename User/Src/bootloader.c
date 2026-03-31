#include "bootloader.h"
#include <stdint.h>
#include "py32f0xx_ll_rcc.h"
#include "py32f0xx_ll_bus.h"

#define ADDR_BL (0x0800f000)

void GoBootloader()
{
  __disable_irq();
  LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_ALL);
  LL_AHB1_GRP1_ReleaseReset(LL_AHB1_GRP1_PERIPH_ALL);
  LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_ALL);
  LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_ALL);
  LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_ALL);
  LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_ALL);
  LL_IOP_GRP1_ForceReset(LL_IOP_GRP1_PERIPH_ALL);
  LL_IOP_GRP1_ReleaseReset(LL_IOP_GRP1_PERIPH_ALL);
  LL_RCC_WriteReg(AHBENR, RCC_AHBENR_FLASHEN|RCC_AHBENR_SRAMEN);
  LL_RCC_WriteReg(APBENR1, 0);
  LL_RCC_WriteReg(APBENR2, 0);
  LL_RCC_WriteReg(IOPENR, 0);
  SysTick->CTRL = 0;
  NVIC->ICER[0] = 0xffffffff;
  NVIC->ICPR[0] = 0xffffffff;
  SCB->VTOR = ADDR_BL;
  uint32_t new_sp = *((uint32_t *)(ADDR_BL));
  uint32_t new_pc = *((uint32_t *)(ADDR_BL+4));
  __enable_irq();
  __set_MSP(new_sp);
  ((void (*)(void))(new_pc))();
}

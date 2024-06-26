/*
* STM32 HID Bootloader - USB HID bootloader for STM32F10X
* Copyright (c) 2018 Bruno Freitas - bruno@brunofreitas.com
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
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stm32f1xx.h>
#include <stm32f1xx_ll_system.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_utils.h>

#include "usb.h"
#include "hid.h"
#include "bitwise.h"
#include "config.h"
#include "boot.h"

typedef void (*funct_ptr)(void);

int want_bootloader(void) {
	return checkAndClearBootloaderFlag() || checkUserCode() || checkKbMatrix();
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
	/* Set FLASH latency */
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

	/* Enable HSE oscillator */
	LL_RCC_HSE_Enable();
	while(LL_RCC_HSE_IsReady() != 1)
	{
	};

	/* Main PLL configuration and activation */
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);

	LL_RCC_PLL_Enable();
	while(LL_RCC_PLL_IsReady() != 1)
	{
	};

	/* Sysclk activation on the main PLL */
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
	{
	};

	/* Set APB1 & APB2 prescaler*/
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

	/* Set systick to 1ms in using frequency set to 72MHz */
	LL_Init1msTick(72000000);

	/* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
	LL_SetSystemCoreClock(72000000);
}
#define SET_REG(addr,val) do { *(volatile uint32_t*)(addr)=val; } while(0)
void setMspAndJump(uint32_t usrAddr) {
    // Dedicated function with no call to any function (appart the last call)
    // This way, there is no manipulation of the stack here, ensuring that GGC
    // didn't insert any pop from the SP after having set the MSP.
    typedef void (*funcPtr)(void);
    uint32_t jumpAddr = *(volatile uint32_t *)(usrAddr + 0x04); /* reset ptr in vector table */

    funcPtr usrMain = (funcPtr) jumpAddr;

    SET_REG(SCB->VTOR, (volatile uint32_t) (usrAddr));

    asm volatile("msr msp, %0"::"g"(*(volatile uint32_t *)usrAddr));
	
    usrMain();                                /* go! */
}
int main() {
	// uint32_t usrSp = ( uint32_t )USER_PROGRAM;
	// uint32_t usrMain = ( uint32_t )(USER_PROGRAM + 0x04); /* reset ptr in vector table */
	NVIC_SystemReset();
	SystemClock_Config();

	setupGPIO();

	if(want_bootloader()) {
		USB_Init(HIDUSB_EPHandler, HIDUSB_Reset);
	} else {
			NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
		   HIDUSB_FlashLock();
   		//USB_Shutdown();
    	//NVIC_DisableIRQ();


//	NVIC_SystemReset();
	//	SystemInit();
//	NVIC_SETFAULTMASK();
		setMspAndJump(USER_PROGRAM);
	// 	SCB->VTOR = USER_PROGRAM;

	// // 	__asm__ volatile(
	// // 		"msr msp, %0\n"
	// // 		"bx %1\n"
	// // 		:: "r" (usrSp), "r" (usrMain));
	// // }
	
	}
}

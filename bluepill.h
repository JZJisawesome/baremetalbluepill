/***Copyright 2021 John Jekel
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
*/

/* Useful library for the blue pill devboard */
//Provides things implemented by bluepill.S, MMIO pointers/registers, datatypes and more

#ifndef BLUEPILL_H
#define BLUEPILL_H

/* Useful C Definitions */
//TODO ensure this is correct
//Signed
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

typedef signed int int_fast8_t;
typedef signed int int_fast16_t;
typedef signed int int_fast32_t;
typedef signed long long int_fast64_t;

typedef signed char int_least8_t;
typedef signed short int_least6_t;
typedef signed int int_least2_t;
typedef signed long long int_least64_t;

typedef signed long long intmax_t;

typedef signed int* intptr_t;

//Unsigned
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef unsigned int uint_fast8_t;
typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;
typedef unsigned long long uint_fast64_t;

typedef unsigned char uint_least8_t;
typedef unsigned short uint_least6_t;
typedef unsigned int uint_least2_t;
typedef unsigned long long uint_least64_t;

typedef unsigned long long uintmax_t;

typedef unsigned int* uintptr_t;

#define bool _Bool
#define true 1
#define false 0
#define __bool_true_false_are_defined 1

#ifdef NDEBUG
    #define assert(condition) ((void)0)
#else//Spin forever if assertion fails (allows debugger to be used)
    #define assert(condition) {if (condition == 0) __hang();}
#endif

/* Useful functions */
extern void __delayInstructions(uint32_t numberOfInstructions);

/* Useful macros */

#define __hang() {while(true);}
#define __reset() {SCB_AIRCR = 0x05FA0004;}//Todo make this an actual function (in assembly)

/* Intrinsics */
#define __bkpt(imm_) {__asm__ __volatile__("bkpt "#imm_);}
#define __dmb {__asm__ __volatile__("dmb");}
#define __dsb {__asm__ __volatile__("dsb");}
#define __isb {__asm__ __volatile__("isb");}
#define __nop() {__asm__ __volatile__("nop");}
#define __sev() {__asm__ __volatile__("sev");}
#define __svc(imm_) {__asm__ __volatile__("svc "#imm_);}
#define __wfe() {__asm__ __volatile__("wfe");}
#define __wfi() {__asm__ __volatile__("wfi");}


/**********************************************************************************************/
/* MMIO Registers and other useful pointers */
//Generally in order from low to high addresses

//Pointers To Storage (Flash, SRAM, USB/CAN SRAM, etc.)
#define FLASH_BASE (*(volatile uint32_t*)(0x08000000))//128k of flash on most clones
#define SYSTEM_MEMORY_BASE (*(volatile uint32_t*)(0x1FFFF000))//Contains 2k stm32 bootloader rom
#define SRAM_BASE (*(volatile uint32_t*)(0x20000000))//20k of sram
#define SHARED_SRAM_BASE (*(volatile uint32_t*)(0x40006000))//512b sram shared with usb/can

//Device Info Registers (Note: not actually registers; part of the stm32 bootloader rom)
#define F_SIZE (*(volatile uint16_t*)(0x1FFFF7E0))
#define UDID_LOW (*(volatile uint32_t*)(0x1FFFF7E8))
#define UDID_MID (*(volatile uint32_t*)(0x1FFFF7EC))
#define UDID_HIGH (*(volatile uint32_t*)(0x1FFFF7F0))

//Alternate IO Function Config Registers
#define AFIO_EVCR (*(volatile uint32_t*)(0x40010000))
#define AFIO_MAPR (*(volatile uint32_t*)(0x40010004))
#define AFIO_EXTICR1 (*(volatile uint32_t*)(0x40010008))
#define AFIO_EXTICR2 (*(volatile uint32_t*)(0x4001000C))
#define AFIO_EXTICR3 (*(volatile uint32_t*)(0x40010010))
#define AFIO_EXTICR4 (*(volatile uint32_t*)(0x40010014))
#define AFIO_MAPR2 (*(volatile uint32_t*)(0x4001001C))

//External Interrupt Config Registers
#define EXTI_IMR (*(volatile uint32_t*)(0x40010400))
#define EXTI_EMR (*(volatile uint32_t*)(0x40010404))
#define EXTI_RTSR (*(volatile uint32_t*)(0x40010408))
#define EXTI_FTSR (*(volatile uint32_t*)(0x4001040C))
#define EXTI_SWIER (*(volatile uint32_t*)(0x40010410))
#define EXTI_PR (*(volatile uint32_t*)(0x40010414))

//GPIO
#define GPIOA_CRL (*(volatile uint32_t*)(0x40010800))
#define GPIOB_CRL (*(volatile uint32_t*)(0x40010C00))
#define GPIOC_CRL (*(volatile uint32_t*)(0x40011000))

#define GPIOA_CRH (*(volatile uint32_t*)(0x40010804))
#define GPIOB_CRH (*(volatile uint32_t*)(0x40010C04))
#define GPIOC_CRH (*(volatile uint32_t*)(0x40011004))

#define GPIOA_IDR (*(volatile uint32_t*)(0x40010808))
#define GPIOB_IDR (*(volatile uint32_t*)(0x40010C08))
#define GPIOC_IDR (*(volatile uint32_t*)(0x40011008))

#define GPIOA_ODR (*(volatile uint32_t*)(0x4001080C))
#define GPIOB_ODR (*(volatile uint32_t*)(0x40010C0C))
#define GPIOC_ODR (*(volatile uint32_t*)(0x4001100C))

#define GPIOA_BSRR (*(volatile uint32_t*)(0x40010810))
#define GPIOB_BSRR (*(volatile uint32_t*)(0x40010C10))
#define GPIOC_BSRR (*(volatile uint32_t*)(0x40011010))

#define GPIOA_BRR (*(volatile uint32_t*)(0x40010814))
#define GPIOB_BRR (*(volatile uint32_t*)(0x40010C14))
#define GPIOC_BRR (*(volatile uint32_t*)(0x40011014))

#define GPIOA_LCKR (*(volatile uint32_t*)(0x40010818))
#define GPIOB_LCKR (*(volatile uint32_t*)(0x40010C18))
#define GPIOC_LCKR (*(volatile uint32_t*)(0x40011018))

//Timers
#define TIM1_CR1 (*(volatile uint32_t*)(0x40012C00))
#define TIM2_CR1 (*(volatile uint32_t*)(0x40000000))
#define TIM3_CR1 (*(volatile uint32_t*)(0x40000400))
#define TIM4_CR1 (*(volatile uint32_t*)(0x40000800))

#define TIM1_CR2 (*(volatile uint32_t*)(0x40012C04))
#define TIM2_CR2 (*(volatile uint32_t*)(0x40000004))
#define TIM3_CR2 (*(volatile uint32_t*)(0x40000404))
#define TIM4_CR2 (*(volatile uint32_t*)(0x40000804))

#define TIM1_SMCR (*(volatile uint32_t*)(0x40012C08))
#define TIM2_SMCR (*(volatile uint32_t*)(0x40000008))
#define TIM3_SMCR (*(volatile uint32_t*)(0x40000408))
#define TIM4_SMCR (*(volatile uint32_t*)(0x40000808))

#define TIM1_DIER (*(volatile uint32_t*)(0x40012C0c))
#define TIM2_DIER (*(volatile uint32_t*)(0x4000000c))
#define TIM3_DIER (*(volatile uint32_t*)(0x4000040c))
#define TIM4_DIER (*(volatile uint32_t*)(0x4000080c))

#define TIM1_SR (*(volatile uint32_t*)(0x40012C10))
#define TIM2_SR (*(volatile uint32_t*)(0x40000010))
#define TIM3_SR (*(volatile uint32_t*)(0x40000410))
#define TIM4_SR (*(volatile uint32_t*)(0x40000810))

#define TIM1_EGR (*(volatile uint32_t*)(0x40012C14))
#define TIM2_EGR (*(volatile uint32_t*)(0x40000014))
#define TIM3_EGR (*(volatile uint32_t*)(0x40000414))
#define TIM4_EGR (*(volatile uint32_t*)(0x40000814))

#define TIM1_CCMR1 (*(volatile uint32_t*)(0x40012C18))
#define TIM2_CCMR1 (*(volatile uint32_t*)(0x40000018))
#define TIM3_CCMR1 (*(volatile uint32_t*)(0x40000418))
#define TIM4_CCMR1 (*(volatile uint32_t*)(0x40000818))

#define TIM1_CCMR2 (*(volatile uint32_t*)(0x40012C1C))
#define TIM2_CCMR2 (*(volatile uint32_t*)(0x4000001C))
#define TIM3_CCMR2 (*(volatile uint32_t*)(0x4000041C))
#define TIM4_CCMR2 (*(volatile uint32_t*)(0x4000081C))

#define TIM1_CCER (*(volatile uint32_t*)(0x40012C20))
#define TIM2_CCER (*(volatile uint32_t*)(0x40000020))
#define TIM3_CCER (*(volatile uint32_t*)(0x40000420))
#define TIM4_CCER (*(volatile uint32_t*)(0x40000820))

#define TIM1_CNT (*(volatile uint32_t*)(0x40012C24))
#define TIM2_CNT (*(volatile uint32_t*)(0x40000024))
#define TIM3_CNT (*(volatile uint32_t*)(0x40000424))
#define TIM4_CNT (*(volatile uint32_t*)(0x40000824))

#define TIM1_PSC (*(volatile uint32_t*)(0x40012C28))
#define TIM2_PSC (*(volatile uint32_t*)(0x40000028))
#define TIM3_PSC (*(volatile uint32_t*)(0x40000428))
#define TIM4_PSC (*(volatile uint32_t*)(0x40000828))

#define TIM1_ARR (*(volatile uint32_t*)(0x40012C2C))
#define TIM2_ARR (*(volatile uint32_t*)(0x4000002C))
#define TIM3_ARR (*(volatile uint32_t*)(0x4000042C))
#define TIM4_ARR (*(volatile uint32_t*)(0x4000082C))

#define TIM1_RCR (*(volatile uint32_t*)(0x40012C30))

#define TIM1_CCR1 (*(volatile uint32_t*)(0x40012C34))
#define TIM2_CCR1 (*(volatile uint32_t*)(0x40000034))
#define TIM3_CCR1 (*(volatile uint32_t*)(0x40000434))
#define TIM4_CCR1 (*(volatile uint32_t*)(0x40000834))

#define TIM1_CCR2 (*(volatile uint32_t*)(0x40012C38))
#define TIM2_CCR2 (*(volatile uint32_t*)(0x40000038))
#define TIM3_CCR2 (*(volatile uint32_t*)(0x40000438))
#define TIM4_CCR2 (*(volatile uint32_t*)(0x40000838))

#define TIM1_CCR3 (*(volatile uint32_t*)(0x40012C3C))
#define TIM2_CCR3 (*(volatile uint32_t*)(0x4000003C))
#define TIM3_CCR3 (*(volatile uint32_t*)(0x4000043C))
#define TIM4_CCR3 (*(volatile uint32_t*)(0x4000083C))

#define TIM1_CCR4 (*(volatile uint32_t*)(0x40012C40))
#define TIM2_CCR4 (*(volatile uint32_t*)(0x40000040))
#define TIM3_CCR4 (*(volatile uint32_t*)(0x40000440))
#define TIM4_CCR4 (*(volatile uint32_t*)(0x40000840))

#define TIM1_BDTR (*(volatile uint32_t*)(0x40012C44))

#define TIM1_DCR (*(volatile uint32_t*)(0x40012C48))
#define TIM2_DCR (*(volatile uint32_t*)(0x40000048))
#define TIM3_DCR (*(volatile uint32_t*)(0x40000448))
#define TIM4_DCR (*(volatile uint32_t*)(0x40000848))

#define TIM1_DMAR (*(volatile uint32_t*)(0x40012C4C))
#define TIM2_DMAR (*(volatile uint32_t*)(0x4000004C))
#define TIM3_DMAR (*(volatile uint32_t*)(0x4000044C))
#define TIM4_DMAR (*(volatile uint32_t*)(0x4000084C))

//DMA
#define DMA_ISR (*(volatile uint32_t*)(0x40012000))
#define DMA_IFCR (*(volatile uint32_t*)(0x40012004))

#define DMA_CCR1 (*(volatile uint32_t*)(0x40012008))
#define DMA_CNDTR1 (*(volatile uint32_t*)(0x4001200C))
#define DMA_CPAR1 (*(volatile uint32_t*)(0x40012010))
#define DMA_CMAR1 (*(volatile uint32_t*)(0x40012014))

#define DMA_CCR2 (*(volatile uint32_t*)(0x4001201C))
#define DMA_CNDTR2 (*(volatile uint32_t*)(0x40012020))
#define DMA_CPAR2 (*(volatile uint32_t*)(0x40012024))
#define DMA_CMAR2 (*(volatile uint32_t*)(0x40012028))

#define DMA_CCR3 (*(volatile uint32_t*)(0x40012030))
#define DMA_CNDTR3 (*(volatile uint32_t*)(0x40012034))
#define DMA_CPAR3 (*(volatile uint32_t*)(0x40012038))
#define DMA_CMAR3 (*(volatile uint32_t*)(0x4001203C))

#define DMA_CCR4 (*(volatile uint32_t*)(0x40012044))
#define DMA_CNDTR4 (*(volatile uint32_t*)(0x40012048))
#define DMA_CPAR4 (*(volatile uint32_t*)(0x4001204C))
#define DMA_CMAR4 (*(volatile uint32_t*)(0x40012050))

#define DMA_CCR5 (*(volatile uint32_t*)(0x40012058))
#define DMA_CNDTR5 (*(volatile uint32_t*)(0x4001205C))
#define DMA_CPAR5 (*(volatile uint32_t*)(0x40012060))
#define DMA_CMAR5 (*(volatile uint32_t*)(0x40012064))

#define DMA_CCR6 (*(volatile uint32_t*)(0x4001206C))
#define DMA_CNDTR6 (*(volatile uint32_t*)(0x40012070))
#define DMA_CPAR6 (*(volatile uint32_t*)(0x40012074))
#define DMA_CMAR6 (*(volatile uint32_t*)(0x40012078))

#define DMA_CCR7 (*(volatile uint32_t*)(0x40012080))
#define DMA_CNDTR7 (*(volatile uint32_t*)(0x40012084))
#define DMA_CPAR7 (*(volatile uint32_t*)(0x40012088))
#define DMA_CMAR7 (*(volatile uint32_t*)(0x4001208C))

//Flash Memory Interface
#define FLASH_ACR (*(volatile uint32_t*)(0x40022000))
#define FLASH_KEYR (*(volatile uint32_t*)(0x40022004))
#define FLASH_OPTKEYR (*(volatile uint32_t*)(0x40022008))
#define FLASH_SR (*(volatile uint32_t*)(0x4002200C))
#define FLASH_CR (*(volatile uint32_t*)(0x40022010))
#define FLASH_AR (*(volatile uint32_t*)(0x40022014))
#define FLASH_OBR (*(volatile uint32_t*)(0x4002201C))
#define FLASH_WRPR (*(volatile uint32_t*)(0x40022020))

//RTC
#define RTC_CRH (*(volatile uint32_t*)(0x40002800))
#define RTC_CRL (*(volatile uint32_t*)(0x40002804))
#define RTC_PRLH (*(volatile uint32_t*)(0x40002808))
#define RTC_PRLL (*(volatile uint32_t*)(0x4000280C))
#define RTC_DIVH (*(volatile uint32_t*)(0x40002810))
#define RTC_DIVL (*(volatile uint32_t*)(0x40002814))
#define RTC_CNTH (*(volatile uint32_t*)(0x40002818))
#define RTC_CNTL (*(volatile uint32_t*)(0x4000281C))
#define RTC_ALRH (*(volatile uint32_t*)(0x40002820))
#define RTC_ALRL (*(volatile uint32_t*)(0x40002824))

//CRC
#define CRC_DR (*(volatile uint32_t*)(0x40023000))
#define CRC_IDR (*(volatile uint32_t*)(0x40023004))
#define CRC_CR (*(volatile uint32_t*)(0x40023008))

//Power Control
#define PWR_CR (*(volatile uint32_t*)(0x40007000))
#define PWR_CSR (*(volatile uint32_t*)(0x40007004))

//Cortex Registers
//SysTick
#define SYST_CTRL (*(volatile uint32_t*)(0xE000E010))
#define SYST_CSR (*(volatile uint32_t*)(0xE000E010))

#define SYST_LOAD (*(volatile uint32_t*)(0xE000E014))
#define SYST_RVR (*(volatile uint32_t*)(0xE000E014))

#define SYST_VAL (*(volatile uint32_t*)(0xE000E018))
#define SYST_CVR (*(volatile uint32_t*)(0xE000E018))

#define SYST_CALIB (*(volatile uint32_t*)(0xE000E01C))

//NVIC
#define NVIC_ISER0 (*(volatile uint32_t*)(0xE000E100))
#define NVIC_ISER1 (*(volatile uint32_t*)(0xE000E104))

#define NVIC_ICER0 (*(volatile uint32_t*)(0xE000E180))
#define NVIC_ICER1 (*(volatile uint32_t*)(0xE000E184))

#define NVIC_ISPR0 (*(volatile uint32_t*)(0xE000E200))
#define NVIC_ISPR1 (*(volatile uint32_t*)(0xE000E204))

#define NVIC_ICPR0 (*(volatile uint32_t*)(0xE000E280))
#define NVIC_ICPR1 (*(volatile uint32_t*)(0xE000E284))

#define NVIC_IABR0 (*(volatile uint32_t*)(0xE000E300))
#define NVIC_IABR1 (*(volatile uint32_t*)(0xE000E304))

#define NVIC_IPR0 (*(volatile uint32_t*)(0xE000E400))
#define NVIC_IPR1 (*(volatile uint32_t*)(0xE000E404))
#define NVIC_IPR2 (*(volatile uint32_t*)(0xE000E408))
#define NVIC_IPR3 (*(volatile uint32_t*)(0xE000E40C))
#define NVIC_IPR4 (*(volatile uint32_t*)(0xE000E410))

#define NVIC_IPR5 (*(volatile uint32_t*)(0xE000E414))
#define NVIC_IPR6 (*(volatile uint32_t*)(0xE000E418))
#define NVIC_IPR7 (*(volatile uint32_t*)(0xE000E41C))
#define NVIC_IPR8 (*(volatile uint32_t*)(0xE000E420))
#define NVIC_IPR9 (*(volatile uint32_t*)(0xE000E424))
#define NVIC_IPR10 (*(volatile uint32_t*)(0xE000E428))

#define NVIC_STIR (*(volatile uint32_t*)(0xE000EF00))

//SCB (System Control Block)
#define SCB_ACTLR (*(volatile uint32_t*)(0xE000E008))
#define SCB_CPUID (*(volatile uint32_t*)(0xE000ED00))//Should be 0x411FC231; clone is 0x412FC231
#define SCB_ICSR (*(volatile uint32_t*)(0xE000ED04))
#define SCB_VTOR (*(volatile uint32_t*)(0xE000ED08))
#define SCB_AIRCR (*(volatile uint32_t*)(0xE000ED0C))
#define SCB_SCR (*(volatile uint32_t*)(0xE000ED10))
#define SCB_CCR (*(volatile uint32_t*)(0xE000ED14))
#define SCB_SHPR1 (*(volatile uint32_t*)(0xE000ED18))
#define SCB_SHPR2 (*(volatile uint32_t*)(0xE000ED1C))
#define SCB_SHPR3 (*(volatile uint32_t*)(0xE000ED20))
#define SCB_SHCRS (*(volatile uint32_t*)(0xE000ED24))
#define SCB_CFSR (*(volatile uint32_t*)(0xE000ED28))
#define SCB_HFSR (*(volatile uint32_t*)(0xE000ED2C))
#define SCB_MMAR (*(volatile uint32_t*)(0xE000ED34))
#define SCB_BFAR (*(volatile uint32_t*)(0xE000ED38))

//Debug Registers
#define DBGMCU_IDCODE  (*(volatile uint32_t*)(0xE0042000))
#define DBGMCU_CR  (*(volatile uint32_t*)(0xE0042004))

#endif//BLUEPILL_H

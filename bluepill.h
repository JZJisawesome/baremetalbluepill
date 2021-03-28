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

/* Useful C Things */
//TODO ensure these is correct
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

typedef unsigned int size_t;

#define bool _Bool
#define true 1
#define false 0
#define __bool_true_false_are_defined 1

#ifdef NDEBUG
    #define assert(condition) ((void)0)
#else//Spin forever if assertion fails (allows debugger to be used)
    #define assert(condition) do {if ((condition) == 0) __hang();} while (0)
#endif

extern int memcmp(const void* lhs, const void* rhs, size_t count);//TODO
extern void* memset(void* dest, int ch, size_t count);//TODO
extern void* memcpy(void* dest, const void* src, size_t count);//TODO
extern void* memmove(void* dest, const void* src, size_t count);//TODO

/* Useful functions */
extern void __delayInstructions(uint32_t numberOfInstructions);//21.1851ns per instruction avg

/* Useful macros */

#define __hang() do {while(true);} while (0)
#define __reset() do {SCB_AIRCR = 0x05FA0004;} while (0)//TODO fit this function inside of the vector table

/* Intrinsics */
#define __bkpt(imm_) do {__asm__ __volatile__("bkpt "#imm_);} while (0)
#define __dmb do {__asm__ __volatile__("dmb");} while (0)
#define __dsb do {__asm__ __volatile__("dsb");} while (0)
#define __isb do {__asm__ __volatile__("isb");} while (0)
#define __nop() do {__asm__ __volatile__("nop");} while (0)
#define __sev() do {__asm__ __volatile__("sev");} while (0)
#define __svc(imm_) do {__asm__ __volatile__("svc "#imm_);} while (0)
#define __wfe() do {__asm__ __volatile__("wfe");} while (0)
#define __wfi() do {__asm__ __volatile__("wfi");} while (0)


/**********************************************************************************************/
/* MMIO Registers and other useful pointers
 *
 * Generally in order from low to high addresses. However, peripherals will be grouped, and the
 * first peripheral "decides" its section's location in the file.
 * 
 * Also note that registers that can be accessed by halfwords and only have into in the least
 * significant 16 bits, will be uint16_t* instead of uint32_t*
 */
//FIXME test these registers with a variety of boards. Some don't like accessing certain registers by halfwords, only by words, while some don't care (ex. had to fix certain dma registers to be accessed by 32 bits)

//Pointers To Storage (Flash, SRAM, USB/CAN SRAM, etc.)
#define FLASH_BASE (*(volatile uint32_t*)(0x08000000))//128k of flash on most clones
#define SYSTEM_MEMORY_BASE (*(volatile uint32_t*)(0x1FFFF000))//Contains 2k stm32 bootloader rom
#define SRAM_BASE (*(volatile uint32_t*)(0x20000000))//20k of sram
#define SHARED_SRAM_BASE (*(volatile uint32_t*)(0x40006000))//512b sram shared with usb/can

//Device Info Registers (Note: not actually registers; part of the stm32 bootloader rom)
#define F_SIZE (*(volatile uint16_t*)(0x1FFFF7E0))//Just 16 bits
#define UDID_BASE (*(volatile uint32_t*)(0x1FFFF7E8))//96 bits
#define UDID_LOW_W (*(volatile uint32_t*)(0x1FFFF7E8))//Low word
#define UDID_MID_W (*(volatile uint32_t*)(0x1FFFF7EC))//Middle word
#define UDID_HIGH_W (*(volatile uint32_t*)(0x1FFFF7F0))//High word

//RTC (Base: 0x40002800)
#define RTC_CRH (*(volatile uint16_t*)(0x40002800))
#define RTC_CRL (*(volatile uint16_t*)(0x40002804))
#define RTC_PRLH (*(volatile uint16_t*)(0x40002808))
#define RTC_PRLL (*(volatile uint16_t*)(0x4000280C))
#define RTC_DIVH (*(volatile uint16_t*)(0x40002810))
#define RTC_DIVL (*(volatile uint16_t*)(0x40002814))
#define RTC_CNTH (*(volatile uint16_t*)(0x40002818))
#define RTC_CNTL (*(volatile uint16_t*)(0x4000281C))
#define RTC_ALRH (*(volatile uint16_t*)(0x40002820))
#define RTC_ALRL (*(volatile uint16_t*)(0x40002824))

//Window Watchdog Registers (Base: 0x40002C00)
#define WWDG_CR (*(volatile uint16_t*)(0x40002C00))
#define WWDG_CFR (*(volatile uint16_t*)(0x40002C04))
#define WWDG_SR (*(volatile uint16_t*)(0x40002C08))

//Independent Watchdog Registers (Base: 0x40003000)
#define IWDG_KR (*(volatile uint16_t*)(0x40003000))
#define IWDG_PR (*(volatile uint16_t*)(0x40003004))
#define IWDG_RLR (*(volatile uint16_t*)(0x40003008))
#define IWDG_SR (*(volatile uint16_t*)(0x4000300C))

//I2C (Bases: 1:0x40005400 2:0x40005800)
#define I2C1_CR1 (*(volatile uint16_t*)(0x40005400))
#define I2C2_CR1 (*(volatile uint16_t*)(0x40005800))

#define I2C1_CR2 (*(volatile uint16_t*)(0x40005404))
#define I2C2_CR2 (*(volatile uint16_t*)(0x40005804))

#define I2C1_OAR1 (*(volatile uint16_t*)(0x40005408))
#define I2C2_OAR1 (*(volatile uint16_t*)(0x40005808))

#define I2C1_OAR2 (*(volatile uint16_t*)(0x4000540C))
#define I2C2_OAR2 (*(volatile uint16_t*)(0x4000580C))

#define I2C1_DR (*(volatile uint16_t*)(0x40005410))
#define I2C2_DR (*(volatile uint16_t*)(0x40005810))

#define I2C1_SR1 (*(volatile uint16_t*)(0x40005414))
#define I2C2_SR1 (*(volatile uint16_t*)(0x40005814))

#define I2C1_SR2 (*(volatile uint16_t*)(0x40005418))
#define I2C2_SR2 (*(volatile uint16_t*)(0x40005818))

#define I2C1_CCR (*(volatile uint16_t*)(0x4000541C))
#define I2C2_CCR (*(volatile uint16_t*)(0x4000581C))

#define I2C1_TRISE (*(volatile uint16_t*)(0x40005420))
#define I2C2_TRISE (*(volatile uint16_t*)(0x40005820))

//USB (Base: 0x40005C00)
#define USB_EP0R (*(volatile uint16_t*)(0x40005C00))
#define USB_EP1R (*(volatile uint16_t*)(0x40005C04))
#define USB_EP2R (*(volatile uint16_t*)(0x40005C08))
#define USB_EP3R (*(volatile uint16_t*)(0x40005C0C))
#define USB_EP4R (*(volatile uint16_t*)(0x40005C10))
#define USB_EP5R (*(volatile uint16_t*)(0x40005C14))
#define USB_EP6R (*(volatile uint16_t*)(0x40005C18))
#define USB_EP7R (*(volatile uint16_t*)(0x40005C1C))

#define USB_CNTR (*(volatile uint16_t*)(0x40005C40))
#define USB_ISTR (*(volatile uint16_t*)(0x40005C44))
#define USB_FNR (*(volatile uint16_t*)(0x40005C48))
#define USB_DADDR (*(volatile uint16_t*)(0x40005C4C))
#define USB_BTABLE (*(volatile uint16_t*)(0x40005C50))

//CAN (Base: 0x40006400)
#define CAN_MCR (*(volatile uint32_t*)(0x40006400))
#define CAN_MSR (*(volatile uint32_t*)(0x40006404))
#define CAN_TSR (*(volatile uint32_t*)(0x40006408))
#define CAN_RF0R (*(volatile uint32_t*)(0x4000640C))
#define CAN_RF1R (*(volatile uint32_t*)(0x40006410))
#define CAN_IER (*(volatile uint32_t*)(0x40006414))
#define CAN_ESR (*(volatile uint32_t*)(0x40006418))
#define CAN_BTR (*(volatile uint32_t*)(0x4000641C))

#define CAN_TI0R (*(volatile uint32_t*)(0x40006580))
#define CAN_TDT0R (*(volatile uint32_t*)(0x40006584))
#define CAN_TDL0R (*(volatile uint32_t*)(0x40006588))
#define CAN_TDH0R (*(volatile uint32_t*)(0x4000658C))
#define CAN_TI1R (*(volatile uint32_t*)(0x40006590))
#define CAN_TDT1R (*(volatile uint32_t*)(0x40006594))
#define CAN_TDL1R (*(volatile uint32_t*)(0x40006598))
#define CAN_TDH1R (*(volatile uint32_t*)(0x4000659C))
#define CAN_TI2R (*(volatile uint32_t*)(0x400065A0))
#define CAN_TDT2R (*(volatile uint32_t*)(0x400065A4))
#define CAN_TDL2R (*(volatile uint32_t*)(0x400065A8))
#define CAN_TDH2R (*(volatile uint32_t*)(0x400065AC))

#define CAN_RI0R (*(volatile uint32_t*)(0x400065B0))
#define CAN_RDT0R (*(volatile uint32_t*)(0x400065B4))
#define CAN_RDL0R (*(volatile uint32_t*)(0x400065B8))
#define CAN_RDH0R (*(volatile uint32_t*)(0x400065BC))
#define CAN_RI1R (*(volatile uint32_t*)(0x400065C0))
#define CAN_RDT1R (*(volatile uint32_t*)(0x400065C4))
#define CAN_RDL1R (*(volatile uint32_t*)(0x400065C8))
#define CAN_RDH1R (*(volatile uint32_t*)(0x400065CC))

#define CAN_FMR (*(volatile uint32_t*)(0x40006600))
#define CAN_FM1R (*(volatile uint32_t*)(0x40006604))
#define CAN_FS1R (*(volatile uint32_t*)(0x4000660C))
#define CAN_FFA1R (*(volatile uint32_t*)(0x40006614))
#define CAN_FA1R (*(volatile uint32_t*)(0x4000661C))

#define CAN_F0R1 (*(volatile uint32_t*)(0x40006640))
#define CAN_F0R2 (*(volatile uint32_t*)(0x40006644))

#define CAN_F1R1 (*(volatile uint32_t*)(0x40006648))
#define CAN_F1R2 (*(volatile uint32_t*)(0x4000664C))

#define CAN_F2R1 (*(volatile uint32_t*)(0x40006650))
#define CAN_F2R2 (*(volatile uint32_t*)(0x40006654))

#define CAN_F3R1 (*(volatile uint32_t*)(0x40006658))
#define CAN_F3R2 (*(volatile uint32_t*)(0x4000665C))

#define CAN_F4R1 (*(volatile uint32_t*)(0x40006660))
#define CAN_F4R2 (*(volatile uint32_t*)(0x40006664))

#define CAN_F5R1 (*(volatile uint32_t*)(0x40006668))
#define CAN_F5R2 (*(volatile uint32_t*)(0x4000666C))

#define CAN_F6R1 (*(volatile uint32_t*)(0x40006670))
#define CAN_F6R2 (*(volatile uint32_t*)(0x40006674))

#define CAN_F7R1 (*(volatile uint32_t*)(0x40006678))
#define CAN_F7R2 (*(volatile uint32_t*)(0x4000667C))

#define CAN_F8R1 (*(volatile uint32_t*)(0x40006680))
#define CAN_F8R2 (*(volatile uint32_t*)(0x40006684))

#define CAN_F9R1 (*(volatile uint32_t*)(0x40006688))
#define CAN_F9R2 (*(volatile uint32_t*)(0x4000668C))

#define CAN_F10R1 (*(volatile uint32_t*)(0x40006690))
#define CAN_F10R2 (*(volatile uint32_t*)(0x40006694))

#define CAN_F11R1 (*(volatile uint32_t*)(0x40006698))
#define CAN_F11R2 (*(volatile uint32_t*)(0x4000669C))

#define CAN_F12R1 (*(volatile uint32_t*)(0x400066A0))
#define CAN_F12R2 (*(volatile uint32_t*)(0x400066A4))

#define CAN_F13R1 (*(volatile uint32_t*)(0x400066A8))
#define CAN_F13R2 (*(volatile uint32_t*)(0x400066AC))

//Backup Registers (Base: 0x40006C00) (Note: This address space contains registers for the RTC)
#define BKP_DR1 (*(volatile uint16_t*)(0x40006C04))
#define BKP_DR2 (*(volatile uint16_t*)(0x40006C08))
#define BKP_DR3 (*(volatile uint16_t*)(0x40006C0C))
#define BKP_DR4 (*(volatile uint16_t*)(0x40006C10))
#define BKP_DR5 (*(volatile uint16_t*)(0x40006C14))
#define BKP_DR6 (*(volatile uint16_t*)(0x40006C18))
#define BKP_DR7 (*(volatile uint16_t*)(0x40006C1C))
#define BKP_DR8 (*(volatile uint16_t*)(0x40006C20))
#define BKP_DR9 (*(volatile uint16_t*)(0x40006C24))
#define BKP_DR10 (*(volatile uint16_t*)(0x40006C28))

#define BKP_RTCCR (*(volatile uint16_t*)(0x40006C2C))

#define BKP_CR (*(volatile uint16_t*)(0x40006C30))
#define BKP_CSR (*(volatile uint16_t*)(0x40006C34))

//Power Control (Base: 0x40007000)
#define PWR_CR (*(volatile uint16_t*)(0x40007000))
#define PWR_CSR (*(volatile uint16_t*)(0x40007004))

//Alternate IO Function Config Registers (Base: 0x40010000)
#define AFIO_EVCR (*(volatile uint32_t*)(0x40010000))
#define AFIO_MAPR (*(volatile uint32_t*)(0x40010004))
#define AFIO_EXTICR1 (*(volatile uint32_t*)(0x40010008))
#define AFIO_EXTICR2 (*(volatile uint32_t*)(0x4001000C))
#define AFIO_EXTICR3 (*(volatile uint32_t*)(0x40010010))
#define AFIO_EXTICR4 (*(volatile uint32_t*)(0x40010014))
#define AFIO_MAPR2 (*(volatile uint32_t*)(0x4001001C))

//External Interrupt Config Registers (Base: 0x40010400)
#define EXTI_IMR (*(volatile uint32_t*)(0x40010400))
#define EXTI_EMR (*(volatile uint32_t*)(0x40010404))
#define EXTI_RTSR (*(volatile uint32_t*)(0x40010408))
#define EXTI_FTSR (*(volatile uint32_t*)(0x4001040C))
#define EXTI_SWIER (*(volatile uint32_t*)(0x40010410))
#define EXTI_PR (*(volatile uint32_t*)(0x40010414))

//GPIO (Bases: A:0x40010800 B:0x40010C00 C:0x40011000)
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

//ADC (Bases: 1:0x40012400 2:0x40012800)
#define ADC1_SR (*(volatile uint32_t*)(0x40012400))
#define ADC2_SR (*(volatile uint32_t*)(0x40012800))

#define ADC1_CR1 (*(volatile uint32_t*)(0x40012404))
#define ADC2_CR1 (*(volatile uint32_t*)(0x40012804))

#define ADC1_CR2 (*(volatile uint32_t*)(0x40012408))
#define ADC2_CR2 (*(volatile uint32_t*)(0x40012808))

#define ADC1_SMPR1 (*(volatile uint32_t*)(0x4001240C))
#define ADC2_SMPR1 (*(volatile uint32_t*)(0x4001280C))

#define ADC1_SMPR2 (*(volatile uint32_t*)(0x40012410))
#define ADC2_SMPR2 (*(volatile uint32_t*)(0x40012810))

#define ADC1_JOFR1 (*(volatile uint32_t*)(0x40012414))
#define ADC2_JOFR1 (*(volatile uint32_t*)(0x40012814))

#define ADC1_JOFR2 (*(volatile uint32_t*)(0x40012418))
#define ADC2_JOFR2 (*(volatile uint32_t*)(0x40012818))

#define ADC1_JOFR3 (*(volatile uint32_t*)(0x4001241C))
#define ADC2_JOFR3 (*(volatile uint32_t*)(0x4001281C))

#define ADC1_JOFR4 (*(volatile uint32_t*)(0x40012420))
#define ADC2_JOFR4 (*(volatile uint32_t*)(0x40012820))

#define ADC1_HTR (*(volatile uint32_t*)(0x40012424))
#define ADC2_HTR (*(volatile uint32_t*)(0x40012824))

#define ADC1_LTR (*(volatile uint32_t*)(0x40012428))
#define ADC2_LTR (*(volatile uint32_t*)(0x40012828))

#define ADC1_SQR1 (*(volatile uint32_t*)(0x4001242C))
#define ADC2_SQR1 (*(volatile uint32_t*)(0x4001282C))

#define ADC1_SQR2 (*(volatile uint32_t*)(0x40012430))
#define ADC2_SQR2 (*(volatile uint32_t*)(0x40012830))

#define ADC1_SQR3 (*(volatile uint32_t*)(0x40012434))
#define ADC2_SQR3 (*(volatile uint32_t*)(0x40012834))

#define ADC1_JSQR (*(volatile uint32_t*)(0x40012438))
#define ADC2_JSQR (*(volatile uint32_t*)(0x40012838))

#define ADC1_JDR1 (*(volatile uint32_t*)(0x4001243C))
#define ADC2_JDR1 (*(volatile uint32_t*)(0x4001283C))

#define ADC1_JDR2 (*(volatile uint32_t*)(0x40012440))
#define ADC2_JDR2 (*(volatile uint32_t*)(0x40012840))

#define ADC1_JDR3 (*(volatile uint32_t*)(0x40012444))
#define ADC2_JDR3 (*(volatile uint32_t*)(0x40012844))

#define ADC1_JDR4 (*(volatile uint32_t*)(0x40012448))
#define ADC2_JDR4 (*(volatile uint32_t*)(0x40012848))

#define ADC1_DR (*(volatile uint32_t*)(0x4001244C))
#define ADC2_DR (*(volatile uint32_t*)(0x4001284C))

//Timers (Bases: 1:0x40012C00 2:0x40000000 3:0x40000400 4:0x40000800)
#define TIM1_CR1 (*(volatile uint16_t*)(0x40012C00))
#define TIM2_CR1 (*(volatile uint16_t*)(0x40000000))
#define TIM3_CR1 (*(volatile uint16_t*)(0x40000400))
#define TIM4_CR1 (*(volatile uint16_t*)(0x40000800))

#define TIM1_CR2 (*(volatile uint16_t*)(0x40012C04))
#define TIM2_CR2 (*(volatile uint16_t*)(0x40000004))
#define TIM3_CR2 (*(volatile uint16_t*)(0x40000404))
#define TIM4_CR2 (*(volatile uint16_t*)(0x40000804))

#define TIM1_SMCR (*(volatile uint16_t*)(0x40012C08))
#define TIM2_SMCR (*(volatile uint16_t*)(0x40000008))
#define TIM3_SMCR (*(volatile uint16_t*)(0x40000408))
#define TIM4_SMCR (*(volatile uint16_t*)(0x40000808))

#define TIM1_DIER (*(volatile uint16_t*)(0x40012C0C))
#define TIM2_DIER (*(volatile uint16_t*)(0x4000000C))
#define TIM3_DIER (*(volatile uint16_t*)(0x4000040C))
#define TIM4_DIER (*(volatile uint16_t*)(0x4000080C))

#define TIM1_SR (*(volatile uint16_t*)(0x40012C10))
#define TIM2_SR (*(volatile uint16_t*)(0x40000010))
#define TIM3_SR (*(volatile uint16_t*)(0x40000410))
#define TIM4_SR (*(volatile uint16_t*)(0x40000810))

#define TIM1_EGR (*(volatile uint16_t*)(0x40012C14))
#define TIM2_EGR (*(volatile uint16_t*)(0x40000014))
#define TIM3_EGR (*(volatile uint16_t*)(0x40000414))
#define TIM4_EGR (*(volatile uint16_t*)(0x40000814))

#define TIM1_CCMR1 (*(volatile uint16_t*)(0x40012C18))
#define TIM2_CCMR1 (*(volatile uint16_t*)(0x40000018))
#define TIM3_CCMR1 (*(volatile uint16_t*)(0x40000418))
#define TIM4_CCMR1 (*(volatile uint16_t*)(0x40000818))

#define TIM1_CCMR2 (*(volatile uint16_t*)(0x40012C1C))
#define TIM2_CCMR2 (*(volatile uint16_t*)(0x4000001C))
#define TIM3_CCMR2 (*(volatile uint16_t*)(0x4000041C))
#define TIM4_CCMR2 (*(volatile uint16_t*)(0x4000081C))

#define TIM1_CCER (*(volatile uint16_t*)(0x40012C20))
#define TIM2_CCER (*(volatile uint16_t*)(0x40000020))
#define TIM3_CCER (*(volatile uint16_t*)(0x40000420))
#define TIM4_CCER (*(volatile uint16_t*)(0x40000820))

#define TIM1_CNT (*(volatile uint16_t*)(0x40012C24))
#define TIM2_CNT (*(volatile uint16_t*)(0x40000024))
#define TIM3_CNT (*(volatile uint16_t*)(0x40000424))
#define TIM4_CNT (*(volatile uint16_t*)(0x40000824))

#define TIM1_PSC (*(volatile uint16_t*)(0x40012C28))
#define TIM2_PSC (*(volatile uint16_t*)(0x40000028))
#define TIM3_PSC (*(volatile uint16_t*)(0x40000428))
#define TIM4_PSC (*(volatile uint16_t*)(0x40000828))

#define TIM1_ARR (*(volatile uint16_t*)(0x40012C2C))
#define TIM2_ARR (*(volatile uint16_t*)(0x4000002C))
#define TIM3_ARR (*(volatile uint16_t*)(0x4000042C))
#define TIM4_ARR (*(volatile uint16_t*)(0x4000082C))

#define TIM1_RCR (*(volatile uint16_t*)(0x40012C30))

#define TIM1_CCR1 (*(volatile uint16_t*)(0x40012C34))
#define TIM2_CCR1 (*(volatile uint16_t*)(0x40000034))
#define TIM3_CCR1 (*(volatile uint16_t*)(0x40000434))
#define TIM4_CCR1 (*(volatile uint16_t*)(0x40000834))

#define TIM1_CCR2 (*(volatile uint16_t*)(0x40012C38))
#define TIM2_CCR2 (*(volatile uint16_t*)(0x40000038))
#define TIM3_CCR2 (*(volatile uint16_t*)(0x40000438))
#define TIM4_CCR2 (*(volatile uint16_t*)(0x40000838))

#define TIM1_CCR3 (*(volatile uint16_t*)(0x40012C3C))
#define TIM2_CCR3 (*(volatile uint16_t*)(0x4000003C))
#define TIM3_CCR3 (*(volatile uint16_t*)(0x4000043C))
#define TIM4_CCR3 (*(volatile uint16_t*)(0x4000083C))

#define TIM1_CCR4 (*(volatile uint16_t*)(0x40012C40))
#define TIM2_CCR4 (*(volatile uint16_t*)(0x40000040))
#define TIM3_CCR4 (*(volatile uint16_t*)(0x40000440))
#define TIM4_CCR4 (*(volatile uint16_t*)(0x40000840))

#define TIM1_BDTR (*(volatile uint16_t*)(0x40012C44))

#define TIM1_DCR (*(volatile uint16_t*)(0x40012C48))
#define TIM2_DCR (*(volatile uint16_t*)(0x40000048))
#define TIM3_DCR (*(volatile uint16_t*)(0x40000448))
#define TIM4_DCR (*(volatile uint16_t*)(0x40000848))

#define TIM1_DMAR (*(volatile uint32_t*)(0x40012C4C))
#define TIM2_DMAR (*(volatile uint32_t*)(0x4000004C))
#define TIM3_DMAR (*(volatile uint32_t*)(0x4000044C))
#define TIM4_DMAR (*(volatile uint32_t*)(0x4000084C))

//SPI (Bases: 1:0x40013000 2:0x40003800)
#define SPI1_CR1 (*(volatile uint16_t*)(0x40013000))
#define SPI2_CR1 (*(volatile uint16_t*)(0x40003800))

#define SPI1_CR2 (*(volatile uint16_t*)(0x40013004))
#define SPI2_CR2 (*(volatile uint16_t*)(0x40003804))

#define SPI1_SR (*(volatile uint16_t*)(0x40013008))
#define SPI2_SR (*(volatile uint16_t*)(0x40003808))

#define SPI1_DR (*(volatile uint16_t*)(0x4001300C))
#define SPI2_DR (*(volatile uint16_t*)(0x4000380C))

#define SPI1_CRCPR (*(volatile uint16_t*)(0x40013010))
#define SPI2_CRCPR (*(volatile uint16_t*)(0x40003810))

#define SPI1_RXCRCR (*(volatile uint16_t*)(0x40013014))
#define SPI2_RXCRCR (*(volatile uint16_t*)(0x40003814))

#define SPI1_TXCRCR (*(volatile uint16_t*)(0x40013018))
#define SPI2_TXCRCR (*(volatile uint16_t*)(0x40003818))

//USART (Bases: 1: 0x40013800 2:0x40004400 3:0x40004800)
#define USART1_SR (*(volatile uint16_t*)(0x40013800))
#define USART2_SR (*(volatile uint16_t*)(0x40004400))
#define USART3_SR (*(volatile uint16_t*)(0x40004800))

#define USART1_DR (*(volatile uint16_t*)(0x40013804))
#define USART2_DR (*(volatile uint16_t*)(0x40004404))
#define USART3_DR (*(volatile uint16_t*)(0x40004804))

#define USART1_BRR (*(volatile uint16_t*)(0x40013808))
#define USART2_BRR (*(volatile uint16_t*)(0x40004408))
#define USART3_BRR (*(volatile uint16_t*)(0x40004808))

#define USART1_CR1 (*(volatile uint16_t*)(0x4001380C))
#define USART2_CR1 (*(volatile uint16_t*)(0x4000440C))
#define USART3_CR1 (*(volatile uint16_t*)(0x4000480C))

#define USART1_CR2 (*(volatile uint16_t*)(0x40013810))
#define USART2_CR2 (*(volatile uint16_t*)(0x40004410))
#define USART3_CR2 (*(volatile uint16_t*)(0x40004810))

#define USART1_CR3 (*(volatile uint16_t*)(0x40013814))
#define USART2_CR3 (*(volatile uint16_t*)(0x40004414))
#define USART3_CR3 (*(volatile uint16_t*)(0x40004814))

#define USART1_GTPR (*(volatile uint16_t*)(0x40013818))
#define USART2_GTPR (*(volatile uint16_t*)(0x40004418))
#define USART3_GTPR (*(volatile uint16_t*)(0x40004818))

//DMA (Base: 0x40020000)
#define DMA_ISR (*(volatile uint32_t*)(0x40020000))
#define DMA_IFCR (*(volatile uint32_t*)(0x40020004))

#define DMA_CCR1 (*(volatile uint32_t*)(0x40020008))
#define DMA_CNDTR1 (*(volatile uint32_t*)(0x4002000C))
#define DMA_CPAR1 (*(volatile uint32_t*)(0x40020010))
#define DMA_CMAR1 (*(volatile uint32_t*)(0x40020014))

#define DMA_CCR2 (*(volatile uint32_t*)(0x4002001C))
#define DMA_CNDTR2 (*(volatile uint32_t*)(0x40020020))
#define DMA_CPAR2 (*(volatile uint32_t*)(0x40020024))
#define DMA_CMAR2 (*(volatile uint32_t*)(0x40020028))

#define DMA_CCR3 (*(volatile uint32_t*)(0x40020030))
#define DMA_CNDTR3 (*(volatile uint32_t*)(0x40020034))
#define DMA_CPAR3 (*(volatile uint32_t*)(0x40020038))
#define DMA_CMAR3 (*(volatile uint32_t*)(0x4002003C))

#define DMA_CCR4 (*(volatile uint32_t*)(0x40020044))
#define DMA_CNDTR4 (*(volatile uint32_t*)(0x40020048))
#define DMA_CPAR4 (*(volatile uint32_t*)(0x4002004C))
#define DMA_CMAR4 (*(volatile uint32_t*)(0x40020050))

#define DMA_CCR5 (*(volatile uint32_t*)(0x40020058))
#define DMA_CNDTR5 (*(volatile uint32_t*)(0x4002005C))
#define DMA_CPAR5 (*(volatile uint32_t*)(0x40020060))
#define DMA_CMAR5 (*(volatile uint32_t*)(0x40020064))

#define DMA_CCR6 (*(volatile uint32_t*)(0x4002006C))
#define DMA_CNDTR6 (*(volatile uint32_t*)(0x40020070))
#define DMA_CPAR6 (*(volatile uint32_t*)(0x40020074))
#define DMA_CMAR6 (*(volatile uint32_t*)(0x40020078))

#define DMA_CCR7 (*(volatile uint32_t*)(0x40020080))
#define DMA_CNDTR7 (*(volatile uint32_t*)(0x40020084))
#define DMA_CPAR7 (*(volatile uint32_t*)(0x40020088))
#define DMA_CMAR7 (*(volatile uint32_t*)(0x4002008C))

//Reset + Clock Control Registers (Base: 0x40021000)//TODO determine if these can be accessed by 16 bits
#define RCC_CR (*(volatile uint32_t*)(0x40021000))
#define RCC_CFGR (*(volatile uint32_t*)(0x40021004))
#define RCC_CIR (*(volatile uint32_t*)(0x40021008))
#define RCC_ABP2RSTR (*(volatile uint32_t*)(0x4002100C))
#define RCC_ABP1RSTR (*(volatile uint32_t*)(0x40021010))
#define RCC_AHBENR (*(volatile uint32_t*)(0x40021014))
#define RCC_APB2ENR (*(volatile uint32_t*)(0x40021018))
#define RCC_APB1ENR (*(volatile uint32_t*)(0x4002101C))
#define RCC_BDCR (*(volatile uint32_t*)(0x40021020))
#define RCC_CSR (*(volatile uint32_t*)(0x40021024))
#define RCC_AHBRSTR (*(volatile uint32_t*)(0x40021028))
#define RCC_CFGR2 (*(volatile uint32_t*)(0x4002102C))

//Flash Memory Interface (Base: 0x40022000)//TODO determine if these can be accessed by 16 bits
#define FLASH_ACR (*(volatile uint32_t*)(0x40022000))
#define FLASH_KEYR (*(volatile uint32_t*)(0x40022004))
#define FLASH_OPTKEYR (*(volatile uint32_t*)(0x40022008))
#define FLASH_SR (*(volatile uint32_t*)(0x4002200C))
#define FLASH_CR (*(volatile uint32_t*)(0x40022010))
#define FLASH_AR (*(volatile uint32_t*)(0x40022014))
#define FLASH_OBR (*(volatile uint32_t*)(0x4002201C))
#define FLASH_WRPR (*(volatile uint32_t*)(0x40022020))

//CRC (Base: 0x40023000)
#define CRC_DR (*(volatile uint32_t*)(0x40023000))
#define CRC_IDR (*(volatile uint32_t*)(0x40023004))
#define CRC_CR (*(volatile uint32_t*)(0x40023008))

//Cortex Registers
//SysTick (Base: 0xE000E010)
#define SYST_CTRL (*(volatile uint32_t*)(0xE000E010))
#define SYST_CSR (*(volatile uint32_t*)(0xE000E010))//Name used by some datasheets

#define SYST_LOAD (*(volatile uint32_t*)(0xE000E014))
#define SYST_RVR (*(volatile uint32_t*)(0xE000E014))//Name used by some datasheets

#define SYST_VAL (*(volatile uint32_t*)(0xE000E018))
#define SYST_CVR (*(volatile uint32_t*)(0xE000E018))//Name used by some datasheets

#define SYST_CALIB (*(volatile uint32_t*)(0xE000E01C))

//NVIC (Base: 0xE000E100; NVIC_STIR is an exception)
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

//SCB (System Control Block) (Base: 0xE000ED00; SCB_ACTLR is an exception)
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

//Debug Registers (Base: 0xE0042000)
#define DBGMCU_IDCODE  (*(volatile uint32_t*)(0xE0042000))
#define DBGMCU_CR  (*(volatile uint32_t*)(0xE0042004))

#endif//BLUEPILL_H

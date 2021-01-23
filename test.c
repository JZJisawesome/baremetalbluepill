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

/* Tests a bunch of functions of the stm32 */

#include "bluepill.h"

//These variable are also neat for testing .bss and .data respectively
volatile uint32_t systickCount = 0;//Counts up every 1 ms
volatile uint16_t systickCount1000 = 123;//Counts up every 1 ms; resets to 0 after 999; therefore is equal to given number only once a second

volatile uint8_t times = 0;//Increases each time exti fires

void pwmstuffs();
void setupSystick();
void extistuffs();

void main()
{
    __delayInstructions(70000000);
    //Setup gpio
    
    GPIOA_CRL = 0x0000bbbb;//Set PA0, PA1, PA2, and PA3 as alternate function 50mhz outputs
    GPIOB_CRL = 0x00000008;//Set PB0 as input-pulldown/pullup (leaving as pulldown though)
    GPIOB_CRH = 0x00003000;//Set PB11 as 50mhz output
    GPIOC_CRH = 0x00300000;//Set PC13 and PC14 as 50mhz outputs
    
    pwmstuffs();
    setupSystick();
    extistuffs();
    
    return;
}

void pwmstuffs()
{
    TIM2_PSC = 0;//72Mhz
    TIM2_CCMR1 = 0x6868;//Enable PWM mode 1 on timer 2 channel 1+2 as well as preload enable
    TIM2_CCMR2 = 0x6868;//Enable PWM mode 1 on timer 2 channel 3+4 as well as preload enable
    TIM2_CR1 = 0x0080;//Enable TIM2_ARR preload
    TIM2_ARR = 0xFFFF;//Use all of the bits of the counter
    
    TIM2_CCR1 = 0x7FFF;//50% duty cycle on channel 1
    TIM2_CCR2 = 0x3FFF;//25% duty cycle on channel 2//This channel will be constantly increased by the systick isr
    TIM2_CCR3 = 0x5555;//33% duty cycle on channel 3//Todo debug why channel 3 does not work (appears dead)
    TIM2_CCR4 = 0x28F;//1% duty cycle on channel 4
    
    TIM2_EGR |= 0x0001;//Generate update event to copy TIM2_PSC, TIM_ARR, and TIM2_CCRx into actual (shadow) registers
    TIM2_CR1 |= 0x0001;//Enable counter (upcounting)
    TIM2_CCER = 0x1111;//Enable channel outputs (non-inverted polarity)
}

void setupSystick()
{
    SYST_RVR = 9000;//1000hz tick speed
    SYST_CVR = 0;//Reset the counter
    SYST_CSR = 0b011;//Use the "external" HCLK/8=9mhz clock for the counter, enable the __ISR_SysTick interrupt and the counter itself
}

void extistuffs()
{
    AFIO_EXTICR1 = 0x0001;//Set EXTI0 to use PB0
    EXTI_RTSR = 0x00000001;//Trigger on the rising edge of PB0
    EXTI_FTSR = 0x00000001;//Trigger on the falling edge of PB0
    EXTI_IMR = 0x0000FFFF;//Enable the EXTI0 interrupt
    NVIC_ISER0 = 0x00000040;//Enable the interrupt in the nvic
}

/* Testing Various interrupts */

__attribute__ ((interrupt ("IRQ"))) void __ISR_EXTI0()
{
    GPIOC_ODR |= 0x00002000;//Set led on
    EXTI_PR = 0x00000001;//Clear pending bit in peripheral register (set to clear)
    NVIC_ICPR0 = 0x00000040;//Clear the interrupt flag in the nvic (set to clear)
    ++times;
}

__attribute__ ((interrupt ("IRQ"))) void __ISR_SysTick()
{
    ++systickCount;
    
    if (++systickCount1000 > 999)
        systickCount1000 = 0;
    
    /* Testing GPIO */
    if (systickCount1000 == 999)//Is only 999 once every second
        GPIOC_ODR ^= 0x00002000;//Toggle PC13
        
    /* Test changing pwm every systick */
    TIM2_CCR2 += 50;
    
    return;
}

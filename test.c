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

volatile uint32_t systickCount = 0;//Counts up every 1 ms
volatile uint16_t systickCount1000 = 1;//Counts up every 1 ms; resets to 0 after 999; therefore is equal to given number only once a second

void main()
{
    __delayInstructions(10000);
    
    GPIOA_CRL = 0x0000bbbb;//Set PA0, PA1, PA2, and PA3 as alternate function 50mhz outputs
    GPIOC_CRH = 0x00300000;//Set PC13 and PC14 as 50mhz outputs
    
    /* PWM */
    
    //Todo debug why none of this (the pwm) seems to work anymore
    //For some reason TIM_CR1 does not write in either instance
    //Compiler is just not generating code for either assignmnet
    
    TIM2_PSC = 0;//72Mhz
    TIM2_CCMR1 = 0x6868;//Enable PWM mode 1 on timer 2 channel 1+2 as well as preload enable
    TIM2_CCMR2 = 0x6868;//Enable PWM mode 1 on timer 2 channel 3+4 as well as preload enable
    TIM2_CR1 = 0x0080;//Enable TIM2_ARR preload
    //(*(volatile uint32_t*)(0x40000000)) = 0x0080;
    TIM2_ARR = 0xFFFF;//Use all of the bits of the counter
    
    TIM2_CCR1 = 0x7FFF;//50% duty cycle on channel 1
    TIM2_CCR2 = 0x3FFF;//25% duty cycle on channel 2//This channel will be constantly increased by the systick isr
    TIM2_CCR3 = 0x5555;//33% duty cycle on channel 3//Todo debug why channel 3 does not work (appears dead)
    TIM2_CCR4 = 0x28F;//1% duty cycle on channel 4
    
    TIM2_EGR |= 0x0001;//Generate update event to copy TIM2_PSC, TIM_ARR, and TIM2_CCRx into actual (shadow) registers
    TIM2_CR1 |= 0x0001;//Enable counter (upcounting)
    TIM2_CCER = 0x1111;//Enable channel outputs (non-inverted polarity)
    
    /* SysTick */
    
    SYST_RVR = 9000;//1000hz tick speed
    SYST_CVR = 0;//Reset the counter
    SYST_CSR = 0b011;//Use the "external" HCLK/8=9mhz clock for the counter, enable the __ISR_SysTick interrupt and the counter itself
    
    
    /* Forever loop for more testing */
    
    while (1)
    {
        
    }
    
    return;//Will fall through to infinite loop if main ever returns
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
}

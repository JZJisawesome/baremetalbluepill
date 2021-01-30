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
//Systick
//Gpio toggling w/ systick
//ADC1 continuous mode with interrupt (left aligned, maximum frequency, calibration beforehand)
//PWM timer 2 channels 1 to 4; 1 constant 50%, 2 constantly increased by systick, 3 constant (but not working at the moment for some reason,) 4 being set to left aligned reading from adc
//UART1 echoing characters at 2000000 baud

#include "bluepill.h"

//These variable are also neat for testing .bss and .data respectively
volatile uint32_t systickCount = 0;//Counts up every 1 ms
volatile uint16_t systickCount1000 = 123;//Counts up every 1 ms; resets to 0 after 999; therefore is equal to given number only once a second

void pwmstuffs();
void setupSystick();
void extistuffs();
void adcStuffs();
void uartStuffs();

void main()
{
    //__delayInstructions(70000000);//testing
    
    //Setup gpio
    GPIOA_CRL ^= 0x0000ffff;//Set PA0, PA1, PA2, and PA3 as alternate function 50mhz outputs (b)
    GPIOA_CRH = 0x000008b0;//Set PA9 as alternate function output and PA10 as pull down input
    //Set PB0 as input-pulldown/pullup (leaving as pulldown though)
    //Set PB1 as analog input
    GPIOB_CRL = 0x00000008;
    GPIOB_CRH = 0x00003000;//Set PB11 as 50mhz output
    GPIOC_CRH = 0x00300000;//Set PC13 as 50mhz output
    
    GPIOA_ODR &= 0xFBFF;//Enable pull down on pa10 (needed because my usb to serial converter struggles to drive the pin low for some reason (PA10 seems to leak current; only a few uamps, but enough to make it a problem))
    //oops, it's actually not enough. Used an external pulldown as well
    
    pwmstuffs();
    setupSystick();
    extistuffs();
    adcStuffs();
    uartStuffs();
    
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
    TIM2_CCR4 = 0x28F;//1% duty cycle on channel 4//Set based on adc readings
    
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
    NVIC_ISER0 = 0x00000040;//Enable the interrupt in the nvic (6) (set to enable)
}

void adcStuffs()
{
    ADC1_CR2 = 0x00000001;//Turn on adc
    __delayInstructions(72);//Ensure t_stab has passed
    ADC1_CR2 |= 0x00000004;//Begin calibration
    
    while (ADC1_CR2 & 0x00000004);//Wait for calibration to finish
    
    //Enable continuous conversion mode and left align result
    ADC1_CR2 |= 0x00000802;
    
    //By default, there is only 1 channel in sequence (ADC_SQR1 L[3:0] = 0000 at reset)
    //So, since we will be reading from PB1, set the first (only) channel in the sequence to 9
    ADC1_SQR3 = 0x00000009;
    
    ADC1_CR1 |= 0x00000020;//Enable the end of conversion interrupt (EOCIE)
    NVIC_ISER0 = 0x00040000;//Enable the interrupt in the nvic (18) (set to enable)
    
    ADC1_CR2 |= 0x00000001;//Begin continuous conversion
}

void uartStuffs()
{
    //USART1_BRR = 0x1D4C;//72MHz / 468.75*16 = 9600 baud
    USART1_BRR = 0x0024;//72Mhz / 2.25*16 = 2000000 baud
    USART1_CR1 = 0x202C;//Enable usart, transmitter, receiver and receive interrupt
    NVIC_ISER1 = 0x0020;//Enable the interrupt in the nvic (37) (set to enable)
}

/* Testing Various interrupts */

__attribute__ ((interrupt ("IRQ"))) void __ISR_USART1()
{
    //Echo!
    char data = USART1_DR;//Read character
    
    if (USART1_SR & 0x0080)//Transmit data register is empty; if full, character is discarded
        USART1_DR = data;//We can echo what we received
}

__attribute__ ((interrupt ("IRQ"))) void __ISR_ADC1_2()
{
    //Write value to TIM2_CCR4 here, shifted 4 bits left (or use left adc alignment)
    
    //testing
    //Read conversion result; also clears eoc bit in ADC1_SR
    //Store as pwm duty cycle
    TIM2_CCR4 = ADC1_DR & 0x0000FFFF;
}

__attribute__ ((interrupt ("IRQ"))) void __ISR_EXTI0()
{
    EXTI_PR = 0x00000001;//Clear pending bit in peripheral register (set to clear)
    GPIOC_ODR |= 0x00002000;//Set led on
    
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

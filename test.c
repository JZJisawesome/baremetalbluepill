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
//Recieved uart character also sent out with spi to shift register!

#include "bluepill.h"

//These variable are also neat for testing .bss and .data respectively
volatile uint32_t systickCount = 0;//Counts up every 1 ms
volatile uint16_t systickCount1000 = 123;//Counts up every 1 ms; resets to 0 after 999; therefore is equal to given number only once a second

void pwmstuffs();
void setupSystick();
void extistuffs();
void adcStuffs();
void spiStuffs();
void i2cStuffs();
void uartStuffs();

void main()
{
    //__delayInstructions(70000000);//testing
    
    //Setup gpio
    //PA4 is set as 50mhz output (0x4 ^ 0x7 = 0x3)
    GPIOA_CRL ^= 0xf0f7ffff;//Set PA0 to 3, 5, 7 as alternate function 50mhz outputs (b)
    GPIOA_CRH = 0x000008b0;//Set PA9 as alternate function output and PA10 as pull down input
    //Set PB0 as input-pulldown/pullup (leaving as pulldown though)
    //Set PB1 as analog input
    //Set PB6 and PB7 as alternative function open drain
    GPIOB_CRL = 0xFF000008;
    GPIOB_CRH = 0x00003000;//Set PB11 as 50mhz output
    GPIOC_CRH = 0x00300000;//Set PC13 as 50mhz output
    
    GPIOA_ODR &= 0xFBFF;//Enable pull down on pa10 (needed because my usb to serial converter struggles to drive the pin low for some reason (PA10 seems to leak current; only a few uamps, but enough to make it a problem))
    //oops, it's actually not enough. Used an external pulldown as well
    
    pwmstuffs();
    setupSystick();
    extistuffs();
    adcStuffs();
    spiStuffs();
    uartStuffs();
    i2cStuffs();//TODO fix//Why is eeprom not responding?
    
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

void spiStuffs()
{
    //Settings
    SPI1_CR1 = 0b0000001111011100;//Enable software input of nss pin held high to keep in master mode, lsbfirst, enable spi, Baud rate = fPCLK/16, Master, CPOL=0, CPHA=0
    
    //TODO use these to toggle RCLK with an interrupt instead of waiting in a loop like below/in the uart interrupt handeler
    //SPI1_CR2 = 0x0080;//Enable tx buffer empty interrupt
    //NVIC_ISER1 = 0x0008;//Enable spi1 interrupt in nvic (35)(set to enable)
    
    //Testing output
    /*
    uint8_t test = 0;
    while (true)
    {
        //Write data
        if (SPI1_SR & 0x0002)//Transmit buffer is empty
            //SPI1_DR = 0xAB;
            SPI1_DR = test++;
        
        //while(~(SPI1_SR & 0x0002) | (SPI1_SR & 0x0080));//Wait for transfer to finish (transmit buffer empty and no longer busy) before toggling rclk
        __delayInstructions(100);//Wait a tiny bit (for transfer to finish, because above does not work)
        
        GPIOA_ODR |= 0x0010;//Set PA4 high
        GPIOA_ODR &= 0xFFEF;//Set PA4 low
        
        __delayInstructions(1000000);
    }
    */
}

void i2cStuffs()//TODO fix why is high for 3 us and low for 9 instead of 5 and 5?
{
    I2C1_CR2 = 0x0024;//Indicate that APB frequency is 36mhz (write 36)
    //CCR=180 and Tpclk1=1/36000000 so: I2C sck period=2*(CCR*Tpclk1)=10us (5us Thigh, 5us Tlow)
    I2C1_CCR = 0x00B4;//Slow mode; CCR=180//TODO figure out why timing is still weird?
    I2C1_TRISE = 0x0025;//TRISE=0d37 (37-1=36 and 36*(1/36000000)=1000ns) (max i2c rise time))
    I2C1_CR1 = 0x0001;//Enable i2c peripheral
    
    //Test sending
    I2C1_CR1 |= 0x0100;//Generate start condition
    while (!(I2C1_SR1 & 0x0001));//Wait for start condition to be sent
    
    I2C1_DR = 0b10100001;//Address of 24C64 eeprom, read mode
    while (!(I2C1_SR1 & 0x0002));//Wait for address to be sent//TODO Why is eeprom not responding? NACK bit set instead of addr bit so this loops forever
    volatile uint8_t dummyRead = I2C1_SR2;//No useful flags in i2c_sr2, but must be read
    
    I2C1_DR = 0xAB;//High byte of eeprom address is ab
    while (!(I2C1_SR1 & 0x0080));//Wait for transmit buffer register to be empty
    while (!(I2C1_SR1 & 0x0004));//Wait for byte transfer finished to be 1
    
    I2C1_DR = 0xCD;//Low byte of eeprom address is cd
    while (!(I2C1_SR1 & 0x0080));//Wait for transmit buffer register to be empty
    while (!(I2C1_SR1 & 0x0004));//Wait for byte transfer finished to be 1
}

/* Testing Various interrupts */


__attribute__ ((interrupt ("IRQ"))) void __ISR_SPI1()
{
    //After an spi transfer completes, toggle PA4 (to high, then low again) to latch data (rclk)
    GPIOA_ODR |= 0x0010;//Set PA4 high
    GPIOA_ODR &= 0xFFEF;//Set PA4 high
    
    //TODO use this instead of busywaiting in usart handler below (more efficient)
}

__attribute__ ((interrupt ("IRQ"))) void __ISR_USART1()
{
    char data = USART1_DR;//Read character
    
    //Echo!
    if (USART1_SR & 0x0080)//Transmit data register is empty; if full, character is discarded
        USART1_DR = data;//We can echo what we received
        
    //Also shift out data to SPI
    //Write data
    if (SPI1_SR & 0x0002)//Transmit buffer is empty
        SPI1_DR = data;
    
    //TODO to the RCLK pin toggling in it's own interrupt instead of waiting here
    //The wait slows everything down; should try to do as little as possible in an isr
    //while(~(SPI1_SR & 0x0002) | (SPI1_SR & 0x0080));//Wait for transfer to finish (transmit buffer empty and no longer busy) before toggling rclk
    __delayInstructions(100);//Wait a tiny bit (for transfer to finish, because above does not work)
    
    GPIOA_ODR |= 0x0010;//Set PA4 high
    GPIOA_ODR &= 0xFFEF;//Set PA4 low
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

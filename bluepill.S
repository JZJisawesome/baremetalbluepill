/* Initializes the STM32F103C8T6 vector table, registers, memory and jumps to main
 * Also implements some useful functions defined by bluepill.h
 *
 * Note: to implement an interrupt handler in assembly, write a global function with the proper name.
 * To implement an interrupt handler in c, write __attribute__ ((interrupt ("IRQ"))) void NAME()
 * where NAME is the proper name of the handler
 *
 * Note that you can also override __clockInit, but memory will not be initialized at that point 
 * so writing in assembly is recommended for that function
*/

@Note: I tried to do this from scratch as much as possible, but needed to look at other sources for certain things

/* Licensing
 *
****Copyright 2021 John Jekel
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
****Referenced code from: https:@github.com/RickKimball/fabooh/blob/master/board/bluepill/startup_stm32.s
 *For the following things: initializing memory, how to do weak aliases, 
 *idea to fallthrough to default ISR handler from __start, other small things
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. Neither the name of STMicroelectronics nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ^1 - rrk modified code to use IT blocks
 *
 * I also looked a bit at stm32duino code and various other things (stackoverflow, etc.)
*/

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

/* Use Arm unified syntax and the thumb instruction set for the Arm Cortex M3 */
.syntax unified
.cpu cortex-m3
.thumb

/* ISR Vector */
.section .isr_vector
@Cortex Interrupts
.word __stack_top@From linker script
.word __ISR_Reset
.word __ISR_NMI
.word __ISR_HardFault
.word __ISR_MemManage
.word __ISR_BusFault
.word __ISR_UsageFault
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@The next 16 bytes are unused (reserved). Put useful functions here instead of wasting flash
.global __delayInstructions@Takes up 8 bytes@1 instruction is approximatly 21.1851ns
__delayInstructions:@uint32_t numberOfInstructions
    subs r0, 2@sub and bcs instructions (also update condition flags)
    bcs __delayInstructions@Keep looping if underflow didn't occur (C=1)
    bx lr@Return
.word 0x00000000@Reserved (unused bytes)
.word 0x00000000@Reserved (unused bytes)
@Continue with the vector table
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
.word __ISR_SVCall
.word __ISR_Debug_Monitor
.asciz "JZJ"@Reserved; leaving a fun message here (3 bytes + 1 null byte = 1 word)
.word __ISR_PendSV
.word __ISR_SysTick
@Peripheral Interrupts
.word __ISR_WWDG
.word __ISR_PVD
.word __ISR_TAMPER
.word __ISR_RTC
.word __ISR_FLASH
.word __ISR_RCC
.word __ISR_EXTI0
.word __ISR_EXTI1
.word __ISR_EXTI2
.word __ISR_EXTI3
.word __ISR_EXTI4
.word __ISR_DMA1_Channel1
.word __ISR_DMA1_Channel2
.word __ISR_DMA1_Channel3
.word __ISR_DMA1_Channel4
.word __ISR_DMA1_Channel5
.word __ISR_DMA1_Channel6
.word __ISR_DMA1_Channel7
.word __ISR_ADC1_2
.word __ISR_USB_HP_CAN_TX
.word __ISR_USB_LP_CAN_RX0
.word __ISR_CAN_RX1
.word __ISR_CAN_SCE
.word __ISR_EXTI9_5
.word __ISR_TIM1_BRK
.word __ISR_TIM1_UP
.word __ISR_TIM1_TRG_COM
.word __ISR_TIM1_CC
.word __ISR_TIM2
.word __ISR_TIM3
.word __ISR_TIM4
.word __ISR_I2C1_EV
.word __ISR_I2C1_ER
.word __ISR_I2C2_EV
.word __ISR_I2C2_ER
.word __ISR_SPI1
.word __ISR_SPI2
.word __ISR_USART1
.word __ISR_USART2
.word __ISR_USART3
.word __ISR_EXTI15_10
.word __ISR_RTCAlarm
.word __ISR_USBWakeup
@The remaining 17 ISRs that should be in this table do not exist on the STM32F103C8T6, so we
@save 68 bytes of flash by simply not including those entries

/* Weak ISR Definitions */
@Allow these handlers to be overridden by user code
@Cortex Interrupts
.weak __ISR_Reset
.weak __ISR_NMI
.weak __ISR_HardFault
.weak __ISR_MemManage
.weak __ISR_BusFault
.weak __ISR_UsageFault
.weak __ISR_SVCall
.weak __ISR_Debug_Monitor
.weak __ISR_PendSV
.weak __ISR_SysTick
@Peripheral Interrupts
.weak __ISR_WWDG
.weak __ISR_PVD
.weak __ISR_TAMPER
.weak __ISR_RTC
.weak __ISR_FLASH
.weak __ISR_RCC
.weak __ISR_EXTI0
.weak __ISR_EXTI1
.weak __ISR_EXTI2
.weak __ISR_EXTI3
.weak __ISR_EXTI4
.weak __ISR_DMA1_Channel1
.weak __ISR_DMA1_Channel2
.weak __ISR_DMA1_Channel3
.weak __ISR_DMA1_Channel4
.weak __ISR_DMA1_Channel5
.weak __ISR_DMA1_Channel6
.weak __ISR_DMA1_Channel7
.weak __ISR_ADC1_2
.weak __ISR_USB_HP_CAN_TX
.weak __ISR_USB_LP_CAN_RX0
.weak __ISR_CAN_RX1
.weak __ISR_CAN_SCE
.weak __ISR_EXTI9_5
.weak __ISR_TIM1_BRK
.weak __ISR_TIM1_UP
.weak __ISR_TIM1_TRG_COM
.weak __ISR_TIM1_CC
.weak __ISR_TIM2
.weak __ISR_TIM3
.weak __ISR_TIM4
.weak __ISR_I2C1_EV
.weak __ISR_I2C1_ER
.weak __ISR_I2C2_EV
.weak __ISR_I2C2_ER
.weak __ISR_SPI1
.weak __ISR_SPI2
.weak __ISR_USART1
.weak __ISR_USART2
.weak __ISR_USART3
.weak __ISR_EXTI15_10
.weak __ISR_RTCAlarm
.weak __ISR_USBWakeup

/* ISR Aliases */
@.thumb_set instead of .set so that the lsb is set in the ISR Vector, even if overridden
@Cortex Interrupts
.thumb_set __ISR_Reset, __start@Only ISR whose default is not __ISR_default
.thumb_set __ISR_NMI, __ISR_default
.thumb_set __ISR_HardFault, __ISR_default
.thumb_set __ISR_MemManage, __ISR_default
.thumb_set __ISR_BusFault, __ISR_default
.thumb_set __ISR_UsageFault, __ISR_default
.thumb_set __ISR_SVCall, __ISR_default
.thumb_set __ISR_Debug_Monitor, __ISR_default
.thumb_set __ISR_PendSV, __ISR_default
.thumb_set __ISR_SysTick, __ISR_default
@Peripheral Interrupts
.thumb_set __ISR_WWDG, __ISR_default
.thumb_set __ISR_PVD, __ISR_default
.thumb_set __ISR_TAMPER, __ISR_default
.thumb_set __ISR_RTC, __ISR_default
.thumb_set __ISR_FLASH, __ISR_default
.thumb_set __ISR_RCC, __ISR_default
.thumb_set __ISR_EXTI0, __ISR_default
.thumb_set __ISR_EXTI1, __ISR_default
.thumb_set __ISR_EXTI2, __ISR_default
.thumb_set __ISR_EXTI3, __ISR_default
.thumb_set __ISR_EXTI4, __ISR_default
.thumb_set __ISR_DMA1_Channel1, __ISR_default
.thumb_set __ISR_DMA1_Channel2, __ISR_default
.thumb_set __ISR_DMA1_Channel3, __ISR_default
.thumb_set __ISR_DMA1_Channel4, __ISR_default
.thumb_set __ISR_DMA1_Channel5, __ISR_default
.thumb_set __ISR_DMA1_Channel6, __ISR_default
.thumb_set __ISR_DMA1_Channel7, __ISR_default
.thumb_set __ISR_ADC1_2, __ISR_default
.thumb_set __ISR_USB_HP_CAN_TX, __ISR_default
.thumb_set __ISR_USB_LP_CAN_RX0, __ISR_default
.thumb_set __ISR_CAN_RX1, __ISR_default
.thumb_set __ISR_CAN_SCE, __ISR_default
.thumb_set __ISR_EXTI9_5, __ISR_default
.thumb_set __ISR_TIM1_BRK, __ISR_default
.thumb_set __ISR_TIM1_UP, __ISR_default
.thumb_set __ISR_TIM1_TRG_COM, __ISR_default
.thumb_set __ISR_TIM1_CC, __ISR_default
.thumb_set __ISR_TIM2, __ISR_default
.thumb_set __ISR_TIM3, __ISR_default
.thumb_set __ISR_TIM4, __ISR_default
.thumb_set __ISR_I2C1_EV, __ISR_default
.thumb_set __ISR_I2C1_ER, __ISR_default
.thumb_set __ISR_I2C2_EV, __ISR_default
.thumb_set __ISR_I2C2_ER, __ISR_default
.thumb_set __ISR_SPI1, __ISR_default
.thumb_set __ISR_SPI2, __ISR_default
.thumb_set __ISR_USART1, __ISR_default
.thumb_set __ISR_USART2, __ISR_default
.thumb_set __ISR_USART3, __ISR_default
.thumb_set __ISR_EXTI15_10, __ISR_default
.thumb_set __ISR_RTCAlarm, __ISR_default
.thumb_set __ISR_USBWakeup, __ISR_default

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

/* Init Code */

.section .init
.global __start
.thumb_func@Reset handler must be explicitly specified as thumb apparently with this
__start:@Calls __clockInit, initializes memory and jumps to main
    @Note: We can use whatever registers we want here because no code is executed before us
    @We don't need to setup sp because the ISR Vector does that for us :)
    
    /* Setup system clocks so that things are fast and peripherals function */
    bl __clockInit
    
    /* Initialize memory sections
     * Note: We do this a word at a time for speed. The linker script word aligns ram sections
     * so we don't have to worry about writing too much/overwriting anything.
     * This comes at a SRAM cost of a maximum of 7 bytes per section in ram, which is fine.
     * Symbols containing the addresses of the sections are defined by the linker script.
    */
    
    @Zero out .bss section in SRAM
    ldr r1, =__BSS_BEGIN__@Inclusive
    ldr r2, =__BSS_END__@Exclusive
    ldr r0, =0x00000000@Constant 0
1:@Zeroing loop
    cmp r1, r2
    itt lo@r1 is still within the bss section
    strdlo r0, r0, [r1], 8@Write 0x0000000000000000 at address of r1 and inc index register by 8
    blo 1b@Repeat loop
    
    @Copy .data section from Flash to SRAM
    ldr r4, =__DATA_BEGIN__@Inclusive
    ldr r5, =__DATA_END__@Exclusive
    ldr r3, =__DATA_BEGIN_IN_FLASH__@Inclusive
2:@Copying loop
    cmp r4, r5
    ittt lo@r4 is still within the .data section
    ldrdlo r0, r2, [r3], 8@Load two words from Flash and increment Flash index register by 8
    strdlo r0, r2, [r4], 8@Store those words to SRAM and increment SRAM index register by 8
    blo 2b@Repeat loop

    /* Jump to main */
    bl main
    @Fallthrough to default handler if main exits

    
.thumb_func
.weak __ISR_default@Can even re-implement __ISR_default if you want
__ISR_default:
    b __ISR_default@If main returns, spin forever@Also used for unimplemented interrupt routines


.weak __clockInit@So you can override the initial clocks/peripherals for low power/etc.
__clockInit:@Sets all clocks to maximum (rated) frequencies and enables all peripheral clocks
    /* Note: We can use whatever registers we want here because no code is executed before us
     * Note2: We look at the datasheet for the reset values of registers in order to avoid
     * having to load their values, or them to set bits, and store them (faster this way).
    */
    
    /* Load addresses/constants early. This potentially helps out the cortex pipeline a bit. */
    ldr r2,     =0x00000032@Constant for setting FLASH_ACR later
    ldr r8,     =0x40022000@FLASH_ACR address
    ldr r9,     =0x40021000@Base address of RCC control registers
    ldr r10,    =0x001D8402@Constant for setting RCC_CFGR later
    ldr r7,     =0x00000055@Constant for setting RCC_AHBENR later
    ldr r6,     =0x00005E7D@Constant for setting RCC_APB2ENR later
    ldr r5,     =0x1AE64807@Constant for setting RCC_APB1ENR later
    ldr r4,     =0x001D0000@Constant for setting RCC_CFGR later
    ldr r3,     =0x01010083@Constant for setting RCC_CR later
    
    /* Set flash latency to two wait states (because will are going to run at 72MHz) */
    str r2, [r8]@Store 0x00000032 to FLASH_ACR
    
    /* Configure clocks/dividers/the pll */
    
    @Configure RCC_CFGR before RCC_CR (PLL settings must be configured before it is enabled)
        @Set PLL mul to 9x (72mhz), HSE to not be divided, and PLL to use the HSE
    str r4, [r9, 0x4]@Store 0x001D0000 to RCC_CFGR
    
    @Configure RCC_CR
        @Enable PLL and HSE; leave HSI/trim stuff alone (keep reset values)
    str r3, [r9, 0x0]@Store 0x01010083 to RCC_CR
    
1:  @Wait for HSE to be stable
    ldr r2, [r9, 0x0]@Load current value of RCC_CR
    tst r2, 0x00020000@Check HSE ready bit (Sets Z flag to 1 if not ready)
    beq 1b@Loop back if HSE bit is still 0 (if Z flag=1)
    
2:  @Wait for PLL to be stable ("locked")
    ldr r0, [r9, 0x0]@Load current value of RCC_CR
    tst r0, 0x02000000@Check PLL ready bit (Sets Z flag to 1 if not ready)
    beq 2b@Loop back if HSE bit is still 0 (if Z flag=1)

    @Configure RCC_CFGR after RCC_CR
        @Leave PLL mul set to 9x (72mhz), HSE to not be divided, and PLL to use the HSE
        @Divide PCLK2 by 6 for ADC (run it at 12mhz which is the highest under the max of 14mhz)
        @Divide APB1 by 2 (maximum of 32mhz)
        @Use PLL as sysclk source
    str r10, [r9, 0x4]@Store 0x001D8402 to RCC_CFGR
    
    /* Enable all peripheral clocks */
    str r7, [r9, 0x14]@Enable everything on the AHB bus (RCC_AHBENR)
    str r6, [r9, 0x18]@Enable everything on the APB2 bus (RCC_APB2ENR)
    str r5, [r9, 0x1c]@Enable everything on the APB1 bus (RCC_APB1ENR)
    
    bx lr@Return

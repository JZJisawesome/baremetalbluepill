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

/* Note, this linker script was referenced: https://github.com/RickKimball/fabooh/blob/master/board/bluepill/stm32f103c8.ld */

ENTRY(__start)

MEMORY
{
    /* 128k flash starts at 0x08000000 */
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 128K
    /* 20K sram starts at 0x20000000 */
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

PROVIDE(__stack_top = ORIGIN(SRAM) + LENGTH(SRAM));

SECTIONS
{
    /* Read only things */
    .text :
    {
        KEEP(*(.isr_vector)) /* TODO figure out why if this is put in its own output section, even with KEEP, the section is still discarded*/
        KEEP(*(.init))
        *(.text*)
    } > FLASH
    
    .rodata :
    {
        *(.rodata*)
    }
    
    /* R/W things/things in ram */
    __DATA_BEGIN_IN_FLASH__ = LOADADDR(.data); /* */
    .data : ALIGN(8) /* Align data to make init code faster at the expense of a few bytes */
    {
        /* Will need to be copied from flash into sram at startup */
        __DATA_BEGIN__ = .;
        *(.data)
        
        __DATA_END__ = .;
    } > SRAM AT> FLASH
    
    .bss : ALIGN(8) /* Align bss to make init code faster at the expense of a few bytes */
    {
        __BSS_BEGIN__ = .;
        *(.bss)
        __BSS_END__ = .;
    } > SRAM /* Can go directly into SRAM because it is not initialized (just zeroed) */
}

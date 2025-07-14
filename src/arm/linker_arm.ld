/*
 * ARM Cortex-M linker script for MiniForth VM
 * Optimized for small microcontrollers
 */

/* Memory layout - adjust for your specific microcontroller */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 32K    /* 32KB Flash */
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 8K     /* 8KB RAM */
}

/* Stack size - minimal for VM operation */
_stack_size = 512;

/* Entry point */
ENTRY(Reset_Handler)

SECTIONS
{
    /* Vector table and code in Flash */
    .text :
    {
        . = ALIGN(4);
        _stext = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* Data section initialization values in Flash */
    _sidata = LOADADDR(.data);

    /* Initialized data in RAM */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH

    /* Uninitialized data in RAM */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM

    /* Stack at the end of RAM */
    .stack :
    {
        . = ALIGN(8);
        . = . + _stack_size;
        . = ALIGN(8);
        _estack = .;
    } > RAM

    /* Discard debug information to save space */
    /DISCARD/ :
    {
        *(.note.gnu.build-id)
        *(.gnu.attributes)
        *(.comment)
        *(.debug*)
    }
}

/* Check for RAM overflow */
ASSERT((_estack <= ORIGIN(RAM) + LENGTH(RAM)), "Stack overflow in RAM")

/* Calculate sizes for optimization reporting */
_flash_used = _etext - _stext + (_edata - _sdata);
_ram_used = _edata - _sdata + _ebss - _sbss + _stack_size;
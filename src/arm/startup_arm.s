/*
 * ARM Cortex-M startup code for MiniForth VM
 * Minimal startup for microcontroller deployment
 */

.syntax unified
.thumb

/* Vector table */
.section .isr_vector, "ax"
.align 2
.globl vtable
vtable:
    .word _estack          /* Initial stack pointer */
    .word Reset_Handler    /* Reset handler */
    .word NMI_Handler      /* NMI handler */
    .word HardFault_Handler /* Hard fault handler */
    .word 0                /* Reserved */
    .word 0                /* Reserved */
    .word 0                /* Reserved */
    .word 0                /* Reserved */
    .word 0                /* Reserved */
    .word 0                /* Reserved */
    .word 0                /* Reserved */
    .word SVC_Handler      /* SVCall handler */
    .word 0                /* Reserved */
    .word 0                /* Reserved */
    .word PendSV_Handler   /* PendSV handler */
    .word SysTick_Handler  /* SysTick handler */

.section .text
.align 2

/* Reset handler */
.globl Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    /* Set stack pointer */
    ldr r0, =_estack
    mov sp, r0
    
    /* Copy data section from flash to RAM */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
    b copy_data_check
copy_data_loop:
    ldr r3, [r2], #4
    str r3, [r0], #4
copy_data_check:
    cmp r0, r1
    blo copy_data_loop
    
    /* Zero BSS section */
    ldr r0, =_sbss
    ldr r1, =_ebss
    mov r2, #0
    b zero_bss_check
zero_bss_loop:
    str r2, [r0], #4
zero_bss_check:
    cmp r0, r1
    blo zero_bss_loop
    
    /* Call main function */
    bl main
    
    /* Infinite loop if main returns */
hang:
    b hang

/* Default interrupt handlers */
.weak NMI_Handler
.weak HardFault_Handler
.weak SVC_Handler
.weak PendSV_Handler
.weak SysTick_Handler

NMI_Handler:
HardFault_Handler:
SVC_Handler:
PendSV_Handler:
SysTick_Handler:
    b hang

.end
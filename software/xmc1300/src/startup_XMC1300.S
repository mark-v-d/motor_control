/*********************************************************************************************************************
 * @file     startup_XMC1300.S
 * @brief    CMSIS Core Device Startup File for Infineon XMC1300 Device Series
 * @version  V1.15
 * @date     05 Jan 2016
 *
 * @cond
 *********************************************************************************************************************
 * Copyright (c) 2012-2016, Infineon Technologies AG
 * All rights reserved.                        
 *                                             
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the 
 * following conditions are met:   
 *                                                                              
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
 * disclaimer.                        
 * 
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following 
 * disclaimer in the documentation and/or other materials provided with the distribution.                       
 * 
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote 
 * products derived from this software without specific prior written permission.                                           
 *                                                                              
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR  
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                                  
 *                                                                              
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with 
 * Infineon Technologies AG dave@infineon.com).                                                          
 *********************************************************************************************************************
 *
 **************************** Change history ********************************
 * V1.0, Oct, 02, 2012 PKB:Startup file for XMC1  
 * V1.1, Oct, 19, 2012 PKB:ERU and MATH interrupt handlers  
 * V1.2, Nov, 02, 2012 PKB:Renamed AllowPLLInitByStartup to AllowClkInitByStartup  
 * V1.3, Dec, 11, 2012 PKB:Attributes of .XmcVeneerCode section changed  
 * V1.4, Dec, 13, 2012 PKB:Removed unwanted interrupts/veneers  
 * V1.5, Jan, 26, 2013 PKB:Corrected the SSW related entries  
 * V1.6, Feb, 13, 2013 PKB:Relative path to Device_Data.h  
 * V1.7, Feb, 19, 2013 PKB:Included XMC1100_SCU.inc
 * V1.8, Jan, 24, 2014 PKB:Removed AllowClkInitStartup and DAVE Extended init
 * V1.9, Feb, 05, 2014 PKB:Removed redundant alignment code from copy+clear funcs
 * V1.10, Feb, 14, 2014 PKB:Added software_init_hook and hardware_init_hook
 * V1.11, May, 06, 2014 JFT:__COPY_FLASH2RAM to initialize ram 
 *                          Added ram_code section initialization
 * V1.12, Sep, 29, 2014 JFT:One single default handler
 *                          Device_Data.h not included, user may use CLKVAL1_SSW
 *                          and CLKVAL2_SSW.
 *                          software_init_hook and hardware_init_hook removed
 *                          Misc optimizations
 * V1.13, Dec, 11,2014 JFT:Default clocking changed, MCLK=32MHz and PCLK=64MHz
 * V1.14, Sep, 03,2015 JFT:SSW default clocking changed, MCLK=8MHz and PCLK=16MHz avoid problems with BMI tool timeout
 * V1.15, Jan, 05,2016 JFT:Fix .reset section attributes
 *
 * @endcond 
 */

	.syntax unified   
	.cpu cortex-m0
	.section .reset, "a", %progbits
	.thumb 
	.align 1
 
/* Reset Handler */
	.thumb_func 
	.globl  Reset_Handler
	.type   Reset_Handler, %function
Reset_Handler: 
/* Initialize interrupt veneer */
	ldr	r1, =eROData
	ldr	r2, =VeneerStart
	ldr	r3, =VeneerEnd
	bl  __copy_data

	ldr	r1, =__initial_sp
	mov	sp,r1

	ldr  r0, =SystemInit
	blx  r0
	
/* Initialize data */
	ldr	r1, =DataLoadAddr
	ldr	r2, =__data_start
	ldr	r3, =__data_end
	bl  __copy_data

/* RAM code */
	ldr	r1, =__ram_code_load
	ldr	r2, =__ram_code_start
	ldr	r3, =__ram_code_end
	bl  __copy_data

	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__

	movs	r0, 0

	subs	r2, r1
	ble	.L_loop3_done

.L_loop3:
	subs	r2, #4
	str	r0, [r1, r2]
	bgt	.L_loop3
.L_loop3_done:

	ldr  r0, =__libc_init_array
	blx  r0

	ldr  r0, =main
	blx  r0

	.thumb_func
	.type __copy_data, %function
__copy_data:
/*  The ranges of copy from/to are specified by following symbols
 *    r1: start of the section to copy from.
 *    r2: start of the section to copy to
 *    r3: end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 *  Uses r0
 */
	subs	r3, r2
	ble	.L_loop_done

.L_loop:
	subs	r3, #4
	ldr	r0, [r1,r3]
	str	r0, [r2,r3]
	bgt	.L_loop

.L_loop_done:
	bx  lr

	.pool
	.size   Reset_Handler,.-Reset_Handler

/* ========== START OF EXCEPTION HANDLER DEFINITION ======================== */
/* A couple of macros to ease definition of the various handlers */
.macro Insert_ExceptionHandler Handler_Func 
	.weak \Handler_Func
	.thumb_set \Handler_Func, Default_handler
.endm    

	.align 1
	
	.thumb_func
	.weak Default_handler
	.type Default_handler, %function
Default_handler:
	b  .
	.size Default_handler, . - Default_handler

	Insert_ExceptionHandler HardFault_Handler
	Insert_ExceptionHandler SVC_Handler
	Insert_ExceptionHandler PendSV_Handler
	Insert_ExceptionHandler SysTick_Handler

	Insert_ExceptionHandler SCU_0_IRQHandler
	Insert_ExceptionHandler SCU_1_IRQHandler
	Insert_ExceptionHandler SCU_2_IRQHandler
	Insert_ExceptionHandler ERU0_0_IRQHandler
	Insert_ExceptionHandler ERU0_1_IRQHandler
	Insert_ExceptionHandler ERU0_2_IRQHandler
	Insert_ExceptionHandler ERU0_3_IRQHandler
	Insert_ExceptionHandler MATH0_0_IRQHandler
	Insert_ExceptionHandler VADC0_C0_0_IRQHandler
	Insert_ExceptionHandler VADC0_C0_1_IRQHandler
	Insert_ExceptionHandler VADC0_G0_0_IRQHandler
	Insert_ExceptionHandler VADC0_G0_1_IRQHandler
	Insert_ExceptionHandler VADC0_G1_0_IRQHandler
	Insert_ExceptionHandler VADC0_G1_1_IRQHandler
	Insert_ExceptionHandler CCU40_0_IRQHandler
	Insert_ExceptionHandler CCU40_1_IRQHandler
	Insert_ExceptionHandler CCU40_2_IRQHandler
	Insert_ExceptionHandler CCU40_3_IRQHandler
	Insert_ExceptionHandler CCU80_0_IRQHandler
	Insert_ExceptionHandler CCU80_1_IRQHandler
	Insert_ExceptionHandler POSIF0_0_IRQHandler
	Insert_ExceptionHandler POSIF0_1_IRQHandler
	Insert_ExceptionHandler USIC0_0_IRQHandler
	Insert_ExceptionHandler USIC0_1_IRQHandler
	Insert_ExceptionHandler USIC0_2_IRQHandler
	Insert_ExceptionHandler USIC0_3_IRQHandler
	Insert_ExceptionHandler USIC0_4_IRQHandler
	Insert_ExceptionHandler USIC0_5_IRQHandler
	Insert_ExceptionHandler BCCU0_0_IRQHandler
   
/* ======================================================================== */

/* ==================VENEERS VENEERS VENEERS VENEERS VENEERS=============== */
	.section ".XmcVeneerCode","ax",%progbits
	
	.align 1
	
	.globl HardFault_Veneer
HardFault_Veneer:
	LDR R0, =HardFault_Handler
	MOV PC,R0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
/* ======================================================================== */
	.globl SVC_Veneer
SVC_Veneer:
	LDR R0, =SVC_Handler
	MOV PC,R0
	.long 0
	.long 0
/* ======================================================================== */
	.globl PendSV_Veneer
PendSV_Veneer:
	LDR R0, =PendSV_Handler
	MOV PC,R0
/* ======================================================================== */
	.globl SysTick_Veneer 
SysTick_Veneer:
	LDR R0, =SysTick_Handler
	MOV PC,R0
/* ======================================================================== */
	.globl SCU_0_Veneer 
SCU_0_Veneer:
	LDR R0, =SCU_0_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl SCU_1_Veneer 
SCU_1_Veneer:
	LDR R0, =SCU_1_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl SCU_2_Veneer
SCU_2_Veneer:
	LDR R0, =SCU_2_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl SCU_3_Veneer 
SCU_3_Veneer:
	LDR R0, =ERU0_0_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl SCU_4_Veneer 
SCU_4_Veneer:
	LDR R0, =ERU0_1_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl SCU_5_Veneer 
SCU_5_Veneer:
	LDR R0, =ERU0_2_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl SCU_6_Veneer 
SCU_6_Veneer:
	LDR R0, =ERU0_3_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl MATH_Veneer 
MATH_Veneer:
	LDR R0, =MATH0_0_IRQHandler
	MOV PC,R0
	.long 0
/* ======================================================================== */
	.globl USIC0_0_Veneer
USIC0_0_Veneer:
	LDR R0, =USIC0_0_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl USIC0_1_Veneer
USIC0_1_Veneer:
	LDR R0, =USIC0_1_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl USIC0_2_Veneer
USIC0_2_Veneer:
	LDR R0, =USIC0_2_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl USIC0_3_Veneer
USIC0_3_Veneer:
	LDR R0, =USIC0_3_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl USIC0_4_Veneer
USIC0_4_Veneer:
	LDR R0, =USIC0_4_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl USIC0_5_Veneer
USIC0_5_Veneer:
	LDR R0, =USIC0_5_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl VADC0_C0_0_Veneer 
VADC0_C0_0_Veneer:
	LDR R0, =VADC0_C0_0_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl VADC0_C0_1_Veneer
VADC0_C0_1_Veneer:
	LDR R0, =VADC0_C0_1_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl VADC0_G0_0_Veneer
VADC0_G0_0_Veneer:
	LDR R0, =VADC0_G0_0_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl VADC0_G0_1_Veneer
VADC0_G0_1_Veneer:
	LDR R0, =VADC0_G0_1_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl VADC0_G1_0_Veneer
VADC0_G1_0_Veneer:
	LDR R0, =VADC0_G1_0_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl VADC0_G1_1_Veneer
VADC0_G1_1_Veneer:
	LDR R0, =VADC0_G1_1_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl CCU40_0_Veneer
CCU40_0_Veneer:
	LDR R0, =CCU40_0_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl CCU40_1_Veneer
CCU40_1_Veneer:
	LDR R0, =CCU40_1_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl CCU40_2_Veneer
CCU40_2_Veneer:
	LDR R0, =CCU40_2_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl CCU40_3_Veneer
CCU40_3_Veneer:
	LDR R0, =CCU40_3_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl CCU80_0_Veneer
CCU80_0_Veneer:
	LDR R0, =CCU80_0_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl CCU80_1_Veneer
CCU80_1_Veneer:
	LDR R0, =CCU80_1_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl POSIF0_0_Veneer
POSIF0_0_Veneer:
	LDR R0, =POSIF0_0_IRQHandler
	MOV PC,R0
/* ======================================================================== */
	.globl POSIF0_1_Veneer
POSIF0_1_Veneer:
	LDR R0, =POSIF0_1_IRQHandler
	MOV PC,R0
	.long 0
	.long 0
/* ======================================================================== */
	.globl BCCU0_0_Veneer
BCCU0_0_Veneer:
	LDR R0, =BCCU0_0_IRQHandler
	MOV PC,R0

/* ======================================================================== */
/* ======================================================================== */

/* ============= END OF INTERRUPT HANDLER DEFINITION ======================== */

	.end

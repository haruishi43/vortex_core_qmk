;/*---------------------------------------------------------------------------------------------------------*/
;/* Holtek Semiconductor Inc.                                                                               */
;/*                                                                                                         */
;/* Copyright (C) Holtek Semiconductor Inc.                                                                 */
;/* All rights reserved.                                                                                    */
;/*                                                                                                         */
;/*-----------------------------------------------------------------------------------------------------------
;  File Name        : startup_ht32f5xxxx_iar_05_iap.s
;  Version          : $Rev:: 4129         $
;  Date             : $Date:: 2019-07-24 #$
;  Description      : Startup code.
;-----------------------------------------------------------------------------------------------------------*/

;  Supported Device
;  ========================================
;   HT32F57331, HT32F57341
;   HT32F57342, HT32F57352

;/* <<< Use Configuration Wizard in Context Menu >>>                                                        */

;// <o>  HT32 Device
;//      <0=> By Project Asm Define
;//      <13=> HT32F57331/41
;//      <14=> HT32F57342/52
USE_HT32_CHIP_SET   EQU     0

_HT32FWID           EQU     0xFFFFFFFF
;_HT32FWID           EQU     0x00057331
;_HT32FWID           EQU     0x00057341
;_HT32FWID           EQU     0x00057342
;_HT32FWID           EQU     0x00057352

HT32F57331_41       EQU     13
HT32F57342_52       EQU     14

  IF USE_HT32_CHIP_SET=0
  ELSE
  #undef  USE_HT32_CHIP
  #define USE_HT32_CHIP      USE_HT32_CHIP_SET
  ENDIF

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACKIAP:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  SystemInit
        PUBLIC  __vector_table

;*******************************************************************************
; Fill-up the Vector Table entries with the exceptions ISR address
;*******************************************************************************
                    DATA
_RESERVED           EQU  0xFFFFFFFF
__vector_table
                    DCD  sfe(CSTACKIAP)                     ; ---, 00, 0x000, Top address of Stack
                    DCD  Reset_Handler                      ; ---, 01, 0x004, Reset Handler
                    DCD  _NMI_Handler                        ; -14, 02, 0x008, NMI Handler
                    DCD  _HardFault_Handler                 ; -13, 03, 0x00C, Hard Fault Handler
                    ;DCD  _RESERVED                          ; ---, 04, 0x010, Reserved
                    ;DCD  _RESERVED                          ; ---, 05, 0x014, Reserved
                    ;DCD  _RESERVED                          ; ---, 06, 0x018, Reserved
                    ;DCD  _RESERVED                          ; ---, 07, 0x01C, Reserved
                    ;DCD  _HT32FWID                          ; ---, 08, 0x020, Reserved
                    ;DCD  _RESERVED                          ; ---, 09, 0x024, Reserved
                    ;DCD  _RESERVED                          ; ---, 10, 0x028, Reserved
                    ;DCD  SVC_Handler                        ; -05, 11, 0x02C, SVC Handler
                    ;DCD  _RESERVED                          ; ---, 12, 0x030, Reserved
                    ;DCD  _RESERVED                          ; ---, 13, 0x034, Reserved
                    ;DCD  PendSV_Handler                     ; -02, 14, 0x038, PendSV Handler
                    ;DCD  SysTick_Handler                    ; -01, 15, 0x03C, SysTick Handler


        THUMB

        PUBWEAK Reset_Handler
        SECTION .resetinit:CODE:REORDER:NOROOT(2)
Reset_Handler
                    LDR     R0, =BootProcess
                    BLX     R0
                    ;LDR     R0, =SystemInit
                    ;BLX     R0
                    LDR     R0, =__iar_program_start
                    BX      R0

BootProcess
                    LDR     R0, =0x40080300
                    LDR     R1,[R0, #0x10]
                    CMP     R1, #0
                    BNE     BP1
                    LDR     R1,[R0, #0x14]
                    CMP     R1, #0
                    BNE     BP1
                    LDR     R1,[R0, #0x18]
                    CMP     R1, #0
                    BNE     BP1
                    LDR     R1,[R0, #0x1C]
                    CMP     R1, #0
                    BEQ     BP2
BP1                 LDR     R0, =0x40080180
                    LDR     R1,[R0, #0xC]
                    LSLS    R1, R1, #4
                    LSRS    R1, R1, #20
                    CMP     R1, #0
                    BEQ     BP3
                    CMP     R1, #5
                    BEQ     BP3
                    CMP     R1, #6
                    BEQ     BP3
BP2                 DSB
                    LDR     R0, =0x20000000
                    LDR     R1, =0x05fa0004
                    STR     R1, [R0]
                    LDR     R1, =0xe000ed00
                    LDR     R0, =0x05fa0004
                    STR     R0, [R1, #0xC]
                    DSB
                    B       .
BP3                 LDR     R0, =0x20000000
                    LDR     R1, [R0]
                    LDR     R0, =0x05fa0004
                    CMP     R0, R1
                    BEQ     BP4
                    BX      LR
BP4                 LDR     R0, =0x40088100
                    LDR     R1, =0x00000001
                    STR     R1, [R0]
                    LDR     R0, =0x20000000
                    LDR     R1, =0x0
                    STR     R1, [R0]
                    BX      LR

        PUBWEAK _NMI_Handler
        PUBWEAK _HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
_NMI_Handler
_HardFault_Handler
                    B       .

;*******************************************************************************
; Vector Table entries for SRAM
;*******************************************************************************
        SECTION .intvec_ram:CODE:ROOT(2)
                    DATA
                    DCD  sfe(CSTACKIAP)                    ; Top address of Stack
                    DCD  Reset_Handler                     ; Reset Handler
                    DCD  NMI_Handler                       ; NMI Handler
                    DCD  HardFault_Handler                 ; Hard Fault Handler
                    ;DCD  0                                 ; Reserved
                    ;DCD  0                                 ; Reserved
                    ;DCD  0                                 ; Reserved
                    ;DCD  0                                 ; Reserved
                    ;DCD  0                                 ; Reserved
                    ;DCD  0                                 ; Reserved
                    ;DCD  0                                 ; Reserved
                    ;DCD  SVC_Handler                       ; SVC Handler
                    ;DCD  0                                 ; Reserved
                    ;DCD  0                                 ; Reserved
                    ;DCD  PendSV_Handler                    ; PendSV Handler
                    ;DCD  SysTick_Handler                   ; SysTick Handler
        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
                    B       .

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
                    B       .

        SECTION .usartisr:CODE:ROOT(2)
        EXTERN  UxART_IRQHandler
        DATA
        DCD  UxART_IRQHandler

                    END

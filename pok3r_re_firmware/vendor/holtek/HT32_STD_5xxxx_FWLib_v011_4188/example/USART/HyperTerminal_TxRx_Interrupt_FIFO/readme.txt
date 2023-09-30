/**
 @page USART_HyperTerminal_TxRx_Interrupt_FIFO

 @verbatim
 * @file    USART/HyperTerminal_TxRx_Interrupt_FIFO/readme.txt
 * @version V1.00
 * @date    2014-06-30
 * @brief   Description of USART/UART FIFO example.
 @endverbatim

@par Example Description:

This example describes how to configure USART & UART simultaneously transmit by interrupt.

The USART_CLK frequency is set to 40/48 MHz.

Both USARTs are configured with 8bits Word length, one stop bit, no parity and baud rate is set to 115200 bps,
remember to enable THREIE & RLSIE interrupt for both USARTs.

The USARTx_IRQHandler will wait inputs until USARTx received four characters from hyper terminal, 
then it will turn on LED2 and send these inputs to hyper terminal.When USARTx finish sending these inputs,it will toggle LED1.

@par Directory Contents:

- USART/HyperTerminal_TxRx_Interrupt_FIFO/main.c               Main program
- USART/HyperTerminal_TxRx_Interrupt_FIFO/ht32fxxxx_it.c       Interrupt handlers

@par Hardware and Software Environment:

- Refer "ht32_board_config.h" for pin assignment.
- Connect a null-modem female/female RS232 cable between the USART0 connector
  and PC serial port. HyperTerminal configuration:
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - BaudRate = 115200 baud
  - flow control: None
- This example can be run on HT32 Series development board.

@par Firmware Disclaimer Information

1. The customer hereby acknowledges and agrees that the program technical documentation, including the
   code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
   proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
   other intellectual property laws.

2. The customer hereby acknowledges and agrees that the program technical documentation, including the
   code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
   other than HOLTEK and the customer.

3. The program technical documentation, including the code, is provided "as is" and for customer reference
   only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
   the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
   the warranties of merchantability, satisfactory quality and fitness for a particular purpose.

 * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
 */

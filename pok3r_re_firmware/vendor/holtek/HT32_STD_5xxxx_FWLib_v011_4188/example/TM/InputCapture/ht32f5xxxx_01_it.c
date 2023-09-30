/*********************************************************************************************************//**
 * @file    TM/InputCapture/ht32f5xxxx_01_it.c
 * @version $Rev:: 2970         $
 * @date    $Date:: 2018-08-03#$
 * @brief   This file provides all interrupt service routine.
 *************************************************************************************************************
 * @attention
 *
 * Firmware Disclaimer Information
 *
 * 1. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
 *    proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
 *    other intellectual property laws.
 *
 * 2. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
 *    other than HOLTEK and the customer.
 *
 * 3. The program technical documentation, including the code, is provided "as is" and for customer reference
 *    only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
 *    the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
 *    the warranties of merchantability, satisfactory quality and fitness for a particular purpose.
 *
 * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
 ************************************************************************************************************/

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board_config.h"

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup TM_Examples TM
  * @{
  */

/** @addtogroup InputCapture
  * @{
  */


/* Private types -------------------------------------------------------------------------------------------*/
typedef struct
{
  u32 OverflowCounter;
  u32 StartValue;
  u32 CapturePulse;
  TM_CHP_Enum ChannelPolarity;
  bool DataValid;
  bool DataOverwrite;
} sPulseCaptureStructure;

/* Private variables ---------------------------------------------------------------------------------------*/
sPulseCaptureStructure CaptureCHx;

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
 * @brief   This function handles NMI exception.
 * @retval  None
 ************************************************************************************************************/
void NMI_Handler(void)
{
}

/*********************************************************************************************************//**
 * @brief   This function handles Hard Fault exception.
 * @retval  None
 ************************************************************************************************************/
void HardFault_Handler(void)
{
  #if 1

  static vu32 gIsContinue = 0;
  /*--------------------------------------------------------------------------------------------------------*/
  /* For development FW, MCU run into the while loop when the hardfault occurred.                           */
  /* 1. Stack Checking                                                                                      */
  /*    When a hard fault exception occurs, MCU push following register into the stack (main or process     */
  /*    stack). Confirm R13(SP) value in the Register Window and typing it to the Memory Windows, you can   */
  /*    check following register, especially the PC value (indicate the last instruction before hard fault).*/
  /*    SP + 0x00    0x04    0x08    0x0C    0x10    0x14    0x18    0x1C                                   */
  /*           R0      R1      R2      R3     R12      LR      PC    xPSR                                   */
  while (gIsContinue == 0)
  {
  }
  /* 2. Step Out to Find the Clue                                                                           */
  /*    Change the variable "gIsContinue" to any other value than zero in a Local or Watch Window, then     */
  /*    step out the HardFault_Handler to reach the first instruction after the instruction which caused    */
  /*    the hard fault.                                                                                     */
  /*--------------------------------------------------------------------------------------------------------*/

  #else

  /*--------------------------------------------------------------------------------------------------------*/
  /* For production FW, you shall consider to reboot the system when hardfault occurred.                    */
  /*--------------------------------------------------------------------------------------------------------*/
  NVIC_SystemReset();

  #endif
}

/*********************************************************************************************************//**
 * @brief   This function handles SVCall exception.
 * @retval  None
 ************************************************************************************************************/
void SVC_Handler(void)
{
}

/*********************************************************************************************************//**
 * @brief   This function handles PendSVC exception.
 * @retval  None
 ************************************************************************************************************/
void PendSV_Handler(void)
{
}

/*********************************************************************************************************//**
 * @brief   This function handles SysTick Handler.
 * @retval  None
 ************************************************************************************************************/
void SysTick_Handler(void)
{
}

/*********************************************************************************************************//**
 * @brief   CaptureProcess function.
 * @retval  None
 ************************************************************************************************************/
void CaptureProcess(sPulseCaptureStructure* cap, u16 capture_value, bool isCapBeforeUpdate)
{
  if (cap->ChannelPolarity == TM_CHP_NONINVERTED)
  {
    /* Reset OverflowCounter and store capture value when rising edge occurred                              */
    if (isCapBeforeUpdate)
    {
      cap->OverflowCounter = 1;
    }
    else
    {
      cap->OverflowCounter = 0;
    }

    cap->StartValue = capture_value;

    /* Change channel polarity to capture when falling edge occur                                           */
    cap->ChannelPolarity = TM_CHP_INVERTED;
  }
  else
  {
    /* Compute pulse width in PCLK unit when falling edge occurred                                          */
    if (isCapBeforeUpdate)
      cap->OverflowCounter--;

    cap->CapturePulse = (cap->OverflowCounter << 16) + capture_value - cap->StartValue;

    if (cap->DataValid)
      cap->DataOverwrite = TRUE;
    else
      cap->DataValid = TRUE;

    /* Change channel polarity to capture when rising edge occur                                            */
    cap->ChannelPolarity = TM_CHP_NONINVERTED;
  }
}

/*********************************************************************************************************//**
 * @brief   This function handles General Purpose Timer interrupt.
 * @retval  None
 ************************************************************************************************************/
void GPTM0_IRQHandler(void)
{
  bool update_flag = FALSE;

  /* store and clear all interrupt flags                                                                    */
  u32 status = HTCFG_INPUT_CAPTURE_GPTM->INTSR;
  u32 cnt = HTCFG_INPUT_CAPTURE_GPTM->CNTR;
  if ((status & TM_INT_UEV) != (HTCFG_INPUT_CAPTURE_GPTM->INTSR & TM_INT_UEV))
  {
    status = HTCFG_INPUT_CAPTURE_GPTM->INTSR;
    cnt = HTCFG_INPUT_CAPTURE_GPTM->CNTR;
  }
  HTCFG_INPUT_CAPTURE_GPTM->INTSR = ~status;

  if (status & TM_INT_UEV)
  {
    update_flag = TRUE;
    /* The OverflowCounter will stop at max value 0xFFFF                                                    */
    if (CaptureCHx.OverflowCounter != 0xFFFF) CaptureCHx.OverflowCounter++;
  }

  if (status & HTCFG_INPUT_CAPTURE_GPTM_CCR)
  {
    u32 cap_value = TM_GetCaptureCompare1(HTCFG_INPUT_CAPTURE_GPTM);
    bool isCapBeforeUpdate = (update_flag && (cap_value > cnt))? TRUE : FALSE;
    CaptureProcess(&CaptureCHx, cap_value, isCapBeforeUpdate);
    TM_ChPolarityConfig(HTCFG_INPUT_CAPTURE_GPTM, HTCFG_INPUT_CAPTURE_GPTM_CHANNEL, CaptureCHx.ChannelPolarity);
  }
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/*********************************************************************************************************//**
 * @file    PWRCU/PowerDownMode/ht32f5xxxx_01_it.c
 * @version $Rev:: 4000         $
 * @date    $Date:: 2019-07-05 #$
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
#include "ht32_board.h"

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup PWRCU_Examples PWRCU
  * @{
  */

/** @addtogroup PowerDownMode
  * @{
  */


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
  /* Toggle LED1                                                                                            */
  HT32F_DVB_LEDToggle(HT_LED1);
}

/*********************************************************************************************************//**
 * @brief   This function handles Event Wake Up interrupt.
 * @retval  None
 * @details The EXTI Event Wakeup interrupt handler as following:
 *    - Get PWRCU_BAKSR and RTC_SR
 *    - Check wakeup source:
 *      - If wakeup by the WAKEUP pin : Turn on LED2
 *      - If wakeup by the RTC Compare Match : Turn on LED3
 * @note The RTC_SR and PWRCU_BAKSR is clear by read action.
 ************************************************************************************************************/
void EVWUP_IRQHandler(void)
{
  u32 wBAKSR = PWRCU_GetFlagStatus();
#ifndef USE_HT32F52230_SK
  u32 wRTCSR = RTC_GetFlagStatus();
#endif
  if (wBAKSR & PWRCU_FLAG_PD)
  {
    /* Check wakeup source                                                                                  */
    if (wBAKSR & PWRCU_FLAG_WUP)
    {
      /* If wakeup by the WAKEUP pin : Turn on LED2                                                         */
      HT32F_DVB_LEDOn(HT_LED2);
    }
#ifndef USE_HT32F52230_SK
    if (wRTCSR & RTC_FLAG_CM)
    {
      /* If wakeup by the RTC Compare Match : Turn on LED3                                                  */
      HT32F_DVB_LEDOn(HT_LED3);
    }
#endif
  }
}

/*********************************************************************************************************//**
 * @brief   This function handles KEY1 button interrupt.
 * @retval  None
 * @details The KEY1 interrupt handler as following:
 *    - Clear EXTI channel n flag.
 *    - Wait till WAKEUP pin inactive then enable WAKEUP pin function.
 *    - Wait till RTC count occurs.
 *    - Compare Match in 3 second.
 *    - Read to clear the RTC_SR register.
 *    - Request to enter Power Down Mode (Both the LDO and the DMOS are disable).
 ************************************************************************************************************/
__STATIC_INLINE void KEY1_Button_Process(void)
{
  if (EXTI_GetEdgeFlag(KEY1_BUTTON_EXTI_CHANNEL))
  {
    /* Clear EXTI channel flag                                                                                */
    EXTI_ClearEdgeFlag(KEY1_BUTTON_EXTI_CHANNEL);

    /* Wait till WAKEUP pin inactive                                                                          */
    {
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
    CKCUClock.Bit.PB = 1;
    CKCUClock.Bit.AFIO = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
    }
    AFIO_GPxConfig(WAKEUP_BUTTON_GPIO_ID, WAKEUP_BUTTON_GPIO_PIN, AFIO_FUN_SYSTEM);
    while (HT32F_DVB_PBGetState(BUTTON_WAKEUP));
    PWRCU_WakeupPinCmd(ENABLE);

    #ifndef USE_HT32F52230_SK
    #if 1
    {
      u32 wRtcCounterTmp = 0;
      /* Wait till RTC count occurs                                                                             */
      wRtcCounterTmp = RTC_GetCounter();
      while (RTC_GetCounter() == wRtcCounterTmp);

      /* Compare Match in 3 second                                                                              */
      RTC_SetCompare(RTC_GetCounter()+ 5);

      /* Read to clear the RTC_SR register                                                                      */
      while (RTC_GetFlagStatus());
    }
    #endif
    #endif

    /* Request to enter Power Down Mode (Both the LDO and the DMOS are disable)                               */
    PWRCU_PowerDown();
  }
}


/*********************************************************************************************************//**
 * @brief   This function handles EXTI interrupt.
 * @retval  None
 ************************************************************************************************************/
void EXTI0_1_IRQHandler(void)
{
  #if defined(USE_HT32F52253_SK)
  KEY1_Button_Process();
  #endif

  #if defined(USE_HT32F52341_SK)
  KEY1_Button_Process();
  #endif

  #if defined(USE_HT32F52352_SK)
  KEY1_Button_Process();
  #endif

  #if defined(USE_HT32F0006_DVB)
  KEY1_Button_Process();
  #endif
}

/*********************************************************************************************************//**
 * @brief   This function handles EXTI interrupt.
 * @retval  None
 ************************************************************************************************************/
void EXTI2_3_IRQHandler(void)
{
  #if defined(USE_HT32F52230_SK)
  KEY1_Button_Process();
  #endif

  #if defined(USE_HT32F50230_SK)
  KEY1_Button_Process();
  #endif

  #if defined(USE_HT32F50241_SK)
  KEY1_Button_Process();
  #endif

  #if defined(USE_HT32F0008_SK)
  KEY1_Button_Process();
  #endif

  #if defined(USE_HT32F0006_DVB)
  KEY1_Button_Process();
  #endif
}

/*********************************************************************************************************//**
 * @brief   This function handles EXTI interrupt.
 * @retval  None
 ************************************************************************************************************/
void EXTI4_15_IRQHandler(void)
{
  #if defined(USE_HT32F52241_SK)
  KEY1_Button_Process();
  #endif

  #if defined(USE_HT32F52354_SK)
  KEY1_Button_Process();
  #endif

  #if defined(USE_HT32F57352_SK)
  KEY1_Button_Process();
  #endif

  #if defined(USE_HT32F57341_SK)
  KEY1_Button_Process();
  #endif

  #if defined(USE_HT32F52367_SK)
  KEY1_Button_Process();
  #endif
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

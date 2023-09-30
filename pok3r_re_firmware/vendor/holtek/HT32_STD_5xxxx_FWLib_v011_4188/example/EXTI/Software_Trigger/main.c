/*********************************************************************************************************//**
 * @file    EXTI/Software_Trigger/main.c
 * @version $Rev:: 3913         $
 * @date    $Date:: 2019-05-23 #$
 * @brief   Main program.
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

/** @addtogroup EXTI_Examples EXTI
  * @{
  */

/** @addtogroup Software_Trigger
  * @{
  */

/* Private function prototypes -----------------------------------------------------------------------------*/
void EXTI_Configuration(void);

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
int main(void)
{
  /* Initialize LEDs                                                                                        */
  HT32F_DVB_LEDInit(HT_LED1);
  HT32F_DVB_LEDInit(HT_LED2);
  HT32F_DVB_LEDInit(HT_LED3);

  EXTI_Configuration();
  /* Activate the corresponding EXTI interrupt                                                              */
  EXTI_SWIntCmd(EXTI_CHANNEL_0, ENABLE);

  while (1);
}

/*********************************************************************************************************//**
  * @brief  EXTI configuration.
  * @retval None
  ***********************************************************************************************************/
void EXTI_Configuration(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  /* Enable EXTI & PE APB clock                                                                             */
  CKCUClock.Bit.PB         = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCUClock.Bit.EXTI       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  /* Select Port as EXTI Trigger Source                                                                     */
  AFIO_EXTISourceConfig(AFIO_EXTI_CH_0, AFIO_ESS_PB);

  /* Enable GPIO Input Function                                                                             */
  GPIO_InputConfig(HT_GPIOB, GPIO_PIN_0, ENABLE);
  GPIO_PullResistorConfig(HT_GPIOB, GPIO_PIN_0, GPIO_PR_UP);
  
  /* Enable EXTI_CHANNEL NVIC Interrupt Channel                                                             */
  NVIC_EnableIRQ(EXTI0_1_IRQn);

  /* Enable EXTI line Interrupt                                                                             */
  EXTI_IntConfig(EXTI_CHANNEL_0, ENABLE);
}

#if (HT32_LIB_DEBUG == 1)
/*********************************************************************************************************//**
  * @brief  Report both the error name of the source file and the source line number.
  * @param  filename: pointer to the source file name.
  * @param  uline: error line source number.
  * @retval None
  ***********************************************************************************************************/
void assert_error(u8* filename, u32 uline)
{
  /*
     This function is called by IP library that the invalid parameters has been passed to the library API.
     Debug message can be added here.
     Example: printf("Parameter Error: file %s on line %d\r\n", filename, uline);
  */

  while (1)
  {
  }
}
#endif


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

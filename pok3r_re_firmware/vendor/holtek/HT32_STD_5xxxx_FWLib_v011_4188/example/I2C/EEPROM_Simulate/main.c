/*********************************************************************************************************//**
 * @file    I2C/EEPROM_Simulate/main.c
 * @version $Rev:: 2959         $
 * @date    $Date:: 2018-08-02 #$
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
#include "i2cm.h"
#include "ht32_board_config.h"

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup I2C_Examples I2C
  * @{
  */

/** @addtogroup EEPROM_Simulate
  * @{
  */


/* Private constants ---------------------------------------------------------------------------------------*/
#define EEPROM_ADDRESS (0x61)

/* Private function prototypes -----------------------------------------------------------------------------*/
void I2CM_Init (void);
u32 I2CM_BufferRead(u16 dev, u8 word_addr, u8* data, u16 count);
u32 I2CM_BufferWrite(u16 dev, u8 word_addr, u8* data, u16 count);

/* Private variables ---------------------------------------------------------------------------------------*/
uc8 TestData[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0A, 0xB, 0xC, 0xD, 0xE, 0xF};
u8 ReadBuffer[16];

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
int main(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  I2C_InitTypeDef I2C_InitStructure = { 0 };
  HT32F_DVB_LEDInit(HT_LED1);
  HT32F_DVB_LEDInit(HT_LED2);

  /* Enable GPIO & AFIO APB clock                                                                           */
  CKCUClock.Bit.AFIO = 1;
  CKCUClock.Bit.PA   = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  /*Init Key 1                                                                                              */
  HT32F_DVB_PBInit(BUTTON_KEY1, BUTTON_MODE_GPIO);

  /* Read the Key1 state to select either I2C master or EEPROM function                                     */
#ifndef LOOP_BACK
  if (!HT32F_DVB_PBGetState(BUTTON_KEY1))
#endif
  {
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
    /******************************
     *  EEPROM simulation         *
     ******************************/

    /*Enable NVIC EEPROM interrupt                                                                          */
    NVIC_EnableIRQ(HTCFG_I2C_EEPROM_IRQn);

    /* Enable EEPROM APB clock                                                                              */
    HTCFG_I2C_EEPROM_CLK(CKCUClock)   = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);

    /* Configure SDA and SCL to I2C1 mode                                                                   */

    AFIO_GPxConfig(HTCFG_I2C_EEPROM_SCL_GPIO_ID, HTCFG_I2C_EEPROM_SCL_AFIO_PIN, AFIO_FUN_I2C);
    AFIO_GPxConfig(HTCFG_I2C_EEPROM_SDA_GPIO_ID, HTCFG_I2C_EEPROM_SDA_AFIO_PIN, AFIO_FUN_I2C);
    /* EEPROM configuration                                                                                 */
    I2C_InitStructure.I2C_GeneralCall = DISABLE;
    I2C_InitStructure.I2C_AddressingMode = I2C_ADDRESSING_7BIT;
    I2C_InitStructure.I2C_Acknowledge = ENABLE;
    I2C_InitStructure.I2C_OwnAddress = EEPROM_ADDRESS;
    I2C_InitStructure.I2C_Speed = 400000;
    I2C_InitStructure.I2C_SpeedOffset = 0;
    I2C_Init(HTCFG_I2C_EEPROM_PORT, &I2C_InitStructure);

    /* Enable EEPROM                                                                                        */
    I2C_Cmd(HTCFG_I2C_EEPROM_PORT, ENABLE);

    /* Enable EEPROM interrupts                                                                             */
    I2C_IntConfig(HTCFG_I2C_EEPROM_PORT, I2C_INT_ADRS | I2C_INT_RXDNE | I2C_INT_TXDE | I2C_INT_RXNACK | I2C_INT_STO, ENABLE);
  }
#ifndef LOOP_BACK
  else
#endif
  {
    /******************************
     *  I2C master                *
     ******************************/
    I2CM_Init();
    if (I2CM_BufferWrite(EEPROM_ADDRESS, 0x00, (u8*)TestData, 16) != I2CM_OK)
    {
      /* Write fail                                                                                         */
      HT32F_DVB_LEDOn(HT_LED2);
      while (1);

    }
    if (I2CM_BufferRead(EEPROM_ADDRESS, 0x00, ReadBuffer, 16) != I2CM_OK)
    {
      /* Read fail                                                                                          */
      HT32F_DVB_LEDOn(HT_LED2);
      while (1);
    }
  }
  HT32F_DVB_LEDOn(HT_LED1);
  /* Infinite loop                                                                                          */
  while (1);
}

/*********************************************************************************************************//**
 * @brief   This function is used to init the I2C master.
 * @retval  None
 ************************************************************************************************************/
void I2CM_Init(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
  I2C_InitTypeDef  I2C_InitStructure = { 0 };

  /* Enable NVIC I2C interrupt                                                                              */
  NVIC_EnableIRQ(HTCFG_I2C_MASTER_IRQn);
  /* Enable I2C & AFIO APB clock                                                                            */
  HTCFG_I2C_MASTER_CLK(CKCUClock) = 1;
  CKCUClock.Bit.AFIO              = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  /* Configure AFIO pins as I2C mode                                                                        */
  //AFIO_GPBConfig(AFIO_PIN_12 | AFIO_PIN_13, AFIO_MODE_7);
  AFIO_GPxConfig(HTCFG_I2C_MASTER_SCL_GPIO_ID, HTCFG_I2C_MASTER_SCL_AFIO_PIN, AFIO_FUN_I2C);
  AFIO_GPxConfig(HTCFG_I2C_MASTER_SCL_GPIO_ID, HTCFG_I2C_MASTER_SDA_AFIO_PIN, AFIO_FUN_I2C);
  /* I2C configuration                                                                                      */
  I2C_InitStructure.I2C_GeneralCall     = DISABLE;
  I2C_InitStructure.I2C_AddressingMode  = I2C_ADDRESSING_7BIT;
  I2C_InitStructure.I2C_Acknowledge     = DISABLE;
  I2C_InitStructure.I2C_OwnAddress      = 0x7F;
  I2C_InitStructure.I2C_Speed           = 400000;
  I2C_InitStructure.I2C_SpeedOffset     = 0;
  I2C_Init(HTCFG_I2C_MASTER_PORT, &I2C_InitStructure);

  /* Enable I2C                                                                                             */
  I2C_Cmd(HTCFG_I2C_MASTER_PORT, ENABLE);

  /* Enable I2C interrupts                                                                                  */
  I2C_IntConfig(HTCFG_I2C_MASTER_PORT, I2C_INT_STA | I2C_INT_ADRS | I2C_INT_RXDNE | I2C_INT_TXDE
                    | I2C_INT_ARBLOS | I2C_INT_RXNACK | I2C_INT_BUSERR | I2C_INT_TOUT , ENABLE);

  return;
}

/*********************************************************************************************************//**
 * @brief   I2C master read data from I2C slave.
 * @retval  None
 ************************************************************************************************************/
u32 I2CM_BufferRead(u16 dev, u8 word_addr, u8* data, u16 count)
{
  I2CM_Transfer.RegAddr = word_addr;
  I2CM_Transfer.Buffer      = data;
  I2CM_Transfer.Length      = count;
  I2CM_Transfer.Counter     = 0;
  I2CM_Transfer.Direction   = I2CM_DIRECTION_IN;
  I2CM_Transfer.DevAddr     = dev;
  I2CM_Transfer.Locked      = TRUE;
  I2CM_Transfer.RetryDownCounter = I2CM_BUS_MAX_RETRY;

  /* Send I2C START                                                                                         */
  I2C_TargetAddressConfig(HTCFG_I2C_MASTER_PORT, I2CM_Transfer.DevAddr, I2C_MASTER_WRITE);

  while (I2CM_Transfer.Locked);
  while (I2C_GetFlagStatus(HTCFG_I2C_MASTER_PORT, I2C_FLAG_BUSBUSY));

  I2CM_Transfer.Buffer = NULL;

  return (u32)I2CM_Transfer.Status;
}

/*********************************************************************************************************//**
 * @brief   I2C master write data to I2C slave.
 * @retval  None
 ************************************************************************************************************/
u32 I2CM_BufferWrite(u16 dev, u8 word_addr, u8* data, u16 count)
{
  I2CM_Transfer.RegAddr     = word_addr;
  I2CM_Transfer.Buffer      = data;
  I2CM_Transfer.Length      = count;
  I2CM_Transfer.Counter     = 0;
  I2CM_Transfer.Direction   = I2CM_DIRECTION_OUT;
  I2CM_Transfer.DevAddr     = dev;
  I2CM_Transfer.Locked      = TRUE;
  I2CM_Transfer.RetryDownCounter = I2CM_BUS_MAX_RETRY;

  /* Send I2C START                                                                                         */
  I2C_TargetAddressConfig(HTCFG_I2C_MASTER_PORT, I2CM_Transfer.DevAddr, I2C_MASTER_WRITE);

  while (I2CM_Transfer.Locked);
  while (I2C_GetFlagStatus(HTCFG_I2C_MASTER_PORT, I2C_FLAG_BUSBUSY));

  I2CM_Transfer.Buffer = NULL;

  return (u32)I2CM_Transfer.Status;
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

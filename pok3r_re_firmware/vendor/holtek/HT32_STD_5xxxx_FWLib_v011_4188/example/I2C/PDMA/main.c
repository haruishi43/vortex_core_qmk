/*********************************************************************************************************//**
 * @file    I2C/PDMA/main.c
 * @version $Rev:: 2492         $
 * @date    $Date:: 2018-01-04 #$
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
#include "ht32_board_config.h"

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup I2C_Examples I2C
  * @{
  */

/** @addtogroup I2C_PDMA
  * @{
  */


/* Private types -------------------------------------------------------------------------------------------*/
typedef enum {Fail = 0, Pass = !Fail} TestResult;

/* Private constants ---------------------------------------------------------------------------------------*/
#define I2C_MASTER_ADDRESS     0x0A
#define I2C_SLAVE_ADDRESS      0x60
#define BufferSize             16
#define ClockSpeed             1000000

/* Private function prototypes -----------------------------------------------------------------------------*/
TestResult CmpBuffer(u8* Buffer1, u8* Buffer2, u32 BufferLength);

/* Private variables ---------------------------------------------------------------------------------------*/
I2C_InitTypeDef  I2C_InitStructure;
PDMACH_InitTypeDef PDMACH_InitStructure;
u8 I2C_Master_Buffer_Rx[BufferSize] = {0};
u8 I2C_Slave_Buffer_Tx[BufferSize] = {0x00, 0x11, 0x22, 0x33,
                                      0x44, 0x55, 0x66, 0x77,
                                      0x88, 0x99, 0xAA, 0xBB,
                                      0xCC, 0xDD, 0xEE, 0xFF};

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
int main(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};

  /* Initialize LED1 & LED2 on HT32 board                                                                   */
  HT32F_DVB_LEDInit(HT_LED1);
  HT32F_DVB_LEDInit(HT_LED2);

  /* Enable PDMA clock                                                                                      */
  /* Enable I2C0 ,I2C1 & AFIO clock                                                                         */
  CKCUClock.Bit.PDMA       = 1;
  CKCUClock.Bit.I2C0       = 1;
  CKCUClock.Bit.I2C1       = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  /* I2C Master Rx PDMA Channel configuration                                                               */
  PDMACH_InitStructure.PDMACH_SrcAddr = (u32) &HTCFG_I2C_MASTER_PORT->DR;
  PDMACH_InitStructure.PDMACH_DstAddr = (u32) &I2C_Master_Buffer_Rx;
  PDMACH_InitStructure.PDMACH_BlkCnt = BufferSize;
  PDMACH_InitStructure.PDMACH_BlkLen = 1;
  PDMACH_InitStructure.PDMACH_DataSize = WIDTH_8BIT;
  PDMACH_InitStructure.PDMACH_Priority = H_PRIO;
  PDMACH_InitStructure.PDMACH_AdrMod = SRC_ADR_FIX | DST_ADR_LIN_INC;
  PDMA_Config(HTCFG_I2C_MASTER_RX_DMA, &PDMACH_InitStructure);

  /* I2C1 Slave Tx PDMA Channel configuration                                                               */
  PDMACH_InitStructure.PDMACH_SrcAddr = (u32) &I2C_Slave_Buffer_Tx;
  PDMACH_InitStructure.PDMACH_DstAddr = (u32) &HT_I2C0->DR;
  PDMACH_InitStructure.PDMACH_Priority = M_PRIO;
  PDMACH_InitStructure.PDMACH_AdrMod = SRC_ADR_LIN_INC | DST_ADR_FIX;
  PDMA_Config(HTCFG_I2C_SLAVE_TX_DMA, &PDMACH_InitStructure);

  /* Enable I2C Rx GE & TC interrupt                                                                        */
  PDMA_IntConfig(HTCFG_I2C_MASTER_RX_DMA, PDMA_INT_GE | PDMA_INT_TC, ENABLE);

  /* Enable I2C Tx GE & TC interrupt                                                                        */
  PDMA_IntConfig(HTCFG_I2C_SLAVE_TX_DMA, PDMA_INT_GE | PDMA_INT_TC, ENABLE);

  /* Enable the corresponding PDMA channel                                                                  */
  PDMA_EnaCmd(HTCFG_I2C_MASTER_RX_DMA, ENABLE);
  PDMA_EnaCmd(HTCFG_I2C_SLAVE_TX_DMA, ENABLE);

/* Configure GPIO to I2C mode for Master                                                                  */
  AFIO_GPxConfig(HTCFG_I2C_MASTER_SCL_GPIO_ID, HTCFG_I2C_MASTER_SCL_AFIO_PIN, AFIO_FUN_I2C);
  AFIO_GPxConfig(HTCFG_I2C_MASTER_SDA_GPIO_ID, HTCFG_I2C_MASTER_SDA_AFIO_PIN, AFIO_FUN_I2C);

  /* Configure GPIO to I2C mode for Slave                                                                   */
  AFIO_GPxConfig(HTCFG_I2C_SLAVE_SCL_GPIO_ID, HTCFG_I2C_SLAVE_SCL_AFIO_PIN, AFIO_FUN_I2C);
  AFIO_GPxConfig(HTCFG_I2C_SLAVE_SDA_GPIO_ID, HTCFG_I2C_SLAVE_SDA_AFIO_PIN, AFIO_FUN_I2C);

  /* I2C Master configuration                                                                               */
  I2C_InitStructure.I2C_GeneralCall = DISABLE;
  I2C_InitStructure.I2C_AddressingMode = I2C_ADDRESSING_7BIT;
  I2C_InitStructure.I2C_Acknowledge = ENABLE;
  I2C_InitStructure.I2C_OwnAddress = I2C_MASTER_ADDRESS;
  I2C_InitStructure.I2C_Speed = ClockSpeed;
  I2C_InitStructure.I2C_SpeedOffset = 0;
  I2C_Init(HTCFG_I2C_MASTER_PORT, &I2C_InitStructure);

  /* I2C Slave configuration                                                                                */
  I2C_InitStructure.I2C_Acknowledge = ENABLE;
  I2C_InitStructure.I2C_OwnAddress = I2C_SLAVE_ADDRESS;
  I2C_Init(HTCFG_I2C_SLAVE_PORT, &I2C_InitStructure);

  /* Enable I2C0 & I2C1                                                                                     */
  I2C_Cmd(HTCFG_I2C_MASTER_PORT, ENABLE);
  I2C_Cmd(HTCFG_I2C_SLAVE_PORT, ENABLE);

  /* Enable PDMA send NACK automatically                                                                    */
  I2C_PDMANACKCmd(HTCFG_I2C_MASTER_PORT, ENABLE);

  /* Send I2C START & I2C slave address for read                                                            */
  I2C_TargetAddressConfig(HTCFG_I2C_MASTER_PORT, I2C_SLAVE_ADDRESS, I2C_MASTER_READ);

  /* Check on Master STA condition and clear it                                                             */
  while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_SEND_START));

  /* Check on Slave Transmitter ADRS condition and clear it                                                 */
  while (!I2C_CheckStatus(HTCFG_I2C_SLAVE_PORT, I2C_SLAVE_ACK_TRANSMITTER_ADDRESS));

  /* Check on Master Receiver ADRS condition and clear it                                                   */
  while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_RECEIVER_MODE));

  /* Enable master I2C Master PDMA Rx request                                                               */
  I2C_PDMACmd(HTCFG_I2C_MASTER_PORT, I2C_PDMAREQ_RX, ENABLE);
  /* Enable slave I2C Slave PDMA Tx request                                                                 */
  I2C_PDMACmd(HTCFG_I2C_SLAVE_PORT, I2C_PDMAREQ_TX, ENABLE);

  /* wait for transmission finished                                                                         */
  while (!PDMA_GetFlagStatus(HTCFG_I2C_SLAVE_TX_DMA, PDMA_FLAG_TC));

  /* Send I2C STOP condition                                                                                */
  I2C_GenerateSTOP(HTCFG_I2C_MASTER_PORT);

  /* Check on Slave Receiver NACK & STO condition                                                           */
  while (!I2C_CheckStatus(HTCFG_I2C_SLAVE_PORT, I2C_SLAVE_RECEIVED_NACK_STOP));

  /* Clear I2C1 RXNACK flag                                                                                 */
  I2C_ClearFlag(HTCFG_I2C_SLAVE_PORT, I2C_FLAG_RXNACK);

  /* Check on validity of received data on I2C Master Buffer_Rx                                             */
  if (CmpBuffer(I2C_Master_Buffer_Rx, I2C_Slave_Buffer_Tx, BufferSize))
  {
    HT32F_DVB_LEDOn(HT_LED1);
  }
  else
  {
    HT32F_DVB_LEDOn(HT_LED2);
  }

  while (1);

}

/*********************************************************************************************************//**
  * @brief  Compare two buffers.
  * @param  Buffer1, Buffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval None
  ***********************************************************************************************************/
TestResult CmpBuffer(u8* Buffer1, u8* Buffer2, u32 BufferLength)
{
  while (BufferLength--)
  {
    if (*Buffer1 != *Buffer2)
    {
      return Fail;
    }

    Buffer1++;
    Buffer2++;
  }

  return Pass;
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

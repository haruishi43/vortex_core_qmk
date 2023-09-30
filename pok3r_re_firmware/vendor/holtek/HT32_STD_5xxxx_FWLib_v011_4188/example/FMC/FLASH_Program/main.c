/*********************************************************************************************************//**
 * @file    FMC/FLASH_Program/main.c
 * @version $Rev:: 4166         $
 * @date    $Date:: 2019-07-28 #$
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

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup FMC_Examples FMC
  * @{
  */

/** @addtogroup FLASH_Program
  * @{
  */


/* Private constants ---------------------------------------------------------------------------------------*/
#define StartAddress    (1024 * 4)
#define EndAddress      (1024 * 8)

#define TEST_DATA       (0x12345678)

/* Private function prototypes -----------------------------------------------------------------------------*/
void FLASH_Erase(void);
void FLASH_Program(void);
void FLASH_Check(void);

/* Global variables ----------------------------------------------------------------------------------------*/
u32 err;

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
int main(void)
{
  /* Erase Flash pages before program data                                                                  */
  FLASH_Erase();
  /* Program Word data                                                                                      */
  FLASH_Program();
  /* Check Flash data                                                                                       */
  FLASH_Check();
  if (err != 0)
  {
    while (1); /* Test Failed                                                                               */
  }

  while (1); /* Test Passed                                                                                 */
}

/*********************************************************************************************************//**
  * @brief Erase FLASH Byte page.
  * @retval None
 ************************************************************************************************************/
void FLASH_Erase(void)
{
  u32 Addr;
  FLASH_State FLASHState;
  for (Addr = StartAddress; Addr < EndAddress; Addr += FLASH_PAGE_SIZE)
  {
    FLASHState = FLASH_ErasePage(Addr);
    if (FLASHState != FLASH_COMPLETE)
    {
      err++;
    }
  }
}

/*********************************************************************************************************//**
  * @brief Program FLASH Byte page.
  * @retval None
 ************************************************************************************************************/
void FLASH_Program(void)
{
  u32 Addr;
  FLASH_State FLASHState;
  for (Addr = StartAddress; Addr < EndAddress; Addr += 4)
  {
    FLASHState = FLASH_ProgramWordData(Addr, TEST_DATA);
    if (FLASHState != FLASH_COMPLETE)
    {
      err++;
    }
  }
}

/*********************************************************************************************************//**
  * @brief Check FLASH Byte page.
  * @retval None
 ************************************************************************************************************/
void FLASH_Check(void)
{
  u32 Addr;

  for (Addr = StartAddress; Addr < EndAddress; Addr += 4)
  {
    if (rw(Addr) != TEST_DATA)
    {
      err++;
    }
  }
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

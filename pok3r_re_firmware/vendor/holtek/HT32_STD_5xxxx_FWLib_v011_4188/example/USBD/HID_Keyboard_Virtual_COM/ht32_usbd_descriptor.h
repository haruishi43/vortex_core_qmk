/*********************************************************************************************************//**
 * @file    USBD/HID_Keyboard_Virtual_COM/ht32_usbd_descriptor.h
 * @version $Rev:: 2873         $
 * @date    $Date:: 2018-06-29 #$
 * @brief   The USB descriptor.
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

/* Define to prevent recursive inclusion -------------------------------------------------------------------*/
#ifndef __HT32_USBD_DESCRIPTOR_H
#define __HT32_USBD_DESCRIPTOR_H

/* Includes ------------------------------------------------------------------------------------------------*/

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup USBD_Examples USBD
  * @{
  */

/** @addtogroup HID_Keyboard_Virtual_COM
  * @{
  */


/* Exported constants --------------------------------------------------------------------------------------*/

#define DESC_LEN_CONFN_T_KB       (DESC_LEN_CONFN + DESC_LEN_INF + DESC_LEN_HID + DESC_LEN_EPT * 1)

#define DESC_LEN_CONFN_T_CDC      (8 + DESC_LEN_INF * 2 + CDC_HEADER_LEN \
                                        + CDC_ABSTRACT_LEN + CDC_UNION_LEN + CDC_CALL_LEN  \
                                        + DESC_LEN_EPT * 3)

#define DESC_LEN_CONFN_T          (u16)(DESC_LEN_CONFN_T_KB + DESC_LEN_CONFN_T_CDC)


/* Exported functions --------------------------------------------------------------------------------------*/
void USBDDesc_Init(USBDCore_Desc_TypeDef *pDesc);


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* __HT32_USBD_DESCRIPTOR_H -------------------------------------------------------------------------*/

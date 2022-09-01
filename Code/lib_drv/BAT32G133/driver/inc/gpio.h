/***********************************************************************************************************************
* Copyright (C) . All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : gpio.h
* Version      :  
* Device(s)    : BAT32G133
* Tool-Chain   : MDK(armcc)
* Description  : This file implements device driver for GPIO module.
* Creation Date: 2019/6/26
***********************************************************************************************************************/
#ifndef GPIO_H
#define GPIO_H
#ifndef __CORE_CM0PLUS_H_GENERIC
#include "BAT32G133.h"
#endif

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum {
	PORT0 = 0,
	PORT1,
	PORT2,
	PORT3,
	
}PORT_TypeDef;

typedef enum {
	PIN0 = 0,
	PIN1,
	PIN2,
	PIN3,
	PIN4,
	PIN5,
	PIN6,
	PIN7,
	
}PIN_TypeDef;

typedef enum {
	INPUT = 0,
	PULLUP_INPUT,
	TTL_INPUT,
	ANALOG_INPUT,
	OUTPUT,
	OPENDRAIN_OUTPUT,
	PULLDOWN_INPUT,
}PIN_ModeDef;

typedef enum {
	P01_CFG = 0x02,
  P02_CFG = 0x04,
  P10_CFG = 0x10,
  P11_CFG = 0x12,
  P12_CFG = 0x14,
  P13_CFG = 0x16,
  P20_CFG = 0x20,
  P21_CFG = 0x22,
  P22_CFG = 0x24,
  P23_CFG = 0x26,
  P24_CFG = 0x28,
  P25_CFG = 0x2A,
  P26_CFG = 0x2C,
  P30_CFG = 0x30,
  P31_CFG = 0x32,
  P32_CFG = 0x34,
  P33_CFG = 0x36,
  P34_CFG = 0x38,
  P35_CFG = 0x3A,
  P36_CFG = 0x3C,
  P37_CFG = 0x3E,
}IO_CfgType;

typedef enum {
	IOCFG_INTP0 = 0x02,
	IOCFG_INTP1 = 0x03,
	IOCFG_INTP2 = 0x04,
	IOCFG_INTP3 = 0x05,
	IOCFG_TI00 = 0x06,
  IOCFG_TI01 = 0x07,
  IOCFG_TI02 = 0x08,
  IOCFG_TI03 = 0x09,
  IOCFG_TI10 = 0x0A,
  IOCFG_TI11 = 0x0B,
  IOCFG_TI12 = 0x0C,
  IOCFG_TI13 = 0x0D,
  IOCFG_TO00 = 0x0E,
  IOCFG_TO01 = 0x0F,
  IOCFG_TO02 = 0x10,
  IOCFG_TO03 = 0x11,
  IOCFG_TO10 = 0x12,
  IOCFG_TO11 = 0x13,
  IOCFG_TO12 = 0x14,
  IOCFG_TO13 = 0x15,
  IOCFG_SCLA0 = 0x16,
  IOCFG_SDAA0 = 0x17,
  IOCFG_CLKBUZ0 = 0x18,
  IOCFG_CLKBUZ1 = 0x19,
  IOCFG_VCOUT0 = 0x1A,
  IOCFG_VCOUT1 = 0x1B,
  IOCFG_RTC1HZ = 0x1C,
  IOCFG_SS00 = 0x1F,
  IOCFG_SCLK00 = 0x20,
  IOCFG_SCLK01 = 0x21,
  IOCFG_SCLK10 = 0x22,
  IOCFG_SCLK11 = 0x23,
  IOCFG_SCLK20 = 0x24,
  IOCFG_SCLK21 = 0x25,
  IOCFG_RXD0 = 0x26,
	IOCFG_SDI00 = 0x26,
  IOCFG_SDI01 = 0x27,
  IOCFG_RXD1 = 0x28,
	IOCFG_SDI10 = 0x28,
  IOCFG_SDI11 = 0x29,
  IOCFG_RXD2 = 0x2A,
	IOCFG_SDI20 = 0x2A,
  IOCFG_SDI21 = 0x2B,
  IOCFG_TXD0 = 0x2C,
	IOCFG_SDO00 = 0x2C,
  IOCFG_SDO01 = 0x2D,
  IOCFG_TXD1 = 0x2E,
	IOCFG_SDO10 = 0x2E,
  IOCFG_SDO11 = 0x2F,
  IOCFG_TXD2 = 0x30,
	IOCFG_SDO20 = 0x30,
  IOCFG_SDO21 = 0x31,
}IO_CfgDef;

#define PIN_MASK_0                 ((uint8_t)(0x01))  /*!< Pin 0 selected    */
#define PIN_MASK_1                 ((uint8_t)(0x02))  /*!< Pin 1 selected    */
#define PIN_MASK_2                 ((uint8_t)(0x04))  /*!< Pin 2 selected    */
#define PIN_MASK_3                 ((uint8_t)(0x08))  /*!< Pin 3 selected    */
#define PIN_MASK_4                 ((uint8_t)(0x10))  /*!< Pin 4 selected    */
#define PIN_MASK_5                 ((uint8_t)(0x20))  /*!< Pin 5 selected    */
#define PIN_MASK_6                 ((uint8_t)(0x40))  /*!< Pin 6 selected    */
#define PIN_MASK_7                 ((uint8_t)(0x80))  /*!< Pin 7 selected    */


/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void GPIO_Output_Enable(__IO uint8_t *port, uint8_t pinMsk);
void GPIO_Input_Enable(__IO uint8_t *port, uint8_t pinMsk);
void GPIO_PullUp_Enable(__IO uint8_t *port, uint8_t pinMsk);
void GPIO_PullUp_Disable(__IO uint8_t *port, uint8_t pinMsk);
void GPIO_PullDown_Enable(__IO uint8_t *port, uint8_t pinMsk);
void GPIO_PullDown_Disable(__IO uint8_t *port, uint8_t pinMsk);
void GPIO_Nch_OpenDrain(__IO uint8_t *port, uint8_t pinMsk);
void GPIO_Set_Value(__IO uint8_t *port, uint8_t value);
uint8_t GPIO_Get_Value(__IO uint8_t *port);


void PORT_Init(PORT_TypeDef PORTx,PIN_TypeDef PINx,PIN_ModeDef MODEx);
void PORT_SetBit(PORT_TypeDef PORTx,PIN_TypeDef PINx);
void PORT_ClrBit(PORT_TypeDef PORTx,PIN_TypeDef PINx);
void PORT_ToggleBit(PORT_TypeDef PORTx,PIN_TypeDef PINx);
uint8_t PORT_GetBit(PORT_TypeDef PORTx,PIN_TypeDef PINx);
void PORT_Cfg(IO_CfgType PORT_CFGx, IO_CfgDef CFGx);

#endif

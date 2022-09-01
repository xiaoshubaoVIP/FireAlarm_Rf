/***********************************************************************************************************************
* Copyright (C) All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* @file    userdefine.h
* @brief   This file includes user definition.
* @version 1.0.0
* @date    2019/12/24
***********************************************************************************************************************/
#ifndef _USER_DEF_H
#define _USER_DEF_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#ifndef __TYPEDEF__

typedef unsigned short 	MD_STATUS;
/* Status list definition */
#define MD_STATUSBASE        (0x00U)
#define MD_OK                (MD_STATUSBASE + 0x00U) /* register setting OK */
#define MD_SPT               (MD_STATUSBASE + 0x01U) /* IIC stop */
#define MD_NACK              (MD_STATUSBASE + 0x02U) /* IIC no ACK */
#define MD_BUSY1             (MD_STATUSBASE + 0x03U) /* busy 1 */
#define MD_BUSY2             (MD_STATUSBASE + 0x04U) /* busy 2 */
#define MD_OVERRUN           (MD_STATUSBASE + 0x05U) /* IIC OVERRUN occur */

/* Error list definition */
#define MD_ERRORBASE         (0x80U)
#define MD_ERROR             (MD_ERRORBASE + 0x00U)  /* error */
#define MD_ARGERROR          (MD_ERRORBASE + 0x01U)  /* error agrument input error */
#define MD_ERROR1            (MD_ERRORBASE + 0x02U)  /* error 1 */
#define MD_ERROR2            (MD_ERRORBASE + 0x03U)  /* error 2 */
#define MD_ERROR3            (MD_ERRORBASE + 0x04U)  /* error 3 */
#define MD_ERROR4            (MD_ERRORBASE + 0x05U)  /* error 4 */
#define MD_ERROR5            (MD_ERRORBASE + 0x06U)  /* error 5 */

/* IO Alternative definition */
#define PTCFG_INTP0            (0x02U)
#define PTCFG_INTP1            (0x03U)
#define PTCFG_INTP2            (0x04U)
#define PTCFG_INTP3            (0x05U)
#define PTCFG_TI00             (0x06U)
#define PTCFG_TI01             (0x07U)
#define PTCFG_TI02             (0x08U)
#define PTCFG_TI03             (0x09U)
#define PTCFG_TI10             (0x0AU)
#define PTCFG_TI11             (0x0BU)
#define PTCFG_TI12             (0x0CU)
#define PTCFG_TI13             (0x0DU)
#define PTCFG_TO00             (0x0EU)
#define PTCFG_TO01             (0x0FU)
#define PTCFG_TO02             (0x10U)
#define PTCFG_TO03             (0x11U)
#define PTCFG_TO10             (0x12U)
#define PTCFG_TO11             (0x13U)
#define PTCFG_TO12             (0x14U)
#define PTCFG_TO13             (0x15U)
#define PTCFG_SCLA0            (0x16U)
#define PTCFG_SDAA0            (0x17U)
#define PTCFG_CLKBUZ0          (0x18U)
#define PTCFG_CLKBUZ1          (0x19U)
#define PTCFG_VCOUT0           (0x1AU)
#define PTCFG_VCOUT1           (0x1BU)
#define PTCFG_RTC1HZ           (0x1CU)
#define PTCFG_SS00             (0x1FU)
#define PTCFG_SCLK00           (0x20U)
#define PTCFG_SCLK01           (0x21U)
#define PTCFG_SCLK10           (0x22U)
#define PTCFG_SCLK11           (0x23U)
#define PTCFG_SCLK20           (0x24U)
#define PTCFG_SCLK21           (0x25U)
#define PTCFG_RXD0             (0x26U)
#define PTCFG_SDI00            (0x26U)
#define PTCFG_SDI01            (0x27U)
#define PTCFG_RXD1             (0x28U)
#define PTCFG_SDI10            (0x28U)
#define PTCFG_SDI11            (0x29U)
#define PTCFG_RXD2             (0x2AU)
#define PTCFG_SDI20            (0x2AU)
#define PTCFG_SDI21            (0x2BU)
#define PTCFG_TXD0             (0x2CU)
#define PTCFG_SDO00            (0x2CU)
#define PTCFG_SDO01            (0x2DU)
#define PTCFG_TXD1             (0x2EU)
#define PTCFG_SDO10            (0x2EU)
#define PTCFG_SDO11            (0x2FU)
#define PTCFG_TXD2             (0x30U)
#define PTCFG_SDO20            (0x30U)
#define PTCFG_SDO21            (0x31U)
#endif

#define TESOPT	*((volatile uint8_t *)(0x4004384C))  /*!< (@ 0x4004384C) PGA protect register                                       */
#define TESCTL	*((volatile uint8_t *)(0x4004384D))  /*!< (@ 0x4004384D) PGA output control register                                */
#define TRMW    *((volatile uint8_t *)(0x40021C08))
#define TRMR    *((volatile uint8_t *)(0x40021C0C))
#define TRMT    *((volatile uint8_t *)(0x40021C10))
#define CNTLD   *((volatile uint16_t *)(0x40021C28)) /*!< (@ 0x40021C28) counter load value of wakeup time             */
	
//#define RTL_SIMULATION

/***********************************************************************************************************************
Macro definitions of SCI usage:
Each channel of SCI has three functions: UART, SPI, and IIC. You can only choose one function to use.
***********************************************************************************************************************/
/* ToDo: You can only define ONE of the following THREE MACROs according to your application */
#define USE_SCI_UART0_TX      /*! Using CH0 of SCI0 as UART Transmitter */
//#define USE_SCI_SPI00         /*! Using CH0 of SCI0 as SPI Transmitter or Receiver */

/* ToDo: You can only define ONE of the following THREE MACROs according to your application */
#define USE_SCI_UART0_RX      /*! Using CH1 of SCI0 as UART Receiver */
//#define USE_SCI_SPI01         /*! Using CH1 of SCI0 as SPI Transmitter or Receiver */

/* ToDo: You can only define ONE of the following THREE MACROs according to your application */
//#define USE_SCI_UART1_TX      /*! Using CH2 of SCI0 as UART Transmitter */
//#define USE_SCI_SPI10         /*! Using CH2 of SCI0 as SPI Transmitter or Receiver */

/* ToDo: You can only define ONE of the following THREE MACROs according to your application */
//#define USE_SCI_UART1_RX      /*! Using CH3 of SCI0 as UART Receiver */
//#define USE_SCI_SPI11         /*! Using CH3 of SCI0 as SPI Transmitter or Receiver */

/* ToDo: You can only define ONE of the following THREE MACROs according to your application */
//#define USE_SCI_UART2_TX      /*! Using CH0 of SCI1 as UART Transmitter */
//#define USE_SCI_SPI20         /*! Using CH0 of SCI1 as SPI Transmitter or Receiver */

/* ToDo: You can only define ONE of the following THREE MACROs according to your application */
//#define USE_SCI_UART2_RX      /*! Using CH1 of SCI1 as UART Receiver */
//#define USE_SCI_SPI21         /*! Using CH1 of SCI1 as SPI Transmitter or Receiver */

/* ToDo: You can define the following MACRO and USE_SCI_UART2_TX or USE_SCI_UART2_RX MACROs to use Infra-Red transmitter or receiver */
//#define USE_IRDA              /*! Enable Infra-Red transmission with UART2 */

/***********************************************************************************************************************
DMA Control Data Set definitions
***********************************************************************************************************************/
#define SPI00_WITH_DMA
#define CTRL_DATA_SPI00 0

#define SPI01_WITH_DMA
#define CTRL_DATA_SPI01 2

#define SPI10_WITH_DMA
#define CTRL_DATA_SPI10 4

#define SPI11_WITH_DMA
#define CTRL_DATA_SPI11 6

#define SPI20_WITH_DMA
#define CTRL_DATA_SPI20 8

#define SPI21_WITH_DMA
#define CTRL_DATA_SPI21 10

#define ADC_WITH_DMA
#define CTRL_DATA_ADC   12


/** @addtogroup Peripherals_Port_Setting_Definations
  * @{
  */

/* ================================================================================================================== */
/* ================                                       TM40                                       ================ */
/* ================================================================================================================== */

/**
  * @brief TM40 TI00~3 and TO00~3 Port Setting (Alternative to any port)
  */

/* ToDo: You can allocate the TI00 to any desired pins with PxxCFG register */
#define TI00_PORT_SETTING() do{ \
        PORT->P30CFG = PTCFG_TI00;	/* allocate TI00 to P30 */ \
        PORT->PM3  |=  (1 << 0);    /* P30 is used as TI00 input */ \
        PORT->PMC3 &= ~(1 << 0);    /* P30 digital function */ \
}while(0)

/* ToDo: You can allocate the TI01 to any desired pins with PxxCFG register */
#define TI01_PORT_SETTING() do{ \
        PORT->P31CFG = PTCFG_TI01;	/* allocate TI01 to P31 */ \
        PORT->PM3  |=  (1 << 1);    /* P31 is used as TI01 input */ \
        PORT->PMC3 &= ~(1 << 1);    /* P31 digital function */ \
}while(0)

/* ToDo: You can allocate the TI02 to any desired pins with PxxCFG register */
#define TI02_PORT_SETTING() do{ \
        PORT->P32CFG = PTCFG_TI02;	/* allocate TI02 to P32 */ \
        PORT->PM3  |=  (1 << 2);    /* P32 is used as TI02 input */ \
        PORT->PMC3 &= ~(1 << 2);    /* P32 digital function */ \
}while(0)

/* ToDo: You can allocate the TI03 to any desired pins with PxxCFG register */
#define TI03_PORT_SETTING() do{ \
        PORT->P33CFG = PTCFG_TI03;	/* allocate TI03 to P33 */ \
        PORT->PM3  |=  (1 << 3);    /* P33 is used as TI03 input */ \
        PORT->PMC3 &= ~(1 << 3);    /* P33 digital function */ \
}while(0)

/* ToDo: You can allocate the TO00 to any desired pins with PxxCFG register */
#define TO00_PORT_SETTING() do{ \
        PORT->P30CFG = PTCFG_TO00;	/* allocate TO00 to P30 */ \
        PORT->PM3  &= ~(1 << 0);    /* P30 is used as TO00 output */ \
        PORT->POM3 &= ~(1 << 0);    /* P30 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 0);    /* P30 digital function */ \
}while(0)

/* ToDo: You can allocate the TO01 to any desired pins with PxxCFG register */
#define TO01_PORT_SETTING() do{ \
        PORT->P31CFG = PTCFG_TO01;	/* allocate TO01 to P31 */ \
        PORT->PM3  &= ~(1 << 1);    /* P31 is used as TO01 output */ \
        PORT->POM3 &= ~(1 << 1);    /* P31 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 1);    /* P31 digital function */ \
}while(0)

/* ToDo: You can allocate the TO02 to any desired pins with PxxCFG register */
#define TO02_PORT_SETTING() do{ \
        PORT->P32CFG = PTCFG_TO02;	/* allocate TO02 to P32 */ \
        PORT->PM3  &= ~(1 << 2);    /* P32 is used as TO02 output */ \
        PORT->POM3 &= ~(1 << 2);    /* P32 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 2);    /* P32 digital function */ \
}while(0)

/* ToDo: You can allocate the TO03 to any desired pins with PxxCFG register */
#define TO03_PORT_SETTING() do{ \
        PORT->P33CFG = PTCFG_TO03;	/* allocate TO03 to P33 */ \
        PORT->PM3  &= ~(1 << 3);    /* P33 is used as TO03 output */ \
        PORT->POM3 &= ~(1 << 3);    /* P33 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 3);    /* P33 digital function */ \
}while(0)

/* ================================================================================================================== */
/* ================                                       TM41                                       ================ */
/* ================================================================================================================== */

/**
  * @brief TM41 TI10~3 and TO10~3 Port Setting 
  */

/* ToDo: You can allocate the TI10 to any desired pins with PxxCFG register */
#define TI10_PORT_SETTING() do{ \
        PORT->P34CFG = PTCFG_TI10;	/* allocate TI10 to P34 */ \
        PORT->PM3  |=  (1 << 4);    /* P34 is used as TI10 input */ \
        PORT->PMC3 &= ~(1 << 4);    /* P34 digital function */ \
}while(0)

/* ToDo: You can allocate the TI11 to any desired pins with PxxCFG register */
#define TI11_PORT_SETTING() do{ \
        PORT->P35CFG = PTCFG_TI11;	/* allocate TI11 to P35 */ \
        PORT->PM3  |=  (1 << 5);    /* P35 is used as TI11 input */ \
        PORT->PMC3 &= ~(1 << 5);    /* P35 digital function */ \
}while(0)

/* ToDo: You can allocate the TI12 to any desired pins with PxxCFG register */
#define TI12_PORT_SETTING() do{ \
        PORT->P36CFG = PTCFG_TI12;	/* allocate TI12 to P36 */ \
        PORT->PM3  |=  (1 << 6);    /* P36 is used as TI12 input */ \
        PORT->PMC3 &= ~(1 << 6);    /* P36 digital function */ \
}while(0)

/* ToDo: You can allocate the TI13 to any desired pins with PxxCFG register */
#define TI13_PORT_SETTING() do{ \
        PORT->P37CFG = PTCFG_TI13;	/* allocate TI13 to P37 */ \
        PORT->PM3  |=  (1 << 7);    /* P37 is used as TI13 input */ \
        PORT->PMC3 &= ~(1 << 7);    /* P37 digital function */ \
}while(0)

/* ToDo: You can allocate the TO10 to any desired pins with PxxCFG register */
#define TO10_PORT_SETTING() do{ \
        PORT->P34CFG = PTCFG_TO10;	/* allocate TO10 to P34 */ \
        PORT->PM3  &= ~(1 << 4);    /* P34 is used as TO10 output */ \
        PORT->POM3 &= ~(1 << 4);    /* P34 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 4);    /* P34 digital function */ \
}while(0)

/* ToDo: You can allocate the TO11 to any desired pins with PxxCFG register */
#define TO11_PORT_SETTING() do{ \
        PORT->P35CFG = PTCFG_TO11;	/* allocate TO11 to P35 */ \
        PORT->PM3  &= ~(1 << 5);    /* P35 is used as TO11 output */ \
        PORT->POM3 &= ~(1 << 5);    /* P35 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 5);    /* P35 digital function */ \
}while(0)

/* ToDo: You can allocate the TO12 to any desired pins with PxxCFG register */
#define TO12_PORT_SETTING() do{ \
        PORT->P36CFG = PTCFG_TO12;	/* allocate TO12 to P36 */ \
        PORT->PM3  &= ~(1 << 6);    /* P36 is used as TO12 output */ \
        PORT->POM3 &= ~(1 << 6);    /* P36 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 6);    /* P36 digital function */ \
}while(0)

/* ToDo: You can allocate the TO13 to any desired pins with PxxCFG register */
#define TO13_PORT_SETTING() do{ \
        PORT->P37CFG = PTCFG_TO13;	/* allocate TO13 to P37 */ \
        PORT->PM3  &= ~(1 << 7);    /* P37 is used as TO13 output */ \
        PORT->POM3 &= ~(1 << 7);    /* P37 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 7);    /* P37 digital function */ \
}while(0)

/* ================================================================================================================== */
/* ================                                      RTC1HZ                                      ================ */
/* ================================================================================================================== */

/**
  * @brief RTC1HZ Port Setting (Alternative to any port)
  */

/* ToDo: You can allocate the RTC1HZ to any desired pins with PxxCFG register */
#define RTC1HZ_PORT_SETTING() do{ \
        PORT->P36CFG = PTCFG_RTC1HZ;/* allocate RTC1HZ to P36 */ \
        PORT->PM3  &= ~(1 << 6);    /* P36 is used as RTC1HZ output */ \
        PORT->POM3 &= ~(1 << 6);    /* P36 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 6);    /* P36 digital function */ \
}while(0)

/* ================================================================================================================== */
/* ================                                      CLKBUZ                                      ================ */
/* ================================================================================================================== */

/**
  * @brief CLKBUZ Port Setting (Alternative to any port)
  */

/* ToDo: You can allocate the CLKBUZ0 to any desired pins with PxxCFG register */
#define CLKBUZ0_PORT_SETTING() do{ \
        PORT->P35CFG = PTCFG_CLKBUZ0; /* allocate CLKBUZ0 to P35 */ \
        PORT->PM3   &= ~(1 << 5);     /* P35 is used as CLKBUZ0 output */ \
        PORT->POM3  &= ~(1 << 5);     /* P35 is normal output mode */ \
        PORT->PMC3  &= ~(1 << 5);     /* P35 digital function */ \
}while(0)

/* ToDo: You can allocate the CLKBUZ1 to any desired pins with PxxCFG register */
#define CLKBUZ1_PORT_SETTING() do{ \
        PORT->P30CFG = PTCFG_CLKBUZ1; /* allocate CLKBUZ1 to P30 */ \
        PORT->PM3   &= ~(1 << 0);     /* P30 is used as CLKBUZ1 output */ \
        PORT->POM3  &= ~(1 << 0);     /* P30 is normal output mode */ \
        PORT->PMC3  &= ~(1 << 0);     /* P30 digital function */ \
}while(0)

/* ================================================================================================================== */
/* ================                                        ADC                                       ================ */
/* ================================================================================================================== */

/**
  * @brief ADC Port Setting
  */

/* ToDo: Please comment out the following unused ANIx setting according to your application needs. */
/* Note: ANI0(P20) and ANI1(P21) is used as SWD I/F; ANI3(P23) and ANI4(P24) is used as UART0 I/F on EVB*/
#define ADC_PORT_SETTING() do{ \
    PORT->PMC2 |= 0x20U;/* Set ANI5(P25) pin: It is necessary for ADC_VREF_AVREFP_AVREFM, used as AVREFM */ \
    PORT->PMC2 |= 0x40U;/* Set ANI6(P26) pin: It is necessary for ADC_VREF_AVREFP_AVREFM, used as AVREFP */ \
    PORT->PMC3 |= 0x01U;/* Set ANI7(P30) pin */ \
    PORT->PMC3 |= 0x02U;/* Set ANI8(P31) pin */ \
    PORT->PMC3 |= 0x04U;/* Set ANI9(P32) pin */ \
    PORT->PMC3 |= 0x08U;/* Set ANI10(P33) pin */ \
    PORT->PMC3 |= 0x10U;/* Set ANI11(P34) pin */ \
    PORT->PMC3 |= 0x20U;/* Set ANI12(P35) pin */ \
    PORT->PMC3 |= 0x40U;/* Set ANI13(P36) pin */ \
    PORT->PMC3 |= 0x80U;/* Set ANI14(P37) pin */ \
}while(0)

/* ================================================================================================================== */
/* ================                                        CMP                                       ================ */
/* ================================================================================================================== */

/**
  * @brief CMP Port Setting
  */

/* ToDo: You can allocate the VCOUT0 to any desired pins with PxxCFG register */
#define VCOUT0_PORT_SETTING() do { \
        PORT->P36CFG = PTCFG_VCOUT0;    /* allocate VCOUT0 to P36 */ \
        PORT->PM3  &= ~(1 << 6);        /* VCOUT0 output to P36 */ \
        PORT->PMC3 &= ~(1 << 6);        /* VCOUT0 output to P36 */ \
}while(0)

/* ToDo: You can allocate the VCOUT1 to any desired pins with PxxCFG register */
#define VCOUT1_PORT_SETTING() do { \
        PORT->P37CFG = PTCFG_VCOUT1;    /* allocate VCOUT1 to P37 */ \
        PORT->PM3  &= ~(1 << 7);        /* VCOUT1 output to P37 */ \
        PORT->PMC3 &= ~(1 << 7);        /* VCOUT1 output to P37 */ \
}while(0)

/* ToDo: Please comment out the VREF0 setting if don't used VREF0 as negative(-) side input of CMP */
#define CMP0_PORT_SETTING() do{ \
        PORT->PMC3  |= (1 << 2);  /* Set VCIN0(P32) pin */ \
        PORT->PMC3  |= (1 << 0);  /* Set VREF0(P30) pin */ \
        VCOUT0_PORT_SETTING();    /* ToDo: Please delete me if you don't output VCOUT0 signal to port */ \
}while(0)

/* ToDo: Please comment out the VCINxx setting if don't used it as positive(+) side input of CMP */
#define CMP1_PORT_SETTING() do{ \
        PORT->PMC2  |= (1 << 0);  /* Set VCIN10(P20) pin */ \
        PORT->PMC2  |= (1 << 1);  /* Set VCIN11(P21) pin */ \
        PORT->PMC2  |= (1 << 6);  /* Set VCIN12(P26) pin */ \
        PORT->PMC2  |= (1 << 5);  /* Set VCIN13(P25) pin */ \
        VCOUT1_PORT_SETTING();    /* ToDo: Please delete me if you don't output VCOUT1 signal to port */ \
}while(0)

/* ================================================================================================================== */
/* ================                                        PGA                                       ================ */
/* ================================================================================================================== */

/**
  * @brief PGA Port Setting
  */

#define PGA0O_PORT_SETTING() do { \
        PORT->PMC3 |= (1 << 1);   /* PGA0O output to P31 */ \
}while(0)

#define PGA1O_PORT_SETTING() do { \
        PORT->PMC3 |= (1 << 2);   /* PGA1O output to P32 */ \
}while(0)

#define PGA0IN_PORT_SETTING() do { \
        PORT->PMC3 |= (1 << 2);  /* PGA0IN input from P32 */ \
}while(0)

#define PGA0GND_PORT_SETTING() do { \
        PORT->PMC3 |= (1 << 5);  /* PGA0GND input from P35 */ \
}while(0)

#define PGA1IN_PORT_SETTING() do { \
        PORT->PMC3 |= (1 << 1);  /* PGA1IN input from P31 */ \
}while(0)

#define PGA1GND_PORT_SETTING() do { \
        PORT->PMC3 |= (1 << 0);  /* PGA1GND input from P30 */ \
}while(0)

/* ================================================================================================================== */
/* ================                                       SCI0                                       ================ */
/* ================================================================================================================== */

/**
  * @brief UART0 Port Setting（Alternative to any port) 
  */

/* ToDo: You can allocate the TXD0 to any desired pins with PxxCFG register */
#define TXD0_PORT_SETTING() do{ \
        PORT->P01CFG = PTCFG_TXD0;  /* allocate TXD0 to P24 */ \
        PORT->PM0  &= ~(1 << 1);    /* P24 is used as TXD0 output */ \
        PORT->POM0 &= ~(1 << 1);    /* P24 is normal output mode */ \
        PORT->PMC0 &= ~(1 << 1);    /* P24 digital function */ \
}while(0)

/* ToDo: You can allocate the RXD0 to any desired pins with PxxCFG register */
#define RXD0_PORT_SETTING() do{ \
        PORT->P02CFG = PTCFG_RXD0;  /* allocate RXD0 to P23 */ \
        PORT->PM0  |=  (1 << 2);    /* P23 is used as RXD0 input */ \
        PORT->PMC0 &= ~(1 << 2);    /* P23 digital function */ \
}while(0)

/**
  * @brief SPI00 Port Setting（Alternative to any port)
  */

/* ToDo: You can allocate the SS00 to any desired pins */
#define SS00_PORT_SETTING() do{ \
        PORT->P10CFG = PTCFG_SS00;   /* allocate SS00 to P10 */ \
        PORT->PM1  |=  (1 << 0);     /* P10 is used as SS00 input */ \
        PORT->PU1  |=  (1 << 0);     /* P10 pull up resistor enable */ \
}while(0)

/* ToDo: You can allocate the SS00 to any desired pins */
#define SS00_PORT_SET() do{ \
        PORT->P1   |=  (1 << 0);     /* P10 output high level */ \
}while(0)

/* ToDo: You can allocate the SS00 to any desired pins */
#define SS00_PORT_CLR() do{ \
        PORT->P1   &= ~(1 << 0);     /* P10 output low level */ \
}while(0)

/* ToDo: You can allocate the SCLKI00 to any desired pins with PxxCFG register */
#define SCLKI00_PORT_SETTING() do{ \
        PORT->P35CFG = PTCFG_SCLK00; /* allocate SCLK00 to P35 */ \
        PORT->PM3  |=  (1 << 5);     /* P35 is used as SCLK00 input */ \
        PORT->PMC3 &= ~(1 << 5);     /* P35 digital function */ \
}while(0)

/* ToDo: You can allocate the SCLKO00 to any desired pins with PxxCFG register */
#define SCLKO00_PORT_SETTING() do{ \
        PORT->P35CFG = PTCFG_SCLK00;/* allocate SCLK00 to P35 */ \
        PORT->PM3  &= ~(1 << 5);    /* P35 is used as SCLK00 output */ \
        PORT->POM3 &= ~(1 << 5);    /* P35 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 5);    /* P35 digital function */ \
}while(0)

/* ToDo: You can allocate the SDO00 to any desired pins with PxxCFG register */
#define SDO00_PORT_SETTING() do{ \
        PORT->P37CFG = PTCFG_SDO00; /* allocate SDO00 to P37 */ \
        PORT->PM3  &= ~(1 << 7);    /* P37 is used as SDO00 output */ \
        PORT->POM3 &= ~(1 << 7);    /* P37 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 7);    /* P37 digital function */ \
}while(0)

/* ToDo: You can allocate the SDI00 to any desired pins with PxxCFG register */
#define SDI00_PORT_SETTING() do{ \
        PORT->P36CFG = PTCFG_SDI00; /* allocate SDI00 to P36 */ \
        PORT->PM3  |=  (1 << 6);    /* P36 is used as SDI00 input */ \
        PORT->PMC3 &= ~(1 << 6);    /* P36 digital function */ \
}while(0)

/**
  * @brief SPI01 Port Setting (Alternative to fixed port)
  */

/* ToDo: You can allocate the SS01 to any desired pins */
#define SS01_PORT_SETTING() do{ \
        PORT->P11CFG = 0x00;         /* P11 default GPIO function */ \
        PORT->P1   |=  (1 << 1);     /* P11 output high level */ \
        PORT->PM1  &= ~(1 << 1);     /* P11 is used as SS01 input */ \
}while(0)

/* ToDo: You can allocate the SS01 to any desired pins */
#define SS01_PORT_SET() do{ \
        PORT->P1   |=  (1 << 1);     /* P11 output high level */ \
}while(0)

/* ToDo: You can allocate the SS01 to any desired pins */
#define SS01_PORT_CLR() do{ \
        PORT->P1   &= ~(1 << 1);     /* P11 output low level */ \
}while(0)

/* ToDo: You can allocate the SCLKI01 to any desired pins with PxxCFG register */
#define SCLKI01_PORT_SETTING() do{ \
        PORT->P30CFG = PTCFG_SCLK01; /* allocate SCLK01 to P30 */ \
        PORT->PM3  |=  (1 << 0);     /* P30 is used as SCLK00 input */ \
        PORT->PMC3 &= ~(1 << 0);     /* P30 digital function */ \
}while(0)

/* ToDo: You can allocate the SCLKO01 to any desired pins with PxxCFG register */
#define SCLKO01_PORT_SETTING() do{ \
        PORT->P30CFG = PTCFG_SCLK01;/* allocate SCLK01 to P30 */ \
        PORT->PM3  &= ~(1 << 0);    /* P30 is used as SCLK00 output */ \
        PORT->POM3 &= ~(1 << 0);    /* P30 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 0);    /* P30 digital function */ \
}while(0)

/* ToDo: You can allocate the SDO01 to any desired pins with PxxCFG register */
#define SDO01_PORT_SETTING() do{ \
        PORT->P32CFG = PTCFG_SDO01; /* allocate SDO01 to P32 */ \
        PORT->PM3  &= ~(1 << 2);    /* P32 is used as SDO01 output */ \
        PORT->POM3 &= ~(1 << 2);    /* P32 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 2);    /* P32 digital function */ \
}while(0)

/* ToDo: You can allocate the SDI01 to any desired pins with PxxCFG register */
#define SDI01_PORT_SETTING() do{ \
        PORT->P31CFG = PTCFG_SDI01; /* allocate SDI01 to P31 */ \
        PORT->PM3  |=  (1 << 1);    /* P31 is used as SDI01 input */ \
        PORT->PMC3 &= ~(1 << 1);    /* P31 digital function */ \
}while(0)


/**
  * @brief UART1 Port Setting (Alternative to any port)
  */

/* ToDo: You can allocate the TXD1 to any desired pins with PxxCFG register */
#define TXD1_PORT_SETTING() do{ \
        PORT->P37CFG = PTCFG_TXD1;  /* allocate TXD1 to P37 */ \
        PORT->PM3  &= ~(1 << 7);    /* P37 is used as TXD1 output */ \
        PORT->POM3 &= ~(1 << 7);    /* P37 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 7);    /* P37 digital function */ \
}while(0)

/* ToDo: You can allocate the RXD1 to any desired pins with PxxCFG register */
#define RXD1_PORT_SETTING() do{ \
        PORT->P36CFG = PTCFG_RXD1;  /* allocate RXD1 to P36 */ \
        PORT->PM3  |=  (1 << 6);    /* P36 is used as RXD1 input */ \
        PORT->PMC3 &= ~(1 << 6);    /* P36 digital function */ \
}while(0)

/**
  * @brief SPI10 Port Setting（Alternative to any port)
  */

/* ToDo: You can allocate the SS10 to any desired pins */
#define SS10_PORT_SETTING() do{ \
        PORT->P10CFG = 0x00;         /* P10 default GPIO function */ \
        PORT->P1   |=  (1 << 0);     /* P10 output high level */ \
        PORT->PM1  &= ~(1 << 0);     /* P10 is used as SS01 input */ \
}while(0)

/* ToDo: You can allocate the SS10 to any desired pins */
#define SS10_PORT_SET() do{ \
        PORT->P1   |=  (1 << 0);     /* P10 output high level */ \
}while(0)

/* ToDo: You can allocate the SS10 to any desired pins */
#define SS10_PORT_CLR() do{ \
        PORT->P1   &= ~(1 << 0);     /* P10 output low level */ \
}while(0)

/* ToDo: You can allocate the SCLKI10 to any desired pins with PxxCFG register */
#define SCLKI10_PORT_SETTING() do{ \
        PORT->P35CFG = PTCFG_SCLK10; /* allocate SCLK10 to P35 */ \
        PORT->PM3  |=  (1 << 5);     /* P35 is used as SCLK10 input */ \
        PORT->PMC3 &= ~(1 << 5);     /* P35 digital function */ \
}while(0)

/* ToDo: You can allocate the SCLKO10 to any desired pins with PxxCFG register */
#define SCLKO10_PORT_SETTING() do{ \
        PORT->P35CFG = PTCFG_SCLK10;/* allocate SCLK10 to P35 */ \
        PORT->PM3  &= ~(1 << 5);    /* P35 is used as SCLK10 output */ \
        PORT->POM3 &= ~(1 << 5);    /* P35 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 5);    /* P35 digital function */ \
}while(0)

/* ToDo: You can allocate the SDO10 to any desired pins with PxxCFG register */
#define SDO10_PORT_SETTING() do{ \
        PORT->P37CFG = PTCFG_SDO10; /* allocate SDO10 to P37 */ \
        PORT->PM3  &= ~(1 << 7);    /* P37 is used as SDO10 output */ \
        PORT->POM3 &= ~(1 << 7);    /* P37 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 7);    /* P37 digital function */ \
}while(0)

/* ToDo: You can allocate the SDI10 to any desired pins with PxxCFG register */
#define SDI10_PORT_SETTING() do{ \
        PORT->P36CFG = PTCFG_SDI10; /* allocate SDI10 to P36 */ \
        PORT->PM3  |=  (1 << 6);    /* P36 is used as SDI10 input */ \
        PORT->PMC3 &= ~(1 << 6);    /* P36 digital function */ \
}while(0)

/**
  * @brief SPI11 Port Setting (Alternative to any port)
  */

/* ToDo: You can allocate the SS11 to any desired pins */
#define SS11_PORT_SETTING() do{ \
        PORT->P11CFG = 0x00;         /* P11 default GPIO function */ \
        PORT->P1   |=  (1 << 1);     /* P11 output high level */ \
        PORT->PM1  &= ~(1 << 1);     /* P11 is used as SS11 input */ \
}while(0)

/* ToDo: You can allocate the SS11 to any desired pins */
#define SS11_PORT_SET() do{ \
        PORT->P1   |=  (1 << 1);     /* P11 output high level */ \
}while(0)

/* ToDo: You can allocate the SS11 to any desired pins */
#define SS11_PORT_CLR() do{ \
        PORT->P1   &= ~(1 << 1);     /* P11 output low level */ \
}while(0)

/* ToDo: You can allocate the SCLKI11 to any desired pins with PxxCFG register */
#define SCLKI11_PORT_SETTING() do{ \
        PORT->P30CFG = PTCFG_SCLK11; /* allocate SCLK11 to P30 */ \
        PORT->PM3  |=  (1 << 0);     /* P30 is used as SCLK00 input */ \
        PORT->PMC3 &= ~(1 << 0);     /* P30 digital function */ \
}while(0)

/* ToDo: You can allocate the SCLKO11 to any desired pins with PxxCFG register */
#define SCLKO11_PORT_SETTING() do{ \
        PORT->P30CFG = PTCFG_SCLK11;/* allocate SCLK11 to P30 */ \
        PORT->PM3  &= ~(1 << 0);    /* P30 is used as SCLK00 output */ \
        PORT->POM3 &= ~(1 << 0);    /* P30 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 0);    /* P30 digital function */ \
}while(0)

/* ToDo: You can allocate the SDO11 to any desired pins with PxxCFG register */
#define SDO11_PORT_SETTING() do{ \
        PORT->P32CFG = PTCFG_SDO11; /* allocate SDO11 to P32 */ \
        PORT->PM3  &= ~(1 << 2);    /* P32 is used as SDO11 output */ \
        PORT->POM3 &= ~(1 << 2);    /* P32 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 2);    /* P32 digital function */ \
}while(0)

/* ToDo: You can allocate the SDI11 to any desired pins with PxxCFG register */
#define SDI11_PORT_SETTING() do{ \
        PORT->P31CFG = PTCFG_SDI11; /* allocate SDI11 to P31 */ \
        PORT->PM3  |=  (1 << 1);    /* P31 is used as SDI11 input */ \
        PORT->PMC3 &= ~(1 << 1);    /* P31 digital function */ \
}while(0)

/* ================================================================================================================== */
/* ================                                       SCI1                                       ================ */
/* ================================================================================================================== */

/**
  * @brief UART2 Port Setting (Alternative to any port)
  */

/* ToDo: You can allocate the TXD2 to any desired pins with PxxCFG register */
#define TXD2_PORT_SETTING() do{ \
        PORT->P37CFG = PTCFG_TXD2;  /* allocate TXD2 to P37 */ \
        PORT->PM3  &= ~(1 << 7);    /* P37 is used as TXD2 output */ \
        PORT->POM3 &= ~(1 << 7);    /* P37 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 7);    /* P37 digital function */ \
}while(0)

/* ToDo: You can allocate the RXD2 to any desired pins with PxxCFG register */
#define RXD2_PORT_SETTING() do{ \
        PORT->P36CFG = PTCFG_RXD2;  /* allocate RXD2 to P36 */ \
        PORT->PM3  |=  (1 << 6);    /* P36 is used as RXD2 input */ \
        PORT->PMC3 &= ~(1 << 6);    /* P36 digital function */ \
}while(0)

/**
  * @brief SPI20 Port Setting（Alternative to any port)
  */

/* ToDo: You can allocate the SS20 to any desired pins */
#define SS20_PORT_SETTING() do{ \
        PORT->P10CFG = 0x00;         /* P10 default GPIO function */ \
        PORT->P1   |=  (1 << 0);     /* P10 output high level */ \
        PORT->PM1  &= ~(1 << 0);     /* P10 is used as SS01 input */ \
}while(0)

/* ToDo: You can allocate the SS20 to any desired pins */
#define SS20_PORT_SET() do{ \
        PORT->P1   |=  (1 << 0);     /* P10 output high level */ \
}while(0)

/* ToDo: You can allocate the SS20 to any desired pins */
#define SS20_PORT_CLR() do{ \
        PORT->P1   &= ~(1 << 0);     /* P10 output low level */ \
}while(0)

/* ToDo: You can allocate the SCLKI20 to any desired pins with PxxCFG register */
#define SCLKI20_PORT_SETTING() do{ \
        PORT->P35CFG = PTCFG_SCLK20; /* allocate SCLK10 to P35 */ \
        PORT->PM3  |=  (1 << 5);     /* P35 is used as SCLK10 input */ \
        PORT->PMC3 &= ~(1 << 5);     /* P35 digital function */ \
}while(0)

/* ToDo: You can allocate the SCLKO20 to any desired pins with PxxCFG register */
#define SCLKO20_PORT_SETTING() do{ \
        PORT->P35CFG = PTCFG_SCLK20;/* allocate SCLK10 to P35 */ \
        PORT->PM3  &= ~(1 << 5);    /* P35 is used as SCLK10 output */ \
        PORT->POM3 &= ~(1 << 5);    /* P35 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 5);    /* P35 digital function */ \
}while(0)

/* ToDo: You can allocate the SDO20 to any desired pins with PxxCFG register */
#define SDO20_PORT_SETTING() do{ \
        PORT->P37CFG = PTCFG_SDO20; /* allocate SDO20 to P37 */ \
        PORT->PM3  &= ~(1 << 7);    /* P37 is used as SDO20 output */ \
        PORT->POM3 &= ~(1 << 7);    /* P37 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 7);    /* P37 digital function */ \
}while(0)

/* ToDo: You can allocate the SDI20 to any desired pins with PxxCFG register */
#define SDI20_PORT_SETTING() do{ \
        PORT->P36CFG = PTCFG_SDI20; /* allocate SDI20 to P36 */ \
        PORT->PM3  |=  (1 << 6);    /* P36 is used as SDI20 input */ \
        PORT->PMC3 &= ~(1 << 6);    /* P36 digital function */ \
}while(0)

/**
  * @brief SPI21 Port Setting (Alternative to any port)
  */

/* ToDo: You can allocate the SS21 to any desired pins */
#define SS21_PORT_SETTING() do{ \
        PORT->P11CFG = 0x00;         /* P11 default GPIO function */ \
        PORT->P1   |=  (1 << 1);     /* P11 output high level */ \
        PORT->PM1  &= ~(1 << 1);     /* P11 is used as SS21 input */ \
}while(0)

/* ToDo: You can allocate the SS21 to any desired pins */
#define SS21_PORT_SET() do{ \
        PORT->P1   |=  (1 << 1);     /* P11 output high level */ \
}while(0)

/* ToDo: You can allocate the SS21 to any desired pins */
#define SS21_PORT_CLR() do{ \
        PORT->P1   &= ~(1 << 1);     /* P11 output low level */ \
}while(0)

/* ToDo: You can allocate the SCLKI21 to any desired pins with PxxCFG register */
#define SCLKI21_PORT_SETTING() do{ \
        PORT->P30CFG = PTCFG_SCLK21; /* allocate SCLK11 to P30 */ \
        PORT->PM3  |=  (1 << 0);     /* P30 is used as SCLK00 input */ \
        PORT->PMC3 &= ~(1 << 0);     /* P30 digital function */ \
}while(0)

/* ToDo: You can allocate the SCLKO21 to any desired pins with PxxCFG register */
#define SCLKO21_PORT_SETTING() do{ \
        PORT->P30CFG = PTCFG_SCLK21;/* allocate SCLK11 to P30 */ \
        PORT->PM3  &= ~(1 << 0);    /* P30 is used as SCLK00 output */ \
        PORT->POM3 &= ~(1 << 0);    /* P30 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 0);    /* P30 digital function */ \
}while(0)

/* ToDo: You can allocate the SDO21 to any desired pins with PxxCFG register */
#define SDO21_PORT_SETTING() do{ \
        PORT->P32CFG = PTCFG_SDO21; /* allocate SDO21 to P32 */ \
        PORT->PM3  &= ~(1 << 2);    /* P32 is used as SDO21 output */ \
        PORT->POM3 &= ~(1 << 2);    /* P32 is normal output mode */ \
        PORT->PMC3 &= ~(1 << 2);    /* P32 digital function */ \
}while(0)

/* ToDo: You can allocate the SDI21 to any desired pins with PxxCFG register */
#define SDI21_PORT_SETTING() do{ \
        PORT->P31CFG = PTCFG_SDI21; /* allocate SDI21 to P31 */ \
        PORT->PM3  |=  (1 << 1);    /* P31 is used as SDI21 input */ \
        PORT->PMC3 &= ~(1 << 1);    /* P31 digital function */ \
}while(0)

/* ================================================================================================================== */
/* ================                                       IICA                                       ================ */
/* ================================================================================================================== */

/**
  * @brief IICA0 Port Setting（Alternative to any port)
  */

/* ToDo: You can allocate the SCLA0 to any desired pins with PxxCFG register */
#define SCLA0_PORT_SETTING() do{ \
        PORT->P36CFG = PTCFG_SCLA0; /* allocate SCLA0 to P36 */ \
        PORT->PM3  &= ~(1 << 6);    /* P36 is used as SCLA0 output */ \
        PORT->PMC3 &= ~(1 << 6);    /* P36 digital function */ \
}while(0)

/* ToDo: You can allocate the SDAA0 to any desired pins with PxxCFG register */
#define SDAA0_PORT_SETTING() do{ \
        PORT->P37CFG = PTCFG_SDAA0; /* allocate SDAA0 to P37 */ \
        PORT->PM3  &= ~(1 << 7);    /* P37 is used as SDAA0 inout */ \
        PORT->PMC3 &= ~(1 << 7);    /* P37 digital function */ \
}while(0)

/* ================================================================================================================== */
/* ================                                       INTP                                       ================ */
/* ================================================================================================================== */
/**
  * @brief INTP Port Setting 
  */

#if 0
/* ToDo: You can allocate the INTP0 to any desired pins with PxxCFG register */
#define INTP0_PORT_SETTING() do{ \
        PORT->P30CFG = PTCFG_INTP0; /* allocate INTP0 to P30 */ \
        PORT->PM3   |=  (1 << 0);   /* P30 is used as INTP0 input */ \
        PORT->PMC3  &= ~(1 << 0);   /* P30 digital function */ \
}while(0)

/* ToDo: You can allocate the INTP1 to any desired pins with PxxCFG register */
#define INTP1_PORT_SETTING() do{ \
        PORT->P31CFG = PTCFG_INTP1; /* allocate INTP1 to P31 */ \
        PORT->PM3   |=  (1 << 1);   /* P31 is used as INTP1 input */ \
        PORT->PMC3  &= ~(1 << 1);   /* P31 digital function */ \
}while(0)

/* ToDo: You can allocate the INTP2 to any desired pins with PxxCFG register */
#define INTP2_PORT_SETTING() do{ \
        PORT->P32CFG = PTCFG_INTP2; /* allocate INTP2 to P32 */ \
        PORT->PM3   |=  (1 << 2);   /* P32 is used as INTP2 input */ \
        PORT->PMC3  &= ~(1 << 2);   /* P32 digital function */ \
}while(0)

/* ToDo: You can allocate the INTP3 to any desired pins with PxxCFG register */
#define INTP3_PORT_SETTING() do{ \
        PORT->P33CFG = PTCFG_INTP3; /* allocate INTP3 to P33 */ \
        PORT->PM3   |=  (1 << 3);   /* P33 is used as INTP3 input */ \
        PORT->PMC3  &= ~(1 << 3);   /* P33 digital function */ \
}while(0)
#else
#define INTP0_PORT_SETTING() do{ \
        PORT->P10CFG = PTCFG_INTP0; /* allocate INTP0 to P10 */ \
        PORT->PM1   |=  (1 << 0);   /* P10 is used as INTP0 input */ \
        PORT->PMC1  &= ~(1 << 0);   /* P10 digital function */ \
}while(0)

/* ToDo: You can allocate the INTP1 to any desired pins with PxxCFG register */
#define INTP1_PORT_SETTING() do{ \
        PORT->P37CFG = PTCFG_INTP1; /* allocate INTP1 to P11 */ \
        PORT->PM3   |=  (1 << 7);   /* P11 is used as INTP1 input */ \
        PORT->PMC3  &= ~(1 << 7);   /* P11 digital function */ \
}while(0)

/* ToDo: You can allocate the INTP2 to any desired pins with PxxCFG register */
#define INTP2_PORT_SETTING() do{ \
        PORT->P26CFG = PTCFG_INTP2; /* allocate INTP2 to P12 */ \
        PORT->PM2   |=  (1 << 6);   /* P12 is used as INTP2 input */ \
        PORT->PMC2  &= ~(1 << 6);   /* P12 digital function */ \
}while(0)							//P26

/* ToDo: You can allocate the INTP3 to any desired pins with PxxCFG register */
#define INTP3_PORT_SETTING() do{ \
        PORT->P20CFG = PTCFG_INTP3; /* allocate INTP3 to P32 */ \
        PORT->PM2   |=  (1 << 0);   /* P33 is used as INTP3 input */ \
        PORT->PMC2  &= ~(1 << 0);   /* P33 digital function */ \
}while(0)							//P37
#endif

#endif

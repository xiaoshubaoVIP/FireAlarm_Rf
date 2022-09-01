/***********************************************************************************************************************
* Copyright (C) All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* @file    sci_user.c
* @brief   This file implements device driver for SCI module.
* @version 1.0.0
* @date    2019/12/24
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include <stdio.h>
#include "userdefine.h"
#include "BAT32G133.h"
#include "sci.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#if defined USE_SCI_UART2_TX 
void IRQ07_Handler(void) __attribute__((alias("uart2_interrupt_send")));
#elif defined USE_SCI_SPI20
void IRQ07_Handler(void) __attribute__((alias("spi20_interrupt")));
#endif

#if defined USE_SCI_UART2_RX
void IRQ08_Handler(void) __attribute__((alias("uart2_interrupt_receive")));
#elif defined USE_SCI_SPI21
void IRQ08_Handler(void) __attribute__((alias("spi21_interrupt")));
#endif

#if defined USE_SCI_UART0_TX
void IRQ10_Handler(void) __attribute__((alias("uart0_interrupt_send")));
#elif defined USE_SCI_SPI00
void IRQ10_Handler(void) __attribute__((alias("spi00_interrupt")));
#endif

#if defined USE_SCI_UART0_RX
void IRQ11_Handler(void) __attribute__((alias("uart0_interrupt_receive")));
#elif defined USE_SCI_SPI01
void IRQ11_Handler(void) __attribute__((alias("spi01_interrupt")));
#endif

#if defined USE_SCI_UART1_TX
void IRQ13_Handler(void) __attribute__((alias("uart1_interrupt_send")));
#elif defined USE_SCI_SPI10
void IRQ13_Handler(void) __attribute__((alias("spi10_interrupt")));
#endif

#if defined USE_SCI_UART1_RX
void IRQ14_Handler(void) __attribute__((alias("uart1_interrupt_receive")));
#elif defined USE_SCI_SPI11
void IRQ14_Handler(void) __attribute__((alias("spi11_interrupt")));
#endif

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: uart0_interrupt_receive
* @brief  UART0 Receive interrupt service routine
* @param  None
* @return None
***********************************************************************************************************************/
extern void UartReceiveInput(unsigned char value);
void uart0_interrupt_receive(void)
{
    volatile uint8_t rx_data;
    volatile uint8_t err_type;
    
    INTC_ClearPendingIRQ(SR0_IRQn);
    err_type = (uint8_t)(SCI0->SSR01 & 0x0007U);
    SCI0->SIR01 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        uart0_callback_error(err_type);
    }
    
    rx_data = SCI0->RXD0;

#if 1
	UartReceiveInput(rx_data);

#else
	UART0_Send(rx_data);
#endif
}
/***********************************************************************************************************************
* Function Name: uart0_interrupt_send
* @brief  UART0 Send interrupt service routine
* @param  None
* @return None
***********************************************************************************************************************/
void uart0_interrupt_send(void)
{
    INTC_ClearPendingIRQ(ST0_IRQn);
    if (g_uart0_tx_count > 0U)
    {
        SCI0->TXD0 = *gp_uart0_tx_address;
        gp_uart0_tx_address++;
        g_uart0_tx_count--;
    }
    else
    {
        uart0_callback_sendend();
    }
}

#if 0  //not use
/***********************************************************************************************************************
* Function Name: uart0_callback_receiveend
* @brief  This function is a callback function when UART0 finishes reception.
* @param  None
* @return None
***********************************************************************************************************************/
static void uart0_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: uart0_callback_softwareoverrun
* @brief  This function is a callback function when UART0 receives an overflow data.
* @param  rx_data - receive data
* @return None
***********************************************************************************************************************/
static void uart0_callback_softwareoverrun(uint16_t rx_data)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
#endif

/***********************************************************************************************************************
* Function Name: uart0_callback_sendend
* @brief  This function is a callback function when UART0 finishes transmission.
* @param  None
* @return None
***********************************************************************************************************************/
static void uart0_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: uart0_callback_error
* @brief  This function is a callback function when UART0 reception error occurs.
* @param  err_type - error type value
* @return None
***********************************************************************************************************************/
static void uart0_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi00_interrupt
* @brief  SPI00 interrupt service routine
* @param  None
* @return None
***********************************************************************************************************************/
void spi00_interrupt(void)
{
    volatile uint8_t err_type;
    volatile uint8_t sio_dummy;

    INTC_ClearPendingIRQ(SPI00_IRQn);
    err_type = (uint8_t)(SCI0->SSR00 & _0001_SCI_OVERRUN_ERROR);
    SCI0->SIR00 = (uint16_t)err_type;

    if (1U == err_type)
    {
        spi00_callback_error(err_type);    /* overrun error occurs */
    }
    else
    {
        if (g_spi00_tx_count > 0U || g_spi00_rx_count > 1U)
        {
            /* reveive data */
            if (0U != gp_spi00_rx_address)
            {
                *gp_spi00_rx_address = SCI0->SIO00;
                gp_spi00_rx_address++;
                g_spi00_rx_count--;
            }
            else
            {
                sio_dummy = SCI0->SIO00;
            }

            /* send data */
            if (0U != gp_spi00_tx_address)
            {
                SCI0->SIO00 = *gp_spi00_tx_address;
                gp_spi00_tx_address++;
                g_spi00_tx_count--;
            }
            else if((SCI0->SMR00 & _4000_SCI_CLOCK_MODE_TI0N) == 0)
            {
                SCI0->SIO00 = 0xFFU;   // dummy write in master reception mode
            }
        }
        else 
        {
            /* receive the last data */
            if (1U == g_spi00_rx_count)
            {
                while ((SCI0->SSR00 & _0020_SCI_VALID_STORED) == 0)
                {
                    ;
                }
                if (0U != gp_spi00_rx_address)
                {
                    *gp_spi00_rx_address = SCI0->SIO00;
                     g_spi00_rx_count--;
                }
                else
                {
                    sio_dummy = SCI0->SIO00;
                }
                spi00_callback_receiveend();    /* complete receive */
            }
            /* wait the last data sent for DMA transfer in master mode */
            if (0U != gp_spi00_tx_address)
            {
                if (((SCI0->SMR00 & _4000_SCI_CLOCK_MODE_TI0N) != 0) && ((SCI0->SCR00 & _4000_SCI_RECEPTION) != 0))
                {
                    ; /* Do not wait in slave transceiver mode */
                }
                else
                {
                    while (SCI0->SSR00 & _0040_SCI_UNDER_EXECUTE)
                    {
                        ;
                    }
                }
                spi00_callback_sendend();    /* complete send */
            }
        }
    }
}
/***********************************************************************************************************************
* Function Name: spi00_callback_receiveend
* @brief  This function is a callback function when spi00 finishes reception.
* @param  None
* @return None
***********************************************************************************************************************/
static void spi00_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
    //SPI00_Stop();
    gp_spi00_rx_address = 0;    /* reinitialize global variable */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi00_callback_error
* @brief  This function is a callback function when spi00 reception error occurs.
* @param  err_type - error type value
* @return None
***********************************************************************************************************************/
static void spi00_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi00_callback_sendend
* @brief  This function is a callback function when spi00 finishes transmission.
* @param  None
* @return None
***********************************************************************************************************************/
static void spi00_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    gp_spi00_tx_address = 0;    /* reinitialize global variable */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi01_interrupt
* @brief  SPI01 interrupt service routine
* @param  None
* @return None
***********************************************************************************************************************/
void spi01_interrupt(void)
{
    volatile uint8_t err_type;
    volatile uint8_t sio_dummy;

    err_type = (uint8_t)(SCI0->SSR01 & _0001_SCI_OVERRUN_ERROR);
    SCI0->SIR01 = (uint16_t)err_type;

    if (1U == err_type)
    {
        spi01_callback_error(err_type);    /* overrun error occurs */
    }
    else
    {
        if (g_spi01_tx_count > 0U || g_spi01_rx_count > 1U)
        {
            /* reveive data */
            if (0U != gp_spi01_rx_address)
            {
                *gp_spi01_rx_address = SCI0->SIO01;
                gp_spi01_rx_address++;
                g_spi01_rx_count--;
            }
            else
            {
                sio_dummy = SCI0->SIO01;
            }

            /* send data */
            if (0U != gp_spi01_tx_address)
            {
                SCI0->SIO01 = *gp_spi01_tx_address;
                gp_spi01_tx_address++;
                g_spi01_tx_count--;
            }
            else if((SCI0->SMR01 & _4000_SCI_CLOCK_MODE_TI0N) == 0)
            {
                SCI0->SIO01 = 0xFFU;   // dummy write in master reception mode
            }

        }
        else 
        {
            /* receive the last data */
            if (1U == g_spi01_rx_count)
            {
                while ((SCI0->SSR01 & _0020_SCI_VALID_STORED) == 0)
                {
                    ;
                }
                if (0U != gp_spi01_rx_address)
                {
                    *gp_spi01_rx_address = SCI0->SIO01;
                     g_spi01_rx_count--;
                }
                else
                {
                    sio_dummy = SCI0->SIO01;
                }
                spi01_callback_receiveend();    /* complete receive */
            }
            /* wait the last data sent for DMA transfer in master mode */
            if (0U != gp_spi01_tx_address)
            {
                if (((SCI0->SMR01 & _4000_SCI_CLOCK_MODE_TI0N) != 0) && ((SCI0->SCR01 & _4000_SCI_RECEPTION) != 0))
                {
                    ; /* Do not wait in slave transceiver mode */
                }
                else
                {
                    while (SCI0->SSR01 & _0040_SCI_UNDER_EXECUTE)
                    {
                        ;
                    }
                }
                spi01_callback_sendend();    /* complete send */
            }
        }
    }
    INTC_ClearPendingIRQ(SPI01_IRQn);
}
/***********************************************************************************************************************
* Function Name: spi01_callback_receiveend
* @brief  This function is a callback function when spi01 finishes reception.
* @param  None
* @return None
***********************************************************************************************************************/
static void spi01_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
    SPI01_Stop();
    gp_spi01_rx_address = 0;    /* reinitialize global variable */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi01_callback_error
* @brief  This function is a callback function when spi01 reception error occurs.
* @param  err_type - error type value
* @return None
***********************************************************************************************************************/
static void spi01_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi01_callback_sendend
* @brief  This function is a callback function when spi01 finishes transmission.
* @param  None
* @return None
***********************************************************************************************************************/
static void spi01_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    SPI01_Stop();
    gp_spi01_tx_address = 0;    /* reinitialize global variable */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: uart1_interrupt_receive
* @brief  UART1 Receive interrupt service routine
* @param  None
* @return None
***********************************************************************************************************************/
void uart1_interrupt_receive(void)
{
    volatile uint8_t rx_data;
    volatile uint8_t err_type;
    
    INTC_ClearPendingIRQ(SR1_IRQn);
    err_type = (uint8_t)(SCI0->SSR03 & 0x0007U);
    SCI0->SIR03 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        uart1_callback_error(err_type);
    }
    
    rx_data = SCI0->RXD1;

    if (g_uart1_rx_length > g_uart1_rx_count)
    {
        *gp_uart1_rx_address = rx_data;
        gp_uart1_rx_address++;
        g_uart1_rx_count++;

        if (g_uart1_rx_length == g_uart1_rx_count)
        {
            uart1_callback_receiveend();
        }
    }
    else
    {
        uart1_callback_softwareoverrun(rx_data);
    }
}
/***********************************************************************************************************************
* Function Name: uart1_interrupt_send
* @brief  UART1 Send interrupt service routine
* @param  None
* @return None
***********************************************************************************************************************/
void uart1_interrupt_send(void)
{
    INTC_ClearPendingIRQ(ST1_IRQn);
    if (g_uart1_tx_count > 0U)
    {
        SCI0->TXD1 = *gp_uart1_tx_address;
        gp_uart1_tx_address++;
        g_uart1_tx_count--;
    }
    else
    {
        uart1_callback_sendend();
    }
}
/***********************************************************************************************************************
* Function Name: uart1_callback_receiveend
* @brief  This function is a callback function when UART1 finishes reception.
* @param  None
* @return None
***********************************************************************************************************************/
static void uart1_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: uart1_callback_softwareoverrun
* @brief  This function is a callback function when UART1 receives an overflow data.
* @param  rx_data - receive data
* @return None
***********************************************************************************************************************/
static void uart1_callback_softwareoverrun(uint16_t rx_data)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: uart1_callback_sendend
* @brief  This function is a callback function when UART1 finishes transmission.
* @param  None
* @return None
***********************************************************************************************************************/
static void uart1_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: uart1_callback_error
* @brief  This function is a callback function when UART1 reception error occurs.
* @param  err_type - error type value
* @return None
***********************************************************************************************************************/
static void uart1_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi10_interrupt
* @brief  SPI10 interrupt service routine
* @param  None
* @return None
***********************************************************************************************************************/
void spi10_interrupt(void)
{
    volatile uint8_t err_type;
    volatile uint8_t sio_dummy;

    INTC_ClearPendingIRQ(SPI10_IRQn);
    err_type = (uint8_t)(SCI0->SSR02 & _0001_SCI_OVERRUN_ERROR);
    SCI0->SIR02 = (uint16_t)err_type;

    if (1U == err_type)
    {
        spi10_callback_error(err_type);    /* overrun error occurs */
    }
    else
    {
        if (g_spi10_tx_count > 0U || g_spi10_rx_count > 1U)
        {
            /* reveive data */
            if (0U != gp_spi10_rx_address)
            {
                *gp_spi10_rx_address = SCI0->SIO10;
                gp_spi10_rx_address++;
                g_spi10_rx_count--;
            }
            else
            {
                sio_dummy = SCI0->SIO10;
            }

            /* send data */
            if (0U != gp_spi10_tx_address)
            {
                SCI0->SIO10 = *gp_spi10_tx_address;
                gp_spi10_tx_address++;
                g_spi10_tx_count--;
            }
            else if((SCI0->SMR02 & _4000_SCI_CLOCK_MODE_TI0N) == 0)
            {
                SCI0->SIO10 = 0xFFU;   // dummy write in master reception mode
            }
        }
        else 
        {
            /* receive the last data */
            if (1U == g_spi10_rx_count)
            {
                while ((SCI0->SSR02 & _0020_SCI_VALID_STORED) == 0)
                {
                    ;
                }
                if (0U != gp_spi10_rx_address)
                {
                    *gp_spi10_rx_address = SCI0->SIO10;
                     g_spi10_rx_count--;
                }
                else
                {
                    sio_dummy = SCI0->SIO10;
                }
                spi10_callback_receiveend();    /* complete receive */
            }
            /* wait the last data sent for DMA transfer in master mode */
            if (0U != gp_spi10_tx_address)
            {
                if (((SCI0->SMR02 & _4000_SCI_CLOCK_MODE_TI0N) != 0) && ((SCI0->SCR02 & _4000_SCI_RECEPTION) != 0))
                {
                    ; /* Do not wait in slave transceiver mode */
                }
                else
                {
                    while (SCI0->SSR02 & _0040_SCI_UNDER_EXECUTE)
                    {
                        ;
                    }
                }
                spi10_callback_sendend();    /* complete send */
            }
        }
    }
}
/***********************************************************************************************************************
* Function Name: spi10_callback_receiveend
* @brief  This function is a callback function when spi10 finishes reception.
* @param  None
* @return None
***********************************************************************************************************************/
static void spi10_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
    //SPI10_Stop();
    gp_spi10_rx_address = 0;    /* reinitialize global variable */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi10_callback_error
* @brief  This function is a callback function when spi10 reception error occurs.
* @param  err_type - error type value
* @return None
***********************************************************************************************************************/
static void spi10_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi10_callback_sendend
* @brief  This function is a callback function when spi10 finishes transmission.
* @param  None
* @return None
***********************************************************************************************************************/
static void spi10_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    gp_spi10_tx_address = 0;    /* reinitialize global variable */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi11_interrupt
* @brief  SPI11 interrupt service routine
* @param  None
* @return None
***********************************************************************************************************************/
void spi11_interrupt(void)
{
    volatile uint8_t err_type;
    volatile uint8_t sio_dummy;

    err_type = (uint8_t)(SCI0->SSR03 & _0001_SCI_OVERRUN_ERROR);
    SCI0->SIR03 = (uint16_t)err_type;

    if (1U == err_type)
    {
        spi11_callback_error(err_type);    /* overrun error occurs */
    }
    else
    {
        if (g_spi11_tx_count > 0U || g_spi11_rx_count > 1U)
        {
            /* reveive data */
            if (0U != gp_spi11_rx_address)
            {
                *gp_spi11_rx_address = SCI0->SIO11;
                gp_spi11_rx_address++;
                g_spi11_rx_count--;
            }
            else
            {
                sio_dummy = SCI0->SIO11;
            }

            /* send data */
            if (0U != gp_spi11_tx_address)
            {
                SCI0->SIO11 = *gp_spi11_tx_address;
                gp_spi11_tx_address++;
                g_spi11_tx_count--;
            }
            else if((SCI0->SMR03 & _4000_SCI_CLOCK_MODE_TI0N) == 0)
            {
                SCI0->SIO11 = 0xFFU;   // dummy write in master reception mode
            }
        }
        else 
        {
            /* receive the last data */
            if (1U == g_spi11_rx_count)
            {
                while ((SCI0->SSR03 & _0020_SCI_VALID_STORED) == 0)
                {
                    ;
                }
                if (0U != gp_spi11_rx_address)
                {
                    *gp_spi11_rx_address = SCI0->SIO11;
                     g_spi11_rx_count--;
                }
                else
                {
                    sio_dummy = SCI0->SIO11;
                }
                spi11_callback_receiveend();    /* complete receive */
            }
            /* wait the last data sent for DMA transfer in master mode */
            if (0U != gp_spi11_tx_address)
            {
                if (((SCI0->SMR03 & _4000_SCI_CLOCK_MODE_TI0N) != 0) && ((SCI0->SCR03 & _4000_SCI_RECEPTION) != 0))
                {
                    ; /* Do not wait in slave transceiver mode */
                }
                else
                {
                    while (SCI0->SSR03 & _0040_SCI_UNDER_EXECUTE)
                    {
                        ;
                    }
                }
                spi11_callback_sendend();    /* complete send */
            }
        }
    }
    INTC_ClearPendingIRQ(SPI11_IRQn);
}
/***********************************************************************************************************************
* Function Name: spi11_callback_receiveend
* @brief  This function is a callback function when spi11 finishes reception.
* @param  None
* @return None
***********************************************************************************************************************/
static void spi11_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
    SPI11_Stop();
    gp_spi11_rx_address = 0;    /* reinitialize global variable */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi11_callback_error
* @brief  This function is a callback function when spi11 reception error occurs.
* @param  err_type - error type value
* @return None
***********************************************************************************************************************/
static void spi11_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi11_callback_sendend
* @brief  This function is a callback function when spi11 finishes transmission.
* @param  None
* @return None
***********************************************************************************************************************/
static void spi11_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    SPI11_Stop();
    gp_spi11_tx_address = 0;    /* reinitialize global variable */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: uart2_interrupt_receive
* @brief  UART2 Receive interrupt service routine
* @param  None
* @return None
***********************************************************************************************************************/
void uart2_interrupt_receive(void)
{
    volatile uint8_t rx_data;
    volatile uint8_t err_type;
    
    INTC_ClearPendingIRQ(SR2_IRQn);
    err_type = (uint8_t)(SCI1->SSR11 & 0x0007U);
    SCI1->SIR11 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        uart2_callback_error(err_type);
    }
    
    rx_data = SCI1->RXD2;

    if (g_uart2_rx_length > g_uart2_rx_count)
    {
        *gp_uart2_rx_address = rx_data;
        gp_uart2_rx_address++;
        g_uart2_rx_count++;

        if (g_uart2_rx_length == g_uart2_rx_count)
        {
            uart2_callback_receiveend();
        }
    }
    else
    {
        uart2_callback_softwareoverrun(rx_data);
    }
    INTC_ClearPendingIRQ(SR2_IRQn);     /* clear INTSR2 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: uart2_interrupt_send
* @brief  UART2 Send interrupt service routine
* @param  None
* @return None
***********************************************************************************************************************/
void uart2_interrupt_send(void)
{
    INTC_ClearPendingIRQ(ST2_IRQn);
    if (g_uart2_tx_count > 0U)
    {
        SCI1->TXD2 = *gp_uart2_tx_address;
        gp_uart2_tx_address++;
        g_uart2_tx_count--;
    }
    else
    {
        uart2_callback_sendend();
    }
    INTC_ClearPendingIRQ(ST2_IRQn);     /* clear INTST2 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: uart2_callback_receiveend
* @brief  This function is a callback function when UART2 finishes reception.
* @param  None
* @return None
***********************************************************************************************************************/
static void uart2_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: uart2_callback_softwareoverrun
* @brief  This function is a callback function when UART2 receives an overflow data.
* @param  rx_data - receive data
* @return None
***********************************************************************************************************************/
static void uart2_callback_softwareoverrun(uint16_t rx_data)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: uart2_callback_sendend
* @brief  This function is a callback function when UART2 finishes transmission.
* @param  None
* @return None
***********************************************************************************************************************/
static void uart2_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: uart2_callback_error
* @brief  This function is a callback function when UART2 reception error occurs.
* @param  err_type - error type value
* @return None
***********************************************************************************************************************/
static void uart2_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi20_interrupt
* @brief  SPI20 interrupt service routine
* @param  None
* @return None
***********************************************************************************************************************/
void spi20_interrupt(void)
{
    volatile uint8_t err_type;
    volatile uint8_t sio_dummy;

    err_type = (uint8_t)(SCI1->SSR10 & _0001_SCI_OVERRUN_ERROR);
    SCI1->SIR10 = (uint16_t)err_type;

    if (1U == err_type)
    {
        spi20_callback_error(err_type);    /* overrun error occurs */
    }
    else
    {
        if (g_spi20_tx_count > 0U || g_spi20_rx_count > 1U)
        {
            /* reveive data */
            if (0U != gp_spi20_rx_address)
            {
                *gp_spi20_rx_address = SCI1->SIO20;
                gp_spi20_rx_address++;
                g_spi20_rx_count--;
            }
            else
            {
                sio_dummy = SCI1->SIO20;
            }

            /* send data */
            if (0U != gp_spi20_tx_address)
            {
                SCI1->SIO20 = *gp_spi20_tx_address;
                gp_spi20_tx_address++;
                g_spi20_tx_count--;
            }
            else if((SCI1->SMR10 & _4000_SCI_CLOCK_MODE_TI0N) == 0)
            {
                SCI1->SIO20 = 0xFFU;   // dummy write in master reception mode
            }
        }
        else 
        {
            /* receive the last data */
            if (1U == g_spi20_rx_count)
            {
                while ((SCI1->SSR10 & _0020_SCI_VALID_STORED) == 0)
                {
                    ;
                }
                if (0U != gp_spi20_rx_address)
                {
                    *gp_spi20_rx_address = SCI1->SIO20;
                     g_spi20_rx_count--;
                }
                else
                {
                    sio_dummy = SCI1->SIO20;
                }
                spi20_callback_receiveend();    /* complete receive */
            }
            /* wait the last data sent for DMA transfer in master mode */
            if (0U != gp_spi20_tx_address)
            {
                if (((SCI1->SMR10 & _4000_SCI_CLOCK_MODE_TI0N) != 0) && ((SCI1->SCR10 & _4000_SCI_RECEPTION) != 0))
                {
                    ; /* Do not wait in slave transceiver mode */
                }
                else
                {
                    while (SCI1->SSR10 & _0040_SCI_UNDER_EXECUTE)
                    {
                        ;
                    }
                }
                spi20_callback_sendend();    /* complete send */
            }
        }
    }
    INTC_ClearPendingIRQ(SPI20_IRQn);
}
/***********************************************************************************************************************
* Function Name: spi20_callback_receiveend
* @brief  This function is a callback function when spi20 finishes reception.
* @param  None
* @return None
***********************************************************************************************************************/
static void spi20_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
    //SPI20_Stop();
    gp_spi20_rx_address = 0;    /* reinitialize global variable */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi20_callback_error
* @brief  This function is a callback function when spi20 reception error occurs.
* @param  err_type - error type value
* @return None
***********************************************************************************************************************/
static void spi20_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi20_callback_sendend
* @brief  This function is a callback function when spi20 finishes transmission.
* @param  None
* @return None
***********************************************************************************************************************/
static void spi20_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    gp_spi20_tx_address = 0;    /* reinitialize global variable */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi21_interrupt
* @brief  SPI21 interrupt service routine
* @param  None
* @return None
***********************************************************************************************************************/
void spi21_interrupt(void)
{
    volatile uint8_t err_type;
    volatile uint8_t sio_dummy;

    err_type = (uint8_t)(SCI1->SSR11 & _0001_SCI_OVERRUN_ERROR);
    SCI1->SIR11 = (uint16_t)err_type;

    if (1U == err_type)
    {
        spi21_callback_error(err_type);    /* overrun error occurs */
    }
    else
    {
        if (g_spi21_tx_count > 0U || g_spi21_rx_count > 1U)
        {
            /* reveive data */
            if (0U != gp_spi21_rx_address)
            {
                *gp_spi21_rx_address = SCI1->SIO21;
                gp_spi21_rx_address++;
                g_spi21_rx_count--;
            }
            else
            {
                sio_dummy = SCI1->SIO21;
            }

            /* send data */
            if (0U != gp_spi21_tx_address)
            {
                SCI1->SIO21 = *gp_spi21_tx_address;
                gp_spi21_tx_address++;
                g_spi21_tx_count--;
            }
            else if((SCI1->SMR11 & _4000_SCI_CLOCK_MODE_TI0N) == 0)
            {
                SCI1->SIO21 = 0xFFU;   // dummy write in master reception mode
            }
        }
        else 
        {
            /* receive the last data */
            if (1U == g_spi21_rx_count)
            {
                while ((SCI1->SSR11 & _0020_SCI_VALID_STORED) == 0)
                {
                    ;
                }
                if (0U != gp_spi21_rx_address)
                {
                    *gp_spi21_rx_address = SCI1->SIO21;
                     g_spi21_rx_count--;
                }
                else
                {
                    sio_dummy = SCI1->SIO21;
                }
                spi21_callback_receiveend();    /* complete receive */
            }
            /* wait the last data sent for DMA transfer in master mode */
            if (0U != gp_spi21_tx_address)
            {
                if (((SCI1->SMR11 & _4000_SCI_CLOCK_MODE_TI0N) != 0) && ((SCI1->SCR11 & _4000_SCI_RECEPTION) != 0))
                {
                    ; /* Do not wait in slave transceiver mode */
                }
                else
                {
                    while (SCI1->SSR11 & _0040_SCI_UNDER_EXECUTE)
                    {
                        ;
                    }
                }
                spi21_callback_sendend();    /* complete send */
            }
        }
    }
    INTC_ClearPendingIRQ(SPI21_IRQn);
}
/***********************************************************************************************************************
* Function Name: spi21_callback_receiveend
* @brief  This function is a callback function when spi21 finishes reception.
* @param  None
* @return None
***********************************************************************************************************************/
static void spi21_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
    //SPI21_Stop();
    gp_spi21_rx_address = 0;    /* reinitialize global variable */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi21_callback_error
* @brief  This function is a callback function when spi21 reception error occurs.
* @param  err_type - error type value
* @return None
***********************************************************************************************************************/
static void spi21_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: spi21_callback_sendend
* @brief  This function is a callback function when spi21 finishes transmission.
* @param  None
* @return None
***********************************************************************************************************************/
static void spi21_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    gp_spi21_tx_address = 0;    /* reinitialize global variable */
    /* End user code. Do not edit comment generated here */
}

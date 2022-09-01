#include "../inc/hal_subghz.h"
#include "gpio.h"

//Radio SPI 
#define	Subghz_Csb_Port			PORT3
#define	Subghz_Csb_Pin			PIN2

#define	Subghz_Fcsb_Port		PORT3
#define	Subghz_Fcsb_Pin			PIN1

#define	Subghz_Sclk_Port		PORT3
#define	Subghz_Sclk_Pin			PIN6

#define	Subghz_Sdio_Port		PORT3
#define	Subghz_Sdio_Pin			PIN5

#define	cmt_spi3_csb_out()		PORT_Init(Subghz_Csb_Port, Subghz_Csb_Pin, OUTPUT)
#define	cmt_spi3_fcsb_out()		PORT_Init(Subghz_Fcsb_Port, Subghz_Fcsb_Pin, OUTPUT)
#define	cmt_spi3_sclk_out()		PORT_Init(Subghz_Sclk_Port, Subghz_Sclk_Pin, OUTPUT)
#define	cmt_spi3_sdio_out()		PORT_Init(Subghz_Sdio_Port, Subghz_Sdio_Pin, OUTPUT)
#define	cmt_spi3_sdio_in()		PORT_Init(Subghz_Sdio_Port, Subghz_Sdio_Pin, INPUT)

#define	cmt_spi3_csb_1()		PORT_SetBit(Subghz_Csb_Port, Subghz_Csb_Pin)
#define	cmt_spi3_csb_0()		PORT_ClrBit(Subghz_Csb_Port, Subghz_Csb_Pin)

#define	cmt_spi3_fcsb_1()		PORT_SetBit(Subghz_Fcsb_Port, Subghz_Fcsb_Pin)
#define	cmt_spi3_fcsb_0()		PORT_ClrBit(Subghz_Fcsb_Port, Subghz_Fcsb_Pin)

#define	cmt_spi3_sclk_1()		PORT_SetBit(Subghz_Sclk_Port, Subghz_Sclk_Pin)
#define	cmt_spi3_sclk_0()		PORT_ClrBit(Subghz_Sclk_Port, Subghz_Sclk_Pin)

#define	cmt_spi3_sdio_1()		PORT_SetBit(Subghz_Sdio_Port, Subghz_Sdio_Pin)
#define	cmt_spi3_sdio_0()		PORT_ClrBit(Subghz_Sdio_Port, Subghz_Sdio_Pin)

#define	cmt_spi3_sdio_read()	PORT_GetBit(Subghz_Sdio_Port, Subghz_Sdio_Pin)


/* ************************************************************************ */
    
void cmt_spi3_delay(void)
{
	unsigned short n = 7;
	while(n--);
}

void cmt_spi3_delay_us(void)
{
	unsigned short n = 8;
	while(n--);
}

void cmt_spi3_init(void)
{
	cmt_spi3_csb_1();
	cmt_spi3_csb_out();
	cmt_spi3_csb_1();   /* CSB has an internal pull-up resistor */

	cmt_spi3_sclk_0();
	cmt_spi3_sclk_out();
	cmt_spi3_sclk_0();   /* SCLK has an internal pull-down resistor */

	cmt_spi3_sdio_1();
	cmt_spi3_sdio_out();
	cmt_spi3_sdio_1();

	cmt_spi3_fcsb_1();
	cmt_spi3_fcsb_out();
	cmt_spi3_fcsb_1();  /* FCSB has an internal pull-up resistor */

	cmt_spi3_delay();
}

void cmt_spi3_send(uint8_t data8)
{
    uint8_t i;

    for(i=0; i<8; i++)
    {
        cmt_spi3_sclk_0();

        /* Send byte on the rising edge of SCLK */
        if(data8 & 0x80)
            cmt_spi3_sdio_1();
        else            
            cmt_spi3_sdio_0();

        cmt_spi3_delay();

        data8 <<= 1;
        cmt_spi3_sclk_1();
        cmt_spi3_delay();
    }
}

uint8_t cmt_spi3_recv(void)
{
    uint8_t i;
    uint8_t data8 = 0xFF;

    for(i=0; i<8; i++)
    {
        cmt_spi3_sclk_0();
        cmt_spi3_delay();
        data8 <<= 1;

        cmt_spi3_sclk_1();

        /* Read byte on the rising edge of SCLK */
        if(cmt_spi3_sdio_read())
            data8 |= 0x01;
        else
            data8 &= ~0x01;

        cmt_spi3_delay();
    }

    return data8;
}

void cmt_spi3_write(uint8_t addr, uint8_t dat)
{
    cmt_spi3_sdio_1();
    cmt_spi3_sdio_out();

    cmt_spi3_sclk_0();
    cmt_spi3_sclk_out();
    cmt_spi3_sclk_0(); 

    cmt_spi3_fcsb_1();
    cmt_spi3_fcsb_out();
    cmt_spi3_fcsb_1();

    cmt_spi3_csb_0();

    /* > 0.5 SCLK cycle */
    cmt_spi3_delay();
    cmt_spi3_delay();

    /* r/w = 0 */
    cmt_spi3_send(addr&0x7F);

    cmt_spi3_send(dat);

    cmt_spi3_sclk_0();

    /* > 0.5 SCLK cycle */
    cmt_spi3_delay();
    cmt_spi3_delay();

    cmt_spi3_csb_1();
    
    cmt_spi3_sdio_1();
    cmt_spi3_sdio_in();
    
    cmt_spi3_fcsb_1();    
}

void cmt_spi3_read(uint8_t addr, uint8_t* p_dat)
{
    cmt_spi3_sdio_1();
    cmt_spi3_sdio_out();

    cmt_spi3_sclk_0();
    cmt_spi3_sclk_out();
    cmt_spi3_sclk_0(); 

    cmt_spi3_fcsb_1();
    cmt_spi3_fcsb_out();
    cmt_spi3_fcsb_1();

    cmt_spi3_csb_0();

    /* > 0.5 SCLK cycle */
    cmt_spi3_delay();
    cmt_spi3_delay();

    /* r/w = 1 */
    cmt_spi3_send(addr|0x80);

    /* Must set SDIO to input before the falling edge of SCLK */
    cmt_spi3_sdio_in();
    
    *p_dat = cmt_spi3_recv();

    cmt_spi3_sclk_0();

    /* > 0.5 SCLK cycle */
    cmt_spi3_delay();
    cmt_spi3_delay();

    cmt_spi3_csb_1();
    
    cmt_spi3_sdio_1();
    cmt_spi3_sdio_in();
    
    cmt_spi3_fcsb_1();
}

void cmt_spi3_write_fifo(const uint8_t* p_buf, uint16_t len)
{
    uint8_t i;

    cmt_spi3_fcsb_1();
    cmt_spi3_fcsb_out();
    cmt_spi3_fcsb_1();

    cmt_spi3_csb_1();
    cmt_spi3_csb_out();
    cmt_spi3_csb_1();

    cmt_spi3_sclk_0();
    cmt_spi3_sclk_out();
    cmt_spi3_sclk_0();

    cmt_spi3_sdio_out();

    for(i=0; i<len; i++)
    {
        cmt_spi3_fcsb_0();

        /* > 1 SCLK cycle */
        cmt_spi3_delay();
        cmt_spi3_delay();

        cmt_spi3_send(p_buf[i]);

        cmt_spi3_sclk_0();

        /* > 2 us */
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();

        cmt_spi3_fcsb_1();

        /* > 4 us */
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
    }

    cmt_spi3_sdio_in();
    
    cmt_spi3_fcsb_1();
}

void cmt_spi3_read_fifo(uint8_t* p_buf, uint16_t len)
{
    uint16_t i;

    cmt_spi3_fcsb_1();
    cmt_spi3_fcsb_out();
    cmt_spi3_fcsb_1();

    cmt_spi3_csb_1();
    cmt_spi3_csb_out();
    cmt_spi3_csb_1();

    cmt_spi3_sclk_0();
    cmt_spi3_sclk_out();
    cmt_spi3_sclk_0();

    cmt_spi3_sdio_in();

    for(i=0; i<len; i++)
    {
        cmt_spi3_fcsb_0();

        /* > 1 SCLK cycle */
        cmt_spi3_delay();
        cmt_spi3_delay();

        p_buf[i] = cmt_spi3_recv();

        cmt_spi3_sclk_0();

        /* > 2 us */
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();

        cmt_spi3_fcsb_1();

        /* > 4 us */
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
    }

    cmt_spi3_sdio_in();
    
    cmt_spi3_fcsb_1();
}

void CMT2300A_InitGpio(void)
{
	cmt_spi3_init();
}

/*! ********************************************************
* @name    CMT2300A_ReadReg
* @desc    Read the CMT2300A register at the specified address.
* @param   addr: register address
* @return  Register value
* *********************************************************/
uint8_t CMT2300A_ReadReg(uint8_t addr)
{
    uint8_t dat = 0xFF;
    cmt_spi3_read(addr, &dat);
	
    return dat;
}

/*! ********************************************************
* @name    CMT2300A_WriteReg
* @desc    Write the CMT2300A register at the specified address.
* @param   addr: register address
*          dat: register value
* *********************************************************/
void CMT2300A_WriteReg(uint8_t addr, uint8_t dat)
{
    cmt_spi3_write(addr, dat);
}

/*! ********************************************************
* @name    CMT2300A_ReadFifo
* @desc    Reads the contents of the CMT2300A FIFO.
* @param   buf: buffer where to copy the FIFO read data
*          len: number of bytes to be read from the FIFO
* *********************************************************/
void CMT2300A_ReadFifo(uint8_t buf[], uint16_t len)
{
    cmt_spi3_read_fifo(buf, len);
}

/*! ********************************************************
* @name    CMT2300A_WriteFifo
* @desc    Writes the buffer contents to the CMT2300A FIFO.
* @param   buf: buffer containing data to be put on the FIFO
*          len: number of bytes to be written to the FIFO
* *********************************************************/
void CMT2300A_WriteFifo(const uint8_t buf[], uint16_t len)
{
    cmt_spi3_write_fifo(buf, len);
}


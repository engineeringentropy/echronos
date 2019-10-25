#include "samd21-usart.h"


/* This configuration function follows the steps in 26.6.2.1 of the SAM D21 Family Datasheet */
void samd21_usart_configure()
{
    uint32_t ctrlA = SERCOM_USART_CTRLA_MODE_USART_INT_CLK |
                     /* SERCOM_USART_CTRLA_CMODE | */ /* Set to 0 for asynchronous */
                     SERCOM_USART_CTRLA_RXPO() |
                     SERCOM_USART_CTRLA_TXPO() |
                     SERCOM_USART_CTRLA_DORD | /* Set to one for LSB first, 0 for MSB first */
                     SERCOM_USART_CTRLA_FORM(0) | /* Parity off */
                     0;
    uint32_t ctrlB = SERCOM_USART_CTRLB_CHSIZE(0) | /* 0 == 8 bits */
                     /* SERCOM_USART_CTRLB_SBMODE | */ /* 0 == one stop bit, 1 == two stop bits */
                     0;
    uint32_t baud = 55470; /* TODO: Replace this with a calculation - this is 9600 at 1MHz */

    SercomUsart* sercom = (SercomUsart*)SERCOM0;
    sercom->CTRLA = ctrlA;
    sercom->CTRLB = ctrlB;
    sercom->BAUD = baud;
    sercom->INTERNCLR = SERCOM_USART_INTENCLR_MASK;
}

void samd21_usart_enable()
{
    SercomUsart* sercom = (SercomUsart*)SERCOM0;
    sercom->CTRLA |= SERCOM_USART_CTRLA_ENABLE;
    sercom->CTRLA |= SERCOM_USART_CTRLB_TXEN;
    sercom->CTRLB |= SERCOM_USART_CTRLB_RXEN;
}

void samd21_usart_disable()
{
    SercomUsart* sercom = (SercomUsart*)SERCOM0;
    sercom->CTRLA &= ~SERCOM_USART_CTRLA_ENABLE;
    sercom->CTRLA &= ~SERCOM_USART_CTRLB_TXEN;
    sercom->CTRLB &= ~SERCOM_USART_CTRLB_RXEN;
}

void samd21_usart_send_byte(uint8_t b)
{
    SercomUsart* sercom = (SercomUsart*)SERCOM0;
    while (!sercom->INTFLAG.DRE) {}
    sercom->DATA.reg = (uint16_t)b;
}
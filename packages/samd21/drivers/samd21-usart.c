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
    uint32_t ctrlB = SERCOM_USART_CTRLB_CHSIZE(0) | /* 0 == 8 bits */
                     /* SERCOM_USART_CTRLB_SBMODE | */ /* 0 == one stop bit, 1 == two stop bits */
    uint32_t baud =      
}
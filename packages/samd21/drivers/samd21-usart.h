#ifndef SAMD21_USART_H_INCLUDED_
#define SAMD21_USART_H_INCLUDED_

#include "samd21.h"

extern void samd21_usart_configure();
extern void samd21_usart_enable();
extern void samd21_usart_disable();
extern void samd21_usart_send_byte(uint8_t b);

#endif /* SAMD21_USART_H_INCLUDED_ */

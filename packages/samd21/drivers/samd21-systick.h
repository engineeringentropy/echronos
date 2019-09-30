#ifndef SAMD21_SYSTICK_H_INCLUDED_
#define SAMD21_SYSTICK_H_INCLUDED_

#include <stdint.h>
#include "samd21.h"

/* SAMD21 SYSTICK counter is clocked off the main clock.
 * It's a 24-bit countdown that fires the Systick IRQ when it hits zero.
 */

#define SAMD21_SYSTICK_MASK (0x00FFFFFF)

extern void samd21_systick_init();
extern void samd21_systick_enable();
extern void samd21_systick_disable();
extern void samd21_systick_set_count(uint32_t count);

#endif /* SAMD21_SYSTICK_H_INCLUDED_ */

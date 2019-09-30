#ifndef SAMD21_REALTIMECOUNTER_INCLUDED_H_
#define SAMD21_REALTIMECOUNTER_INCLUDED_H_

#include <stdint.h>
#include "samd21.h"

uint32_t samd21_rtc_get_count();

void samd21_rtc_init_count32();

#endif /* SAMD21_REALTIMECOUNTER_INCLUDED_H_ */

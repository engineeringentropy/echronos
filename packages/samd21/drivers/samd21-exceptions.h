#ifndef SAMD21_EXCEPTIONS_H_INCLUDED_
#define SAMD21_EXCEPTIONS_H_INCLUDED_

#include "samd21.h"

/* We need access to this elsewhere.
 * It breaks the abstraction, but we should be able to fix that later.
 */
extern DeviceVectors exception_table;

#endif /* SAMD21_EXCEPTIONS_H_INCLUDED_ */

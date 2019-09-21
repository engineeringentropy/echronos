#include <stdint.h>

/* One stack per task.
 * Remember that there're only 32KiB on the processor.
 */
static uint32_t stack_{{idx}}[{{stack_size}}] __attribute__((aligned(8)));

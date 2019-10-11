#include <stdint.h>
#include "samd21.h"

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

void __libc_init_array(void);

/* Default empty handler */
void Dummy_Handler(void);
void Reset_Handler(void);

#define VNUL (Dummy_Handler)
/* Exception Table */
__attribute__ ((section(".vectors")))
const DeviceVectors exception_table = {
        (void*) (&_estack), 
        Reset_Handler, 
        VNUL, VNUL, /* NMI, HardFault */
        VNUL, VNUL, VNUL, VNUL, VNUL, VNUL, VNUL, /* Reserved */
        VNUL, /* SVC_Handler */
        VNUL, VNUL, /* Reserved */
        VNUL, VNUL, /* PendSV, SysTick */
        VNUL, VNUL, VNUL, VNUL, VNUL, VNUL, VNUL, /* PM, SYSCTRL, WDT, RTC, EIC, NVMCTRL, DMAC */
        VNUL, /* USB Handler */
        VNUL, VNUL, VNUL, VNUL, VNUL, VNUL, VNUL, /* EVSYS, SERCOM0-5 */
        VNUL, VNUL, VNUL, VNUL, VNUL, VNUL, VNUL, VNUL, /* TCC1, TCC2, TC3-TC7 */
        VNUL, VNUL, VNUL, VNUL, VNUL, VNUL /* ADC, AC, DAC, PTC, I2S, AC1, TCC3 */
};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
        uint32_t *pSrc, *pDest;

        /* Initialize the relocate segment */
        pSrc = &_etext;
        pDest = &_srelocate;

        if (pSrc != pDest) {
                for (; pDest < &_erelocate;) {
                        *pDest++ = *pSrc++;
                }
        }

        /* Clear the zero segment */
        for (pDest = &_szero; pDest < &_ezero;) {
                *pDest++ = 0;
        }

        /* Set the vector table base address */
        pSrc = (uint32_t *) & _sfixed;
        SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

        /* Change default QOS values to have the best performance and correct USB behaviour */
        SBMATRIX->SFR[SBMATRIX_SLAVE_HMCRAMC0].reg = 2;
#if defined(ID_USB)
        USB->DEVICE.QOSCTRL.bit.CQOS = 2;
        USB->DEVICE.QOSCTRL.bit.DQOS = 2;
#endif
        DMAC->QOSCTRL.bit.DQOS = 2;
        DMAC->QOSCTRL.bit.FQOS = 2;
        DMAC->QOSCTRL.bit.WRBQOS = 2;

        /* Overwriting the default value of the NVMCTRL.CTRLB.MANW bit (errata reference 13134) */
        NVMCTRL->CTRLB.bit.MANW = 1;

        /* Initialize the C library */
        __libc_init_array();

        /* Branch to main function */
        main();

        /* Infinite loop */
        while (1);
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Dummy_Handler(void)
{
        while (1) {
        }
}
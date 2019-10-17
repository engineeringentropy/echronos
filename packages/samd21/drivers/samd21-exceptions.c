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

/* External functions */
extern int main();

void __libc_init_array(void);

/* Default empty handler */
void Dummy_Handler(void);
void Reset_Handler(void);

#define REF_HANDLER(name) name

/* This is 512 bytes of MSP stack */
uint32_t MSPStack[128] __attribute__((aligned(8)));
#define MSPStackEnd ((void*)&MSPStack[129])

uint32_t debug_cookie;
void set_cookie(uint32_t c)
{
        debug_cookie = c;
}

#define VNUL (Dummy_Handler)

extern void rtos_internal_svc_handler();
extern void rtos_internal_pendsv_handler();
extern void exception_preempt_trampoline_systick();

/* Exception handlers so that we can tell which exception got taken */
void exception_hard_fault()
{
        while(1) { }
}

/* Exception Table */
__attribute__ ((section(".vectors")))
DeviceVectors exception_table = {
        .pvStack = MSPStackEnd,
        .pfnReset_Handler = REF_HANDLER(Reset_Handler),
        .pfnNMI_Handler = VNUL,
        .pfnHardFault_Handler = REF_HANDLER(exception_hard_fault),
        .pvReservedM12 = VNUL, /* NMI, HardFault */
        .pvReservedM11 = VNUL,
        .pvReservedM10 = VNUL,
        .pvReservedM9 = VNUL,
        .pvReservedM8 = VNUL,
        .pvReservedM7 = VNUL,
        .pvReservedM6 = VNUL, /* Reserved */
        .pfnSVC_Handler = REF_HANDLER(rtos_internal_svc_handler), /* SVC_Handler */
        .pvReservedM4 = VNUL,
        .pvReservedM3 = VNUL, /* Reserved */
        .pfnPendSV_Handler = REF_HANDLER(rtos_internal_pendsv_handler),
        .pfnSysTick_Handler = REF_HANDLER(exception_preempt_trampoline_systick), /* PendSV, SysTick */
        .pfnPM_Handler = VNUL,
        .pfnSYSCTRL_Handler = VNUL,
        .pfnWDT_Handler = VNUL,
        .pfnRTC_Handler = VNUL,
        .pfnEIC_Handler = VNUL,
        .pfnNVMCTRL_Handler = VNUL,
        .pfnDMAC_Handler = VNUL, /* PM, SYSCTRL, WDT, RTC, EIC, NVMCTRL, DMAC */
        .pfnUSB_Handler = VNUL, /* USB Handler */
        .pfnEVSYS_Handler = VNUL,
        .pfnSERCOM0_Handler = VNUL,
        .pfnSERCOM1_Handler = VNUL,
        .pfnSERCOM2_Handler = VNUL,
        .pfnSERCOM3_Handler = VNUL,
        .pfnSERCOM4_Handler = VNUL,
        .pfnSERCOM5_Handler = VNUL, /* EVSYS, SERCOM0-5 */
        .pfnTCC0_Handler = VNUL,
        .pfnTCC1_Handler = VNUL,
        .pfnTCC2_Handler = VNUL,
        .pfnTC3_Handler = VNUL,
        .pfnTC4_Handler = VNUL,
        .pfnTC5_Handler = VNUL,
        .pvReserved21 = VNUL,
        .pvReserved22 = VNUL, /* TCC1, TCC2, TC3-TC7 */
        .pfnADC_Handler = VNUL,
        .pfnAC_Handler = VNUL,
        .pfnDAC_Handler = VNUL,
        .pfnPTC_Handler = VNUL,
        .pfnI2S_Handler = VNUL,
        .pvReserved28 = VNUL /* ADC, AC, DAC, PTC, I2S, AC1, TCC3 */
};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
        uint32_t *pSrc, *pDest;

        set_cookie(0);

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
        set_cookie(1);
        main();

        /* Infinite loop */
        while (1);
}

void Dummy_Handler(void)
{
        while (1) {
        }
}
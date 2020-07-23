# Architecture Notes for SAML10
Note that we're deliberately not talking about the SAML11

## SAML10:
    - One MPU for 4 regions
    - One SysTick timer (clocked by core frequency)
    - One NVIC Table
    - No Security Attribution Unit

## eChronos components
    - components/context-switch-preempt-saml10 +
    - components/context-switch-saml10 + 
    - components/interrupt-event-saml10 +
    - components/stack-saml10 +
    - components/timer-saml10 +
    - packages/saml10
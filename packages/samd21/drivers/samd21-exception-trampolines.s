/*
 * From the ARM Procedure Call Standard:
 *  A subroutine must preserve the contents of the registers r4-r8, r10, r11 and SP 
 *       (and r9 in PCS variants that designate r9 as v6).
 * This routine is cribbed from the ARMv7 implementation.
 */

/*<module>
  <code_gen>template</code_gen>
  <schema>
   <entry name="trampolines" type="list" default="[]">
     <entry name="trampoline" type="dict">
      <entry name="name" type="c_ident" />
      <entry name="handler" type="c_ident" />
     </entry>
   </entry>
  </schema>
</module>*/                                                 

/**
 * Set the 'preemption pending' status so that a preemption will occur at the soonest possible opportunity.
 */
.global rtos_internal_preempt_pend
/* void rtos_internal_preempt_pend(void); */
rtos_internal_preempt_pend:
        ldr r0, =0xE000ED04
        ldr r1, =0x10000000
        str r1, [r0]
        bx lr

trampoline_completion:
        /* If the handler returned 0, jump to the final bx as pre-emption is not required. */
        cmp r0, #0
        beq exception_return

        /* Set PendSV to do the loop */
        ldr r0, =0xE000ED04
        ldr r1, =0x10000000
        str r1, [r0]

exception_return:
        bx lr

{{#trampolines}}
.global exception_preempt_trampoline_{{name}}
exception_preempt_trampoline_{{name}}:
        /* Note: We don't care about saving the value of ip (it is scratch), but it is important to keep the stack
         * 8-byte aligned, so push it as a dummy 
         * On entry to an exception, the stack is guaranteed to be 8 byte aligned.
         * NOTE: This is ON THE HANDLER/MSP stack, NOT THE PSP stack!
         */
        push {r7, lr}
        bl {{handler}}
        b trampoline_completion
{{/trampolines}}

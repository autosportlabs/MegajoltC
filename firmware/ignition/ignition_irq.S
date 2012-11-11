

@
@  IRQ wrappers.
@
@  These call a C function.
@  We switch to supervisor mode and reenable interrupts to allow nesting.
@
@

  .text
  .code 32
  .align   2

@
@  Macros
@

  .macro   irq_wrapper_nested, C_function

@ Save registers on stack
  sub r14,r14,#4 @ fix up for return
  stmfd r13!,{r14}
  mrs  r14,spsr
  stmfd r13!,{r14}

@ Acknowledge the IVR for debugging to support Protected Mode
  ldr   r14,=0xFFFFF100
  str   r14,[r14]

 @ swich to system mode and enable IRQ, but not FIQ
  msr cpsr_c,#0x5F

  @push stack
  stmfd r13!,{r0-r12,r14}


@ Call the function
  ldr r0,=\C_function
  mov lr,pc
  bx  r0

  @ pop stack
   ldmfd r13!,{r0-r12,r14}

 @ swich to interrupt mode and disable IRQs and FIQs
  msr cpsr_c,#0xD2

@End of interrupt by doing a write to AIC_EOICR
  ldr  r14,=0xFFFFF130
  str  r14,[r14]

  @ Unstack the saved spsr
  ldmfd r13!,{r14}
  msr  spsr_all,r14

  @ Return from interrupt (unstacking the modified r14)
  ldmfd r13!,{pc}^

  .endm

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  .macro   irq_wrapper_not_nested, C_function

@ Save registers on stack
  sub r14,r14,#4 @ fix up for return
  stmfd r13!,{r0-r12,r14}

@ Acknowledge the IVR for debugging to support Protected Mode
  ldr   r14,=0xFFFFF100
  str   r14,[r14]

@ Call the function
  ldr r0,=\C_function
  mov lr,pc
  bx  r0

@End of interrupt by doing a write to AIC_EOICR
  ldr  r14,=0xFFFFF130
  str  r14,[r14]

  @ Return from interrupt (unstacking the modified r14)
  ldmfd r13!,{r0-r12,pc}^

  .endm

@
@	ISRs
@
@


  .extern coilPackCharge_irq_handler
  .global coilPackCharge_irq_entry
coilPackCharge_irq_entry:
  irq_wrapper_nested coilPackCharge_irq_handler

  .extern coilPackFire_irq_handler
  .global coilPackFire_irq_entry
coilPackFire_irq_entry:
  irq_wrapper_nested coilPackFire_irq_handler

  .extern triggerWheel_irq_handler
  .global triggerWheel_irq_entry
triggerWheel_irq_entry:
  irq_wrapper_nested triggerWheel_irq_handler

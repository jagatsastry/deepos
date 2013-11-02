.global _irq0
# 32: IRQ0
_irq0:
    cli
    jmp irq_common_stub_0

.extern irq_handler
irq_common_stub_0:
            pushq %rax
            pushq %rcx
            pushq %rdx
            pushq %rsi
            pushq %rdi
            pushq %r8
            pushq %r9
            pushq %r10
            pushq %r11
            movq  %rsp,%rdi
            addq  $72, %rdi
            call irq0_handler
            popq %r11
            popq %r10
            popq %r9
            popq %r8
            popq %rdi
            popq %rsi
            popq %rdx
            popq %rcx
            popq %rax
            sti
        iretq


.global _irq1
# 32: IRQ1
_irq1:
    cli
    jmp irq_common_stub_1

.extern kb_handler
irq_common_stub_1:
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
            call irq1_handler
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

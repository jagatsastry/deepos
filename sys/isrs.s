#Based on Bran's tutorial at osdever.net/bkerndev
.global _isr0
_isr0:
    cli
    jmp isr_common_stub

.extern fault_handler

isr_common_stub:
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
            call fault_handler 
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

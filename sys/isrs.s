#Based on Bran's tutorial at osdever.net/bkerndev
.global _isr0
.global _isr80

_isr0:
    cli
    jmp isr_common_stub

_isr80:
    jmp isr_common_stub_syscall

.extern fault_handler
.extern syscall_handler


isr_common_stub: 
            pushq %rax
            pushq %rbx 
            pushq %rcx 
            pushq %rdx 
            pushq %rsi 
            pushq %rdi 
            pushq %r8 
            pushq %r9 
            pushq %r10 
            pushq %r11 
            movq  %rsp,%rdi 
            addq  $80, %rdi  
            call fault_handler 
            popq %r11 
            popq %r10 
            popq %r9 
            popq %r8 
            popq %rdi 
            popq %rsi 
            popq %rdx 
            popq %rcx 
            popq %rbx 
            popq %rax 
            sti
        iretq

isr_common_stub_syscall:
            pushq %rax
            pushq %rbx
            pushq %rcx
            pushq %rdx
            pushq %rsi
            pushq %rdi
            pushq %rbp
            pushq %r8
            pushq %r9
            pushq %r10
            pushq %r11
            pushq %r12
            pushq %r13
            pushq %r14
            pushq %r15
            movq  %rsp,%rdi
            call syscall_handler
            popq %r15
            popq %r14
            popq %r13
            popq %r12
            popq %r11
            popq %r10
            popq %r9
            popq %r8
            popq %rbp
            popq %rdi
            popq %rsi
            popq %rdx
            popq %rcx
            popq %rbx
            popq %rax
         iretq 

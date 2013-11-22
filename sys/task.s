
.global _enter_user_mode
_enter_user_mode:
    pushfq
    popq %rbx
    cli
    movq $0x23, %rax  # user mode data selector is 0x20 (GDT entry 3). Also sets RPL to 3
    movq %rax, %ds
    movq %rax, %es
    movq %rax, %fs
    movq %rax, %gs
    pushq $0x23 #   ; SS, notice it uses same selector as above
    pushq %rsp #   ; RSP
    pushq %rbx #      ; RFLAGS
    #pushfq
    pushq $0x1b  # ; CS, user mode code selector is 0x18. With RPL 3 this is 0x1b
    leaq 1f, %rax #  ; RIP first
    pushq %rax
    iretq
    1:
      addq $0x8, %rsp #Fix the stack. Stack still has 0x23. It needs return rip.
      ret

.global _idt_load

_idt_load:
    lidt (%rdi)
    retq

void exit(int status) {
     __asm__( "movq $0x01, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"g" (&status):"rbx","rdx") ;
    return;
}

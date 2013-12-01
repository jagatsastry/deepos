void sleep(int sec) {
     __asm__( "movq $0x06, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"g" (&sec):"rbx","rdx") ;
    return;
}

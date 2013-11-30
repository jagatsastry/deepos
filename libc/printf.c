int printf(const char *format, ...) {
       __asm__( "movq $0x00, %%rbx;\
                 movq %0, %%rdx;\
                 movq $0x0C, %%rcx;\
                 int $0x80;\
                 "::"g" (format)) ;
	return 0;
}

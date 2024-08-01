#include "cpuid_asm.h"

void cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        __asm volatile("cpuid"
            : "=a" (*eax),
              "=b" (*ebx),
              "=c" (*ecx),
              "=d" (*edx)
            : "0" (*eax), "2" (*ecx));
}

/**
 * Test for a Cyrix cpu if there's no cpuid
 * @see https://elixir.bootlin.com/linux/v6.10.2/source/arch/x86/kernel/cpu/cyrix.c#L397
 */
static int cyrix_52_test(void) {
        unsigned int test;

        __asm__ __volatile__(
             "sahf\n\t"		/* clear flags (%eax = 0x0005) */
             "div %b2\n\t"	/* divide 5 by 2 */
             "lahf"		/* store flags into %ah */
             : "=a" (test)
             : "0" (5), "q" (2)
             : "cc");

        /* AH is 0x02 on Cyrix after the divide.. */
        return (unsigned char) (test >> 8) == 0x02;
}

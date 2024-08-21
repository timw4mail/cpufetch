#ifndef __CPUID_ASM__
#define __CPUID_ASM__

#include <stdint.h>

void cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
static int cyrix_52_test(void);

#endif

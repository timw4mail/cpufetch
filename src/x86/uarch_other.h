#ifndef __UARCH__
#define __UARCH__

#include <stdint.h>

#include "cpuid.h"
#include "uarch.h"

char* infer_other_cpu_name_from_uarch(struct uarch* arch);
struct uarch* get_uarch_from_cpuid_other(struct cpuInfo* cpu, uint32_t ef, uint32_t f, uint32_t em, uint32_t m, int s);

#endif

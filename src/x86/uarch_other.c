#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uarch.h"
#include "../common/global.h"

/**
 * Microarchitecture lookup for smaller / more obscure vendors 
 */
 
// If we cannot get the CPU name from CPUID, try to infer it from uarch
char* infer_other_cpu_name_from_uarch(struct uarch* arch) {
  char* cpu_name = NULL;
  if (arch == NULL) {
    printErr("infer_cpu_name_from_uarch: Unable to find CPU name");
    cpu_name = ecalloc(strlen(STRING_UNKNOWN) + 1, sizeof(char));
    strcpy(cpu_name, STRING_UNKNOWN);
    return cpu_name;
  }
  
  return cpu_name;
}

struct uarch* get_uarch_from_cpuid_cyrix(uint32_t ef, uint32_t f, uint32_t em, uint32_t m, int s) {
  struct uarch *arch = emalloc(sizeof(struct uarch));

  // EF: Extended Family                                                           //
  // F:  Family                                                                    //
  // EM: Extended Model                                                            //
  // M: Model                                                                      //
  // S: Stepping                                                                   //
  // ----------------------------------------------------------------------------- //
  //                 EF  F  EM   M   S                                             //
  UARCH_START
  CHECK_UARCH(arch,  0,  4,  0,  9,  NA, "5x86",        UARCH_5X86,         UNK) // https://sandpile.org/x86/cpuid.htm#level_0000_0001h
  CHECK_UARCH(arch,  0,  5,  0,  2,  NA, "M1 (6x86)",     UARCH_M1,           UNK) // https://sandpile.org/x86/cpuid.htm#level_0000_0001h
  CHECK_UARCH(arch,  0,  5,  0,  4,  NA, "MediaGX GXm", UARCH_MEDIA_GX,     350) // https://www.cpu-world.com/CPUs/MediaGX/index.html
  CHECK_UARCH(arch,  0,  6,  0,  0,   1, "M2 (6x86MX)",   UARCH_M2,           UNK) // https://sandpile.org/x86/cpuid.htm#level_0000_0001h
  UARCH_END

  return arch;
}

struct uarch* get_uarch_from_cpuid_natsemi(uint32_t ef, uint32_t f, uint32_t em, uint32_t m, int s) {
  struct uarch *arch = emalloc(sizeof(struct uarch));

  // EF: Extended Family                                                           //
  // F:  Family                                                                    //
  // EM: Extended Model                                                            //
  // M: Model                                                                      //
  // S: Stepping                                                                   //
  // ----------------------------------------------------------------------------- //
  //                 EF  F  EM   M   S                                             //
  UARCH_START
  CHECK_UARCH(arch,  0,  5,  0,  4,  NA, "Geode GX1",  UARCH_GEODE,        180) // https://sandpile.org/x86/cpuid.htm#level_0000_0001h
  CHECK_UARCH(arch,  0,  5,  0,  5,  NA, "Geode GX2",  UARCH_GEODE,        150) // https://www.cpu-world.com/CPUs/Geode_GX2/index.html
  UARCH_END

  return arch;
}

struct uarch* get_uarch_from_cpuid_other(struct cpuInfo* cpu, uint32_t dump, uint32_t ef, uint32_t f, uint32_t em, uint32_t m, int s) {
  switch (cpu->cpu_vendor) {
    case CPU_VENDOR_CYRIX:
      return get_uarch_from_cpuid_cyrix(ef, f, em, m, s);

    case CPU_VENDOR_NATSEMI:
      return get_uarch_from_cpuid_natsemi(ef, f, em, m, s);

    default:
      printBug("Invalid CPU vendor: %d", cpu->cpu_vendor);
      return NULL;
  }
}

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
  // TODO: implement
}
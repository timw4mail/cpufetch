#ifndef __SOC__
#define __SOC__

#include "../common/cpu.h"
#include <stdint.h>

typedef int32_t SOC;

enum {
  SOC_VENDOR_UNKNOWN,
  SOC_VENDOR_SIFIVE,
  SOC_VENDOR_STARFIVE
};

struct system_on_chip {
  SOC soc_model;
  VENDOR soc_vendor;
  int32_t process;
  char* soc_name;
  char* raw_name;
};

struct system_on_chip* get_soc(void);
char* get_soc_name(struct system_on_chip* soc);
VENDOR get_soc_vendor(struct system_on_chip* soc);
char* get_str_process(struct system_on_chip* soc);

#endif

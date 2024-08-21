#ifndef __UARCH__
#define __UARCH__

#include <stdint.h>

#include "cpuid.h"
#include "../common/global.h"

typedef uint32_t MICROARCH;

// Data not available
#define NA                   -1

// Unknown manufacturing process
#define UNK                  -1

enum {
  UARCH_UNKNOWN,
  // INTEL //
  UARCH_I486,
  UARCH_P5,
  UARCH_P5_MMX,
  UARCH_P6_PRO,
  UARCH_P6_PENTIUM_II,
  UARCH_P6_PENTIUM_III,
  UARCH_DOTHAN,
  UARCH_YONAH,
  UARCH_MEROM,
  UARCH_PENYR,
  UARCH_NEHALEM,
  UARCH_WESTMERE,
  UARCH_BONNELL,
  UARCH_SALTWELL,
  UARCH_SANDY_BRIDGE,
  UARCH_SILVERMONT,
  UARCH_IVY_BRIDGE,
  UARCH_HASWELL,
  UARCH_BROADWELL,
  UARCH_AIRMONT,
  UARCH_KABY_LAKE,
  UARCH_COMET_LAKE,
  UARCH_ROCKET_LAKE,
  UARCH_AMBER_LAKE,
  UARCH_WHISKEY_LAKE,
  UARCH_SKYLAKE,
  UARCH_CASCADE_LAKE,
  UARCH_COOPER_LAKE,
  UARCH_KNIGHTS_LANDING,
  UARCH_KNIGHTS_MILL,
  UARCH_GOLDMONT,
  UARCH_PALM_COVE,
  UARCH_SUNNY_COVE,
  UARCH_GOLDMONT_PLUS,
  UARCH_TREMONT,
  UARCH_LAKEMONT,
  UARCH_COFFEE_LAKE,
  UARCH_ITANIUM,
  UARCH_KNIGHTS_FERRY,
  UARCH_KNIGHTS_CORNER,
  UARCH_WILLAMETTE,
  UARCH_NORTHWOOD,
  UARCH_PRESCOTT,
  UARCH_CEDAR_MILL,
  UARCH_ITANIUM2,
  UARCH_ICE_LAKE,
  UARCH_TIGER_LAKE,
  UARCH_ALDER_LAKE,
  UARCH_RAPTOR_LAKE,
  // AMD //
  UARCH_AM486,
  UARCH_AM5X86,
  UARCH_SSA5,
  UARCH_K5,
  UARCH_K6,
  UARCH_K7,
  UARCH_K8,
  UARCH_K10,
  UARCH_PUMA_2008,
  UARCH_BOBCAT,
  UARCH_BULLDOZER,
  UARCH_PILEDRIVER,
  UARCH_STEAMROLLER,
  UARCH_EXCAVATOR,
  UARCH_JAGUAR,
  UARCH_PUMA_2014,
  UARCH_ZEN,
  UARCH_ZEN_PLUS,
  UARCH_ZEN2,
  UARCH_ZEN3,
  UARCH_ZEN3_PLUS,
  UARCH_ZEN4,
  UARCH_ZEN4C,
  // Cyrix & National Semiconductor //
  UARCH_5X86,
  UARCH_M1,
  UARCH_MEDIA_GX,
  UARCH_M2,
  UARCH_GEODE
};

struct uarch {
  MICROARCH uarch;
  char* uarch_str;
  int32_t process; // measured in nanometers
};

void fill_uarch(struct uarch* arch, char* str, MICROARCH u, uint32_t process);

#define UARCH_START if (false) {}
#define CHECK_UARCH(arch, ef_, f_, em_, m_, s_, str, uarch, process) \
   else if (ef_ == ef && f_ == f && (em_ == NA || em_ == em) && (m_ == NA || m_ == m) && (s_ == NA || s_ == s)) fill_uarch(arch, str, uarch, process);
#define UARCH_END else { printBugCheckRelease("Unknown microarchitecture detected: M=0x%X EM=0x%X F=0x%X EF=0x%X S=0x%X", m, em, f, ef, s); \
fill_uarch(arch, STRING_UNKNOWN, UARCH_UNKNOWN, UNK); }

inline void fill_uarch(struct uarch* arch, char* str, MICROARCH u, uint32_t process)
 {
  arch->uarch_str = emalloc(sizeof(char) * (strlen(str)+1));
  strcpy(arch->uarch_str, str);
  arch->uarch = u;
  arch->process= process;
}

struct uarch* get_uarch_from_cpuid(struct cpuInfo* cpu, uint32_t dump, uint32_t ef, uint32_t f, uint32_t em, uint32_t m, int s);
char* infer_cpu_name_from_uarch(struct uarch* arch);
bool vpus_are_AVX512(struct cpuInfo* cpu);
bool is_knights_landing(struct cpuInfo* cpu);
int get_number_of_vpus(struct cpuInfo* cpu);
bool choose_new_intel_logo_uarch(struct cpuInfo* cpu);
char* get_str_uarch(struct cpuInfo* cpu);
char* get_str_process(struct cpuInfo* cpu);
void free_uarch_struct(struct uarch* arch);

#endif

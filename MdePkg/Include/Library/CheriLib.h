#ifndef CHERI_LIB
#define CHERI_LIB

#include "cherireg.h"

#define cheri_setaddress(x, y)  __builtin_cheri_address_set((x), (y))
#define cheri_setbounds(x, y)   __builtin_cheri_bounds_set((x), (y))
#define cheri_clearperm(x, y)   __builtin_cheri_perms_and((x), ~(y))
#define cheri_getlen(x) __builtin_cheri_length_get((x))
#define cheri_getoffset(x)      __builtin_cheri_offset_get((x))
#define cheri_gettag(x)         __builtin_cheri_tag_get((x))
#define cheri_getpcc() __builtin_cheri_program_counter_get()

#if 0
void *kernel_root_cap = (void *)(INTPTR_T) - 1;

static inline VOID *
MakeCap(UINT64 addr)
{
  VOID *Cap;

  /* TODO */

#ifdef __CHERI_PURE_CAPABILITY__
  Cap = (VOID *)cheri_setaddress(kernel_root_cap, addr);
#else
  Cap = (VOID *)addr;
#endif

  return (Cap);
};

static inline UINTPTR_T
MakeUCap(UINT64 addr)
{
  UINTPTR_T Cap;

  /* TODO */

#ifdef __CHERI_PURE_CAPABILITY__
  Cap = (VOID *)cheri_setaddress(kernel_root_cap, addr);
#else
  Cap = (VOID *)addr;
#endif

  return (Cap);
};
#endif

VOID * MakeCap(UINT64 addr);
UINTPTR_T MakeUCap(UINT64 addr);
VOID cheri_init_capabilities(VOID * __capability kroot);
void crt_init_globals(void *arg, void *data_cap, void *code_cap, UINT64 base_addr, UINT64 obj_offset);

#endif

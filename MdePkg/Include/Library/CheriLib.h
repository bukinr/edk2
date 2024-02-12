#ifndef CHERI_LIB
#define CHERI_LIB

#include "cherireg.h"

#define cheri_setaddress(x, y)  __builtin_cheri_address_set((x), (y))
#define cheri_setbounds(x, y)   __builtin_cheri_bounds_set((x), (y))
#define cheri_clearperm(x, y)   __builtin_cheri_perms_and((x), ~(y))

#if 0
void *kernel_root_cap = (void *)(INTPTR_T) - 1;

static inline UINTPTR_T
MakeCap(UINTN addr)
{
  UINTPTR_T Cap;

  /* TODO */

#ifdef __CHERI_PURE_CAPABILITY__
  Cap = (UINTPTR_T)cheri_setaddress(kernel_root_cap, addr);
#else
aaa
  Cap = (UINTPTR_T)addr;
#endif

  return (Cap);
};
#endif

UINTPTR_T MakeCap(UINTN addr);
VOID cheri_init_capabilities(VOID * __capability kroot);

#endif

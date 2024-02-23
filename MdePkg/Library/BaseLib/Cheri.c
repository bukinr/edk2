#include <Library/cherireg.h>

#define cheri_setaddress(x, y)  __builtin_cheri_address_set((x), (y))
#define cheri_setbounds(x, y)   __builtin_cheri_bounds_set((x), (y))
#define cheri_clearperm(x, y)   __builtin_cheri_perms_and((x), ~(y))

//static void *kernel_root_cap = (void *)(INTPTR_T) - 1;

#if __has_feature(capabilities)
static void *kernel_root_cap;

VOID
cheri_init_capabilities(VOID * __capability kroot)
{

  kernel_root_cap = kroot;

}
#endif

//  kernel_root_cap = cheri_clearperm(kroot, (CHERI_PERM_SEAL | CHERI_PERM_UNSEAL));


UINTPTR_T
MakeUCap(UINT64 addr)
{
  UINTPTR_T Cap;

  /* TODO */

#ifdef __CHERI_PURE_CAPABILITY__
  Cap = (UINTPTR_T)cheri_setaddress(kernel_root_cap, addr);
#else
  Cap = (UINTPTR_T)addr;
#endif

  return (Cap);
};

VOID *
MakeCap(UINT64 addr)
{
  VOID * Cap;

  /* TODO */

#ifdef __CHERI_PURE_CAPABILITY__
  Cap = (VOID *)cheri_setaddress(kernel_root_cap, addr);
#else
  Cap = (VOID *)addr;
#endif

  return (Cap);
};

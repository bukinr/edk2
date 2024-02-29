/** @file
  Entry point to the DXE Core.

Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>

#include <Library/DxeCoreEntryPoint.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/CheriLib.h>

//
// Cache copy of HobList pointer.
//
VOID  *gHobList = NULL;

/**
  The entry point of PE/COFF Image for the DXE Core.

  This function is the entry point for the DXE Core. This function is required to call
  ProcessModuleEntryPointList() and ProcessModuleEntryPointList() is never expected to return.
  The DXE Core is responsible for calling ProcessLibraryConstructorList() as soon as the EFI
  System Table and the image handle for the DXE Core itself have been established.
  If ProcessModuleEntryPointList() returns, then ASSERT() and halt the system.

  @param  HobStart  The pointer to the beginning of the HOB List passed in from the PEI Phase.

**/
VOID
EFIAPI
_ModuleEntryPoint (
  IN VOID  *HobStart
  )
{
  VOID* ddc_reg;
  VOID* dcc_reg;
  __volatile__ UINT64 addr;

  __asm__ __volatile__("mrs %0, ddc" : "=C" (ddc_reg));
  __asm__ __volatile__("adr %0, #0" : "=C" (dcc_reg));

  addr = ((UINT64)cheri_getpcc() & ~0xfff) - 0x1000;

  crt_init_globals(NULL, ddc_reg, dcc_reg, addr, 0);
  cheri_init_capabilities(ddc_reg);

  DEBUG((DEBUG_LOAD | DEBUG_INFO, "%a: Image at addr %x relocated\r\n",
    __func__, addr));

  //
  // Cache a pointer to the HobList
  //
  gHobList = HobStart;

  //
  // Call the DXE Core entry point
  //
  ProcessModuleEntryPointList (HobStart);

  //
  // Should never return
  //
  ASSERT (FALSE);
  CpuDeadLoop ();
}

/**
  Required by the EBC compiler and identical in functionality to _ModuleEntryPoint().

  This function is required to call _ModuleEntryPoint() passing in HobStart.

  @param  HobStart  The pointer to the beginning of the HOB List passed in from the PEI Phase.

**/
VOID
EFIAPI
EfiMain (
  IN VOID  *HobStart
  )
{
  _ModuleEntryPoint (HobStart);
}

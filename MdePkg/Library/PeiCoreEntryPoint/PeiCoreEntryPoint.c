/** @file
  Entry point to a the PEI Core.

Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>

//
// The Library classes this module produced
//
#include <Library/PeiCoreEntryPoint.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/CheriLib.h>

/**
  The entry point of PE/COFF Image for the PEI Core.

  This function is the entry point for the PEI Foundation, which allows the SEC phase
  to pass information about the stack, temporary RAM and the Boot Firmware Volume.
  In addition, it also allows the SEC phase to pass services and data forward for use
  during the PEI phase in the form of one or more PPIs.
  There is no limit to the number of additional PPIs that can be passed from SEC into
  the PEI Foundation. As part of its initialization phase, the PEI Foundation will add
  these SEC-hosted PPIs to its PPI database such that both the PEI Foundation and any
  modules can leverage the associated service calls and/or code in these early PPIs.
  This function is required to call ProcessModuleEntryPointList() with the Context
  parameter set to NULL.  ProcessModuleEntryPoint() is never expected to return.
  The PEI Core is responsible for calling ProcessLibraryConstructorList() as soon as
  the PEI Services Table and the file handle for the PEI Core itself have been established.
  If ProcessModuleEntryPointList() returns, then ASSERT() and halt the system.

  @param SecCoreData  Points to a data structure containing information about the
                      PEI core's operating environment, such as the size and
                      location of temporary RAM, the stack location and the BFV
                      location.

  @param PpiList      Points to a list of one or more PPI descriptors to be
                      installed initially by the PEI core. An empty PPI list
                      consists of a single descriptor with the end-tag
                      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST.
                      As part of its initialization phase, the PEI Foundation will
                      add these SEC-hosted PPIs to its PPI database, such that both
                      the PEI Foundation and any modules can leverage the associated
                      service calls and/or code in these early PPIs.

**/

VOID
EFIAPI
_ModuleEntryPoint (
  IN CONST  EFI_SEC_PEI_HAND_OFF    *SecCoreData,
  IN CONST  EFI_PEI_PPI_DESCRIPTOR  *PpiList
  )
{
  VOID* ddc_reg;
  VOID* dcc_reg;
  __volatile__ UINT64 addr;

#if 0
  if ((UINT64)SecCoreData->BootFirmwareVolumeBase == 0x12345678)
    while (1);
  if ((UINT64)SecCoreData->TemporaryRamBase == 0x12345679)
    while (1);
#endif

  __asm__ __volatile__("mrs %0, ddc" : "=C" (ddc_reg));
  __asm__ __volatile__("adr %0, #0" : "=C" (dcc_reg));

  addr = ((UINT64)cheri_getpcc() & ~0xfff) - 0x1000;

  crt_init_globals(NULL, ddc_reg, dcc_reg, addr, 0);
  cheri_init_capabilities(ddc_reg);

  DEBUG((DEBUG_LOAD | DEBUG_INFO, "Image at addr %x relocated\r\n", addr));

  ProcessModuleEntryPointList (SecCoreData, PpiList, NULL);

  //
  // Should never return
  //
  ASSERT (FALSE);
  CpuDeadLoop ();
}

/**
  Required by the EBC compiler and identical in functionality to _ModuleEntryPoint().

  This function is required to call _ModuleEntryPoint() passing in SecCoreData and PpiList.

  @param SecCoreData  Points to a data structure containing information about the PEI core's
                      operating environment, such as the size and location of temporary RAM,
                      the stack location and the BFV location.

  @param PpiList      Points to a list of one or more PPI descriptors to be installed
                      initially by the PEI core.  An empty PPI list consists of
                      a single descriptor with the end-tag
                      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST.
                      As part of its initialization phase, the PEI Foundation will
                      add these SEC-hosted PPIs to its PPI database, such that both
                      the PEI Foundationand any modules can leverage the associated
                      service calls and/or code in these early PPIs.

**/
VOID
EFIAPI
EfiMain (
  IN CONST  EFI_SEC_PEI_HAND_OFF    *SecCoreData,
  IN CONST  EFI_PEI_PPI_DESCRIPTOR  *PpiList
  )
{
  _ModuleEntryPoint (SecCoreData, PpiList);
}

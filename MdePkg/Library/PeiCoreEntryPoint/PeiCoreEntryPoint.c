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

  __asm__ __volatile__("mov c11, c0");
  __asm__ __volatile__("mov c12, c1");

  __asm__ __volatile__("mrs c1, ddc");
  __asm__ __volatile__("adr c2, #0");
  __asm__ __volatile__("mov x3, #0xB000");
  __asm__ __volatile__("movk x3, #0xE003, lsl 16");
  __asm__ __volatile__("bl crt_init_globals");

  __asm__ __volatile__("mov c0, c11");
  __asm__ __volatile__("mov c1, c12");
  __asm__ __volatile__("mov x2, 0");

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

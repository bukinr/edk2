/** @file
  Entry point to a PEIM.

Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>

#include <Library/PeimEntryPoint.h>
#include <Library/DebugLib.h>
#include <Library/CheriLib.h>

/**
  The entry point of PE/COFF Image for a PEIM.

  This function is the entry point for a PEIM.  This function must call ProcessLibraryConstructorList()
  and ProcessModuleEntryPointList().  The return value from ProcessModuleEntryPointList() is returned.
  If _gPeimRevision is not zero and PeiServices->Hdr.Revision is less than _gPeimRevison, then ASSERT().

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval  EFI_SUCCESS   The PEIM executed normally.
  @retval  !EFI_SUCCESS  The PEIM failed to execute normally.
**/
EFI_STATUS
EFIAPI
_ModuleEntryPoint (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  VOID* ddc_reg;
  VOID* dcc_reg;
  __volatile__ UINT64 addr;

  addr = ((UINT64)cheri_getpcc() & ~0xfff) - 0x1000;

  __asm__ __volatile__("mrs %0, ddc" : "=C" (ddc_reg));
  __asm__ __volatile__("adr %0, #0" : "=C" (dcc_reg));
  crt_init_globals(NULL, ddc_reg, dcc_reg, addr, 0);
  cheri_init_capabilities(ddc_reg);

  DEBUG((DEBUG_LOAD | DEBUG_INFO, "PEIM Image Relocated\r\n"));

  if (_gPeimRevision != 0) {
    //
    // Make sure that the PEI spec revision of the platform is >= PEI spec revision of the driver
    //
    ASSERT ((*PeiServices)->Hdr.Revision >= _gPeimRevision);
  }

  //
  // Call constructor for all libraries
  //
  ProcessLibraryConstructorList (FileHandle, PeiServices);

  //
  // Call the driver entry point
  //
  return ProcessModuleEntryPointList (FileHandle, PeiServices);
}

/**
  Required by the EBC compiler and identical in functionality to _ModuleEntryPoint().

  This function is required to call _ModuleEntryPoint() passing in FileHandle and PeiServices.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCCESS  The PEIM executed normally.
  @retval !EFI_SUCCESS The PEIM failed to execute normally.

**/
EFI_STATUS
EFIAPI
EfiMain (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  return _ModuleEntryPoint (FileHandle, PeiServices);
}

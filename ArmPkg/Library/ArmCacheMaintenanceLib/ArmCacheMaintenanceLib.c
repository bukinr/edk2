/** @file

  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  Copyright (c) 2011 - 2021, ARM Limited. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Base.h>
#include <Library/ArmLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

STATIC
VOID
CacheRangeOperation (
  IN  VOID            *Start,
  IN  UINTN           Length,
  IN  LINE_OPERATION  LineOperation,
  IN  UINTN           LineLength
  )
{
  UINTN  ArmCacheLineAlignmentMask;
  // Align address (rounding down)
  UINTPTR_T  AlignedAddress;
  UINTPTR_T  EndAddress;

  ArmCacheLineAlignmentMask = LineLength - 1;
  AlignedAddress            = (UINTPTR_T)Start - ((UINTPTR_T)Start & ArmCacheLineAlignmentMask);
  EndAddress                = (UINTPTR_T)Start + Length;

  // Perform the line operation on an address in each cache line
  while (AlignedAddress < EndAddress) {
    LineOperation (AlignedAddress);
    AlignedAddress += LineLength;
  }

  ArmDataSynchronizationBarrier ();
}

VOID
EFIAPI
InvalidateInstructionCache (
  VOID
  )
{
  ASSERT (FALSE);
}

VOID
EFIAPI
InvalidateDataCache (
  VOID
  )
{
  ASSERT (FALSE);
}

VOID *
EFIAPI
InvalidateInstructionCacheRange (
  IN      VOID   *Address,
  IN      UINTN  Length
  )
{
  CacheRangeOperation (
    Address,
    Length,
    ArmCleanDataCacheEntryToPoUByMVA,
    ArmDataCacheLineLength ()
    );
  CacheRangeOperation (
    Address,
    Length,
    ArmInvalidateInstructionCacheEntryToPoUByMVA,
    ArmInstructionCacheLineLength ()
    );

  ArmInstructionSynchronizationBarrier ();

  return Address;
}

VOID
EFIAPI
WriteBackInvalidateDataCache (
  VOID
  )
{
  ASSERT (FALSE);
}

VOID *
EFIAPI
WriteBackInvalidateDataCacheRange (
  IN      VOID   *Address,
  IN      UINTN  Length
  )
{
  CacheRangeOperation (
    Address,
    Length,
    ArmCleanInvalidateDataCacheEntryByMVA,
    ArmDataCacheLineLength ()
    );
  return Address;
}

VOID
EFIAPI
WriteBackDataCache (
  VOID
  )
{
  ASSERT (FALSE);
}

VOID *
EFIAPI
WriteBackDataCacheRange (
  IN      VOID   *Address,
  IN      UINTN  Length
  )
{
  CacheRangeOperation (
    Address,
    Length,
    ArmCleanDataCacheEntryByMVA,
    ArmDataCacheLineLength ()
    );
  return Address;
}

VOID *
EFIAPI
InvalidateDataCacheRange (
  IN      VOID   *Address,
  IN      UINTN  Length
  )
{
  CacheRangeOperation (
    Address,
    Length,
    ArmInvalidateDataCacheEntryByMVA,
    ArmDataCacheLineLength ()
    );
  return Address;
}

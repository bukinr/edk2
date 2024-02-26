//
//  Copyright (c) 2011-2014, ARM Limited. All rights reserved.
//
//  SPDX-License-Identifier: BSD-2-Clause-Patent
//
//

#include <AsmMacroIoLibV8.h>
#include "armreg.h"
#include "hypervisor.h"
#include <Library/CheriLib.h>

#define LENTRY(sym)                                   \
	.text		;                             \
	.align 2; .type sym,#function; sym:           \

#define LEND(sym) .ltorg	; .size sym, . - sym

/*
 * If we are started in EL2, configure the required hypervisor
 * registers and drop to EL1.
 */
LENTRY(drop_to_el1)
#ifdef __CHERI_PURE_CAPABILITY__
	.arch_extension noc64
	.arch_extension a64c
#endif
	mrs	x23, CurrentEL
	lsr	x23, x23, #2
	cmp	x23, #0x2
	b.eq	1f
	ret
1:

#if 1
	/*
	 * On some hardware, e.g., Apple M1, we can't clear E2H, so make sure we
	 * don't trap to EL2 for SIMD register usage to have at least a
	 * minimally usable system.
	 */
	tst	x4, #HCR_E2H
	mov	x3, #CPTR_RES1	/* HCR_E2H == 0 */
	mov	x5, #CPTR_FPEN	/* HCR_E2H == 1 */
	csel	x2, x3, x5, eq
	msr	cptr_el2, x2
#endif
	
#if __has_feature(capabilities)
	/*
	 * Wait for the write to cptr_el2 to complete. It will enable the
	 * use of capabilities at EL2 that we need below. When not using
	 * capabilities this is unneeded as the eret instruction will
	 * act as in place of this barrier.
	 */
	isb
#endif

#if 1
	/* Don't trap to EL2 for CP15 traps */
	msr	hstr_el2, xzr

	/* Enable access to the physical timers at EL1 */
	mrs	x2, cnthctl_el2
	orr	x2, x2, #(CNTHCTL_EL1PCTEN | CNTHCTL_EL1PCEN)
	msr	cnthctl_el2, x2

	/* Set the counter offset to a known value */
	msr	cntvoff_el2, xzr

	/* Zero vttbr_el2 so a hypervisor can tell the host and guest apart */
	msr	vttbr_el2, xzr

	mov	x2, #(PSR_DAIF | PSR_M_EL1h)
	msr	spsr_el2, x2
#endif

#ifdef __CHERI_PURE_CAPABILITY__
	/* Enter exception handlers in C64 mode */
	mrs	x2, cctlr_el2
	orr	x2, x2, #(CCTLR_EL2_C64E_MASK)
	msr	cctlr_el2, x2

	/* Clear DDC_EL2 */
	msr	ddc, czr
#endif
	/* Set the address to return to our return address */
#if __has_feature(capabilities)
	cvtp	c30, x30
	msr	celr_el2, c30
#else
	msr	elr_el2, x30
#endif
	isb

	eret
#ifdef __CHERI_PURE_CAPABILITY__
	.arch_extension c64
#endif

	.align 3
.Lsctlr_res1:
	.quad SCTLR_RES1
LEND(drop_to_el1)

/*
 * Initialize morello on a cpu
 */
.macro morello_cpu_init
#if __has_feature(capabilities)
	/* Enable Morello instructions at EL0 and EL1 */
	mrs     x2, cpacr_el1
	bic     x2, x2, CPACR_CEN_MASK
	orr     x2, x2, CPACR_CEN_TRAP_NONE
	msr     cpacr_el1, x2
	isb

	/*
	 * Allow access to CNTVCT_EL0 without PCC System permission and enable
	 * capability sealing for branch and link at EL0.
         *
	 * XXXBFG should this be done somewhere else? Maybe eventually per-process or
	 * compartment?
	 */
	mrs x2, cctlr_el0
	orr x2, x2, #(CCTLR_PERMVCT_MASK | CCTLR_SBL_MASK)
	msr cctlr_el0, x2

#ifdef __CHERI_PURE_CAPABILITY__
	/*
	 * Enable capablity sealing for branch and link at EL1
	 * Use PCC/DDC address interpretation.
	 * Use DDC as base for adrdp.
	 */
	mrs     x2, cctlr_el1
	bic     x2, x2, #(CCTLR_PCCBO_MASK | CCTLR_DDCBO_MASK | CCTLR_ADRDPB_MASK)
	orr     x2, x2, #(CCTLR_SBL_MASK)
	msr     cctlr_el1, x2

	/* We assume that we enter here in a64 mode. */
	bx      #4
	.arch_extension c64
#endif
#endif
.endmacro

ASM_FUNC(_ModuleEntryPoint)
  hlt 0x1
#ifdef __CHERI_PURE_CAPABILITY__
.arch_extension noc64
.arch_extension a64c
#endif

  mov x19, x0
  mov x20, x1

  bl drop_to_el1

  morello_cpu_init

#if 1
  ldr     x0, =~CHERI_PERMS_KERNEL_DATA
  mrs     c1, ddc
  clrperm c1, c1, x0
  ldr     x0, =~CHERI_PERMS_KERNEL_CODE
  adr     c2, #0
  clrperm c2, c2, x0
  mov     x3, 0x3000
  movk    x3, 0xe002, lsl 16
  mov     x4, 0
  bl      crt_init_globals
#else
 #ifdef __CHERI_PURE_CAPABILITY__
	        /* Do capability relocations */
	        ldr     x0, =~CHERI_PERMS_KERNEL_DATA
	        mrs     c1, ddc
	        clrperm c1, c1, x0
	        ldr     x0, =~CHERI_PERMS_KERNEL_CODE
	        adr     c2, #0
	        clrperm c2, c2, x0
	        /* Assume can we reach _DYNAMIC from PCC */
	        adrp    c0, _DYNAMIC
	        add     c0, c0, :lo12:_DYNAMIC
	        //bl      elf_reloc_self
  #endif
#endif
	
  /* Initialize root capability. */
  mrs   c0, ddc
  bl    cheri_init_capabilities

  mov c0, c19
  mov c1, c20

  // Do early platform specific actions
  bl    ASM_PFX(ArmPlatformPeiBootAction)

// NOTE: We could be booting from EL3, EL2 or EL1. Need to correctly detect
//       and configure the system accordingly. EL2 is default if possible.
// If we started in EL3 we need to switch and run at EL2.
// If we are running at EL2 stay in EL2
// If we are starting at EL1 stay in EL1.	

// If started at EL3 Sec is run and switches to EL2 before jumping to PEI.
// If started at EL1 or EL2 Sec jumps directly to PEI without making any
// changes.

// Which EL are we running at? Every EL needs some level of setup...
// We should not run this code in EL3
  EL1_OR_EL2(x0)
1:bl    ASM_PFX(SetupExceptionLevel1)
  b     ASM_PFX(MainEntryPoint)
2:bl    ASM_PFX(SetupExceptionLevel2)
  b     ASM_PFX(MainEntryPoint)

ASM_PFX(MainEntryPoint):
  // Identify CPU ID
  bl    ASM_PFX(ArmReadMpidr)
  // Keep a copy of the MpId register value
  mov   x5, x0

  // Is it the Primary Core ?
  bl    ASM_PFX(ArmPlatformIsPrimaryCore)

  // Get the top of the primary stacks (and the base of the secondary stacks)
  MOV64 (x1, FixedPcdGet64(PcdCPUCoresStackBase) + FixedPcdGet32(PcdCPUCorePrimaryStackSize))
  cvtd c1, x1

  // x0 is equal to 1 if I am the primary core
  cmp   x0, #1
  b.eq   _SetupPrimaryCoreStack

_SetupSecondaryCoreStack:
  // x1 contains the base of the secondary stacks

  // Get the Core Position
  mov   x6, x1      // Save base of the secondary stacks
  mov   x0, x5
  bl    ASM_PFX(ArmPlatformGetCorePosition)
  // The stack starts at the top of the stack region. Add '1' to the Core Position to get the top of the stack
  add   x0, x0, #1

  // StackOffset = CorePos * StackSize
  MOV32 (x2, FixedPcdGet32(PcdCPUCoreSecondaryStackSize))
  mul   x0, x0, x2
  // SP = StackBase + StackOffset
  add   sp, x6, x0

_PrepareArguments:
  // The PEI Core Entry Point has been computed by GenFV and stored in the second entry of the Reset Vector
  MOV64 (x2, FixedPcdGet64(PcdFvBaseAddress))
  cvtd  c2, x2
  ldr   x1, [x2, #8]
  cvtd  c1, x1

  // Move sec startup address into a data register
  // Ensure we're jumping to FV version of the code (not boot remapped alias)
  // ldr   x3, =ASM_PFX(CEntryPoint)
  // cvtd  c3, x3
  adr c3, CEntryPoint

  // Set the frame pointer to NULL so any backtraces terminate here
  mov   x29, xzr

  // Jump to PrePeiCore C code
  //    x0 = mp_id
  //    c1 = pei_core_address
  mov   x0, x5
  blr   c3

_SetupPrimaryCoreStack:
  mov   csp, c1
  MOV64 (x8, FixedPcdGet64 (PcdCPUCoresStackBase))
  MOV64 (x9, FixedPcdGet32 (PcdInitValueInTempStack) |\
             FixedPcdGet32 (PcdInitValueInTempStack) << 32)
  cvtd  c8, x8
  cvtd  c9, x9

0:stp   c9, c9, [c8], #32
  cmp   x8, x1
  b.lt  0b
  b     _PrepareArguments

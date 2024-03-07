#------------------------------------------------------------------------------
#
# Copyright (c) 2009-2013, ARM Ltd.  All rights reserved.
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
#------------------------------------------------------------------------------
.text
.p2align 3

GCC_ASM_EXPORT(SetJump)
GCC_ASM_EXPORT(InternalLongJump)

#define GPR_LAYOUT                         \
        REG_PAIR (c19, c20,  0);           \
        REG_PAIR (c21, c22, 32);           \
        REG_PAIR (c23, c24, 64);           \
        REG_PAIR (c25, c26, 96);           \
        REG_PAIR (c27, c28, 128);           \
        REG_PAIR (c29, c30, 160);/*FP, LR*/ \
        REG_ONE  (c16,      192) /*IP0*/

#define FPR_LAYOUT                      \
        REG_PAIR ( d8,  d9, 208);       \
        REG_PAIR (d10, d11, 240);       \
        REG_PAIR (d12, d13, 272);       \
        REG_PAIR (d14, d15, 304);

#/**
#  Saves the current CPU context that can be restored with a call to LongJump() and returns 0.#
#
#  Saves the current CPU context in the buffer specified by JumpBuffer and returns 0.  The initial
#  call to SetJump() must always return 0.  Subsequent calls to LongJump() cause a non-zero
#  value to be returned by SetJump().
#
#  If JumpBuffer is NULL, then ASSERT().
#  For IPF CPUs, if JumpBuffer is not aligned on a 16-byte boundary, then ASSERT().
#
#  @param  JumpBuffer    A pointer to CPU context buffer.
#
#**/
#
#UINTN
#EFIAPI
#SetJump (
#  IN      BASE_LIBRARY_JUMP_BUFFER  *JumpBuffer  // X0
#  );
#
ASM_PFX(SetJump):
        AARCH64_BTI(c)
        mov     c16, csp // use IP0 so save SP
#define REG_PAIR(REG1, REG2, OFFS)      stp REG1, REG2, [c0, OFFS]
#define REG_ONE(REG1, OFFS)             str REG1, [c0, OFFS]
        GPR_LAYOUT
        FPR_LAYOUT
#undef REG_PAIR
#undef REG_ONE
        mov     w0, #0
        ret

#/**
#  Restores the CPU context that was saved with SetJump().#
#
#  Restores the CPU context from the buffer specified by JumpBuffer.
#  This function never returns to the caller.
#  Instead is resumes execution based on the state of JumpBuffer.
#
#  @param  JumpBuffer    A pointer to CPU context buffer.
#  @param  Value         The value to return when the SetJump() context is restored.
#
#**/
#VOID
#EFIAPI
#InternalLongJump (
#  IN      BASE_LIBRARY_JUMP_BUFFER  *JumpBuffer,  // X0
#  IN      UINTN                     Value         // X1
#  );
#
ASM_PFX(InternalLongJump):
        AARCH64_BTI(c)
#define REG_PAIR(REG1, REG2, OFFS)      ldp REG1, REG2, [c0, OFFS]
#define REG_ONE(REG1, OFFS)             ldr REG1, [c0, OFFS]
        GPR_LAYOUT
        FPR_LAYOUT
#undef REG_PAIR
#undef REG_ONE
        mov     csp, c16
        cmp     w1, #0
        mov     w0, #1
        csel    w0, w1, w0, ne
        ret

ASM_FUNCTION_REMOVE_IF_UNREFERENCED

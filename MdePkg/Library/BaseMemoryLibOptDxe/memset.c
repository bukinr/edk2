/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 1992-2007 The FreeBSD Project. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if 0
#include <sys/cdefs.h>
#define	LIBKERN_INLINE

#include <sys/types.h>
#include <sys/libkern.h>
#endif

#if 0
#define CAPABILITY_VERSION

#ifdef CAPABILITY_VERSION
#define	__CAP		__capability
#else
#define	__CAP
#endif

#define	__CAPSUFFIX(func)	(func)

void * __CAP
__CAPSUFFIX(InternalMemSetMem)(void * __CAP b, int c, __SIZE_TYPE__ len)
{
	char * __CAP bb;

	for (bb = (char * __CAP)b; len--; )
		*bb++ = c;
	return (b);
}

void * __CAP
__CAPSUFFIX(InternalMemSetMem16)(void * __CAP b, int c, __SIZE_TYPE__ len)
{
	char * __CAP bb;

	for (bb = (char * __CAP)b; len--; )
		*bb++ = c;
	return (b);
}

void * __CAP
__CAPSUFFIX(InternalMemSetMem32)(void * __CAP b, int c, __SIZE_TYPE__ len)
{
	char * __CAP bb;

	for (bb = (char * __CAP)b; len--; )
		*bb++ = c;
	return (b);
}

void * __CAP
__CAPSUFFIX(InternalMemSetMem64)(void * __CAP b, int c, __SIZE_TYPE__ len)
{
	char * __CAP bb;

	for (bb = (char * __CAP)b; len--; )
		*bb++ = c;
	return (b);
}

void * __CAP
__CAPSUFFIX(InternalMemZeroMem)(void * __CAP b, UINTN len)
{
	char * __CAP bb;

	for (bb = (char * __CAP)b; len--; )
		*bb++ = 0;
	return (b);
}
#endif

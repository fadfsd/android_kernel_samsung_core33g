/*
 * kgdb support for ARC
 *
 * Copyright (C) 2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ARC_KGDB_H__
#define __ARC_KGDB_H__

#ifdef CONFIG_KGDB

#include <asm/ptrace.h>

/* to ensure compatibility with Linux 2.6.35, we don't implement the get/set
 * register API yet */
#undef DBG_MAX_REG_NUM

<<<<<<< HEAD
#define GDB_MAX_REGS		87
=======
#define GDB_MAX_REGS		39
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

#define BREAK_INSTR_SIZE	2
#define CACHE_FLUSH_IS_SAFE	1
#define NUMREGBYTES		(GDB_MAX_REGS * 4)
#define BUFMAX			2048

static inline void arch_kgdb_breakpoint(void)
{
	__asm__ __volatile__ ("trap_s	0x4\n");
}

extern void kgdb_trap(struct pt_regs *regs, int param);

<<<<<<< HEAD
/* This is the numbering of registers according to the GDB. See GDB's
 * arc-tdep.h for details.
 *
 * Registers are ordered for GDB 7.5. It is incompatible with GDB 6.8. */
enum arc_linux_regnums {
=======
enum arc700_linux_regnums {
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	_R0		= 0,
	_R1, _R2, _R3, _R4, _R5, _R6, _R7, _R8, _R9, _R10, _R11, _R12, _R13,
	_R14, _R15, _R16, _R17, _R18, _R19, _R20, _R21, _R22, _R23, _R24,
	_R25, _R26,
<<<<<<< HEAD
	_FP		= 27,
	__SP		= 28,
	_R30		= 30,
	_BLINK		= 31,
	_LP_COUNT	= 60,
	_STOP_PC	= 64,
	_RET		= 64,
	_LP_START	= 65,
	_LP_END		= 66,
	_STATUS32	= 67,
	_ECR		= 76,
	_BTA		= 82,
=======
	_BTA		= 27,
	_LP_START	= 28,
	_LP_END		= 29,
	_LP_COUNT	= 30,
	_STATUS32	= 31,
	_BLINK		= 32,
	_FP		= 33,
	__SP		= 34,
	_EFA		= 35,
	_RET		= 36,
	_ORIG_R8	= 37,
	_STOP_PC	= 38
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
};

#else
#define kgdb_trap(regs, param)
#endif

#endif	/* __ARC_KGDB_H__ */

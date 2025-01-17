/*
 * arch/xtensa/include/asm/traps.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2012 Tensilica Inc.
 */
#ifndef _XTENSA_TRAPS_H
#define _XTENSA_TRAPS_H

#include <asm/ptrace.h>

/*
 * handler must be either of the following:
 *  void (*)(struct pt_regs *regs);
 *  void (*)(struct pt_regs *regs, unsigned long exccause);
 */
extern void * __init trap_set_handler(int cause, void *handler);
extern void do_unhandled(struct pt_regs *regs, unsigned long exccause);

static inline void spill_registers(void)
{
<<<<<<< HEAD
#if XCHAL_NUM_AREGS > 16
	__asm__ __volatile__ (
		"	call8	1f\n"
		"	_j	2f\n"
		"	retw\n"
		"	.align	4\n"
		"1:\n"
#if XCHAL_NUM_AREGS == 32
		"	_entry	a1, 32\n"
		"	addi	a8, a0, 3\n"
		"	_entry	a1, 16\n"
		"	mov	a12, a12\n"
		"	retw\n"
#else
		"	_entry	a1, 48\n"
		"	call12	1f\n"
		"	retw\n"
		"	.align	4\n"
		"1:\n"
		"	.rept	(" __stringify(XCHAL_NUM_AREGS) " - 16) / 12\n"
		"	_entry	a1, 48\n"
		"	mov	a12, a0\n"
		"	.endr\n"
		"	_entry	a1, 16\n"
#if XCHAL_NUM_AREGS % 12 == 0
		"	mov	a12, a12\n"
#elif XCHAL_NUM_AREGS % 12 == 4
		"	mov	a4, a4\n"
#elif XCHAL_NUM_AREGS % 12 == 8
		"	mov	a8, a8\n"
#endif
		"	retw\n"
#endif
		"2:\n"
		: : : "a8", "a9", "memory");
#else
	__asm__ __volatile__ (
		"	mov	a12, a12\n"
		: : : "memory");
#endif
=======

	__asm__ __volatile__ (
		"movi	a14, "__stringify((1 << PS_EXCM_BIT) | LOCKLEVEL)"\n\t"
		"mov	a12, a0\n\t"
		"rsr	a13, sar\n\t"
		"xsr	a14, ps\n\t"
		"movi	a0, _spill_registers\n\t"
		"rsync\n\t"
		"callx0 a0\n\t"
		"mov	a0, a12\n\t"
		"wsr	a13, sar\n\t"
		"wsr	a14, ps\n\t"
		: :
#if defined(CONFIG_FRAME_POINTER)
		: "a2", "a3", "a4",       "a11", "a12", "a13", "a14", "a15",
#else
		: "a2", "a3", "a4", "a7", "a11", "a12", "a13", "a14", "a15",
#endif
		  "memory");
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

#endif /* _XTENSA_TRAPS_H */

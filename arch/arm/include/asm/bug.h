#ifndef _ASMARM_BUG_H
#define _ASMARM_BUG_H

#include <linux/linkage.h>
<<<<<<< HEAD
#include <linux/types.h>
#include <asm/opcodes.h>
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

#ifdef CONFIG_BUG

/*
 * Use a suitable undefined instruction to use for ARM/Thumb2 bug handling.
 * We need to be careful not to conflict with those used by other modules and
 * the register_undef_hook() system.
 */
#ifdef CONFIG_THUMB2_KERNEL
#define BUG_INSTR_VALUE 0xde02
<<<<<<< HEAD
#define BUG_INSTR(__value) __inst_thumb16(__value)
#else
#define BUG_INSTR_VALUE 0xe7f001f2
#define BUG_INSTR(__value) __inst_arm(__value)
=======
#define BUG_INSTR_TYPE ".hword "
#else
#define BUG_INSTR_VALUE 0xe7f001f2
#define BUG_INSTR_TYPE ".word "
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#endif


#define BUG() _BUG(__FILE__, __LINE__, BUG_INSTR_VALUE)
#define _BUG(file, line, value) __BUG(file, line, value)

#ifdef CONFIG_DEBUG_BUGVERBOSE

/*
 * The extra indirection is to ensure that the __FILE__ string comes through
 * OK. Many version of gcc do not support the asm %c parameter which would be
 * preferable to this unpleasantness. We use mergeable string sections to
 * avoid multiple copies of the string appearing in the kernel image.
 */

#define __BUG(__file, __line, __value)				\
do {								\
<<<<<<< HEAD
	asm volatile("1:\t" BUG_INSTR(__value) "\n"  \
=======
	asm volatile("1:\t" BUG_INSTR_TYPE #__value "\n"	\
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		".pushsection .rodata.str, \"aMS\", %progbits, 1\n" \
		"2:\t.asciz " #__file "\n" 			\
		".popsection\n" 				\
		".pushsection __bug_table,\"a\"\n"		\
		"3:\t.word 1b, 2b\n"				\
		"\t.hword " #__line ", 0\n"			\
		".popsection");					\
	unreachable();						\
} while (0)

#else  /* not CONFIG_DEBUG_BUGVERBOSE */

#define __BUG(__file, __line, __value)				\
do {								\
<<<<<<< HEAD
	asm volatile(BUG_INSTR(__value) "\n");			\
=======
	asm volatile(BUG_INSTR_TYPE #__value);			\
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	unreachable();						\
} while (0)
#endif  /* CONFIG_DEBUG_BUGVERBOSE */

#define HAVE_ARCH_BUG
#endif  /* CONFIG_BUG */

#include <asm-generic/bug.h>

struct pt_regs;
void die(const char *msg, struct pt_regs *regs, int err);

struct siginfo;
void arm_notify_die(const char *str, struct pt_regs *regs, struct siginfo *info,
		unsigned long err, unsigned long trap);

#ifdef CONFIG_ARM_LPAE
#define FAULT_CODE_ALIGNMENT	33
#define FAULT_CODE_DEBUG	34
#else
#define FAULT_CODE_ALIGNMENT	1
#define FAULT_CODE_DEBUG	2
#endif

void hook_fault_code(int nr, int (*fn)(unsigned long, unsigned int,
				       struct pt_regs *),
		     int sig, int code, const char *name);

void hook_ifault_code(int nr, int (*fn)(unsigned long, unsigned int,
				       struct pt_regs *),
		     int sig, int code, const char *name);

extern asmlinkage void c_backtrace(unsigned long fp, int pmode);

struct mm_struct;
extern void show_pte(struct mm_struct *mm, unsigned long addr);
extern void __show_regs(struct pt_regs *);

#endif

/*
 * OpenRISC signal.c
 *
 * Linux architectural port borrowing liberally from similar works of
 * others.  All original copyrights apply as per the original source
 * declaration.
 *
 * Modifications for the OpenRISC architecture:
 * Copyright (C) 2003 Matjaz Breskvar <phoenix@bsemi.com>
 * Copyright (C) 2010-2011 Jonas Bonn <jonas@southpole.se>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/ptrace.h>
#include <linux/unistd.h>
#include <linux/stddef.h>
#include <linux/tracehook.h>

#include <asm/processor.h>
<<<<<<< HEAD
#include <asm/syscall.h>
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#include <asm/ucontext.h>
#include <asm/uaccess.h>

#define DEBUG_SIG 0

struct rt_sigframe {
<<<<<<< HEAD
=======
	struct siginfo *pinfo;
	void *puc;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	struct siginfo info;
	struct ucontext uc;
	unsigned char retcode[16];	/* trampoline code */
};

<<<<<<< HEAD
static int restore_sigcontext(struct pt_regs *regs,
			      struct sigcontext __user *sc)
{
	int err = 0;

	/* Always make any pending restarted system calls return -EINTR */
=======
static int restore_sigcontext(struct pt_regs *regs, struct sigcontext *sc)
{
	unsigned int err = 0;

	/* Alwys make any pending restarted system call return -EINTR */
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	current_thread_info()->restart_block.fn = do_no_restart_syscall;

	/*
	 * Restore the regs from &sc->regs.
	 * (sc is already checked for VERIFY_READ since the sigframe was
	 *  checked in sys_sigreturn previously)
	 */
<<<<<<< HEAD
	err |= __copy_from_user(regs, sc->regs.gpr, 32 * sizeof(unsigned long));
	err |= __copy_from_user(&regs->pc, &sc->regs.pc, sizeof(unsigned long));
	err |= __copy_from_user(&regs->sr, &sc->regs.sr, sizeof(unsigned long));
=======
	if (__copy_from_user(regs, sc->regs.gpr, 32 * sizeof(unsigned long)))
		goto badframe;
	if (__copy_from_user(&regs->pc, &sc->regs.pc, sizeof(unsigned long)))
		goto badframe;
	if (__copy_from_user(&regs->sr, &sc->regs.sr, sizeof(unsigned long)))
		goto badframe;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	/* make sure the SM-bit is cleared so user-mode cannot fool us */
	regs->sr &= ~SPR_SR_SM;

<<<<<<< HEAD
	regs->orig_gpr11 = -1;	/* Avoid syscall restart checks */

=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	/* TODO: the other ports use regs->orig_XX to disable syscall checks
	 * after this completes, but we don't use that mechanism. maybe we can
	 * use it now ?
	 */

	return err;
<<<<<<< HEAD
=======

badframe:
	return 1;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

asmlinkage long _sys_rt_sigreturn(struct pt_regs *regs)
{
	struct rt_sigframe *frame = (struct rt_sigframe __user *)regs->sp;
	sigset_t set;

	/*
	 * Since we stacked the signal on a dword boundary,
	 * then frame should be dword aligned here.  If it's
	 * not, then the user is trying to mess with us.
	 */
	if (((long)frame) & 3)
		goto badframe;

	if (!access_ok(VERIFY_READ, frame, sizeof(*frame)))
		goto badframe;
	if (__copy_from_user(&set, &frame->uc.uc_sigmask, sizeof(set)))
		goto badframe;

	set_current_blocked(&set);

	if (restore_sigcontext(regs, &frame->uc.uc_mcontext))
		goto badframe;

	if (restore_altstack(&frame->uc.uc_stack))
		goto badframe;

	return regs->gpr[11];

badframe:
	force_sig(SIGSEGV, current);
	return 0;
}

/*
 * Set up a signal frame.
 */

<<<<<<< HEAD
static int setup_sigcontext(struct pt_regs *regs, struct sigcontext __user *sc)
=======
static int setup_sigcontext(struct sigcontext *sc, struct pt_regs *regs,
			    unsigned long mask)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
{
	int err = 0;

	/* copy the regs */
<<<<<<< HEAD
	/* There should be no need to save callee-saved registers here...
	 * ...but we save them anyway.  Revisit this
	 */
=======

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	err |= __copy_to_user(sc->regs.gpr, regs, 32 * sizeof(unsigned long));
	err |= __copy_to_user(&sc->regs.pc, &regs->pc, sizeof(unsigned long));
	err |= __copy_to_user(&sc->regs.sr, &regs->sr, sizeof(unsigned long));

<<<<<<< HEAD
=======
	/* then some other stuff */

	err |= __put_user(mask, &sc->oldmask);

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	return err;
}

static inline unsigned long align_sigframe(unsigned long sp)
{
	return sp & ~3UL;
}

/*
 * Work out where the signal frame should go.  It's either on the user stack
 * or the alternate stack.
 */

static inline void __user *get_sigframe(struct k_sigaction *ka,
					struct pt_regs *regs, size_t frame_size)
{
	unsigned long sp = regs->sp;
	int onsigstack = on_sig_stack(sp);

	/* redzone */
	sp -= STACK_FRAME_OVERHEAD;

	/* This is the X/Open sanctioned signal stack switching.  */
	if ((ka->sa.sa_flags & SA_ONSTACK) && !onsigstack) {
		if (current->sas_ss_size)
			sp = current->sas_ss_sp + current->sas_ss_size;
	}

	sp = align_sigframe(sp - frame_size);

	/*
	 * If we are on the alternate signal stack and would overflow it, don't.
	 * Return an always-bogus address instead so we will die with SIGSEGV.
	 */
	if (onsigstack && !likely(on_sig_stack(sp)))
		return (void __user *)-1L;

	return (void __user *)sp;
}

/* grab and setup a signal frame.
 *
 * basically we stack a lot of state info, and arrange for the
 * user-mode program to return to the kernel using either a
 * trampoline which performs the syscall sigreturn, or a provided
 * user-mode trampoline.
 */
static int setup_rt_frame(int sig, struct k_sigaction *ka, siginfo_t *info,
			  sigset_t *set, struct pt_regs *regs)
{
	struct rt_sigframe *frame;
	unsigned long return_ip;
	int err = 0;

	frame = get_sigframe(ka, regs, sizeof(*frame));
<<<<<<< HEAD
	if (!access_ok(VERIFY_WRITE, frame, sizeof(*frame)))
		goto give_sigsegv;

	/* Create siginfo.  */
	if (ka->sa.sa_flags & SA_SIGINFO)
		err |= copy_siginfo_to_user(&frame->info, info);

	/* Create the ucontext.  */
	err |= __put_user(0, &frame->uc.uc_flags);
	err |= __put_user(NULL, &frame->uc.uc_link);
	err |= __save_altstack(&frame->uc.uc_stack, regs->sp);
	err |= setup_sigcontext(regs, &frame->uc.uc_mcontext);
=======

	if (!access_ok(VERIFY_WRITE, frame, sizeof(*frame)))
		goto give_sigsegv;

	err |= __put_user(&frame->info, &frame->pinfo);
	err |= __put_user(&frame->uc, &frame->puc);

	if (ka->sa.sa_flags & SA_SIGINFO)
		err |= copy_siginfo_to_user(&frame->info, info);
	if (err)
		goto give_sigsegv;

	/* Clear all the bits of the ucontext we don't use.  */
	err |= __clear_user(&frame->uc, offsetof(struct ucontext, uc_mcontext));
	err |= __put_user(0, &frame->uc.uc_flags);
	err |= __put_user(NULL, &frame->uc.uc_link);
	err |= __save_altstack(&frame->uc.uc_stack, regs->sp);
	err |= setup_sigcontext(&frame->uc.uc_mcontext, regs, set->sig[0]);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	err |= __copy_to_user(&frame->uc.uc_sigmask, set, sizeof(*set));

	if (err)
		goto give_sigsegv;

	/* trampoline - the desired return ip is the retcode itself */
	return_ip = (unsigned long)&frame->retcode;
<<<<<<< HEAD
	/* This is:
		l.ori r11,r0,__NR_sigreturn
		l.sys 1
	 */
	err |= __put_user(0xa960,             (short *)(frame->retcode + 0));
	err |= __put_user(__NR_rt_sigreturn,  (short *)(frame->retcode + 2));
=======
	/* This is l.ori r11,r0,__NR_sigreturn, l.sys 1 */
	err |= __put_user(0xa960, (short *)(frame->retcode + 0));
	err |= __put_user(__NR_rt_sigreturn, (short *)(frame->retcode + 2));
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	err |= __put_user(0x20000001, (unsigned long *)(frame->retcode + 4));
	err |= __put_user(0x15000000, (unsigned long *)(frame->retcode + 8));

	if (err)
		goto give_sigsegv;

	/* TODO what is the current->exec_domain stuff and invmap ? */

	/* Set up registers for signal handler */
	regs->pc = (unsigned long)ka->sa.sa_handler; /* what we enter NOW */
	regs->gpr[9] = (unsigned long)return_ip;     /* what we enter LATER */
	regs->gpr[3] = (unsigned long)sig;           /* arg 1: signo */
	regs->gpr[4] = (unsigned long)&frame->info;  /* arg 2: (siginfo_t*) */
	regs->gpr[5] = (unsigned long)&frame->uc;    /* arg 3: ucontext */

	/* actually move the usp to reflect the stacked frame */
	regs->sp = (unsigned long)frame;

	return 0;

give_sigsegv:
	force_sigsegv(sig, current);
	return -EFAULT;
}

static inline void
handle_signal(unsigned long sig,
	      siginfo_t *info, struct k_sigaction *ka,
	      struct pt_regs *regs)
{
	int ret;

	ret = setup_rt_frame(sig, ka, info, sigmask_to_save(), regs);
	if (ret)
		return;

	signal_delivered(sig, info, ka, regs,
				 test_thread_flag(TIF_SINGLESTEP));
}

/*
 * Note that 'init' is a special process: it doesn't get signals it doesn't
 * want to handle. Thus you cannot kill init even with a SIGKILL even by
 * mistake.
 *
 * Also note that the regs structure given here as an argument, is the latest
 * pushed pt_regs. It may or may not be the same as the first pushed registers
 * when the initial usermode->kernelmode transition took place. Therefore
 * we can use user_mode(regs) to see if we came directly from kernel or user
 * mode below.
 */

<<<<<<< HEAD
int do_signal(struct pt_regs *regs, int syscall)
=======
void do_signal(struct pt_regs *regs)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
{
	siginfo_t info;
	int signr;
	struct k_sigaction ka;
<<<<<<< HEAD
	unsigned long continue_addr = 0;
	unsigned long restart_addr = 0;
	unsigned long retval = 0;
	int restart = 0;

	if (syscall) {
		continue_addr = regs->pc;
		restart_addr = continue_addr - 4;
		retval = regs->gpr[11];

		/*
		 * Setup syscall restart here so that a debugger will
		 * see the already changed PC.
		 */
		switch (retval) {
		case -ERESTART_RESTARTBLOCK:
			restart = -2;
			/* Fall through */
		case -ERESTARTNOHAND:
		case -ERESTARTSYS:
		case -ERESTARTNOINTR:
			restart++;
			regs->gpr[11] = regs->orig_gpr11;
			regs->pc = restart_addr;
			break;
		}
	}

	/*
	 * Get the signal to deliver.  When running under ptrace, at this
	 * point the debugger may change all our registers ...
	 */
	signr = get_signal_to_deliver(&info, &ka, regs, NULL);
	/*
	 * Depending on the signal settings we may need to revert the
	 * decision to restart the system call.  But skip this if a
	 * debugger has chosen to restart at a different PC.
	 */
	if (signr > 0) {
		if (unlikely(restart) && regs->pc == restart_addr) {
			if (retval == -ERESTARTNOHAND ||
			    retval == -ERESTART_RESTARTBLOCK
			    || (retval == -ERESTARTSYS
			        && !(ka.sa.sa_flags & SA_RESTART))) {
				/* No automatic restart */
				regs->gpr[11] = -EINTR;
				regs->pc = continue_addr;
			}
		}

		handle_signal(signr, &info, &ka, regs);
	} else {
		/* no handler */
		restore_saved_sigmask();
		/*
		 * Restore pt_regs PC as syscall restart will be handled by
		 * kernel without return to userspace
		 */
		if (unlikely(restart) && regs->pc == restart_addr) {
			regs->pc = continue_addr;
			return restart;
		}
	}

	return 0;
}

asmlinkage int
do_work_pending(struct pt_regs *regs, unsigned int thread_flags, int syscall)
{
	do {
		if (likely(thread_flags & _TIF_NEED_RESCHED)) {
			schedule();
		} else {
			if (unlikely(!user_mode(regs)))
				return 0;
			local_irq_enable();
			if (thread_flags & _TIF_SIGPENDING) {
				int restart = do_signal(regs, syscall);
				if (unlikely(restart)) {
					/*
					 * Restart without handlers.
					 * Deal with it without leaving
					 * the kernel space.
					 */
					return restart;
				}
				syscall = 0;
			} else {
				clear_thread_flag(TIF_NOTIFY_RESUME);
				tracehook_notify_resume(regs);
			}
		}
		local_irq_disable();
		thread_flags = current_thread_info()->flags;
	} while (thread_flags & _TIF_WORK_MASK);
	return 0;
=======

	/*
	 * We want the common case to go fast, which
	 * is why we may in certain cases get here from
	 * kernel mode. Just return without doing anything
	 * if so.
	 */
	if (!user_mode(regs))
		return;

	signr = get_signal_to_deliver(&info, &ka, regs, NULL);

	/* If we are coming out of a syscall then we need
	 * to check if the syscall was interrupted and wants to be
	 * restarted after handling the signal.  If so, the original
	 * syscall number is put back into r11 and the PC rewound to
	 * point at the l.sys instruction that resulted in the
	 * original syscall.  Syscall results other than the four
	 * below mean that the syscall executed to completion and no
	 * restart is necessary.
	 */
	if (regs->orig_gpr11) {
		int restart = 0;

		switch (regs->gpr[11]) {
		case -ERESTART_RESTARTBLOCK:
		case -ERESTARTNOHAND:
			/* Restart if there is no signal handler */
			restart = (signr <= 0);
			break;
		case -ERESTARTSYS:
			/* Restart if there no signal handler or
			 * SA_RESTART flag is set */
			restart = (signr <= 0 || (ka.sa.sa_flags & SA_RESTART));
			break;
		case -ERESTARTNOINTR:
			/* Always restart */
			restart = 1;
			break;
		}

		if (restart) {
			if (regs->gpr[11] == -ERESTART_RESTARTBLOCK)
				regs->gpr[11] = __NR_restart_syscall;
			else
				regs->gpr[11] = regs->orig_gpr11;
			regs->pc -= 4;
		} else {
			regs->gpr[11] = -EINTR;
		}
	}

	if (signr <= 0) {
		/* no signal to deliver so we just put the saved sigmask
		 * back */
		restore_saved_sigmask();
	} else {		/* signr > 0 */
		/* Whee!  Actually deliver the signal.  */
		handle_signal(signr, &info, &ka, regs);
	}

	return;
}

asmlinkage void do_notify_resume(struct pt_regs *regs)
{
	if (current_thread_info()->flags & _TIF_SIGPENDING)
		do_signal(regs);

	if (current_thread_info()->flags & _TIF_NOTIFY_RESUME) {
		clear_thread_flag(TIF_NOTIFY_RESUME);
		tracehook_notify_resume(regs);
	}
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

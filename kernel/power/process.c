/*
 * drivers/power/process.c - Functions for starting/stopping processes on 
 *                           suspend transitions.
 *
 * Originally from swsusp.
 */


#undef DEBUG

#include <linux/interrupt.h>
#include <linux/oom.h>
#include <linux/suspend.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/freezer.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/kmod.h>
<<<<<<< HEAD
=======
#include <linux/wakelock.h>
#include "power.h"
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

/* 
 * Timeout for stopping processes
 */
unsigned int __read_mostly freeze_timeout_msecs = 20 * MSEC_PER_SEC;

static int try_to_freeze_tasks(bool user_only)
{
	struct task_struct *g, *p;
	unsigned long end_time;
	unsigned int todo;
	bool wq_busy = false;
	struct timeval start, end;
<<<<<<< HEAD
	u64 elapsed_csecs64;
	unsigned int elapsed_csecs;
	bool wakeup = false;
=======
	u64 elapsed_msecs64;
	unsigned int elapsed_msecs;
	bool wakeup = false;
	int sleep_usecs = USEC_PER_MSEC;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	do_gettimeofday(&start);

	end_time = jiffies + msecs_to_jiffies(freeze_timeout_msecs);

	if (!user_only)
		freeze_workqueues_begin();

	while (true) {
		todo = 0;
		read_lock(&tasklist_lock);
		do_each_thread(g, p) {
			if (p == current || !freeze_task(p))
				continue;

			if (!freezer_should_skip(p))
				todo++;
		} while_each_thread(g, p);
		read_unlock(&tasklist_lock);

		if (!user_only) {
			wq_busy = freeze_workqueues_busy();
			todo += wq_busy;
		}

<<<<<<< HEAD
=======
		if (todo && has_wake_lock(WAKE_LOCK_SUSPEND)) {
			wakeup = 1;
			break;
		}
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		if (!todo || time_after(jiffies, end_time))
			break;

		if (pm_wakeup_pending()) {
			wakeup = true;
			break;
		}

		/*
		 * We need to retry, but first give the freezing tasks some
<<<<<<< HEAD
		 * time to enter the refrigerator.
		 */
		msleep(10);
	}

	do_gettimeofday(&end);
	elapsed_csecs64 = timeval_to_ns(&end) - timeval_to_ns(&start);
	do_div(elapsed_csecs64, NSEC_PER_SEC / 100);
	elapsed_csecs = elapsed_csecs64;

	if (todo) {
		printk("\n");
		printk(KERN_ERR "Freezing of tasks %s after %d.%02d seconds "
		       "(%d tasks refusing to freeze, wq_busy=%d):\n",
		       wakeup ? "aborted" : "failed",
		       elapsed_csecs / 100, elapsed_csecs % 100,
=======
		 * time to enter the refrigerator.  Start with an initial
		 * 1 ms sleep followed by exponential backoff until 8 ms.
		 */
		usleep_range(sleep_usecs / 2, sleep_usecs);
		if (sleep_usecs < 8 * USEC_PER_MSEC)
			sleep_usecs *= 2;
	}

	do_gettimeofday(&end);
	elapsed_msecs64 = timeval_to_ns(&end) - timeval_to_ns(&start);
	do_div(elapsed_msecs64, NSEC_PER_MSEC);
	elapsed_msecs = elapsed_msecs64;

	if (todo) {
		printk("\n");
		printk(KERN_ERR "Freezing of tasks %s after %d.%03d seconds "
		       "(%d tasks refusing to freeze, wq_busy=%d):\n",
		       wakeup ? "aborted" : "failed",
		       elapsed_msecs / 1000, elapsed_msecs % 1000,
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		       todo - wq_busy, wq_busy);

		if (!wakeup) {
			read_lock(&tasklist_lock);
			do_each_thread(g, p) {
				if (p != current && !freezer_should_skip(p)
				    && freezing(p) && !frozen(p))
					sched_show_task(p);
			} while_each_thread(g, p);
			read_unlock(&tasklist_lock);
		}
	} else {
<<<<<<< HEAD
		printk("(elapsed %d.%02d seconds) ", elapsed_csecs / 100,
			elapsed_csecs % 100);
=======
		printk("(elapsed %d.%03d seconds) ", elapsed_msecs / 1000,
			elapsed_msecs % 1000);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	}

	return todo ? -EBUSY : 0;
}

<<<<<<< HEAD
/*
 * Returns true if all freezable tasks (except for current) are frozen already
 */
static bool check_frozen_processes(void)
{
	struct task_struct *g, *p;
	bool ret = true;

	read_lock(&tasklist_lock);
	for_each_process_thread(g, p) {
		if (p != current && !freezer_should_skip(p) &&
		    !frozen(p)) {
			ret = false;
			goto done;
		}
	}
done:
	read_unlock(&tasklist_lock);

	return ret;
}

=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
/**
 * freeze_processes - Signal user space processes to enter the refrigerator.
 *
 * On success, returns 0.  On failure, -errno and system is fully thawed.
 */
int freeze_processes(void)
{
	int error;
<<<<<<< HEAD
	int oom_kills_saved;
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	error = __usermodehelper_disable(UMH_FREEZING);
	if (error)
		return error;

	if (!pm_freezing)
		atomic_inc(&system_freezing_cnt);

	printk("Freezing user space processes ... ");
	pm_freezing = true;
<<<<<<< HEAD
	oom_kills_saved = oom_kills_count();
	error = try_to_freeze_tasks(true);
	if (!error) {
		__usermodehelper_set_disable_depth(UMH_DISABLED);
		oom_killer_disable();

		/*
		 * There might have been an OOM kill while we were
		 * freezing tasks and the killed task might be still
		 * on the way out so we have to double check for race.
		 */
		if (oom_kills_count() != oom_kills_saved &&
				!check_frozen_processes()) {
			__usermodehelper_set_disable_depth(UMH_ENABLED);
			printk("OOM in progress.");
			error = -EBUSY;
			goto done;
		}
		printk("done.");
	}
done:
=======
	error = try_to_freeze_tasks(true);
	if (!error) {
		printk("done.");
		__usermodehelper_set_disable_depth(UMH_DISABLED);
		oom_killer_disable();
	}
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	printk("\n");
	BUG_ON(in_atomic());

	if (error)
		thaw_processes();
	return error;
}

/**
 * freeze_kernel_threads - Make freezable kernel threads go to the refrigerator.
 *
 * On success, returns 0.  On failure, -errno and only the kernel threads are
 * thawed, so as to give a chance to the caller to do additional cleanups
 * (if any) before thawing the userspace tasks. So, it is the responsibility
 * of the caller to thaw the userspace tasks, when the time is right.
 */
int freeze_kernel_threads(void)
{
	int error;

<<<<<<< HEAD
=======
	error = suspend_sys_sync_wait();
	if (error)
		return error;

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	printk("Freezing remaining freezable tasks ... ");
	pm_nosig_freezing = true;
	error = try_to_freeze_tasks(false);
	if (!error)
		printk("done.");

	printk("\n");
	BUG_ON(in_atomic());

	if (error)
		thaw_kernel_threads();
	return error;
}

void thaw_processes(void)
{
	struct task_struct *g, *p;

	if (pm_freezing)
		atomic_dec(&system_freezing_cnt);
	pm_freezing = false;
	pm_nosig_freezing = false;

	oom_killer_enable();

	printk("Restarting tasks ... ");

<<<<<<< HEAD
	__usermodehelper_set_disable_depth(UMH_FREEZING);
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	thaw_workqueues();

	read_lock(&tasklist_lock);
	do_each_thread(g, p) {
		__thaw_task(p);
	} while_each_thread(g, p);
	read_unlock(&tasklist_lock);

	usermodehelper_enable();

	schedule();
	printk("done.\n");
}

void thaw_kernel_threads(void)
{
	struct task_struct *g, *p;

	pm_nosig_freezing = false;
	printk("Restarting kernel threads ... ");

	thaw_workqueues();

	read_lock(&tasklist_lock);
	do_each_thread(g, p) {
		if (p->flags & (PF_KTHREAD | PF_WQ_WORKER))
			__thaw_task(p);
	} while_each_thread(g, p);
	read_unlock(&tasklist_lock);

	schedule();
	printk("done.\n");
}

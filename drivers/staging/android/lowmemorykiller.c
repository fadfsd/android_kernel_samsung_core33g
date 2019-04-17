/* drivers/misc/lowmemorykiller.c
 *
 * The lowmemorykiller driver lets user-space specify a set of memory thresholds
 * where processes with a range of oom_score_adj values will get killed. Specify
 * the minimum oom_score_adj values in
 * /sys/module/lowmemorykiller/parameters/adj and the number of free pages in
 * /sys/module/lowmemorykiller/parameters/minfree. Both files take a comma
 * separated list of numbers in ascending order.
 *
 * For example, write "0,8" to /sys/module/lowmemorykiller/parameters/adj and
 * "1024,4096" to /sys/module/lowmemorykiller/parameters/minfree to kill
 * processes with a oom_score_adj value of 8 or higher when the free memory
 * drops below 4096 pages and kill processes with a oom_score_adj value of 0 or
 * higher when the free memory drops below 1024 pages.
 *
 * The driver considers memory used for caches to be free, but if a large
 * percentage of the cached memory is locked this can be very inaccurate
 * and processes may not get killed until the normal oom killer is triggered.
 *
 * Copyright (C) 2007-2008 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/oom.h>
#include <linux/sched.h>
<<<<<<< HEAD
#include <linux/rcupdate.h>
#include <linux/profile.h>
#include <linux/notifier.h>
<<<<<<< HEAD
=======
#include <linux/swap.h>
#include <linux/rcupdate.h>
#include <linux/notifier.h>
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/swap.h>
#include <linux/fs.h>
<<<<<<< HEAD
#include <linux/cpuset.h>
#include <linux/show_mem_notifier.h>
#include <linux/vmpressure.h>

#define CREATE_TRACE_POINTS
#include <trace/events/almk.h>

#include <linux/ratelimit.h>

#define LMK_COUNT_READ

#ifdef CONFIG_SEC_DEBUG_LMK_COUNT_INFO
#define OOM_COUNT_READ
=======
#include <linux/sched/rt.h>

#include <linux/ratelimit.h>

#define ENHANCED_LMK_ROUTINE
#define LMK_COUNT_READ

#ifdef ENHANCED_LMK_ROUTINE
#define LOWMEM_DEATHPENDING_DEPTH 3
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#endif

#ifdef LMK_COUNT_READ
static uint32_t lmk_count = 0;
#endif

#ifdef CONFIG_SEC_OOM_KILLER
#define MULTIPLE_OOM_KILLER
<<<<<<< HEAD
=======
#define OOM_COUNT_READ
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#endif

#ifdef OOM_COUNT_READ
static uint32_t oom_count = 0;
#endif

#ifdef MULTIPLE_OOM_KILLER
#define OOM_DEPTH 7
#endif
<<<<<<< HEAD
=======
>>>>>>> parent of 59a54da8838... core33g: Import SM-G360H_KK_Opensource

static uint32_t lowmem_debug_level = 1;
static short lowmem_adj[6] = {
=======

#ifdef CONFIG_HIGHMEM
#define _ZONE ZONE_HIGHMEM
#else
#define _ZONE ZONE_NORMAL
#endif

static uint32_t lowmem_debug_level = 1;
static int lowmem_adj[6] = {
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	0,
	1,
	6,
	12,
};
static int lowmem_adj_size = 4;
static int lowmem_minfree[6] = {
	3 * 512,	/* 6MB */
	2 * 1024,	/* 8MB */
	4 * 1024,	/* 16MB */
	16 * 1024,	/* 64MB */
};
static int lowmem_minfree_size = 4;

static unsigned long lowmem_deathpending_timeout;

#define lowmem_print(level, x...)			\
	do {						\
		if (lowmem_debug_level >= (level))	\
			pr_info(x);			\
	} while (0)

<<<<<<< HEAD
<<<<<<< HEAD
static atomic_t shift_adj = ATOMIC_INIT(0);
static short adj_max_shift = 353;

/* User knob to enable/disable adaptive lmk feature */
static int enable_adaptive_lmk = 1;
module_param_named(enable_adaptive_lmk, enable_adaptive_lmk, int,
	S_IRUGO | S_IWUSR);

/*
 * This parameter controls the behaviour of LMK when vmpressure is in
 * the range of 90-94. Adaptive lmk triggers based on number of file
 * pages wrt vmpressure_file_min, when vmpressure is in the range of
 * 90-94. Usually this is a pseudo minfree value, higher than the
 * highest configured value in minfree array.
 */
static int vmpressure_file_min;
module_param_named(vmpressure_file_min, vmpressure_file_min, int,
	S_IRUGO | S_IWUSR);

enum {
	VMPRESSURE_NO_ADJUST = 0,
	VMPRESSURE_ADJUST_ENCROACH,
	VMPRESSURE_ADJUST_NORMAL,
};

int adjust_minadj(short *min_score_adj)
{
	int ret = VMPRESSURE_NO_ADJUST;

	if (!enable_adaptive_lmk)
		return 0;

	if (atomic_read(&shift_adj) &&
		(*min_score_adj > adj_max_shift)) {
		if (*min_score_adj == OOM_SCORE_ADJ_MAX + 1)
			ret = VMPRESSURE_ADJUST_ENCROACH;
		else
			ret = VMPRESSURE_ADJUST_NORMAL;
		*min_score_adj = adj_max_shift;
	}
	atomic_set(&shift_adj, 0);

	return ret;
}

static int lmk_vmpressure_notifier(struct notifier_block *nb,
			unsigned long action, void *data)
{
	int other_free, other_file;
	unsigned long pressure = action;
	int array_size = ARRAY_SIZE(lowmem_adj);

	if (!enable_adaptive_lmk)
		return 0;

	if (pressure >= 95) {
		other_file = global_page_state(NR_FILE_PAGES) -
			global_page_state(NR_SHMEM) -
			total_swapcache_pages();
		other_free = global_page_state(NR_FREE_PAGES);

		atomic_set(&shift_adj, 1);
		trace_almk_vmpressure(pressure, other_free, other_file);
	} else if (pressure >= 90) {
		if (lowmem_adj_size < array_size)
			array_size = lowmem_adj_size;
		if (lowmem_minfree_size < array_size)
			array_size = lowmem_minfree_size;

		other_file = global_page_state(NR_FILE_PAGES) -
			global_page_state(NR_SHMEM) -
			total_swapcache_pages();

		other_free = global_page_state(NR_FREE_PAGES);

		if ((other_free < lowmem_minfree[array_size - 1]) &&
			(other_file < vmpressure_file_min)) {
				atomic_set(&shift_adj, 1);
				trace_almk_vmpressure(pressure, other_free,
					other_file);
		}
	} else if (atomic_read(&shift_adj)) {
		/*
		 * shift_adj would have been set by a previous invocation
		 * of notifier, which is not followed by a lowmem_shrink yet.
		 * Since vmpressure has improved, reset shift_adj to avoid
		 * false adaptive LMK trigger.
		 */
		trace_almk_vmpressure(pressure, other_free, other_file);
		atomic_set(&shift_adj, 0);
	}

	return 0;
}

static struct notifier_block lmk_vmpr_nb = {
	.notifier_call = lmk_vmpressure_notifier,
};

=======


#ifdef CONFIG_ANDROID_LOW_MEMORY_KILLER_AUTODETECT_OOM_ADJ_VALUES
static int lowmem_oom_adj_to_oom_score_adj(int oom_adj);
static int lowmem_oom_score_adj_to_oom_adj(int oom_score_adj);
#define OOM_SCORE_ADJ_TO_OOM_ADJ(__SCORE_ADJ__)   lowmem_oom_score_adj_to_oom_adj(__SCORE_ADJ__)
#define OOM_ADJ_TO_OOM_SCORE_ADJ(__ADJ__)   lowmem_oom_adj_to_oom_score_adj(__ADJ__)
#else
#define OOM_SCORE_ADJ_TO_OOM_ADJ(__SCORE_ADJ__)  (__SCORE_ADJ__)
#define OOM_ADJ_TO_OOM_SCORE_ADJ(__ADJ__)    (__ADJ__)
#endif

#if defined(CONFIG_SEC_DEBUG_LMK_MEMINFO)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
static void dump_tasks_info(void)
{
	struct task_struct *p;
	struct task_struct *task;

<<<<<<< HEAD
	pr_info("[ pid ]   uid	tgid total_vm	   rss cpu oom_adj oom_score_adj name\n");
=======
	pr_info("[ pid ]   uid  tgid total_vm      rss cpu oom_score_adj name\n");
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	for_each_process(p) {
		/* check unkillable tasks */
		if (is_global_init(p))
			continue;
		if (p->flags & PF_KTHREAD)
			continue;

		task = find_lock_task_mm(p);
		if (!task) {
			/*
			* This is a kthread or all of p's threads have already
<<<<<<< HEAD
			* detached their mm's.	There's no need to report
=======
			* detached their mm's.  There's no need to report
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
			* them; they can't be oom killed anyway.
			*/
			continue;
		}

<<<<<<< HEAD
		pr_info("[%5d] %5d %5d %8lu %8lu %3u     %5d %s\n",
				task->pid, task_uid(task), task->tgid,
				task->mm->total_vm, get_mm_rss(task->mm),
				task_cpu(task),
				task->signal->oom_score_adj, task->comm);
		task_unlock(task);
	}
}

static int test_task_flag(struct task_struct *p, int flag)
{
	struct task_struct *t;

	for_each_thread(p, t) {
=======
		pr_info("[%5d] %5d %5d %8lu %8lu %3u         %5d %s\n",
		task->pid, task_uid(task), task->tgid,
		task->mm->total_vm, get_mm_rss(task->mm),
		task_cpu(task),
		task->signal->oom_score_adj, task->comm);
		task_unlock(task);
	}
}
#endif

#if defined(CONFIG_ZRAM) && !defined(CONFIG_RUNTIME_COMPCACHE)
extern ssize_t  zram_mem_usage(void);
extern ssize_t zram_mem_free_percent(void);
static uint lmk_lowmem_threshold_adj = 2;
module_param_named(lmk_lowmem_threshold_adj, lmk_lowmem_threshold_adj, uint, S_IRUGO | S_IWUSR);

static uint zone_wmark_ok_safe_gap = 256;
module_param_named(zone_wmark_ok_safe_gap, zone_wmark_ok_safe_gap, uint, S_IRUGO | S_IWUSR);
int cacl_zram_score_adj(void)
{
	struct sysinfo swap_info;
	ssize_t  swap_free_percent = 0;
	ssize_t zram_free_percent = 0;
	ssize_t  ret = 0;

	si_swapinfo(&swap_info);
	if(!swap_info.totalswap)
	{
		return  -1;
	}

	swap_free_percent =  swap_info.freeswap * 100/swap_info.totalswap;
	zram_free_percent =  zram_mem_free_percent();
	if(zram_free_percent < 0)
		return -1;

	ret = (swap_free_percent <  zram_free_percent) ?  swap_free_percent :  zram_free_percent;

	return OOM_ADJ_TO_OOM_SCORE_ADJ(ret*OOM_ADJUST_MAX/100);
}
#endif

static int test_task_flag(struct task_struct *p, int flag)
{
	struct task_struct *t = p;

	do {
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		task_lock(t);
		if (test_tsk_thread_flag(t, flag)) {
			task_unlock(t);
			return 1;
		}
		task_unlock(t);
<<<<<<< HEAD
	}
=======
	} while_each_thread(p, t);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	return 0;
}

static DEFINE_MUTEX(scan_mutex);

<<<<<<< HEAD
=======
>>>>>>> parent of 59a54da8838... core33g: Import SM-G360H_KK_Opensource
static int lowmem_shrink(struct shrinker *s, struct shrink_control *sc)
{
	struct task_struct *tsk;
	struct task_struct *selected = NULL;
	int rem = 0;
	int tasksize;
	int i;
<<<<<<< HEAD
	int ret = 0;
	short min_score_adj = OOM_SCORE_ADJ_MAX + 1;
	int minfree = 0;
	int selected_tasksize = 0;
	short selected_oom_score_adj;
	int array_size = ARRAY_SIZE(lowmem_adj);
	int other_free;
	int other_file;
	unsigned long nr_to_scan = sc->nr_to_scan;
#ifdef CONFIG_SEC_DEBUG_LMK_MEMINFO
	static DEFINE_RATELIMIT_STATE(lmk_rs, DEFAULT_RATELIMIT_INTERVAL, 1);
#endif
	unsigned long nr_cma_free;
	struct reclaim_state *reclaim_state = current->reclaim_state;
=======
/*
  * It's reasonable to grant the dying task an even higher priority to
  * be sure it will be scheduled sooner and free the desired pmem.
  * It was suggested using SCHED_FIFO:1 (the lowest RT priority),
  * so that this task won't interfere with any running RT task.
  */
static void boost_dying_task_prio(struct task_struct *p)
{
         if (!rt_task(p)) {
                 struct sched_param param;
                 param.sched_priority = 1;
                 sched_setscheduler_nocheck(p, SCHED_FIFO, &param);
         }
}


typedef struct lmk_debug_info
{
	short min_score_adj;
	short zram_score_adj;
	ssize_t zram_free_percent;
	ssize_t zram_mem_usage;
}lmk_debug_info;

static int lowmem_shrink(struct shrinker *s, struct shrink_control *sc)
{
	lmk_debug_info  lmk_info = {0};
	struct task_struct *tsk;
#ifdef ENHANCED_LMK_ROUTINE
	struct task_struct *selected[LOWMEM_DEATHPENDING_DEPTH] = {NULL,};
#else
	struct task_struct *selected = NULL;
#endif
	int rem = 0;
	int tasksize;
	int i;
	int min_score_adj = OOM_SCORE_ADJ_MAX + 1;
	int minfree = 0;
#ifdef ENHANCED_LMK_ROUTINE
	int selected_tasksize[LOWMEM_DEATHPENDING_DEPTH] = {0,};
	int selected_oom_score_adj[LOWMEM_DEATHPENDING_DEPTH] = {OOM_ADJUST_MAX,};
	int all_selected_oom = 0;
	int max_selected_oom_idx = 0;
#else
	int selected_tasksize = 0;
	int selected_oom_score_adj;
#endif
	int array_size = ARRAY_SIZE(lowmem_adj);
	int other_free;
	int other_file;
	bool is_need_lmk_kill = true;
	unsigned long nr_to_scan = sc->nr_to_scan;

#if defined(CONFIG_ZRAM) && !defined(CONFIG_RUNTIME_COMPCACHE)
	int zram_score_adj = 0;
#endif
#ifdef CONFIG_SEC_DEBUG_LMK_MEMINFO
	static DEFINE_RATELIMIT_STATE(lmk_rs, DEFAULT_RATELIMIT_INTERVAL, 1);
#endif
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	if (nr_to_scan > 0) {
		if (mutex_lock_interruptible(&scan_mutex) < 0)
			return 0;
	}

	other_free = global_page_state(NR_FREE_PAGES);
<<<<<<< HEAD

	nr_cma_free = global_page_state(NR_FREE_CMA_PAGES);
	if (!current_is_kswapd() || sc->priority <= 6)
		other_free -= nr_cma_free;

=======
	
	if (!current_is_kswapd())
		other_free -= global_page_state(NR_FREE_CMA_PAGES);
		
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	if (global_page_state(NR_SHMEM) + total_swapcache_pages() <
		global_page_state(NR_FILE_PAGES))
		other_file = global_page_state(NR_FILE_PAGES) -
						global_page_state(NR_SHMEM) -
						total_swapcache_pages();
	else
		other_file = 0;
<<<<<<< HEAD
=======
	short min_score_adj = OOM_SCORE_ADJ_MAX + 1;
	int selected_tasksize = 0;
	short selected_oom_score_adj;
	int array_size = ARRAY_SIZE(lowmem_adj);
	int other_free = global_page_state(NR_FREE_PAGES) - totalreserve_pages;
	int other_file = global_page_state(NR_FILE_PAGES) -
						global_page_state(NR_SHMEM);
>>>>>>> parent of 59a54da8838... core33g: Import SM-G360H_KK_Opensource
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	if (lowmem_adj_size < array_size)
		array_size = lowmem_adj_size;
	if (lowmem_minfree_size < array_size)
		array_size = lowmem_minfree_size;
	for (i = 0; i < array_size; i++) {
<<<<<<< HEAD
		if (other_free < lowmem_minfree[i] &&
		    other_file < lowmem_minfree[i]) {
=======
		minfree = lowmem_minfree[i];
		if (other_free < minfree && other_file < minfree) {
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
			min_score_adj = lowmem_adj[i];
			break;
		}
	}
<<<<<<< HEAD
<<<<<<< HEAD
	if (nr_to_scan > 0) {
		ret = adjust_minadj(&min_score_adj);
=======
	if (sc->nr_to_scan > 0)
>>>>>>> parent of 59a54da8838... core33g: Import SM-G360H_KK_Opensource
		lowmem_print(3, "lowmem_shrink %lu, %x, ofree %d %d, ma %hd\n",
				sc->nr_to_scan, sc->gfp_mask, other_free,
				other_file, min_score_adj);
	}

=======
	if (nr_to_scan > 0)
		lowmem_print(3, "lowmem_shrink %lu, %x, ofree %d %d, ma %hd\n",
				nr_to_scan, sc->gfp_mask, other_free,
				other_file, min_score_adj);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	rem = global_page_state(NR_ACTIVE_ANON) +
		global_page_state(NR_ACTIVE_FILE) +
		global_page_state(NR_INACTIVE_ANON) +
		global_page_state(NR_INACTIVE_FILE);
<<<<<<< HEAD
	if (sc->nr_to_scan <= 0 || min_score_adj == OOM_SCORE_ADJ_MAX + 1) {
		lowmem_print(5, "lowmem_shrink %lu, %x, return %d\n",
<<<<<<< HEAD
=======
	if (nr_to_scan <= 0 || min_score_adj == OOM_SCORE_ADJ_MAX + 1) {
		lowmem_print(5, "lowmem_shrink %lu, %x, return %d\n",
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
			     nr_to_scan, sc->gfp_mask, rem);

		if (nr_to_scan > 0)
			mutex_unlock(&scan_mutex);

<<<<<<< HEAD
		if ((min_score_adj == OOM_SCORE_ADJ_MAX + 1) &&
			(nr_to_scan > 0))
			trace_almk_shrink(0, ret, other_free, other_file, 0);

=======
			     sc->nr_to_scan, sc->gfp_mask, rem);
>>>>>>> parent of 59a54da8838... core33g: Import SM-G360H_KK_Opensource
		return rem;
	}
	selected_oom_score_adj = min_score_adj;
=======
		return rem;
	}

#if defined(CONFIG_ZRAM) && !defined(CONFIG_RUNTIME_COMPCACHE)
	zram_score_adj = cacl_zram_score_adj();
	if(zram_score_adj  < 0 )
	{
		zram_score_adj = min_score_adj;
	}
	else
	{
		lmk_info.zram_mem_usage = zram_mem_usage();
		lmk_info.zram_free_percent = zram_mem_free_percent();
	}

	lmk_info.min_score_adj = min_score_adj;
	lmk_info.zram_score_adj = zram_score_adj;

	if(min_score_adj < zram_score_adj)
	{
		gfp_t gfp_mask;
		struct zone *preferred_zone;
		struct zonelist *zonelist;
		enum zone_type high_zoneidx;
		gfp_mask = sc->gfp_mask;
		zonelist = node_zonelist(0, gfp_mask);
		high_zoneidx = gfp_zone(gfp_mask);
		first_zones_zonelist(zonelist, high_zoneidx, NULL, &preferred_zone);
		if (zram_score_adj <= OOM_ADJ_TO_OOM_SCORE_ADJ(lmk_lowmem_threshold_adj))
		{
			printk("%s:min:%d, zram:%d, threshold:%d\r\n", __func__, min_score_adj,zram_score_adj, OOM_ADJ_TO_OOM_SCORE_ADJ(lmk_lowmem_threshold_adj));
			if(!min_score_adj)
				is_need_lmk_kill = false;

			zram_score_adj = min_score_adj;
		}
		else if (!zone_watermark_ok_safe(preferred_zone, 0, min_wmark_pages(preferred_zone)  + zone_wmark_ok_safe_gap, 0, 0))
		{
			zram_score_adj =  (min_score_adj + zram_score_adj)/2;
		}
		else
		{
			lowmem_print(2, "ZRAM: return min_score_adj:%d, zram_score_adj:%d\r\n", min_score_adj, zram_score_adj);
			if (nr_to_scan > 0)
				mutex_unlock(&scan_mutex);
			return rem;
		}
	}
	lowmem_print(2, "ZRAM: min_score_adj:%d, zram_score_adj:%d\r\n", min_score_adj, zram_score_adj);
	min_score_adj = zram_score_adj;
#endif
#ifdef ENHANCED_LMK_ROUTINE
	for (i = 0; i < LOWMEM_DEATHPENDING_DEPTH; i++)
		selected_oom_score_adj[i] = min_score_adj;
#else
	selected_oom_score_adj = min_score_adj;
#endif
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	rcu_read_lock();
	for_each_process(tsk) {
		struct task_struct *p;
<<<<<<< HEAD
		short oom_score_adj;

		if (tsk->flags & PF_KTHREAD ||
			tsk->state & TASK_UNINTERRUPTIBLE)
			continue;

<<<<<<< HEAD
=======
		int oom_score_adj;
#ifdef ENHANCED_LMK_ROUTINE
		int is_exist_oom_task = 0;
#endif

		if (tsk->flags & PF_KTHREAD)
			continue;

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		/* if task no longer has any memory ignore it */
		if (test_task_flag(tsk, TIF_MM_RELEASED))
			continue;

<<<<<<< HEAD
		if (time_before_eq(jiffies, lowmem_deathpending_timeout)) {
			if (test_task_flag(tsk, TIF_MEMDIE)) {
=======
		if (test_task_flag(tsk, TIF_MEMDIE)) {
			if (time_before_eq(jiffies, lowmem_deathpending_timeout)) {
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
				rcu_read_unlock();
				/* give the system time to free up the memory */
				msleep_interruptible(20);
				mutex_unlock(&scan_mutex);
				return 0;
			}
<<<<<<< HEAD
		}

=======
>>>>>>> parent of 59a54da8838... core33g: Import SM-G360H_KK_Opensource
=======
			continue;
		}

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		p = find_lock_task_mm(tsk);
		if (!p)
			continue;

<<<<<<< HEAD
		if (test_tsk_thread_flag(p, TIF_MEMDIE) &&
		    time_before_eq(jiffies, lowmem_deathpending_timeout)) {
			task_unlock(p);
<<<<<<< HEAD
			continue;
		}
		tasksize = get_mm_rss(p->mm);

		task_unlock(p);
		if (tasksize <= 0)
			continue;
=======
		oom_score_adj = p->signal->oom_score_adj;
		if (oom_score_adj < min_score_adj) {
			task_unlock(p);
			continue;
		}
#if defined(CONFIG_ZRAM) && !defined(CONFIG_RUNTIME_COMPCACHE)
		tasksize = get_mm_rss(p->mm) + get_mm_counter(p->mm, MM_SWAPENTS);
#else
		tasksize = get_mm_rss(p->mm);
#endif
		task_unlock(p);
		if (tasksize <= 0)
			continue;

#ifdef ENHANCED_LMK_ROUTINE
		if (all_selected_oom < LOWMEM_DEATHPENDING_DEPTH) {
			for (i = 0; i < LOWMEM_DEATHPENDING_DEPTH; i++) {
				if (!selected[i]) {
					is_exist_oom_task = 1;
					max_selected_oom_idx = i;
					break;
				}
			}
		} else if (selected_oom_score_adj[max_selected_oom_idx] < oom_score_adj ||
			(selected_oom_score_adj[max_selected_oom_idx] == oom_score_adj &&
			selected_tasksize[max_selected_oom_idx] < tasksize)) {
			is_exist_oom_task = 1;
		}

		if (is_exist_oom_task) {
			selected[max_selected_oom_idx] = p;
			selected_tasksize[max_selected_oom_idx] = tasksize;
			selected_oom_score_adj[max_selected_oom_idx] = oom_score_adj;

			if (all_selected_oom < LOWMEM_DEATHPENDING_DEPTH)
				all_selected_oom++;

			if (all_selected_oom == LOWMEM_DEATHPENDING_DEPTH) {
				for (i = 0; i < LOWMEM_DEATHPENDING_DEPTH; i++) {
					if (selected_oom_score_adj[i] < selected_oom_score_adj[max_selected_oom_idx])
						max_selected_oom_idx = i;
					else if (selected_oom_score_adj[i] == selected_oom_score_adj[max_selected_oom_idx] &&
							selected_tasksize[i] < selected_tasksize[max_selected_oom_idx])
						max_selected_oom_idx = i;
				}
			}

			lowmem_print(2, "select '%s' (%d), adj %hd, size %d, to kill\n",
					p->comm, p->pid, OOM_SCORE_ADJ_TO_OOM_ADJ(oom_score_adj), tasksize);
		}
#else
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		if (selected) {
			if (oom_score_adj < selected_oom_score_adj)
				continue;
			if (oom_score_adj == selected_oom_score_adj &&
			    tasksize <= selected_tasksize)
				continue;
		}
		selected = p;
		selected_tasksize = tasksize;
		selected_oom_score_adj = oom_score_adj;
<<<<<<< HEAD
		lowmem_print(3, "select '%s' (%d), adj %hd, size %d, to kill\n",
			     p->comm, p->pid, oom_score_adj, tasksize);
	}
	if (selected) {
		lowmem_print(1, "Killing '%s' (%d), adj %hd,\n" \
				"   to free %ldkB on behalf of '%s' (%d) because\n" \
				"   cache %ldkB is below limit %ldkB for oom_score_adj %hd\n" \
				"   Free memory is %ldkB above reserved.\n" \
				"   is_kswapd %d priority %d\n" \
				"   Free CMA is %ldkB\n" \
				"   Total reserve is %ldkB\n" \
				"   Total free pages is %ldkB\n" \
				"   Total file cache is %ldkB\n" \
				"   Slab Reclaimable is %ldkB\n" \
				"   Slab UnReclaimable is %ldkB\n" \
				"   Total Slab is %ldkB\n" \
				"   GFP mask is 0x%x\n",
			     selected->comm, selected->pid,
			     selected_oom_score_adj,
			     selected_tasksize * (long)(PAGE_SIZE / 1024),
			     current->comm, current->pid,
			     other_file * (long)(PAGE_SIZE / 1024),
			     minfree * (long)(PAGE_SIZE / 1024),
			     min_score_adj,
			     other_free * (long)(PAGE_SIZE / 1024),
			     !!current_is_kswapd(),
			     sc->priority,
			     nr_cma_free *
				(long)(PAGE_SIZE / 1024),
			     totalreserve_pages * (long)(PAGE_SIZE / 1024),
			     global_page_state(NR_FREE_PAGES) *
				(long)(PAGE_SIZE / 1024),
			     global_page_state(NR_FILE_PAGES) *
				(long)(PAGE_SIZE / 1024),
			     global_page_state(NR_SLAB_RECLAIMABLE) *
				(long)(PAGE_SIZE / 1024),
			     global_page_state(NR_SLAB_UNRECLAIMABLE) *
				(long)(PAGE_SIZE / 1024),
			     global_page_state(NR_SLAB_RECLAIMABLE) *
				(long)(PAGE_SIZE / 1024) +
			     global_page_state(NR_SLAB_UNRECLAIMABLE) *
				(long)(PAGE_SIZE / 1024),
			     sc->gfp_mask);

		if (lowmem_debug_level >= 2 && selected_oom_score_adj == 0) {
			show_mem(SHOW_MEM_FILTER_NODES);
			dump_tasks(NULL, NULL);
			show_mem_call_notifiers();
		}

		lowmem_deathpending_timeout = jiffies + HZ;
=======
		lowmem_print(2, "select '%s' (%d), adj %hd, size %d, to kill\n",
			     p->comm, p->pid, OOM_SCORE_ADJ_TO_OOM_ADJ(oom_score_adj), tasksize);
#endif
	}
#ifdef ENHANCED_LMK_ROUTINE
	for (i = 0; i < LOWMEM_DEATHPENDING_DEPTH; i++) {
		if (selected[i] && (selected_oom_score_adj[i] || is_need_lmk_kill)) {
			lowmem_print(1, "Killing '%s' (%d), adj %hd,\n" \
					"   to free %ldkB on behalf of '%s' (%d) because\n" \
					"   cache %ldkB is below limit %ldkB for oom_score_adj %hd\n" \
					"   Free memory is %ldkB above reserved\n"	\
					"   min adj %hd zram: adj %hd free %d%% usage %dkB\n",
					selected[i]->comm, selected[i]->pid,
					OOM_SCORE_ADJ_TO_OOM_ADJ(selected_oom_score_adj[i]),
					selected_tasksize[i] * (long)(PAGE_SIZE / 1024),
					current->comm, current->pid,
					other_file * (long)(PAGE_SIZE / 1024),
					minfree * (long)(PAGE_SIZE / 1024),
					OOM_SCORE_ADJ_TO_OOM_ADJ(min_score_adj),
					other_free * (long)(PAGE_SIZE / 1024),
					OOM_SCORE_ADJ_TO_OOM_ADJ(lmk_info.min_score_adj),
					OOM_SCORE_ADJ_TO_OOM_ADJ(lmk_info.zram_score_adj),
					lmk_info.zram_free_percent,
					lmk_info.zram_mem_usage*PAGE_SIZE /1024);
			lowmem_deathpending_timeout = jiffies + HZ;

			//Improve the priority of killed process can accelerate the process to die,
			//and the process memory would be released quickly
			boost_dying_task_prio(selected[i]);

			send_sig(SIGKILL, selected[i], 0);
			set_tsk_thread_flag(selected[i], TIF_MEMDIE);
			rem -= selected_tasksize[i];
#ifdef LMK_COUNT_READ
			lmk_count++;
#endif
		}
	}
	if (selected[0] && (selected_oom_score_adj[0] || is_need_lmk_kill))
	{
		rcu_read_unlock();
#ifdef CONFIG_SEC_DEBUG_LMK_MEMINFO
		if (__ratelimit(&lmk_rs)) {
			dump_tasks_info();
		}
#endif
		/* give the system time to free up the memory */
		msleep_interruptible(20);
	} else
		rcu_read_unlock();
#else
	if (selected && (selected_oom_score_adj || is_need_lmk_kill)) {
			struct sysinfo si;
			si_swapinfo(&si);
			lowmem_print(1, "Killing '%s' (%d), adj %hd,\n" \
				"   to free %ldkB on behalf of '%s' (%d) because\n" \
				"   cache %ldkB is below limit %ldkB for oom_score_adj %hd\n" \
				"   Free memory is %ldkB above reserved\n"	\
				"   swaptotal is %ldkB, swapfree is %ldkB\n",
				selected->comm, selected->pid,
				OOM_SCORE_ADJ_TO_OOM_ADJ(selected_oom_score_adj),
				selected_tasksize * (long)(PAGE_SIZE / 1024),
				current->comm, current->pid,
				other_file * (long)(PAGE_SIZE / 1024),
				minfree * (long)(PAGE_SIZE / 1024),
				OOM_SCORE_ADJ_TO_OOM_ADJ(min_score_adj),
				other_free * (long)(PAGE_SIZE / 1024),
			     si.totalswap * (long)(PAGE_SIZE / 1024),
			     si.freeswap * (long)(PAGE_SIZE / 1024));

		lowmem_deathpending_timeout = jiffies + HZ;

		//Improve the priority of killed process can accelerate the process to die,
		//and the process memory would be released quickly
		boost_dying_task_prio(selected);

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		send_sig(SIGKILL, selected, 0);
		set_tsk_thread_flag(selected, TIF_MEMDIE);
		rem -= selected_tasksize;
		rcu_read_unlock();
#ifdef LMK_COUNT_READ
		lmk_count++;
#endif
#ifdef CONFIG_SEC_DEBUG_LMK_MEMINFO
		if (__ratelimit(&lmk_rs)) {
<<<<<<< HEAD
			lowmem_print(1, "lowmem_shrink %lu, %x, ofree %d %d, ma %d\n",
					nr_to_scan, sc->gfp_mask, other_free,
					other_file, min_score_adj);
			show_mem(SHOW_MEM_FILTER_NODES);
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
			dump_tasks_info();
		}
#endif
		/* give the system time to free up the memory */
		msleep_interruptible(20);
<<<<<<< HEAD

		trace_almk_shrink(selected_tasksize, ret,
			other_free, other_file, selected_oom_score_adj);

		if(reclaim_state)
			reclaim_state->reclaimed_slab = selected_tasksize;
	} else {
		trace_almk_shrink(1, ret, other_free, other_file, 0);
		rcu_read_unlock();
	}

=======
	} else
		rcu_read_unlock();
#endif
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	lowmem_print(4, "lowmem_shrink %lu, %x, return %d\n",
		     nr_to_scan, sc->gfp_mask, rem);
	mutex_unlock(&scan_mutex);
	return rem;
}

/*
 * CONFIG_SEC_OOM_KILLER : klaatu@sec
 *
 * The way to select victim by oom-killer provided by
 * linux kernel is totally different from android policy.
 * Hence, it makes more sense that we select the oom victim
 * as android does when LMK is invoked.
 *
*/
#ifdef CONFIG_SEC_OOM_KILLER

static int android_oom_handler(struct notifier_block *nb,
				      unsigned long val, void *data)
{
	struct task_struct *tsk;
#ifdef MULTIPLE_OOM_KILLER
	struct task_struct *selected[OOM_DEPTH] = {NULL,};
#else
	struct task_struct *selected = NULL;
#endif
	int rem = 0;
	int tasksize;
	int i;
	int min_score_adj = OOM_SCORE_ADJ_MAX + 1;
#ifdef MULTIPLE_OOM_KILLER
	int selected_tasksize[OOM_DEPTH] = {0,};
	int selected_oom_score_adj[OOM_DEPTH] = {OOM_ADJUST_MAX,};
	int all_selected_oom = 0;
	int max_selected_oom_idx = 0;
#else
	int selected_tasksize = 0;
	int selected_oom_score_adj;
#endif
<<<<<<< HEAD
	static DEFINE_RATELIMIT_STATE(oom_rs, DEFAULT_RATELIMIT_INTERVAL/5, 1);

	unsigned long *freed = data;

=======
#ifdef CONFIG_SEC_DEBUG_LMK_MEMINFO
	static DEFINE_RATELIMIT_STATE(oom_rs, DEFAULT_RATELIMIT_INTERVAL/5, 1);
#endif

	unsigned long *freed = data;
	
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	/* show status */
	pr_warning("%s invoked Android-oom-killer: "
		"oom_score_adj=%d\n",
		current->comm,
		current->signal->oom_score_adj);
<<<<<<< HEAD
=======
#ifdef CONFIG_SEC_DEBUG_LMK_MEMINFO
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	dump_stack();
	show_mem(SHOW_MEM_FILTER_NODES);
	if (__ratelimit(&oom_rs))
		dump_tasks_info();
<<<<<<< HEAD
=======
#endif
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	min_score_adj = 0;
#ifdef MULTIPLE_OOM_KILLER
	for (i = 0; i < OOM_DEPTH; i++)
		selected_oom_score_adj[i] = min_score_adj;
#else
	selected_oom_score_adj = min_score_adj;
#endif

	read_lock(&tasklist_lock);
	for_each_process(tsk) {
		struct task_struct *p;
		int oom_score_adj;
#ifdef MULTIPLE_OOM_KILLER
		int is_exist_oom_task = 0;
#endif

<<<<<<< HEAD
		if (tsk->flags & PF_KTHREAD ||
			tsk->state & TASK_UNINTERRUPTIBLE)
=======
		if (tsk->flags & PF_KTHREAD)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
			continue;

		p = find_lock_task_mm(tsk);
		if (!p)
			continue;

<<<<<<< HEAD
=======
			rcu_read_unlock();
			return 0;
		}
>>>>>>> parent of 59a54da8838... core33g: Import SM-G360H_KK_Opensource
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		oom_score_adj = p->signal->oom_score_adj;
		if (oom_score_adj < min_score_adj) {
			task_unlock(p);
			continue;
		}
		tasksize = get_mm_rss(p->mm);
		task_unlock(p);
		if (tasksize <= 0)
			continue;
<<<<<<< HEAD
=======

		lowmem_print(2, "oom: ------ %d (%s), adj %d, size %d\n",
			     p->pid, p->comm, oom_score_adj, tasksize);
#ifdef MULTIPLE_OOM_KILLER
		if (all_selected_oom < OOM_DEPTH) {
			for (i = 0; i < OOM_DEPTH; i++) {
				if (!selected[i]) {
					is_exist_oom_task = 1;
					max_selected_oom_idx = i;
					break;
				}
			}
		} else if (selected_oom_score_adj[max_selected_oom_idx] < oom_score_adj ||
			(selected_oom_score_adj[max_selected_oom_idx] == oom_score_adj &&
			selected_tasksize[max_selected_oom_idx] < tasksize)) {
			is_exist_oom_task = 1;
		}

		if (is_exist_oom_task) {
			selected[max_selected_oom_idx] = p;
			selected_tasksize[max_selected_oom_idx] = tasksize;
			selected_oom_score_adj[max_selected_oom_idx] = oom_score_adj;

			if (all_selected_oom < OOM_DEPTH)
				all_selected_oom++;

			if (all_selected_oom == OOM_DEPTH) {
				for (i = 0; i < OOM_DEPTH; i++) {
					if (selected_oom_score_adj[i] < selected_oom_score_adj[max_selected_oom_idx])
						max_selected_oom_idx = i;
					else if (selected_oom_score_adj[i] == selected_oom_score_adj[max_selected_oom_idx] &&
						selected_tasksize[i] < selected_tasksize[max_selected_oom_idx])
						max_selected_oom_idx = i;
				}
			}

			lowmem_print(2, "oom: max_selected_oom_idx(%d) select %d (%s), adj %d, \
					size %d, to kill\n",
				max_selected_oom_idx, p->pid, p->comm, oom_score_adj, tasksize);
		}
#else
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		if (selected) {
			if (oom_score_adj < selected_oom_score_adj)
				continue;
			if (oom_score_adj == selected_oom_score_adj &&
			    tasksize <= selected_tasksize)
				continue;
		}
		selected = p;
		selected_tasksize = tasksize;
		selected_oom_score_adj = oom_score_adj;
<<<<<<< HEAD
		lowmem_print(2, "select %d (%s), adj %hd, size %d, to kill\n",
			     p->pid, p->comm, oom_score_adj, tasksize);
<<<<<<< HEAD
=======
		lowmem_print(2, "oom: select %d (%s), adj %d, size %d, to kill\n",
			     p->pid, p->comm, oom_score_adj, tasksize);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#endif
	}
#ifdef MULTIPLE_OOM_KILLER
	for (i = 0; i < OOM_DEPTH; i++) {
		if (selected[i]) {
			lowmem_print(1, "oom: send sigkill to %d (%s), adj %d,\
				     size %d\n",
				     selected[i]->pid, selected[i]->comm,
				     selected_oom_score_adj[i],
				     selected_tasksize[i]);
<<<<<<< HEAD
			send_sig(SIGKILL, selected[i], 0);
=======

			//Improve the priority of killed process can accelerate the process to die,
			//and the process memory would be released quickly
			boost_dying_task_prio(selected[i]);

			send_sig(SIGKILL, selected[i], 0);
			set_tsk_thread_flag(selected[i], TIF_MEMDIE);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
			rem -= selected_tasksize[i];
			*freed += (unsigned long)selected_tasksize[i];
#ifdef OOM_COUNT_READ
			oom_count++;
#endif
<<<<<<< HEAD
		}
=======
>>>>>>> parent of 59a54da8838... core33g: Import SM-G360H_KK_Opensource
	}
	if (selected) {
		lowmem_print(1, "send sigkill to %d (%s), adj %hd, size %d\n",
			     selected->pid, selected->comm,
			     selected_oom_score_adj, selected_tasksize);
<<<<<<< HEAD
=======
		lowmem_deathpending_timeout = jiffies + HZ;
>>>>>>> parent of 59a54da8838... core33g: Import SM-G360H_KK_Opensource
		send_sig(SIGKILL, selected, 0);
		set_tsk_thread_flag(selected, TIF_MEMDIE);
		rem -= selected_tasksize;
	}
	lowmem_print(4, "lowmem_shrink %lu, %x, return %d\n",
		     sc->nr_to_scan, sc->gfp_mask, rem);
	rcu_read_unlock();
	return rem;
}

=======

		}
	}
#else
	if (selected) {
		lowmem_print(1, "oom: send sigkill to %d (%s), adj %d, size %d\n",
			     selected->pid, selected->comm,
			     selected_oom_score_adj, selected_tasksize);

		//Improve the priority of killed process can accelerate the process to die,
		//and the process memory would be released quickly
		boost_dying_task_prio(selected);

		send_sig(SIGKILL, selected, 0);
		set_tsk_thread_flag(selected, TIF_MEMDIE);
		rem -= selected_tasksize;
		*freed += (unsigned long)selected_tasksize;
#ifdef OOM_COUNT_READ
		oom_count++;
#endif
	}
#endif
	read_unlock(&tasklist_lock);

	lowmem_print(2, "oom: get memory %lu", *freed);
	return rem;
}

static struct notifier_block android_oom_notifier = {
	.notifier_call = android_oom_handler,
};
#endif /* CONFIG_SEC_OOM_KILLER */

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
static struct shrinker lowmem_shrinker = {
	.shrink = lowmem_shrink,
	.seeks = DEFAULT_SEEKS * 16
};

static int __init lowmem_init(void)
{
	register_shrinker(&lowmem_shrinker);
<<<<<<< HEAD
<<<<<<< HEAD
	vmpressure_notifier_register(&lmk_vmpr_nb);
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#ifdef CONFIG_SEC_OOM_KILLER
	register_oom_notifier(&android_oom_notifier);
#endif

<<<<<<< HEAD
=======
>>>>>>> parent of 59a54da8838... core33g: Import SM-G360H_KK_Opensource
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	return 0;
}

static void __exit lowmem_exit(void)
{
	unregister_shrinker(&lowmem_shrinker);
}

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#ifdef CONFIG_ADAPTIVE_KSM
int get_minfree_high_value(void)
{
	int array_size = ARRAY_SIZE(lowmem_adj);
	if (lowmem_adj_size < array_size)
		array_size = lowmem_adj_size;
	if (lowmem_minfree_size < array_size)
		array_size = lowmem_minfree_size;

	return lowmem_minfree[array_size-1];
}
EXPORT_SYMBOL(get_minfree_high_value);
#endif
<<<<<<< HEAD
#ifdef CONFIG_ANDROID_LOW_MEMORY_KILLER_AUTODETECT_OOM_ADJ_VALUES
static short lowmem_oom_adj_to_oom_score_adj(short oom_adj)
=======

#ifdef CONFIG_ANDROID_LOW_MEMORY_KILLER_AUTODETECT_OOM_ADJ_VALUES
static int lowmem_oom_adj_to_oom_score_adj(int oom_adj)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
{
	if (oom_adj == OOM_ADJUST_MAX)
		return OOM_SCORE_ADJ_MAX;
	else
		return (oom_adj * OOM_SCORE_ADJ_MAX) / -OOM_DISABLE;
}

static void lowmem_autodetect_oom_adj_values(void)
{
	int i;
<<<<<<< HEAD
	short oom_adj;
	short oom_score_adj;
=======
	int oom_adj;
	int oom_score_adj;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	int array_size = ARRAY_SIZE(lowmem_adj);

	if (lowmem_adj_size < array_size)
		array_size = lowmem_adj_size;

	if (array_size <= 0)
		return;

	oom_adj = lowmem_adj[array_size - 1];
	if (oom_adj > OOM_ADJUST_MAX)
		return;

	oom_score_adj = lowmem_oom_adj_to_oom_score_adj(oom_adj);
	if (oom_score_adj <= OOM_ADJUST_MAX)
		return;

	lowmem_print(1, "lowmem_shrink: convert oom_adj to oom_score_adj:\n");
	for (i = 0; i < array_size; i++) {
		oom_adj = lowmem_adj[i];
		oom_score_adj = lowmem_oom_adj_to_oom_score_adj(oom_adj);
		lowmem_adj[i] = oom_score_adj;
		lowmem_print(1, "oom_adj %d => oom_score_adj %d\n",
			     oom_adj, oom_score_adj);
	}
}

static int lowmem_adj_array_set(const char *val, const struct kernel_param *kp)
{
	int ret;

	ret = param_array_ops.set(val, kp);

	/* HACK: Autodetect oom_adj values in lowmem_adj array */
	lowmem_autodetect_oom_adj_values();

	return ret;
}

<<<<<<< HEAD
static int lowmem_adj_array_get(char *buffer, const struct kernel_param *kp)
{
=======
static int lowmem_oom_score_adj_to_oom_adj(int oom_score_adj)
{
	if (oom_score_adj == OOM_SCORE_ADJ_MAX)
		return OOM_ADJUST_MAX;
	else
		return  (oom_score_adj * (-OOM_DISABLE) + OOM_SCORE_ADJ_MAX - 1) /  OOM_SCORE_ADJ_MAX; 
}

static uint32_t oom_score_to_oom_enable = 1;

static int lowmem_adj_array_get(char *buffer, const struct kernel_param *kp)
{
	if(oom_score_to_oom_enable)
	{
		int oom_adj[6] = {0};
		int t = 0;
		for(t = 0; t < 6; t++)
		{
			oom_adj[t] = lowmem_oom_score_adj_to_oom_adj(lowmem_adj[t]);
		}
		return sprintf(buffer, "%d,%d,%d,%d,%d,%d", oom_adj[0], oom_adj[1], oom_adj[2], oom_adj[3], oom_adj[4], oom_adj[5]);
	}

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	return param_array_ops.get(buffer, kp);
}

static void lowmem_adj_array_free(void *arg)
{
	param_array_ops.free(arg);
}

static struct kernel_param_ops lowmem_adj_array_ops = {
	.set = lowmem_adj_array_set,
	.get = lowmem_adj_array_get,
	.free = lowmem_adj_array_free,
};

static const struct kparam_array __param_arr_adj = {
	.max = ARRAY_SIZE(lowmem_adj),
	.num = &lowmem_adj_size,
<<<<<<< HEAD
	.ops = &param_ops_short,
	.elemsize = sizeof(lowmem_adj[0]),
	.elem = lowmem_adj,
};
=======
	.ops = &param_ops_int,
	.elemsize = sizeof(lowmem_adj[0]),
	.elem = lowmem_adj,
};


module_param_array_named(oom_score_adj, lowmem_adj, int, &lowmem_adj_size, S_IRUGO | S_IWUSR);

module_param_named(oom_score_to_oom_enable, oom_score_to_oom_enable, uint, S_IRUGO | S_IWUSR);

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#endif

module_param_named(cost, lowmem_shrinker.seeks, int, S_IRUGO | S_IWUSR);
#ifdef CONFIG_ANDROID_LOW_MEMORY_KILLER_AUTODETECT_OOM_ADJ_VALUES
__module_param_call(MODULE_PARAM_PREFIX, adj,
		    &lowmem_adj_array_ops,
		    .arr = &__param_arr_adj,
		    S_IRUGO | S_IWUSR, -1);
<<<<<<< HEAD
__MODULE_PARM_TYPE(adj, "array of short");
#else
=======
module_param_named(cost, lowmem_shrinker.seeks, int, S_IRUGO | S_IWUSR);
>>>>>>> parent of 59a54da8838... core33g: Import SM-G360H_KK_Opensource
module_param_array_named(adj, lowmem_adj, short, &lowmem_adj_size,
			 S_IRUGO | S_IWUSR);
module_param_array_named(minfree, lowmem_minfree, uint, &lowmem_minfree_size,
			 S_IRUGO | S_IWUSR);
module_param_named(debug_level, lowmem_debug_level, uint, S_IRUGO | S_IWUSR);
<<<<<<< HEAD
=======
__MODULE_PARM_TYPE(adj, "array of int");
#else
module_param_array_named(adj, lowmem_adj, int, &lowmem_adj_size,
			 S_IRUGO | S_IWUSR);
#endif
module_param_array_named(minfree, lowmem_minfree, uint, &lowmem_minfree_size,
			 S_IRUGO | S_IWUSR);
module_param_named(debug_level, lowmem_debug_level, uint, S_IRUGO | S_IWUSR);

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#ifdef LMK_COUNT_READ
module_param_named(lmkcount, lmk_count, uint, S_IRUGO);
#endif
#ifdef OOM_COUNT_READ
module_param_named(oomcount, oom_count, uint, S_IRUGO);
#endif
<<<<<<< HEAD
=======
>>>>>>> parent of 59a54da8838... core33g: Import SM-G360H_KK_Opensource
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

module_init(lowmem_init);
module_exit(lowmem_exit);

MODULE_LICENSE("GPL");


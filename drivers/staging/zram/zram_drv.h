/*
 * Compressed RAM block device
 *
 * Copyright (C) 2008, 2009, 2010  Nitin Gupta
 *
 * This code is released using a dual license strategy: BSD/GPL
 * You can choose the licence that better fits your requirements.
 *
 * Released under the terms of 3-clause BSD License
 * Released under the terms of GNU General Public License Version 2.0
 *
 * Project home: http://compcache.googlecode.com
 */

#ifndef _ZRAM_DRV_H_
#define _ZRAM_DRV_H_

#include <linux/spinlock.h>
#include <linux/mutex.h>
<<<<<<< HEAD

#include "../zsmalloc/zsmalloc.h"
=======
#include <linux/zsmalloc.h>
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

/*
 * Some arbitrary value. This is just to catch
 * invalid value for num_devices module parameter.
 */
static const unsigned max_num_devices = 32;

/*-- Configurable parameters */

/*
 * Pages that compress to size greater than this are stored
 * uncompressed in memory.
 */
static const size_t max_zpage_size = PAGE_SIZE / 4 * 3;

/*
 * NOTE: max_zpage_size must be less than or equal to:
 *   ZS_MAX_ALLOC_SIZE. Otherwise, zs_malloc() would
 * always return failure.
 */

/*-- End of configurable params */

#define SECTOR_SHIFT		9
#define SECTOR_SIZE		(1 << SECTOR_SHIFT)
#define SECTORS_PER_PAGE_SHIFT	(PAGE_SHIFT - SECTOR_SHIFT)
#define SECTORS_PER_PAGE	(1 << SECTORS_PER_PAGE_SHIFT)
#define ZRAM_LOGICAL_BLOCK_SHIFT 12
#define ZRAM_LOGICAL_BLOCK_SIZE	(1 << ZRAM_LOGICAL_BLOCK_SHIFT)
#define ZRAM_SECTOR_PER_LOGICAL_BLOCK	\
	(1 << (ZRAM_LOGICAL_BLOCK_SHIFT - SECTOR_SHIFT))

/* Flags for zram pages (table[page_no].flags) */
enum zram_pageflags {
	/* Page consists entirely of zeros */
	ZRAM_ZERO,

	__NR_ZRAM_PAGEFLAGS,
};

/*-- Data structures */

/* Allocated for each disk page */
struct table {
	unsigned long handle;
	u16 size;	/* object size (excluding header) */
	u8 count;	/* object ref count (not yet used) */
	u8 flags;
} __aligned(4);

struct zram_stats {
	u64 compr_size;		/* compressed size of pages stored */
	u64 num_reads;		/* failed + successful */
	u64 num_writes;		/* --do-- */
	u64 failed_reads;	/* should NEVER! happen */
	u64 failed_writes;	/* can happen when memory is too low */
	u64 invalid_io;		/* non-page-aligned I/O requests */
	u64 notify_free;	/* no. of swap slot free notifications */
	u32 pages_zero;		/* no. of zero filled pages */
	u32 pages_stored;	/* no. of pages currently stored */
	u32 good_compress;	/* % of pages with compression ratio<=50% */
	u32 bad_compress;	/* % of pages with compression ratio>=75% */
};

struct zram_meta {
	void *compress_workmem;
	void *compress_buffer;
	struct table *table;
	struct zs_pool *mem_pool;
};

struct zram {
	struct zram_meta *meta;
	spinlock_t stat64_lock;	/* protect 64-bit stats */
<<<<<<< HEAD
	struct rw_semaphore lock; /* protect compression buffers, table,
				   * 32bit stat counters against concurrent
				   * notifications, reads and writes */
=======
	struct rw_semaphore lock; /* protect compression buffers and table
				   * against concurrent read and writes */
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	struct request_queue *queue;
	struct gendisk *disk;
	int init_done;
	/* Prevent concurrent execution of device init, reset and R/W request */
	struct rw_semaphore init_lock;
	/*
	 * This is the limit on amount of *uncompressed* worth of data
	 * we can store in a disk.
	 */
	u64 disksize;	/* bytes */

	struct zram_stats stats;
};

extern struct zram *zram_devices;
unsigned int zram_get_num_devices(void);
#ifdef CONFIG_SYSFS
extern struct attribute_group zram_disk_attr_group;
#endif

extern void zram_reset_device(struct zram *zram);
extern struct zram_meta *zram_meta_alloc(u64 disksize);
extern void zram_meta_free(struct zram_meta *meta);
extern void zram_init_device(struct zram *zram, struct zram_meta *meta);

#endif

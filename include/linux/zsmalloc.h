/*
 * zsmalloc memory allocator
 *
 * Copyright (C) 2011  Nitin Gupta
 * Copyright (C) 2012, 2013 Minchan Kim
 *
 * This code is released using a dual license strategy: BSD/GPL
 * You can choose the license that better fits your requirements.
 *
 * Released under the terms of 3-clause BSD License
 * Released under the terms of GNU General Public License Version 2.0
 */

#ifndef _ZS_MALLOC_H_
#define _ZS_MALLOC_H_

#include <linux/types.h>

/*
 * zsmalloc mapping modes
 *
<<<<<<< HEAD
 * NOTE: These only make a difference when a mapped object spans pages
=======
 * NOTE: These only make a difference when a mapped object spans pages.
 * They also have no effect when PGTABLE_MAPPING is selected.
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
 */
enum zs_mapmode {
	ZS_MM_RW, /* normal read-write mapping */
	ZS_MM_RO, /* read-only (no copy-out at unmap time) */
	ZS_MM_WO /* write-only (no copy-in at map time) */
<<<<<<< HEAD
=======
	/*
	 * NOTE: ZS_MM_WO should only be used for initializing new
	 * (uninitialized) allocations.  Partial writes to already
	 * initialized allocations should use ZS_MM_RW to preserve the
	 * existing data.
	 */
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
};

struct zs_pool;

<<<<<<< HEAD
struct zs_pool *zs_create_pool(char *name, gfp_t flags);
=======
struct zs_ops {
	int (*evict)(struct zs_pool *pool, unsigned long handle);
};

struct zs_pool *zs_create_pool(gfp_t flags, struct zs_ops *ops);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
void zs_destroy_pool(struct zs_pool *pool);

unsigned long zs_malloc(struct zs_pool *pool, size_t size);
void zs_free(struct zs_pool *pool, unsigned long obj);
<<<<<<< HEAD
=======
int zs_shrink(struct zs_pool *pool);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

void *zs_map_object(struct zs_pool *pool, unsigned long handle,
			enum zs_mapmode mm);
void zs_unmap_object(struct zs_pool *pool, unsigned long handle);

unsigned long zs_get_total_pages(struct zs_pool *pool);
<<<<<<< HEAD
unsigned long zs_compact(struct zs_pool *pool);
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

#endif

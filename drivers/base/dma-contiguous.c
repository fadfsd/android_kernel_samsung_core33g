/*
 * Contiguous Memory Allocator for DMA mapping framework
 * Copyright (c) 2010-2011 by Samsung Electronics.
 * Written by:
 *	Marek Szyprowski <m.szyprowski@samsung.com>
 *	Michal Nazarewicz <mina86@mina86.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License or (at your optional) any later version of the license.
 */

#define pr_fmt(fmt) "cma: " fmt

#ifdef CONFIG_CMA_DEBUG
#ifndef DEBUG
#  define DEBUG
#endif
#endif

#include <asm/page.h>
#include <asm/dma-contiguous.h>

#include <linux/memblock.h>
#include <linux/err.h>
#include <linux/mm.h>
#include <linux/mutex.h>
#include <linux/page-isolation.h>
#include <linux/sizes.h>
#include <linux/slab.h>
#include <linux/swap.h>
#include <linux/mm_types.h>
#include <linux/dma-contiguous.h>

<<<<<<< HEAD
=======
#ifdef CONFIG_CMA
#include <linux/cmainfo.h>
static phys_addr_t cmalimit = 0;
static unsigned long dma_declare_count = 0;
static cma_area_t cma_areas[NR_CMA_AREA] = {0};
#endif

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
struct cma {
	unsigned long	base_pfn;
	unsigned long	count;
	unsigned long	*bitmap;
<<<<<<< HEAD
=======
	unsigned long	cma_count;
	unsigned long	threshold_count;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
};

struct cma *dma_contiguous_default_area;

#ifdef CONFIG_CMA_SIZE_MBYTES
#define CMA_SIZE_MBYTES CONFIG_CMA_SIZE_MBYTES
#else
#define CMA_SIZE_MBYTES 0
#endif

/*
 * Default global CMA area size can be defined in kernel's .config.
 * This is usefull mainly for distro maintainers to create a kernel
 * that works correctly for most supported systems.
 * The size can be set in bytes or as a percentage of the total memory
 * in the system.
 *
 * Users, who want to set the size of global CMA area for their system
 * should use cma= kernel parameter.
 */
static const phys_addr_t size_bytes = CMA_SIZE_MBYTES * SZ_1M;
static phys_addr_t size_cmdline = -1;

static int __init early_cma(char *p)
{
	pr_debug("%s(%s)\n", __func__, p);
	size_cmdline = memparse(p, &p);
	return 0;
}
early_param("cma", early_cma);

#ifdef CONFIG_CMA_SIZE_PERCENTAGE

static phys_addr_t __init __maybe_unused cma_early_percent_memory(void)
{
	struct memblock_region *reg;
	unsigned long total_pages = 0;

	/*
	 * We cannot use memblock_phys_mem_size() here, because
	 * memblock_analyze() has not been called yet.
	 */
	for_each_memblock(memory, reg)
		total_pages += memblock_region_memory_end_pfn(reg) -
			       memblock_region_memory_base_pfn(reg);

	return (total_pages * CONFIG_CMA_SIZE_PERCENTAGE / 100) << PAGE_SHIFT;
}

#else

static inline __maybe_unused phys_addr_t cma_early_percent_memory(void)
{
	return 0;
}

#endif

/**
 * dma_contiguous_reserve() - reserve area for contiguous memory handling
 * @limit: End address of the reserved memory (optional, 0 for any).
 *
 * This function reserves memory from early allocator. It should be
 * called by arch specific code once the early allocator (memblock or bootmem)
 * has been activated and all other subsystems have already allocated/reserved
 * memory.
 */
void __init dma_contiguous_reserve(phys_addr_t limit)
{
	phys_addr_t selected_size = 0;

	pr_debug("%s(limit %08lx)\n", __func__, (unsigned long)limit);

	if (size_cmdline != -1) {
		selected_size = size_cmdline;
	} else {
#ifdef CONFIG_CMA_SIZE_SEL_MBYTES
		selected_size = size_bytes;
#elif defined(CONFIG_CMA_SIZE_SEL_PERCENTAGE)
		selected_size = cma_early_percent_memory();
#elif defined(CONFIG_CMA_SIZE_SEL_MIN)
		selected_size = min(size_bytes, cma_early_percent_memory());
#elif defined(CONFIG_CMA_SIZE_SEL_MAX)
		selected_size = max(size_bytes, cma_early_percent_memory());
#endif
	}

	if (selected_size) {
		pr_debug("%s: reserving %ld MiB for global area\n", __func__,
			 (unsigned long)selected_size / SZ_1M);

<<<<<<< HEAD
		dma_declare_contiguous(NULL, selected_size, 0, limit);
=======
		dma_contiguous_reserve_area(NULL, selected_size, 0, limit, 0, 0);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	}
};

static DEFINE_MUTEX(cma_mutex);

static __init int cma_activate_area(unsigned long base_pfn, unsigned long count)
{
	unsigned long pfn = base_pfn;
	unsigned i = count >> pageblock_order;
	struct zone *zone;

	WARN_ON_ONCE(!pfn_valid(pfn));
	zone = page_zone(pfn_to_page(pfn));

	do {
		unsigned j;
		base_pfn = pfn;
		for (j = pageblock_nr_pages; j; --j, pfn++) {
			WARN_ON_ONCE(!pfn_valid(pfn));
			if (page_zone(pfn_to_page(pfn)) != zone)
				return -EINVAL;
		}
		init_cma_reserved_pageblock(pfn_to_page(base_pfn));
	} while (--i);
	return 0;
}

static __init struct cma *cma_create_area(unsigned long base_pfn,
<<<<<<< HEAD
				     unsigned long count)
=======
				     unsigned long count, unsigned long cma_count,
				     unsigned long threshold_count)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
{
	int bitmap_size = BITS_TO_LONGS(count) * sizeof(long);
	struct cma *cma;
	int ret = -ENOMEM;

<<<<<<< HEAD
	pr_debug("%s(base %08lx, count %lx)\n", __func__, base_pfn, count);
=======
	pr_debug("%s(base %08lx, count %lx, cma_count %lx, threshold_count %lx)\n",
		 __func__, base_pfn, count, cma_count, threshold_count);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	cma = kmalloc(sizeof *cma, GFP_KERNEL);
	if (!cma)
		return ERR_PTR(-ENOMEM);

	cma->base_pfn = base_pfn;
	cma->count = count;
	cma->bitmap = kzalloc(bitmap_size, GFP_KERNEL);
<<<<<<< HEAD
=======
	cma->cma_count = cma_count;
	cma->threshold_count = threshold_count;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	if (!cma->bitmap)
		goto no_mem;

<<<<<<< HEAD
	ret = cma_activate_area(base_pfn, count);
	if (ret)
		goto error;
=======
	if(cma_count) {
		ret = cma_activate_area(base_pfn, cma_count);
		if (ret)
			goto error;
	}
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	pr_debug("%s: returned %p\n", __func__, (void *)cma);
	return cma;

error:
	kfree(cma->bitmap);
no_mem:
	kfree(cma);
	return ERR_PTR(ret);
}

static struct cma_reserved {
	phys_addr_t start;
	unsigned long size;
	struct device *dev;
<<<<<<< HEAD
=======
	unsigned long cma_size;
	unsigned long threshold_size;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
} cma_reserved[MAX_CMA_AREAS] __initdata;
static unsigned cma_reserved_count __initdata;

static int __init cma_init_reserved_areas(void)
{
	struct cma_reserved *r = cma_reserved;
	unsigned i = cma_reserved_count;

	pr_debug("%s()\n", __func__);

	for (; i; --i, ++r) {
		struct cma *cma;
		cma = cma_create_area(PFN_DOWN(r->start),
<<<<<<< HEAD
				      r->size >> PAGE_SHIFT);
		if (!IS_ERR(cma))
			dev_set_cma_area(r->dev, cma);
=======
				      r->size >> PAGE_SHIFT, r->cma_size >> PAGE_SHIFT,
				      r->threshold_size >> PAGE_SHIFT);
		if (!IS_ERR(cma)) {
			pr_info("%s: cma %p, base %lx\n", __func__, (void *)cma, (unsigned long)r->start);
			dev_set_cma_area(r->dev, cma);
		}
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	}
	return 0;
}
core_initcall(cma_init_reserved_areas);

/**
<<<<<<< HEAD
 * dma_declare_contiguous() - reserve area for contiguous memory handling
=======
 * dma_contiguous_reserve_area() - reserve area for contiguous memory handling
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
 *			      for particular device
 * @dev:   Pointer to device structure.
 * @size:  Size of the reserved memory.
 * @base:  Start address of the reserved memory (optional, 0 for any).
 * @limit: End address of the reserved memory (optional, 0 for any).
 *
 * This function reserves memory for specified device. It should be
 * called by board specific code when early allocator (memblock or bootmem)
 * is still activate.
 */
<<<<<<< HEAD
int __init dma_declare_contiguous(struct device *dev, phys_addr_t size,
				  phys_addr_t base, phys_addr_t limit)
=======
int __init dma_contiguous_reserve_area(struct device *dev, phys_addr_t size,
				  phys_addr_t base, phys_addr_t limit,
				  phys_addr_t reserve_size, phys_addr_t threshold_size)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
{
	struct cma_reserved *r = &cma_reserved[cma_reserved_count];
	phys_addr_t alignment;

<<<<<<< HEAD
	pr_debug("%s(size %lx, base %08lx, limit %08lx)\n", __func__,
		 (unsigned long)size, (unsigned long)base,
		 (unsigned long)limit);
=======
	pr_debug("%s(size %lx, base %08lx, limit %08lx, reserve_size %lx, threshold_size %lx)\n",
		 __func__, (unsigned long)size, (unsigned long)base,
		 (unsigned long)limit, (unsigned long)reserve_size,
		 (unsigned long)threshold_size);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	/* Sanity checks */
	if (cma_reserved_count == ARRAY_SIZE(cma_reserved)) {
		pr_err("Not enough slots for CMA reserved regions!\n");
		return -ENOSPC;
	}

<<<<<<< HEAD
	if (!size)
=======
	if (!size || reserve_size > size)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		return -EINVAL;

	/* Sanitise input arguments */
	alignment = PAGE_SIZE << max(MAX_ORDER - 1, pageblock_order);
	base = ALIGN(base, alignment);
<<<<<<< HEAD
	size = ALIGN(size, alignment);
=======
	if(!reserve_size)
		size = ALIGN(size, alignment);
	else
		size = ALIGN((size - reserve_size), alignment) + reserve_size;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	limit &= ~(alignment - 1);

	/* Reserve memory */
	if (base) {
		if (memblock_is_region_reserved(base, size) ||
		    memblock_reserve(base, size) < 0) {
			base = -EBUSY;
			goto err;
		}
	} else {
		/*
		 * Use __memblock_alloc_base() since
		 * memblock_alloc_base() panic()s.
		 */
		phys_addr_t addr = __memblock_alloc_base(size, alignment, limit);
		if (!addr) {
			base = -ENOMEM;
			goto err;
		} else {
			base = addr;
		}
	}

	/*
	 * Each reserved area must be initialised later, when more kernel
	 * subsystems (like slab allocator) are available.
	 */
	r->start = base;
	r->size = size;
	r->dev = dev;
<<<<<<< HEAD
	cma_reserved_count++;
	pr_info("CMA: reserved %ld MiB at %08lx\n", (unsigned long)size / SZ_1M,
		(unsigned long)base);

	/* Architecture specific contiguous memory fixup. */
	dma_contiguous_early_fixup(base, size);
=======
	r->cma_size = ALIGN((size - reserve_size), alignment);
	r->threshold_size = ALIGN(threshold_size, PAGE_SIZE);
	cma_reserved_count++;
	pr_info("CMA: reserved %ld MiB at %08lx(cma %ld MB, reserve %ld MB, threshold %ld MB)\n",
		(unsigned long)size / SZ_1M, (unsigned long)base,
		r->cma_size / SZ_1M, (unsigned long)reserve_size / SZ_1M,
		(unsigned long)r->threshold_size / SZ_1M);
#ifdef CONFIG_CMA
	cmalimit += (unsigned long)size / SZ_1K ;
	if(NULL != dev)
	{
		cma_areas[dma_declare_count].cma_phy_start	= (unsigned long)base;
		cma_areas[dma_declare_count].cma_phy_end	= (unsigned long)(base + size);
		cma_areas[dma_declare_count].flag			= CMA_AREA_DEVICE;
	} else {
		cma_areas[dma_declare_count].cma_phy_start	= (unsigned long)base;
		cma_areas[dma_declare_count].cma_phy_end	= (unsigned long)(base + size);
		cma_areas[dma_declare_count].flag			= CMA_AREA_GLOBAL;
	}

	dma_declare_count++;
#endif

	/* Architecture specific contiguous memory fixup. */
	dma_contiguous_early_fixup(base, r->cma_size);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	return 0;
err:
	pr_err("CMA: failed to reserve %ld MiB\n", (unsigned long)size / SZ_1M);
	return base;
}

/**
 * dma_alloc_from_contiguous() - allocate pages from contiguous area
 * @dev:   Pointer to device for which the allocation is performed.
 * @count: Requested number of pages.
 * @align: Requested alignment of pages (in PAGE_SIZE order).
 *
 * This function allocates memory buffer for specified device. It uses
 * device specific contiguous memory area if available or the default
 * global one. Requires architecture specific get_dev_cma_area() helper
 * function.
 */
struct page *dma_alloc_from_contiguous(struct device *dev, int count,
				       unsigned int align)
{
	unsigned long mask, pfn, pageno, start = 0;
	struct cma *cma = dev_get_cma_area(dev);
	struct page *page = NULL;
	int ret;
<<<<<<< HEAD
=======
	int reserve_drain = 0;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	if (!cma || !cma->count)
		return NULL;

	if (align > CONFIG_CMA_ALIGNMENT)
		align = CONFIG_CMA_ALIGNMENT;

	pr_debug("%s(cma %p, count %d, align %d)\n", __func__, (void *)cma,
		 count, align);

	if (!count)
		return NULL;

	mask = (1 << align) - 1;

	mutex_lock(&cma_mutex);

	for (;;) {
<<<<<<< HEAD
		pageno = bitmap_find_next_zero_area(cma->bitmap, cma->count,
						    start, count, mask);
		if (pageno >= cma->count)
			break;

		pfn = cma->base_pfn + pageno;
		ret = alloc_contig_range(pfn, pfn + count, MIGRATE_CMA);
=======
		ret = 0;

		if(cma->threshold_count > 0 &&
		    count >= cma->threshold_count &&
		    !reserve_drain)
			start = cma->cma_count;

		pageno = bitmap_find_next_zero_area(cma->bitmap, cma->count,
						    start, count, mask);
		if (pageno >= cma->count) {
			if(cma->threshold_count > 0 &&
			    count >= cma->threshold_count &&
			    !reserve_drain) {
				reserve_drain = 1;
				start = 0;
				continue;
			} else {
				break;
			}
		}

		pfn = cma->base_pfn + pageno;

		if(pageno < cma->cma_count && pageno + count > cma->cma_count) {
			if(reserve_drain)
				break;
			start = cma->cma_count;
			continue;
		}

		if(pageno + count <= cma->cma_count)
			ret = alloc_contig_range(pfn, pfn + count, MIGRATE_CMA);
		else
			pr_debug("%s(): allocate from reserved memory\n",
				 __func__);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		if (ret == 0) {
			bitmap_set(cma->bitmap, pageno, count);
			page = pfn_to_page(pfn);
			break;
		} else if (ret != -EBUSY) {
			break;
		}
		pr_debug("%s(): memory range at %p is busy, retrying\n",
			 __func__, pfn_to_page(pfn));
		/* try again with a bit different memory target */
		start = pageno + mask + 1;
	}

	mutex_unlock(&cma_mutex);
	pr_debug("%s(): returned %p\n", __func__, page);
	return page;
}

/**
 * dma_release_from_contiguous() - release allocated pages
 * @dev:   Pointer to device for which the pages were allocated.
 * @pages: Allocated pages.
 * @count: Number of allocated pages.
 *
 * This function releases memory allocated by dma_alloc_from_contiguous().
 * It returns false when provided pages do not belong to contiguous area and
 * true otherwise.
 */
bool dma_release_from_contiguous(struct device *dev, struct page *pages,
				 int count)
{
	struct cma *cma = dev_get_cma_area(dev);
	unsigned long pfn;

	if (!cma || !pages)
		return false;

<<<<<<< HEAD
	pr_debug("%s(page %p)\n", __func__, (void *)pages);
=======
	pr_debug("%s(page %p, count %d)\n", __func__, (void *)pages, count);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	pfn = page_to_pfn(pages);

	if (pfn < cma->base_pfn || pfn >= cma->base_pfn + cma->count)
		return false;

	VM_BUG_ON(pfn + count > cma->base_pfn + cma->count);

	mutex_lock(&cma_mutex);
	bitmap_clear(cma->bitmap, pfn - cma->base_pfn, count);
<<<<<<< HEAD
	free_contig_range(pfn, count);
=======
	if(pfn + count <= cma->base_pfn + cma->cma_count)
		free_contig_range(pfn, count);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	mutex_unlock(&cma_mutex);

	return true;
}
<<<<<<< HEAD
=======

#ifdef CONFIG_CMA
void cma_mem_info(cmainfo_t *cmainfo)
{
	unsigned int i;
	cmainfo->totalcma = cmalimit;

	for(i = 0; i < dma_declare_count; i++)
	{
		cmainfo->cma_areas[i].cma_phy_start = cma_areas[i].cma_phy_start;
		cmainfo->cma_areas[i].cma_phy_end	= cma_areas[i].cma_phy_end;
		cmainfo->cma_areas[i].flag			= cma_areas[i].flag;
	}

	cmainfo->nr_cma_areas = dma_declare_count;
}
#endif
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

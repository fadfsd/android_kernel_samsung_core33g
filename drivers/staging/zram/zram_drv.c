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

#define KMSG_COMPONENT "zram"
#define pr_fmt(fmt) KMSG_COMPONENT ": " fmt

#ifdef CONFIG_ZRAM_DEBUG
#define DEBUG
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bio.h>
#include <linux/bitops.h>
#include <linux/blkdev.h>
#include <linux/buffer_head.h>
#include <linux/device.h>
#include <linux/genhd.h>
#include <linux/highmem.h>
#include <linux/slab.h>
#include <linux/lzo.h>
#include <linux/string.h>
#include <linux/vmalloc.h>

#include "zram_drv.h"

<<<<<<< HEAD
=======
#define LZO_ALGO_SW  0
#define LZO_ALGO_HW  1

#ifdef CONFIG_LZO_HW_ALGO
#define  zram_compress           lzo1x_1_compress_hw
#define  zram_decompress_safe    lzo1x_decompress_safe_hw
static uint lzo_algo_type = LZO_ALGO_HW;
bool lzo_sw_flag;
#else
#define  zram_compress           lzo1x_1_compress
#define  zram_decompress_safe    lzo1x_decompress_safe
static uint lzo_algo_type = LZO_ALGO_SW;
#endif

module_param_named(lzo_algo_type, lzo_algo_type, uint, S_IRUGO);

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
/* Globals */
static int zram_major;
struct zram *zram_devices;

/* Module params (documentation at end) */
static unsigned int num_devices = 1;

static void zram_stat64_add(struct zram *zram, u64 *v, u64 inc)
{
	spin_lock(&zram->stat64_lock);
	*v = *v + inc;
	spin_unlock(&zram->stat64_lock);
}

static void zram_stat64_sub(struct zram *zram, u64 *v, u64 dec)
{
	spin_lock(&zram->stat64_lock);
	*v = *v - dec;
	spin_unlock(&zram->stat64_lock);
}

static void zram_stat64_inc(struct zram *zram, u64 *v)
{
	zram_stat64_add(zram, v, 1);
}

static int zram_test_flag(struct zram_meta *meta, u32 index,
			enum zram_pageflags flag)
{
	return meta->table[index].flags & BIT(flag);
}

static void zram_set_flag(struct zram_meta *meta, u32 index,
			enum zram_pageflags flag)
{
	meta->table[index].flags |= BIT(flag);
}

static void zram_clear_flag(struct zram_meta *meta, u32 index,
			enum zram_pageflags flag)
{
	meta->table[index].flags &= ~BIT(flag);
}

static int page_zero_filled(void *ptr)
{
	unsigned int pos;
	unsigned long *page;

	page = (unsigned long *)ptr;

	for (pos = 0; pos != PAGE_SIZE / sizeof(*page); pos++) {
		if (page[pos])
			return 0;
	}

	return 1;
}

static void zram_free_page(struct zram *zram, size_t index)
{
	struct zram_meta *meta = zram->meta;
	unsigned long handle = meta->table[index].handle;
	u16 size = meta->table[index].size;

	if (unlikely(!handle)) {
		/*
		 * No memory is allocated for zero filled pages.
		 * Simply clear zero page flag.
		 */
		if (zram_test_flag(meta, index, ZRAM_ZERO)) {
			zram_clear_flag(meta, index, ZRAM_ZERO);
			zram->stats.pages_zero--;
		}
		return;
	}

	if (unlikely(size > max_zpage_size))
		zram->stats.bad_compress--;

	zs_free(meta->mem_pool, handle);

	if (size <= PAGE_SIZE / 2)
		zram->stats.good_compress--;

	zram_stat64_sub(zram, &zram->stats.compr_size,
			meta->table[index].size);
	zram->stats.pages_stored--;

	meta->table[index].handle = 0;
	meta->table[index].size = 0;
}

static void handle_zero_page(struct bio_vec *bvec)
{
	struct page *page = bvec->bv_page;
	void *user_mem;

	user_mem = kmap_atomic(page);
	memset(user_mem + bvec->bv_offset, 0, bvec->bv_len);
	kunmap_atomic(user_mem);

	flush_dcache_page(page);
}

static inline int is_partial_io(struct bio_vec *bvec)
{
	return bvec->bv_len != PAGE_SIZE;
}

static int zram_decompress_page(struct zram *zram, char *mem, u32 index)
{
	int ret = LZO_E_OK;
	size_t clen = PAGE_SIZE;
	unsigned char *cmem;
	struct zram_meta *meta = zram->meta;
	unsigned long handle = meta->table[index].handle;

	if (!handle || zram_test_flag(meta, index, ZRAM_ZERO)) {
		memset(mem, 0, PAGE_SIZE);
		return 0;
	}

	cmem = zs_map_object(meta->mem_pool, handle, ZS_MM_RO);
	if (meta->table[index].size == PAGE_SIZE)
		memcpy(mem, cmem, PAGE_SIZE);
	else
<<<<<<< HEAD
		ret = lzo1x_decompress_safe(cmem, meta->table[index].size,
=======
		ret = zram_decompress_safe(cmem, meta->table[index].size,
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
						mem, &clen);
	zs_unmap_object(meta->mem_pool, handle);

	/* Should NEVER happen. Return bio error if it does. */
	if (unlikely(ret != LZO_E_OK)) {
		pr_err("Decompression failed! err=%d, page=%u\n", ret, index);
		zram_stat64_inc(zram, &zram->stats.failed_reads);
		return ret;
	}

	return 0;
}

static int zram_bvec_read(struct zram *zram, struct bio_vec *bvec,
			  u32 index, int offset, struct bio *bio)
{
	int ret;
	struct page *page;
	unsigned char *user_mem, *uncmem = NULL;
	struct zram_meta *meta = zram->meta;
	page = bvec->bv_page;

	if (unlikely(!meta->table[index].handle) ||
			zram_test_flag(meta, index, ZRAM_ZERO)) {
		handle_zero_page(bvec);
		return 0;
	}

	if (is_partial_io(bvec))
		/* Use  a temporary buffer to decompress the page */
		uncmem = kmalloc(PAGE_SIZE, GFP_NOIO);

	user_mem = kmap_atomic(page);
	if (!is_partial_io(bvec))
		uncmem = user_mem;

	if (!uncmem) {
		pr_info("Unable to allocate temp memory\n");
		ret = -ENOMEM;
		goto out_cleanup;
	}

	ret = zram_decompress_page(zram, uncmem, index);
	/* Should NEVER happen. Return bio error if it does. */
	if (unlikely(ret != LZO_E_OK))
		goto out_cleanup;

	if (is_partial_io(bvec))
		memcpy(user_mem + bvec->bv_offset, uncmem + offset,
				bvec->bv_len);

	flush_dcache_page(page);
	ret = 0;
out_cleanup:
	kunmap_atomic(user_mem);
	if (is_partial_io(bvec))
		kfree(uncmem);
	return ret;
}

static int zram_bvec_write(struct zram *zram, struct bio_vec *bvec, u32 index,
			   int offset)
{
	int ret = 0;
	size_t clen;
	unsigned long handle;
	struct page *page;
	unsigned char *user_mem, *cmem, *src, *uncmem = NULL;
	struct zram_meta *meta = zram->meta;

	page = bvec->bv_page;
	src = meta->compress_buffer;

	if (is_partial_io(bvec)) {
		/*
		 * This is a partial IO. We need to read the full page
		 * before to write the changes.
		 */
		uncmem = kmalloc(PAGE_SIZE, GFP_NOIO);
		if (!uncmem) {
			ret = -ENOMEM;
			goto out;
		}
		ret = zram_decompress_page(zram, uncmem, index);
		if (ret)
			goto out;
	}

	/*
	 * System overwrites unused sectors. Free memory associated
	 * with this sector now.
	 */
	if (meta->table[index].handle ||
	    zram_test_flag(meta, index, ZRAM_ZERO))
		zram_free_page(zram, index);

	user_mem = kmap_atomic(page);

	if (is_partial_io(bvec)) {
		memcpy(uncmem + offset, user_mem + bvec->bv_offset,
		       bvec->bv_len);
		kunmap_atomic(user_mem);
		user_mem = NULL;
	} else {
		uncmem = user_mem;
	}

	if (page_zero_filled(uncmem)) {
		kunmap_atomic(user_mem);
		zram->stats.pages_zero++;
		zram_set_flag(meta, index, ZRAM_ZERO);
		ret = 0;
		goto out;
	}

<<<<<<< HEAD
	ret = lzo1x_1_compress(uncmem, PAGE_SIZE, src, &clen,
=======
	ret = zram_compress(uncmem, PAGE_SIZE, src, &clen,
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
			       meta->compress_workmem);

	if (!is_partial_io(bvec)) {
		kunmap_atomic(user_mem);
		user_mem = NULL;
		uncmem = NULL;
	}

	if (unlikely(ret != LZO_E_OK)) {
<<<<<<< HEAD
		pr_err("Compression failed! err=%d\n", ret);
		goto out;
=======
#ifdef CONFIG_LZO_HW_ALGO
		if(ret == LZO_HW_OUT_LEN_ERROR)
		{
			clen = PAGE_SIZE;
			ret =  LZO_E_OK;
		}
		else
#endif
		{
			pr_err("Compression failed! err=%d\n", ret);
			goto out;
		}
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	}

	if (unlikely(clen > max_zpage_size)) {
		zram->stats.bad_compress++;
		clen = PAGE_SIZE;
		src = NULL;
		if (is_partial_io(bvec))
			src = uncmem;
	}

	handle = zs_malloc(meta->mem_pool, clen);
	if (!handle) {
		pr_info("Error allocating memory for compressed "
			"page: %u, size=%zu\n", index, clen);
		ret = -ENOMEM;
		goto out;
	}
	cmem = zs_map_object(meta->mem_pool, handle, ZS_MM_WO);

	if ((clen == PAGE_SIZE) && !is_partial_io(bvec))
		src = kmap_atomic(page);
	memcpy(cmem, src, clen);
	if ((clen == PAGE_SIZE) && !is_partial_io(bvec))
		kunmap_atomic(src);

	zs_unmap_object(meta->mem_pool, handle);

	meta->table[index].handle = handle;
	meta->table[index].size = clen;

	/* Update stats */
	zram_stat64_add(zram, &zram->stats.compr_size, clen);
	zram->stats.pages_stored++;
	if (clen <= PAGE_SIZE / 2)
		zram->stats.good_compress++;

out:
	if (is_partial_io(bvec))
		kfree(uncmem);

	if (ret)
		zram_stat64_inc(zram, &zram->stats.failed_writes);
	return ret;
}

static int zram_bvec_rw(struct zram *zram, struct bio_vec *bvec, u32 index,
			int offset, struct bio *bio, int rw)
{
	int ret;

	if (rw == READ) {
		down_read(&zram->lock);
		ret = zram_bvec_read(zram, bvec, index, offset, bio);
		up_read(&zram->lock);
	} else {
		down_write(&zram->lock);
		ret = zram_bvec_write(zram, bvec, index, offset);
		up_write(&zram->lock);
	}

	return ret;
}

static void update_position(u32 *index, int *offset, struct bio_vec *bvec)
{
	if (*offset + bvec->bv_len >= PAGE_SIZE)
		(*index)++;
	*offset = (*offset + bvec->bv_len) % PAGE_SIZE;
}

static void __zram_make_request(struct zram *zram, struct bio *bio, int rw)
{
	int i, offset;
	u32 index;
	struct bio_vec *bvec;

	switch (rw) {
	case READ:
		zram_stat64_inc(zram, &zram->stats.num_reads);
		break;
	case WRITE:
		zram_stat64_inc(zram, &zram->stats.num_writes);
		break;
	}

	index = bio->bi_sector >> SECTORS_PER_PAGE_SHIFT;
	offset = (bio->bi_sector & (SECTORS_PER_PAGE - 1)) << SECTOR_SHIFT;

	bio_for_each_segment(bvec, bio, i) {
		int max_transfer_size = PAGE_SIZE - offset;

		if (bvec->bv_len > max_transfer_size) {
			/*
			 * zram_bvec_rw() can only make operation on a single
			 * zram page. Split the bio vector.
			 */
			struct bio_vec bv;

			bv.bv_page = bvec->bv_page;
			bv.bv_len = max_transfer_size;
			bv.bv_offset = bvec->bv_offset;

			if (zram_bvec_rw(zram, &bv, index, offset, bio, rw) < 0)
				goto out;

			bv.bv_len = bvec->bv_len - max_transfer_size;
			bv.bv_offset += max_transfer_size;
			if (zram_bvec_rw(zram, &bv, index+1, 0, bio, rw) < 0)
				goto out;
		} else
			if (zram_bvec_rw(zram, bvec, index, offset, bio, rw)
			    < 0)
				goto out;

		update_position(&index, &offset, bvec);
	}

	set_bit(BIO_UPTODATE, &bio->bi_flags);
	bio_endio(bio, 0);
	return;

out:
	bio_io_error(bio);
}

/*
 * Check if request is within bounds and aligned on zram logical blocks.
 */
static inline int valid_io_request(struct zram *zram, struct bio *bio)
{
	u64 start, end, bound;

	/* unaligned request */
	if (unlikely(bio->bi_sector & (ZRAM_SECTOR_PER_LOGICAL_BLOCK - 1)))
		return 0;
	if (unlikely(bio->bi_size & (ZRAM_LOGICAL_BLOCK_SIZE - 1)))
		return 0;

	start = bio->bi_sector;
	end = start + (bio->bi_size >> SECTOR_SHIFT);
	bound = zram->disksize >> SECTOR_SHIFT;
	/* out of range range */
	if (unlikely(start >= bound || end > bound || start > end))
		return 0;

	/* I/O request is valid */
	return 1;
}

/*
 * Handler function for all zram I/O requests.
 */
static void zram_make_request(struct request_queue *queue, struct bio *bio)
{
	struct zram *zram = queue->queuedata;

	down_read(&zram->init_lock);
	if (unlikely(!zram->init_done))
		goto error;

	if (!valid_io_request(zram, bio)) {
		zram_stat64_inc(zram, &zram->stats.invalid_io);
		goto error;
	}

	__zram_make_request(zram, bio, bio_data_dir(bio));
	up_read(&zram->init_lock);

	return;

error:
	up_read(&zram->init_lock);
	bio_io_error(bio);
}

static void __zram_reset_device(struct zram *zram)
{
	size_t index;
	struct zram_meta *meta;

	if (!zram->init_done)
		return;

	meta = zram->meta;
	zram->init_done = 0;

	/* Free all pages that are still in this zram device */
	for (index = 0; index < zram->disksize >> PAGE_SHIFT; index++) {
		unsigned long handle = meta->table[index].handle;
		if (!handle)
			continue;

		zs_free(meta->mem_pool, handle);
	}

	zram_meta_free(zram->meta);
	zram->meta = NULL;
	/* Reset stats */
	memset(&zram->stats, 0, sizeof(zram->stats));

	zram->disksize = 0;
	set_capacity(zram->disk, 0);
}

void zram_reset_device(struct zram *zram)
{
	down_write(&zram->init_lock);
	__zram_reset_device(zram);
	up_write(&zram->init_lock);
}

void zram_meta_free(struct zram_meta *meta)
{
	zs_destroy_pool(meta->mem_pool);
	kfree(meta->compress_workmem);
	free_pages((unsigned long)meta->compress_buffer, 1);
	vfree(meta->table);
	kfree(meta);
}

struct zram_meta *zram_meta_alloc(u64 disksize)
{
	size_t num_pages;
	struct zram_meta *meta = kmalloc(sizeof(*meta), GFP_KERNEL);
	if (!meta)
		goto out;

	meta->compress_workmem = kzalloc(LZO1X_MEM_COMPRESS, GFP_KERNEL);
	if (!meta->compress_workmem)
		goto free_meta;

	meta->compress_buffer =
		(void *)__get_free_pages(GFP_KERNEL | __GFP_ZERO, 1);
	if (!meta->compress_buffer) {
		pr_err("Error allocating compressor buffer space\n");
		goto free_workmem;
	}

	num_pages = disksize >> PAGE_SHIFT;
	meta->table = vzalloc(num_pages * sizeof(*meta->table));
	if (!meta->table) {
		pr_err("Error allocating zram address table\n");
		goto free_buffer;
	}

<<<<<<< HEAD
	meta->mem_pool = zs_create_pool(GFP_NOIO | __GFP_HIGHMEM);
=======
	meta->mem_pool = zs_create_pool(GFP_NOIO | __GFP_HIGHMEM, NULL);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	if (!meta->mem_pool) {
		pr_err("Error creating memory pool\n");
		goto free_table;
	}

	return meta;

free_table:
	vfree(meta->table);
free_buffer:
	free_pages((unsigned long)meta->compress_buffer, 1);
free_workmem:
	kfree(meta->compress_workmem);
free_meta:
	kfree(meta);
	meta = NULL;
out:
	return meta;
}

void zram_init_device(struct zram *zram, struct zram_meta *meta)
{
	if (zram->disksize > 2 * (totalram_pages << PAGE_SHIFT)) {
		pr_info(
		"There is little point creating a zram of greater than "
		"twice the size of memory since we expect a 2:1 compression "
		"ratio. Note that zram uses about 0.1%% of the size of "
		"the disk when not in use so a huge zram is "
		"wasteful.\n"
		"\tMemory Size: %lu kB\n"
		"\tSize you selected: %llu kB\n"
		"Continuing anyway ...\n",
		(totalram_pages << PAGE_SHIFT) >> 10, zram->disksize >> 10
		);
	}

	/* zram devices sort of resembles non-rotational disks */
	queue_flag_set_unlocked(QUEUE_FLAG_NONROT, zram->disk->queue);

	zram->meta = meta;
	zram->init_done = 1;

	pr_debug("Initialization done!\n");
}

static void zram_slot_free_notify(struct block_device *bdev,
				unsigned long index)
{
	struct zram *zram;

	zram = bdev->bd_disk->private_data;
<<<<<<< HEAD
	down_write(&zram->lock);
	zram_free_page(zram, index);
	up_write(&zram->lock);
=======
	zram_free_page(zram, index);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	zram_stat64_inc(zram, &zram->stats.notify_free);
}

static const struct block_device_operations zram_devops = {
	.swap_slot_free_notify = zram_slot_free_notify,
	.owner = THIS_MODULE
};

static int create_device(struct zram *zram, int device_id)
{
	int ret = -ENOMEM;

	init_rwsem(&zram->lock);
	init_rwsem(&zram->init_lock);
	spin_lock_init(&zram->stat64_lock);

	zram->queue = blk_alloc_queue(GFP_KERNEL);
	if (!zram->queue) {
		pr_err("Error allocating disk queue for device %d\n",
			device_id);
		goto out;
	}

	blk_queue_make_request(zram->queue, zram_make_request);
	zram->queue->queuedata = zram;

	 /* gendisk structure */
	zram->disk = alloc_disk(1);
	if (!zram->disk) {
		pr_warn("Error allocating disk structure for device %d\n",
			device_id);
		goto out_free_queue;
	}

	zram->disk->major = zram_major;
	zram->disk->first_minor = device_id;
	zram->disk->fops = &zram_devops;
	zram->disk->queue = zram->queue;
	zram->disk->private_data = zram;
	snprintf(zram->disk->disk_name, 16, "zram%d", device_id);

	/* Actual capacity set using syfs (/sys/block/zram<id>/disksize */
	set_capacity(zram->disk, 0);

	/*
	 * To ensure that we always get PAGE_SIZE aligned
	 * and n*PAGE_SIZED sized I/O requests.
	 */
	blk_queue_physical_block_size(zram->disk->queue, PAGE_SIZE);
	blk_queue_logical_block_size(zram->disk->queue,
					ZRAM_LOGICAL_BLOCK_SIZE);
	blk_queue_io_min(zram->disk->queue, PAGE_SIZE);
	blk_queue_io_opt(zram->disk->queue, PAGE_SIZE);

	add_disk(zram->disk);

	ret = sysfs_create_group(&disk_to_dev(zram->disk)->kobj,
				&zram_disk_attr_group);
	if (ret < 0) {
		pr_warn("Error creating sysfs group");
		goto out_free_disk;
	}

	zram->init_done = 0;
	return 0;

out_free_disk:
	del_gendisk(zram->disk);
	put_disk(zram->disk);
out_free_queue:
	blk_cleanup_queue(zram->queue);
out:
	return ret;
}

static void destroy_device(struct zram *zram)
{
	sysfs_remove_group(&disk_to_dev(zram->disk)->kobj,
			&zram_disk_attr_group);

	if (zram->disk) {
		del_gendisk(zram->disk);
		put_disk(zram->disk);
	}

	if (zram->queue)
		blk_cleanup_queue(zram->queue);
}

unsigned int zram_get_num_devices(void)
{
	return num_devices;
}

static int __init zram_init(void)
{
	int ret, dev_id;

	if (num_devices > max_num_devices) {
		pr_warn("Invalid value for num_devices: %u\n",
				num_devices);
		ret = -EINVAL;
		goto out;
	}

	zram_major = register_blkdev(0, "zram");
	if (zram_major <= 0) {
		pr_warn("Unable to get major number\n");
		ret = -EBUSY;
		goto out;
	}

	/* Allocate the device array and initialize each one */
	zram_devices = kzalloc(num_devices * sizeof(struct zram), GFP_KERNEL);
	if (!zram_devices) {
		ret = -ENOMEM;
		goto unregister;
	}

	for (dev_id = 0; dev_id < num_devices; dev_id++) {
		ret = create_device(&zram_devices[dev_id], dev_id);
		if (ret)
			goto free_devices;
	}
<<<<<<< HEAD

=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	pr_info("Created %u device(s) ...\n", num_devices);

	return 0;

free_devices:
	while (dev_id)
		destroy_device(&zram_devices[--dev_id]);
	kfree(zram_devices);
unregister:
	unregister_blkdev(zram_major, "zram");
out:
	return ret;
}

static void __exit zram_exit(void)
{
	int i;
	struct zram *zram;

	for (i = 0; i < num_devices; i++) {
		zram = &zram_devices[i];

		get_disk(zram->disk);
		destroy_device(zram);
		zram_reset_device(zram);
		put_disk(zram->disk);
	}

	unregister_blkdev(zram_major, "zram");

	kfree(zram_devices);
	pr_debug("Cleanup done!\n");
}

module_param(num_devices, uint, 0);
MODULE_PARM_DESC(num_devices, "Number of zram devices");

module_init(zram_init);
module_exit(zram_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Nitin Gupta <ngupta@vflare.org>");
MODULE_DESCRIPTION("Compressed RAM Block Device");

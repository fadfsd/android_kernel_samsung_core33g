/*
 * fs/f2fs/checkpoint.c
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *             http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/fs.h>
#include <linux/bio.h>
#include <linux/mpage.h>
#include <linux/writeback.h>
#include <linux/blkdev.h>
#include <linux/f2fs_fs.h>
#include <linux/pagevec.h>
#include <linux/swap.h>

#include "f2fs.h"
#include "node.h"
#include "segment.h"
<<<<<<< HEAD
#include "trace.h"
#include <trace/events/f2fs.h>

static struct kmem_cache *ino_entry_slab;
struct kmem_cache *inode_entry_slab;

void f2fs_stop_checkpoint(struct f2fs_sb_info *sbi, bool end_io)
{
	set_ckpt_flags(sbi, CP_ERROR_FLAG);
	sbi->sb->s_flags |= MS_RDONLY;
	if (!end_io)
		f2fs_flush_merged_bios(sbi);
}
=======
#include <trace/events/f2fs.h>

static struct kmem_cache *orphan_entry_slab;
static struct kmem_cache *inode_entry_slab;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

/*
 * We guarantee no failure on the returned page.
 */
struct page *grab_meta_page(struct f2fs_sb_info *sbi, pgoff_t index)
{
<<<<<<< HEAD
	struct address_space *mapping = META_MAPPING(sbi);
	struct page *page = NULL;
repeat:
	page = f2fs_grab_cache_page(mapping, index, false);
=======
	struct address_space *mapping = sbi->meta_inode->i_mapping;
	struct page *page = NULL;
repeat:
	page = grab_cache_page(mapping, index);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	if (!page) {
		cond_resched();
		goto repeat;
	}
<<<<<<< HEAD
	f2fs_wait_on_page_writeback(page, META, true);
	if (!PageUptodate(page))
		SetPageUptodate(page);
=======

	/* We wait writeback only inside grab_meta_page() */
	wait_on_page_writeback(page);
	SetPageUptodate(page);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	return page;
}

/*
 * We guarantee no failure on the returned page.
 */
<<<<<<< HEAD
static struct page *__get_meta_page(struct f2fs_sb_info *sbi, pgoff_t index,
							bool is_meta)
{
	struct address_space *mapping = META_MAPPING(sbi);
	struct page *page;
	struct f2fs_io_info fio = {
		.sbi = sbi,
		.type = META,
		.rw = READ_SYNC | REQ_META | REQ_PRIO,
		.old_blkaddr = index,
		.new_blkaddr = index,
		.encrypted_page = NULL,
	};

	if (unlikely(!is_meta))
		fio.rw &= ~REQ_META;
repeat:
	page = f2fs_grab_cache_page(mapping, index, false);
=======
struct page *get_meta_page(struct f2fs_sb_info *sbi, pgoff_t index)
{
	struct address_space *mapping = sbi->meta_inode->i_mapping;
	struct page *page;
repeat:
	page = grab_cache_page(mapping, index);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	if (!page) {
		cond_resched();
		goto repeat;
	}
	if (PageUptodate(page))
		goto out;

<<<<<<< HEAD
	fio.page = page;

	if (f2fs_submit_page_bio(&fio)) {
		f2fs_put_page(page, 1);
		goto repeat;
	}

	lock_page(page);
	if (unlikely(page->mapping != mapping)) {
		f2fs_put_page(page, 1);
		goto repeat;
	}

	/*
	 * if there is any IO error when accessing device, make our filesystem
	 * readonly and make sure do not write checkpoint with non-uptodate
	 * meta page.
	 */
	if (unlikely(!PageUptodate(page)))
		f2fs_stop_checkpoint(sbi, false);
=======
	if (f2fs_readpage(sbi, page, index, READ_SYNC))
		goto repeat;

	lock_page(page);
	if (page->mapping != mapping) {
		f2fs_put_page(page, 1);
		goto repeat;
	}
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
out:
	mark_page_accessed(page);
	return page;
}

<<<<<<< HEAD
struct page *get_meta_page(struct f2fs_sb_info *sbi, pgoff_t index)
{
	return __get_meta_page(sbi, index, true);
}

/* for POR only */
struct page *get_tmp_page(struct f2fs_sb_info *sbi, pgoff_t index)
{
	return __get_meta_page(sbi, index, false);
}

bool is_valid_blkaddr(struct f2fs_sb_info *sbi, block_t blkaddr, int type)
{
	switch (type) {
	case META_NAT:
		break;
	case META_SIT:
		if (unlikely(blkaddr >= SIT_BLK_CNT(sbi)))
			return false;
		break;
	case META_SSA:
		if (unlikely(blkaddr >= MAIN_BLKADDR(sbi) ||
			blkaddr < SM_I(sbi)->ssa_blkaddr))
			return false;
		break;
	case META_CP:
		if (unlikely(blkaddr >= SIT_I(sbi)->sit_base_addr ||
			blkaddr < __start_cp_addr(sbi)))
			return false;
		break;
	case META_POR:
		if (unlikely(blkaddr >= MAX_BLKADDR(sbi) ||
			blkaddr < MAIN_BLKADDR(sbi)))
			return false;
		break;
	default:
		BUG();
	}

	return true;
}

/*
 * Readahead CP/NAT/SIT/SSA pages
 */
int ra_meta_pages(struct f2fs_sb_info *sbi, block_t start, int nrpages,
							int type, bool sync)
{
	struct page *page;
	block_t blkno = start;
	struct f2fs_io_info fio = {
		.sbi = sbi,
		.type = META,
		.rw = sync ? (READ_SYNC | REQ_META | REQ_PRIO) : READA,
		.encrypted_page = NULL,
	};
	struct blk_plug plug;

	if (unlikely(type == META_POR))
		fio.rw &= ~REQ_META;

	blk_start_plug(&plug);
	for (; nrpages-- > 0; blkno++) {

		if (!is_valid_blkaddr(sbi, blkno, type))
			goto out;

		switch (type) {
		case META_NAT:
			if (unlikely(blkno >=
					NAT_BLOCK_OFFSET(NM_I(sbi)->max_nid)))
				blkno = 0;
			/* get nat block addr */
			fio.new_blkaddr = current_nat_addr(sbi,
					blkno * NAT_ENTRY_PER_BLOCK);
			break;
		case META_SIT:
			/* get sit block addr */
			fio.new_blkaddr = current_sit_addr(sbi,
					blkno * SIT_ENTRY_PER_BLOCK);
			break;
		case META_SSA:
		case META_CP:
		case META_POR:
			fio.new_blkaddr = blkno;
			break;
		default:
			BUG();
		}

		page = f2fs_grab_cache_page(META_MAPPING(sbi),
						fio.new_blkaddr, false);
		if (!page)
			continue;
		if (PageUptodate(page)) {
			f2fs_put_page(page, 1);
			continue;
		}

		fio.page = page;
		fio.old_blkaddr = fio.new_blkaddr;
		f2fs_submit_page_mbio(&fio);
		f2fs_put_page(page, 0);
	}
out:
	f2fs_submit_merged_bio(sbi, META, READ);
	blk_finish_plug(&plug);
	return blkno - start;
}

void ra_meta_pages_cond(struct f2fs_sb_info *sbi, pgoff_t index)
{
	struct page *page;
	bool readahead = false;

	page = find_get_page(META_MAPPING(sbi), index);
	if (!page || !PageUptodate(page))
		readahead = true;
	f2fs_put_page(page, 0);

	if (readahead)
		ra_meta_pages(sbi, index, BIO_MAX_PAGES, META_POR, true);
}

static int f2fs_write_meta_page(struct page *page,
				struct writeback_control *wbc)
{
	struct f2fs_sb_info *sbi = F2FS_P_SB(page);

	trace_f2fs_writepage(page, META);

	if (unlikely(is_sbi_flag_set(sbi, SBI_POR_DOING)))
		goto redirty_out;
	if (wbc->for_reclaim && page->index < GET_SUM_BLOCK(sbi, 0))
		goto redirty_out;
	if (unlikely(f2fs_cp_error(sbi)))
		goto redirty_out;

	write_meta_page(sbi, page);
	dec_page_count(sbi, F2FS_DIRTY_META);

	if (wbc->for_reclaim)
		f2fs_submit_merged_bio_cond(sbi, NULL, page, 0, META, WRITE);

	unlock_page(page);

	if (unlikely(f2fs_cp_error(sbi)))
		f2fs_submit_merged_bio(sbi, META, WRITE);

	return 0;

redirty_out:
	redirty_page_for_writepage(wbc, page);
	return AOP_WRITEPAGE_ACTIVATE;
=======
static int f2fs_write_meta_page(struct page *page,
				struct writeback_control *wbc)
{
	struct inode *inode = page->mapping->host;
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);

	/* Should not write any meta pages, if any IO error was occurred */
	if (wbc->for_reclaim ||
			is_set_ckpt_flags(F2FS_CKPT(sbi), CP_ERROR_FLAG)) {
		dec_page_count(sbi, F2FS_DIRTY_META);
		wbc->pages_skipped++;
		set_page_dirty(page);
		return AOP_WRITEPAGE_ACTIVATE;
	}

	wait_on_page_writeback(page);

	write_meta_page(sbi, page);
	dec_page_count(sbi, F2FS_DIRTY_META);
	unlock_page(page);
	return 0;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

static int f2fs_write_meta_pages(struct address_space *mapping,
				struct writeback_control *wbc)
{
<<<<<<< HEAD
	struct f2fs_sb_info *sbi = F2FS_M_SB(mapping);
	long diff, written;

	/* collect a number of dirty meta pages and write together */
	if (wbc->for_kupdate ||
		get_pages(sbi, F2FS_DIRTY_META) < nr_pages_to_skip(sbi, META))
		goto skip_write;

	trace_f2fs_writepages(mapping->host, wbc, META);

	/* if mounting is failed, skip writing node pages */
	mutex_lock(&sbi->cp_mutex);
	diff = nr_pages_to_write(sbi, META, wbc);
	written = sync_meta_pages(sbi, META, wbc->nr_to_write);
	mutex_unlock(&sbi->cp_mutex);
	wbc->nr_to_write = max((long)0, wbc->nr_to_write - written - diff);
	return 0;

skip_write:
	wbc->pages_skipped += get_pages(sbi, F2FS_DIRTY_META);
	trace_f2fs_writepages(mapping->host, wbc, META);
=======
	struct f2fs_sb_info *sbi = F2FS_SB(mapping->host->i_sb);
	struct block_device *bdev = sbi->sb->s_bdev;
	long written;

	if (wbc->for_kupdate)
		return 0;

	if (get_pages(sbi, F2FS_DIRTY_META) == 0)
		return 0;

	/* if mounting is failed, skip writing node pages */
	mutex_lock(&sbi->cp_mutex);
	written = sync_meta_pages(sbi, META, bio_get_nr_vecs(bdev));
	mutex_unlock(&sbi->cp_mutex);
	wbc->nr_to_write -= written;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	return 0;
}

long sync_meta_pages(struct f2fs_sb_info *sbi, enum page_type type,
						long nr_to_write)
{
<<<<<<< HEAD
	struct address_space *mapping = META_MAPPING(sbi);
	pgoff_t index = 0, end = ULONG_MAX, prev = ULONG_MAX;
=======
	struct address_space *mapping = sbi->meta_inode->i_mapping;
	pgoff_t index = 0, end = LONG_MAX;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	struct pagevec pvec;
	long nwritten = 0;
	struct writeback_control wbc = {
		.for_reclaim = 0,
	};
<<<<<<< HEAD
	struct blk_plug plug;

	pagevec_init(&pvec, 0);

	blk_start_plug(&plug);

=======

	pagevec_init(&pvec, 0);

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	while (index <= end) {
		int i, nr_pages;
		nr_pages = pagevec_lookup_tag(&pvec, mapping, &index,
				PAGECACHE_TAG_DIRTY,
				min(end - index, (pgoff_t)PAGEVEC_SIZE-1) + 1);
<<<<<<< HEAD
		if (unlikely(nr_pages == 0))
=======
		if (nr_pages == 0)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
			break;

		for (i = 0; i < nr_pages; i++) {
			struct page *page = pvec.pages[i];
<<<<<<< HEAD

			if (prev == ULONG_MAX)
				prev = page->index - 1;
			if (nr_to_write != LONG_MAX && page->index != prev + 1) {
				pagevec_release(&pvec);
				goto stop;
			}

			lock_page(page);

			if (unlikely(page->mapping != mapping)) {
continue_unlock:
				unlock_page(page);
				continue;
			}
			if (!PageDirty(page)) {
				/* someone wrote it for us */
				goto continue_unlock;
			}

			f2fs_wait_on_page_writeback(page, META, true);

			BUG_ON(PageWriteback(page));
			if (!clear_page_dirty_for_io(page))
				goto continue_unlock;

			if (mapping->a_ops->writepage(page, &wbc)) {
				unlock_page(page);
				break;
			}
			nwritten++;
			prev = page->index;
			if (unlikely(nwritten >= nr_to_write))
=======
			lock_page(page);
			BUG_ON(page->mapping != mapping);
			BUG_ON(!PageDirty(page));
			clear_page_dirty_for_io(page);
			if (f2fs_write_meta_page(page, &wbc)) {
				unlock_page(page);
				break;
			}
			if (nwritten++ >= nr_to_write)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
				break;
		}
		pagevec_release(&pvec);
		cond_resched();
	}
<<<<<<< HEAD
stop:
	if (nwritten)
		f2fs_submit_merged_bio(sbi, type, WRITE);

	blk_finish_plug(&plug);
=======

	if (nwritten)
		f2fs_submit_bio(sbi, type, nr_to_write == LONG_MAX);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	return nwritten;
}

static int f2fs_set_meta_page_dirty(struct page *page)
{
<<<<<<< HEAD
	trace_f2fs_set_page_dirty(page, META);

	if (!PageUptodate(page))
		SetPageUptodate(page);
	if (!PageDirty(page)) {
		f2fs_set_page_dirty_nobuffers(page);
		inc_page_count(F2FS_P_SB(page), F2FS_DIRTY_META);
		SetPagePrivate(page);
		f2fs_trace_pid(page);
=======
	struct address_space *mapping = page->mapping;
	struct f2fs_sb_info *sbi = F2FS_SB(mapping->host->i_sb);

	SetPageUptodate(page);
	if (!PageDirty(page)) {
		__set_page_dirty_nobuffers(page);
		inc_page_count(sbi, F2FS_DIRTY_META);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		return 1;
	}
	return 0;
}

const struct address_space_operations f2fs_meta_aops = {
	.writepage	= f2fs_write_meta_page,
	.writepages	= f2fs_write_meta_pages,
	.set_page_dirty	= f2fs_set_meta_page_dirty,
<<<<<<< HEAD
	.invalidatepage = f2fs_invalidate_page,
	.releasepage	= f2fs_release_page,
};

static void __add_ino_entry(struct f2fs_sb_info *sbi, nid_t ino, int type)
{
	struct inode_management *im = &sbi->im[type];
	struct ino_entry *e, *tmp;

	tmp = f2fs_kmem_cache_alloc(ino_entry_slab, GFP_NOFS);
retry:
	radix_tree_preload(GFP_NOFS | __GFP_NOFAIL);

	spin_lock(&im->ino_lock);
	e = radix_tree_lookup(&im->ino_root, ino);
	if (!e) {
		e = tmp;
		if (radix_tree_insert(&im->ino_root, ino, e)) {
			spin_unlock(&im->ino_lock);
			radix_tree_preload_end();
			goto retry;
		}
		memset(e, 0, sizeof(struct ino_entry));
		e->ino = ino;

		list_add_tail(&e->list, &im->ino_list);
		if (type != ORPHAN_INO)
			im->ino_num++;
	}
	spin_unlock(&im->ino_lock);
	radix_tree_preload_end();

	if (e != tmp)
		kmem_cache_free(ino_entry_slab, tmp);
}

static void __remove_ino_entry(struct f2fs_sb_info *sbi, nid_t ino, int type)
{
	struct inode_management *im = &sbi->im[type];
	struct ino_entry *e;

	spin_lock(&im->ino_lock);
	e = radix_tree_lookup(&im->ino_root, ino);
	if (e) {
		list_del(&e->list);
		radix_tree_delete(&im->ino_root, ino);
		im->ino_num--;
		spin_unlock(&im->ino_lock);
		kmem_cache_free(ino_entry_slab, e);
		return;
	}
	spin_unlock(&im->ino_lock);
}

void add_ino_entry(struct f2fs_sb_info *sbi, nid_t ino, int type)
{
	/* add new dirty ino entry into list */
	__add_ino_entry(sbi, ino, type);
}

void remove_ino_entry(struct f2fs_sb_info *sbi, nid_t ino, int type)
{
	/* remove dirty ino entry from list */
	__remove_ino_entry(sbi, ino, type);
}

/* mode should be APPEND_INO or UPDATE_INO */
bool exist_written_data(struct f2fs_sb_info *sbi, nid_t ino, int mode)
{
	struct inode_management *im = &sbi->im[mode];
	struct ino_entry *e;

	spin_lock(&im->ino_lock);
	e = radix_tree_lookup(&im->ino_root, ino);
	spin_unlock(&im->ino_lock);
	return e ? true : false;
}

void release_ino_entry(struct f2fs_sb_info *sbi, bool all)
{
	struct ino_entry *e, *tmp;
	int i;

	for (i = all ? ORPHAN_INO: APPEND_INO; i <= UPDATE_INO; i++) {
		struct inode_management *im = &sbi->im[i];

		spin_lock(&im->ino_lock);
		list_for_each_entry_safe(e, tmp, &im->ino_list, list) {
			list_del(&e->list);
			radix_tree_delete(&im->ino_root, e->ino);
			kmem_cache_free(ino_entry_slab, e);
			im->ino_num--;
		}
		spin_unlock(&im->ino_lock);
	}
}

int acquire_orphan_inode(struct f2fs_sb_info *sbi)
{
	struct inode_management *im = &sbi->im[ORPHAN_INO];
	int err = 0;

	spin_lock(&im->ino_lock);

#ifdef CONFIG_F2FS_FAULT_INJECTION
	if (time_to_inject(sbi, FAULT_ORPHAN)) {
		spin_unlock(&im->ino_lock);
		return -ENOSPC;
	}
#endif
	if (unlikely(im->ino_num >= sbi->max_orphans))
		err = -ENOSPC;
	else
		im->ino_num++;
	spin_unlock(&im->ino_lock);

	return err;
}

void release_orphan_inode(struct f2fs_sb_info *sbi)
{
	struct inode_management *im = &sbi->im[ORPHAN_INO];

	spin_lock(&im->ino_lock);
	f2fs_bug_on(sbi, im->ino_num == 0);
	im->ino_num--;
	spin_unlock(&im->ino_lock);
}

void add_orphan_inode(struct inode *inode)
{
	/* add new orphan ino entry into list */
	__add_ino_entry(F2FS_I_SB(inode), inode->i_ino, ORPHAN_INO);
	update_inode_page(inode);
=======
};

int check_orphan_space(struct f2fs_sb_info *sbi)
{
	unsigned int max_orphans;
	int err = 0;

	/*
	 * considering 512 blocks in a segment 5 blocks are needed for cp
	 * and log segment summaries. Remaining blocks are used to keep
	 * orphan entries with the limitation one reserved segment
	 * for cp pack we can have max 1020*507 orphan entries
	 */
	max_orphans = (sbi->blocks_per_seg - 5) * F2FS_ORPHANS_PER_BLOCK;
	mutex_lock(&sbi->orphan_inode_mutex);
	if (sbi->n_orphans >= max_orphans)
		err = -ENOSPC;
	mutex_unlock(&sbi->orphan_inode_mutex);
	return err;
}

void add_orphan_inode(struct f2fs_sb_info *sbi, nid_t ino)
{
	struct list_head *head, *this;
	struct orphan_inode_entry *new = NULL, *orphan = NULL;

	mutex_lock(&sbi->orphan_inode_mutex);
	head = &sbi->orphan_inode_list;
	list_for_each(this, head) {
		orphan = list_entry(this, struct orphan_inode_entry, list);
		if (orphan->ino == ino)
			goto out;
		if (orphan->ino > ino)
			break;
		orphan = NULL;
	}
retry:
	new = kmem_cache_alloc(orphan_entry_slab, GFP_ATOMIC);
	if (!new) {
		cond_resched();
		goto retry;
	}
	new->ino = ino;

	/* add new_oentry into list which is sorted by inode number */
	if (orphan)
		list_add(&new->list, this->prev);
	else
		list_add_tail(&new->list, head);

	sbi->n_orphans++;
out:
	mutex_unlock(&sbi->orphan_inode_mutex);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

void remove_orphan_inode(struct f2fs_sb_info *sbi, nid_t ino)
{
<<<<<<< HEAD
	/* remove orphan entry from orphan list */
	__remove_ino_entry(sbi, ino, ORPHAN_INO);
}

static int recover_orphan_inode(struct f2fs_sb_info *sbi, nid_t ino)
{
	struct inode *inode;
	struct node_info ni;
	int err = acquire_orphan_inode(sbi);

	if (err) {
		set_sbi_flag(sbi, SBI_NEED_FSCK);
		f2fs_msg(sbi->sb, KERN_WARNING,
				"%s: orphan failed (ino=%x), run fsck to fix.",
				__func__, ino);
		return err;
	}

	__add_ino_entry(sbi, ino, ORPHAN_INO);

	inode = f2fs_iget_retry(sbi->sb, ino);
	if (IS_ERR(inode)) {
		/*
		 * there should be a bug that we can't find the entry
		 * to orphan inode.
		 */
		f2fs_bug_on(sbi, PTR_ERR(inode) == -ENOENT);
		return PTR_ERR(inode);
	}

=======
	struct list_head *this, *next, *head;
	struct orphan_inode_entry *orphan;

	mutex_lock(&sbi->orphan_inode_mutex);
	head = &sbi->orphan_inode_list;
	list_for_each_safe(this, next, head) {
		orphan = list_entry(this, struct orphan_inode_entry, list);
		if (orphan->ino == ino) {
			list_del(&orphan->list);
			kmem_cache_free(orphan_entry_slab, orphan);
			sbi->n_orphans--;
			break;
		}
	}
	mutex_unlock(&sbi->orphan_inode_mutex);
}

static void recover_orphan_inode(struct f2fs_sb_info *sbi, nid_t ino)
{
	struct inode *inode = f2fs_iget(sbi->sb, ino);
	BUG_ON(IS_ERR(inode));
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	clear_nlink(inode);

	/* truncate all the data during iput */
	iput(inode);
<<<<<<< HEAD

	get_node_info(sbi, ino, &ni);

	/* ENOMEM was fully retried in f2fs_evict_inode. */
	if (ni.blk_addr != NULL_ADDR) {
		set_sbi_flag(sbi, SBI_NEED_FSCK);
		f2fs_msg(sbi->sb, KERN_WARNING,
			"%s: orphan failed (ino=%x), run fsck to fix.",
				__func__, ino);
		return -EIO;
	}
	__remove_ino_entry(sbi, ino, ORPHAN_INO);
	return 0;
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

int recover_orphan_inodes(struct f2fs_sb_info *sbi)
{
<<<<<<< HEAD
	block_t start_blk, orphan_blocks, i, j;
	int err;

	if (!is_set_ckpt_flags(sbi, CP_ORPHAN_PRESENT_FLAG))
		return 0;

	start_blk = __start_cp_addr(sbi) + 1 + __cp_payload(sbi);
	orphan_blocks = __start_sum_addr(sbi) - 1 - __cp_payload(sbi);

	ra_meta_pages(sbi, start_blk, orphan_blocks, META_CP, true);

	for (i = 0; i < orphan_blocks; i++) {
=======
	block_t start_blk, orphan_blkaddr, i, j;

	if (!is_set_ckpt_flags(F2FS_CKPT(sbi), CP_ORPHAN_PRESENT_FLAG))
		return 0;

	sbi->por_doing = 1;
	start_blk = __start_cp_addr(sbi) + 1;
	orphan_blkaddr = __start_sum_addr(sbi) - 1;

	for (i = 0; i < orphan_blkaddr; i++) {
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		struct page *page = get_meta_page(sbi, start_blk + i);
		struct f2fs_orphan_block *orphan_blk;

		orphan_blk = (struct f2fs_orphan_block *)page_address(page);
		for (j = 0; j < le32_to_cpu(orphan_blk->entry_count); j++) {
			nid_t ino = le32_to_cpu(orphan_blk->ino[j]);
<<<<<<< HEAD
			err = recover_orphan_inode(sbi, ino);
			if (err) {
				f2fs_put_page(page, 1);
				return err;
			}
=======
			recover_orphan_inode(sbi, ino);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		}
		f2fs_put_page(page, 1);
	}
	/* clear Orphan Flag */
<<<<<<< HEAD
	clear_ckpt_flags(sbi, CP_ORPHAN_PRESENT_FLAG);
=======
	clear_ckpt_flags(F2FS_CKPT(sbi), CP_ORPHAN_PRESENT_FLAG);
	sbi->por_doing = 0;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	return 0;
}

static void write_orphan_inodes(struct f2fs_sb_info *sbi, block_t start_blk)
{
<<<<<<< HEAD
	struct list_head *head;
	struct f2fs_orphan_block *orphan_blk = NULL;
	unsigned int nentries = 0;
	unsigned short index = 1;
	unsigned short orphan_blocks;
	struct page *page = NULL;
	struct ino_entry *orphan = NULL;
	struct inode_management *im = &sbi->im[ORPHAN_INO];

	orphan_blocks = GET_ORPHAN_BLOCKS(im->ino_num);

	/*
	 * we don't need to do spin_lock(&im->ino_lock) here, since all the
	 * orphan inode operations are covered under f2fs_lock_op().
	 * And, spin_lock should be avoided due to page operations below.
	 */
	head = &im->ino_list;

	/* loop for each orphan inode entry and write them in Jornal block */
	list_for_each_entry(orphan, head, list) {
		if (!page) {
			page = grab_meta_page(sbi, start_blk++);
			orphan_blk =
				(struct f2fs_orphan_block *)page_address(page);
			memset(orphan_blk, 0, sizeof(*orphan_blk));
		}

		orphan_blk->ino[nentries++] = cpu_to_le32(orphan->ino);
=======
	struct list_head *head, *this, *next;
	struct f2fs_orphan_block *orphan_blk = NULL;
	struct page *page = NULL;
	unsigned int nentries = 0;
	unsigned short index = 1;
	unsigned short orphan_blocks;

	orphan_blocks = (unsigned short)((sbi->n_orphans +
		(F2FS_ORPHANS_PER_BLOCK - 1)) / F2FS_ORPHANS_PER_BLOCK);

	mutex_lock(&sbi->orphan_inode_mutex);
	head = &sbi->orphan_inode_list;

	/* loop for each orphan inode entry and write them in Jornal block */
	list_for_each_safe(this, next, head) {
		struct orphan_inode_entry *orphan;

		orphan = list_entry(this, struct orphan_inode_entry, list);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

		if (nentries == F2FS_ORPHANS_PER_BLOCK) {
			/*
			 * an orphan block is full of 1020 entries,
			 * then we need to flush current orphan blocks
			 * and bring another one in memory
			 */
			orphan_blk->blk_addr = cpu_to_le16(index);
			orphan_blk->blk_count = cpu_to_le16(orphan_blocks);
			orphan_blk->entry_count = cpu_to_le32(nentries);
			set_page_dirty(page);
			f2fs_put_page(page, 1);
			index++;
<<<<<<< HEAD
			nentries = 0;
			page = NULL;
		}
	}

	if (page) {
		orphan_blk->blk_addr = cpu_to_le16(index);
		orphan_blk->blk_count = cpu_to_le16(orphan_blocks);
		orphan_blk->entry_count = cpu_to_le32(nentries);
		set_page_dirty(page);
		f2fs_put_page(page, 1);
	}
}

static int get_checkpoint_version(struct f2fs_sb_info *sbi, block_t cp_addr,
		struct f2fs_checkpoint **cp_block, struct page **cp_page,
		unsigned long long *version)
{
	unsigned long blk_size = sbi->blocksize;
	size_t crc_offset = 0;
	__u32 crc = 0;

	*cp_page = get_meta_page(sbi, cp_addr);
	*cp_block = (struct f2fs_checkpoint *)page_address(*cp_page);

	crc_offset = le32_to_cpu((*cp_block)->checksum_offset);
	if (crc_offset >= blk_size) {
		f2fs_msg(sbi->sb, KERN_WARNING,
			"invalid crc_offset: %zu", crc_offset);
		return -EINVAL;
	}

	crc = le32_to_cpu(*((__le32 *)((unsigned char *)*cp_block
							+ crc_offset)));
	if (!f2fs_crc_valid(sbi, crc, *cp_block, crc_offset)) {
		f2fs_msg(sbi->sb, KERN_WARNING, "invalid crc value");
		return -EINVAL;
	}

	*version = cur_cp_version(*cp_block);
	return 0;
=======
			start_blk++;
			nentries = 0;
			page = NULL;
		}
		if (page)
			goto page_exist;

		page = grab_meta_page(sbi, start_blk);
		orphan_blk = (struct f2fs_orphan_block *)page_address(page);
		memset(orphan_blk, 0, sizeof(*orphan_blk));
page_exist:
		orphan_blk->ino[nentries++] = cpu_to_le32(orphan->ino);
	}
	if (!page)
		goto end;

	orphan_blk->blk_addr = cpu_to_le16(index);
	orphan_blk->blk_count = cpu_to_le16(orphan_blocks);
	orphan_blk->entry_count = cpu_to_le32(nentries);
	set_page_dirty(page);
	f2fs_put_page(page, 1);
end:
	mutex_unlock(&sbi->orphan_inode_mutex);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

static struct page *validate_checkpoint(struct f2fs_sb_info *sbi,
				block_t cp_addr, unsigned long long *version)
{
<<<<<<< HEAD
	struct page *cp_page_1 = NULL, *cp_page_2 = NULL;
	struct f2fs_checkpoint *cp_block = NULL;
	unsigned long long cur_version = 0, pre_version = 0;
	int err;

	err = get_checkpoint_version(sbi, cp_addr, &cp_block,
					&cp_page_1, version);
	if (err)
		goto invalid_cp1;
	pre_version = *version;

	cp_addr += le32_to_cpu(cp_block->cp_pack_total_block_count) - 1;
	err = get_checkpoint_version(sbi, cp_addr, &cp_block,
					&cp_page_2, version);
	if (err)
		goto invalid_cp2;
	cur_version = *version;
=======
	struct page *cp_page_1, *cp_page_2 = NULL;
	unsigned long blk_size = sbi->blocksize;
	struct f2fs_checkpoint *cp_block;
	unsigned long long cur_version = 0, pre_version = 0;
	unsigned int crc = 0;
	size_t crc_offset;

	/* Read the 1st cp block in this CP pack */
	cp_page_1 = get_meta_page(sbi, cp_addr);

	/* get the version number */
	cp_block = (struct f2fs_checkpoint *)page_address(cp_page_1);
	crc_offset = le32_to_cpu(cp_block->checksum_offset);
	if (crc_offset >= blk_size)
		goto invalid_cp1;

	crc = *(unsigned int *)((unsigned char *)cp_block + crc_offset);
	if (!f2fs_crc_valid(crc, cp_block, crc_offset))
		goto invalid_cp1;

	pre_version = le64_to_cpu(cp_block->checkpoint_ver);

	/* Read the 2nd cp block in this CP pack */
	cp_addr += le32_to_cpu(cp_block->cp_pack_total_block_count) - 1;
	cp_page_2 = get_meta_page(sbi, cp_addr);

	cp_block = (struct f2fs_checkpoint *)page_address(cp_page_2);
	crc_offset = le32_to_cpu(cp_block->checksum_offset);
	if (crc_offset >= blk_size)
		goto invalid_cp2;

	crc = *(unsigned int *)((unsigned char *)cp_block + crc_offset);
	if (!f2fs_crc_valid(crc, cp_block, crc_offset))
		goto invalid_cp2;

	cur_version = le64_to_cpu(cp_block->checkpoint_ver);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	if (cur_version == pre_version) {
		*version = cur_version;
		f2fs_put_page(cp_page_2, 1);
		return cp_page_1;
	}
invalid_cp2:
	f2fs_put_page(cp_page_2, 1);
invalid_cp1:
	f2fs_put_page(cp_page_1, 1);
	return NULL;
}

int get_valid_checkpoint(struct f2fs_sb_info *sbi)
{
	struct f2fs_checkpoint *cp_block;
	struct f2fs_super_block *fsb = sbi->raw_super;
	struct page *cp1, *cp2, *cur_page;
	unsigned long blk_size = sbi->blocksize;
	unsigned long long cp1_version = 0, cp2_version = 0;
	unsigned long long cp_start_blk_no;
<<<<<<< HEAD
	unsigned int cp_blks = 1 + __cp_payload(sbi);
	block_t cp_blk_no;
	int i;

	sbi->ckpt = kzalloc(cp_blks * blk_size, GFP_KERNEL);
=======

	sbi->ckpt = kzalloc(blk_size, GFP_KERNEL);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	if (!sbi->ckpt)
		return -ENOMEM;
	/*
	 * Finding out valid cp block involves read both
	 * sets( cp pack1 and cp pack 2)
	 */
	cp_start_blk_no = le32_to_cpu(fsb->cp_blkaddr);
	cp1 = validate_checkpoint(sbi, cp_start_blk_no, &cp1_version);

	/* The second checkpoint pack should start at the next segment */
<<<<<<< HEAD
	cp_start_blk_no += ((unsigned long long)1) <<
				le32_to_cpu(fsb->log_blocks_per_seg);
=======
	cp_start_blk_no += 1 << le32_to_cpu(fsb->log_blocks_per_seg);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	cp2 = validate_checkpoint(sbi, cp_start_blk_no, &cp2_version);

	if (cp1 && cp2) {
		if (ver_after(cp2_version, cp1_version))
			cur_page = cp2;
		else
			cur_page = cp1;
	} else if (cp1) {
		cur_page = cp1;
	} else if (cp2) {
		cur_page = cp2;
	} else {
		goto fail_no_cp;
	}

	cp_block = (struct f2fs_checkpoint *)page_address(cur_page);
	memcpy(sbi->ckpt, cp_block, blk_size);

<<<<<<< HEAD
	/* Sanity checking of checkpoint */
	if (sanity_check_ckpt(sbi))
		goto free_fail_no_cp;

	if (cur_page == cp1)
		sbi->cur_cp_pack = 1;
	else
		sbi->cur_cp_pack = 2;

	if (cp_blks <= 1)
		goto done;

	cp_blk_no = le32_to_cpu(fsb->cp_blkaddr);
	if (cur_page == cp2)
		cp_blk_no += 1 << le32_to_cpu(fsb->log_blocks_per_seg);

	for (i = 1; i < cp_blks; i++) {
		void *sit_bitmap_ptr;
		unsigned char *ckpt = (unsigned char *)sbi->ckpt;

		cur_page = get_meta_page(sbi, cp_blk_no + i);
		sit_bitmap_ptr = page_address(cur_page);
		memcpy(ckpt + i * blk_size, sit_bitmap_ptr, blk_size);
		f2fs_put_page(cur_page, 1);
	}
done:
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	f2fs_put_page(cp1, 1);
	f2fs_put_page(cp2, 1);
	return 0;

<<<<<<< HEAD
free_fail_no_cp:
	f2fs_put_page(cp1, 1);
	f2fs_put_page(cp2, 1);
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
fail_no_cp:
	kfree(sbi->ckpt);
	return -EINVAL;
}

<<<<<<< HEAD
static void __add_dirty_inode(struct inode *inode, enum inode_type type)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	int flag = (type == DIR_INODE) ? FI_DIRTY_DIR : FI_DIRTY_FILE;

	if (is_inode_flag_set(inode, flag))
		return;

	set_inode_flag(inode, flag);
	list_add_tail(&F2FS_I(inode)->dirty_list, &sbi->inode_list[type]);
	stat_inc_dirty_inode(sbi, type);
}

static void __remove_dirty_inode(struct inode *inode, enum inode_type type)
{
	int flag = (type == DIR_INODE) ? FI_DIRTY_DIR : FI_DIRTY_FILE;

	if (get_dirty_pages(inode) || !is_inode_flag_set(inode, flag))
		return;

	list_del_init(&F2FS_I(inode)->dirty_list);
	clear_inode_flag(inode, flag);
	stat_dec_dirty_inode(F2FS_I_SB(inode), type);
}

void update_dirty_page(struct inode *inode, struct page *page)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	enum inode_type type = S_ISDIR(inode->i_mode) ? DIR_INODE : FILE_INODE;

	if (!S_ISDIR(inode->i_mode) && !S_ISREG(inode->i_mode) &&
			!S_ISLNK(inode->i_mode))
		return;

	spin_lock(&sbi->inode_lock[type]);
	if (type != FILE_INODE || test_opt(sbi, DATA_FLUSH))
		__add_dirty_inode(inode, type);
	inode_inc_dirty_pages(inode);
	spin_unlock(&sbi->inode_lock[type]);

	SetPagePrivate(page);
	f2fs_trace_pid(page);
}

void remove_dirty_inode(struct inode *inode)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	enum inode_type type = S_ISDIR(inode->i_mode) ? DIR_INODE : FILE_INODE;

	if (!S_ISDIR(inode->i_mode) && !S_ISREG(inode->i_mode) &&
			!S_ISLNK(inode->i_mode))
		return;

	if (type == FILE_INODE && !test_opt(sbi, DATA_FLUSH))
		return;

	spin_lock(&sbi->inode_lock[type]);
	__remove_dirty_inode(inode, type);
	spin_unlock(&sbi->inode_lock[type]);
}

int sync_dirty_inodes(struct f2fs_sb_info *sbi, enum inode_type type)
{
	struct list_head *head;
	struct inode *inode;
	struct f2fs_inode_info *fi;
	bool is_dir = (type == DIR_INODE);

	trace_f2fs_sync_dirty_inodes_enter(sbi->sb, is_dir,
				get_pages(sbi, is_dir ?
				F2FS_DIRTY_DENTS : F2FS_DIRTY_DATA));
retry:
	if (unlikely(f2fs_cp_error(sbi)))
		return -EIO;

	spin_lock(&sbi->inode_lock[type]);

	head = &sbi->inode_list[type];
	if (list_empty(head)) {
		spin_unlock(&sbi->inode_lock[type]);
		trace_f2fs_sync_dirty_inodes_exit(sbi->sb, is_dir,
				get_pages(sbi, is_dir ?
				F2FS_DIRTY_DENTS : F2FS_DIRTY_DATA));
		return 0;
	}
	fi = list_entry(head->next, struct f2fs_inode_info, dirty_list);
	inode = igrab(&fi->vfs_inode);
	spin_unlock(&sbi->inode_lock[type]);
	if (inode) {
		filemap_fdatawrite(inode->i_mapping);
=======
void set_dirty_dir_page(struct inode *inode, struct page *page)
{
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
	struct list_head *head = &sbi->dir_inode_list;
	struct dir_inode_entry *new;
	struct list_head *this;

	if (!S_ISDIR(inode->i_mode))
		return;
retry:
	new = kmem_cache_alloc(inode_entry_slab, GFP_NOFS);
	if (!new) {
		cond_resched();
		goto retry;
	}
	new->inode = inode;
	INIT_LIST_HEAD(&new->list);

	spin_lock(&sbi->dir_inode_lock);
	list_for_each(this, head) {
		struct dir_inode_entry *entry;
		entry = list_entry(this, struct dir_inode_entry, list);
		if (entry->inode == inode) {
			kmem_cache_free(inode_entry_slab, new);
			goto out;
		}
	}
	list_add_tail(&new->list, head);
	sbi->n_dirty_dirs++;

	BUG_ON(!S_ISDIR(inode->i_mode));
out:
	inc_page_count(sbi, F2FS_DIRTY_DENTS);
	inode_inc_dirty_dents(inode);
	SetPagePrivate(page);

	spin_unlock(&sbi->dir_inode_lock);
}

void remove_dirty_dir_inode(struct inode *inode)
{
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
	struct list_head *head = &sbi->dir_inode_list;
	struct list_head *this;

	if (!S_ISDIR(inode->i_mode))
		return;

	spin_lock(&sbi->dir_inode_lock);
	if (atomic_read(&F2FS_I(inode)->dirty_dents))
		goto out;

	list_for_each(this, head) {
		struct dir_inode_entry *entry;
		entry = list_entry(this, struct dir_inode_entry, list);
		if (entry->inode == inode) {
			list_del(&entry->list);
			kmem_cache_free(inode_entry_slab, entry);
			sbi->n_dirty_dirs--;
			break;
		}
	}
out:
	spin_unlock(&sbi->dir_inode_lock);
}

void sync_dirty_dir_inodes(struct f2fs_sb_info *sbi)
{
	struct list_head *head = &sbi->dir_inode_list;
	struct dir_inode_entry *entry;
	struct inode *inode;
retry:
	spin_lock(&sbi->dir_inode_lock);
	if (list_empty(head)) {
		spin_unlock(&sbi->dir_inode_lock);
		return;
	}
	entry = list_entry(head->next, struct dir_inode_entry, list);
	inode = igrab(entry->inode);
	spin_unlock(&sbi->dir_inode_lock);
	if (inode) {
		filemap_flush(inode->i_mapping);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		iput(inode);
	} else {
		/*
		 * We should submit bio, since it exists several
		 * wribacking dentry pages in the freeing inode.
		 */
<<<<<<< HEAD
		f2fs_submit_merged_bio(sbi, DATA, WRITE);
		cond_resched();
=======
		f2fs_submit_bio(sbi, DATA, true);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	}
	goto retry;
}

<<<<<<< HEAD
int f2fs_sync_inode_meta(struct f2fs_sb_info *sbi)
{
	struct list_head *head = &sbi->inode_list[DIRTY_META];
	struct inode *inode;
	struct f2fs_inode_info *fi;
	s64 total = get_pages(sbi, F2FS_DIRTY_IMETA);

	while (total--) {
		if (unlikely(f2fs_cp_error(sbi)))
			return -EIO;

		spin_lock(&sbi->inode_lock[DIRTY_META]);
		if (list_empty(head)) {
			spin_unlock(&sbi->inode_lock[DIRTY_META]);
			return 0;
		}
		fi = list_entry(head->next, struct f2fs_inode_info,
							gdirty_list);
		inode = igrab(&fi->vfs_inode);
		spin_unlock(&sbi->inode_lock[DIRTY_META]);
		if (inode) {
			sync_inode_metadata(inode, 0);

			/* it's on eviction */
			if (is_inode_flag_set(inode, FI_DIRTY_INODE))
				update_inode_page(inode);
			iput(inode);
		}
	};
	return 0;
}

/*
 * Freeze all the FS-operations for checkpoint.
 */
static int block_operations(struct f2fs_sb_info *sbi)
=======
/*
 * Freeze all the FS-operations for checkpoint.
 */
static void block_operations(struct f2fs_sb_info *sbi)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
{
	struct writeback_control wbc = {
		.sync_mode = WB_SYNC_ALL,
		.nr_to_write = LONG_MAX,
		.for_reclaim = 0,
	};
	struct blk_plug plug;
<<<<<<< HEAD
	int err = 0;
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	blk_start_plug(&plug);

retry_flush_dents:
<<<<<<< HEAD
	f2fs_lock_all(sbi);
	/* write all the dirty dentry pages */
	if (get_pages(sbi, F2FS_DIRTY_DENTS)) {
		f2fs_unlock_all(sbi);
		err = sync_dirty_inodes(sbi, DIR_INODE);
		if (err)
			goto out;
		goto retry_flush_dents;
	}

	if (get_pages(sbi, F2FS_DIRTY_IMETA)) {
		f2fs_unlock_all(sbi);
		err = f2fs_sync_inode_meta(sbi);
		if (err)
			goto out;
=======
	mutex_lock_all(sbi);

	/* write all the dirty dentry pages */
	if (get_pages(sbi, F2FS_DIRTY_DENTS)) {
		mutex_unlock_all(sbi);
		sync_dirty_dir_inodes(sbi);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		goto retry_flush_dents;
	}

	/*
<<<<<<< HEAD
	 * POR: we should ensure that there are no dirty node pages
	 * until finishing nat/sit flush.
	 */
retry_flush_nodes:
	down_write(&sbi->node_write);

	if (get_pages(sbi, F2FS_DIRTY_NODES)) {
		up_write(&sbi->node_write);
		err = sync_node_pages(sbi, &wbc);
		if (err) {
			f2fs_unlock_all(sbi);
			goto out;
		}
		goto retry_flush_nodes;
	}
out:
	blk_finish_plug(&plug);
	return err;
=======
	 * POR: we should ensure that there is no dirty node pages
	 * until finishing nat/sit flush.
	 */
retry_flush_nodes:
	mutex_lock(&sbi->node_write);

	if (get_pages(sbi, F2FS_DIRTY_NODES)) {
		mutex_unlock(&sbi->node_write);
		sync_node_pages(sbi, 0, &wbc);
		goto retry_flush_nodes;
	}
	blk_finish_plug(&plug);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

static void unblock_operations(struct f2fs_sb_info *sbi)
{
<<<<<<< HEAD
	up_write(&sbi->node_write);

	build_free_nids(sbi, false);
	f2fs_unlock_all(sbi);
}

static void wait_on_all_pages_writeback(struct f2fs_sb_info *sbi)
{
	DEFINE_WAIT(wait);

	for (;;) {
		prepare_to_wait(&sbi->cp_wait, &wait, TASK_UNINTERRUPTIBLE);

		if (!get_pages(sbi, F2FS_WB_CP_DATA))
			break;

		io_schedule_timeout(5*HZ);
	}
	finish_wait(&sbi->cp_wait, &wait);
}

static void update_ckpt_flags(struct f2fs_sb_info *sbi, struct cp_control *cpc)
{
	unsigned long orphan_num = sbi->im[ORPHAN_INO].ino_num;
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);

	spin_lock(&sbi->cp_lock);

	if (cpc->reason == CP_UMOUNT)
		__set_ckpt_flags(ckpt, CP_UMOUNT_FLAG);
	else
		__clear_ckpt_flags(ckpt, CP_UMOUNT_FLAG);

	if (cpc->reason == CP_FASTBOOT)
		__set_ckpt_flags(ckpt, CP_FASTBOOT_FLAG);
	else
		__clear_ckpt_flags(ckpt, CP_FASTBOOT_FLAG);

	if (orphan_num)
		__set_ckpt_flags(ckpt, CP_ORPHAN_PRESENT_FLAG);
	else
		__clear_ckpt_flags(ckpt, CP_ORPHAN_PRESENT_FLAG);

	if (is_sbi_flag_set(sbi, SBI_NEED_FSCK))
		__set_ckpt_flags(ckpt, CP_FSCK_FLAG);

	/* set this flag to activate crc|cp_ver for recovery */
	__set_ckpt_flags(ckpt, CP_CRC_RECOVERY_FLAG);

	spin_unlock(&sbi->cp_lock);
}

static int do_checkpoint(struct f2fs_sb_info *sbi, struct cp_control *cpc)
{
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);
	struct f2fs_nm_info *nm_i = NM_I(sbi);
	unsigned long orphan_num = sbi->im[ORPHAN_INO].ino_num;
	nid_t last_nid = nm_i->next_scan_nid;
	block_t start_blk;
	unsigned int data_sum_blocks, orphan_blocks;
	__u32 crc32 = 0;
	int i;
	int cp_payload_blks = __cp_payload(sbi);
	struct super_block *sb = sbi->sb;
	struct curseg_info *seg_i = CURSEG_I(sbi, CURSEG_HOT_NODE);
	u64 kbytes_written;

	/* Flush all the NAT/SIT pages */
	while (get_pages(sbi, F2FS_DIRTY_META)) {
		sync_meta_pages(sbi, META, LONG_MAX);
		if (unlikely(f2fs_cp_error(sbi)))
			return -EIO;
	}
=======
	mutex_unlock(&sbi->node_write);
	mutex_unlock_all(sbi);
}

static void do_checkpoint(struct f2fs_sb_info *sbi, bool is_umount)
{
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);
	nid_t last_nid = 0;
	block_t start_blk;
	struct page *cp_page;
	unsigned int data_sum_blocks, orphan_blocks;
	unsigned int crc32 = 0;
	void *kaddr;
	int i;

	/* Flush all the NAT/SIT pages */
	while (get_pages(sbi, F2FS_DIRTY_META))
		sync_meta_pages(sbi, META, LONG_MAX);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	next_free_nid(sbi, &last_nid);

	/*
	 * modify checkpoint
	 * version number is already updated
	 */
	ckpt->elapsed_time = cpu_to_le64(get_mtime(sbi));
	ckpt->valid_block_count = cpu_to_le64(valid_user_blocks(sbi));
	ckpt->free_segment_count = cpu_to_le32(free_segments(sbi));
<<<<<<< HEAD
	for (i = 0; i < NR_CURSEG_NODE_TYPE; i++) {
=======
	for (i = 0; i < 3; i++) {
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		ckpt->cur_node_segno[i] =
			cpu_to_le32(curseg_segno(sbi, i + CURSEG_HOT_NODE));
		ckpt->cur_node_blkoff[i] =
			cpu_to_le16(curseg_blkoff(sbi, i + CURSEG_HOT_NODE));
		ckpt->alloc_type[i + CURSEG_HOT_NODE] =
				curseg_alloc_type(sbi, i + CURSEG_HOT_NODE);
	}
<<<<<<< HEAD
	for (i = 0; i < NR_CURSEG_DATA_TYPE; i++) {
=======
	for (i = 0; i < 3; i++) {
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		ckpt->cur_data_segno[i] =
			cpu_to_le32(curseg_segno(sbi, i + CURSEG_HOT_DATA));
		ckpt->cur_data_blkoff[i] =
			cpu_to_le16(curseg_blkoff(sbi, i + CURSEG_HOT_DATA));
		ckpt->alloc_type[i + CURSEG_HOT_DATA] =
				curseg_alloc_type(sbi, i + CURSEG_HOT_DATA);
	}

	ckpt->valid_node_count = cpu_to_le32(valid_node_count(sbi));
	ckpt->valid_inode_count = cpu_to_le32(valid_inode_count(sbi));
	ckpt->next_free_nid = cpu_to_le32(last_nid);

	/* 2 cp  + n data seg summary + orphan inode blocks */
<<<<<<< HEAD
	data_sum_blocks = npages_for_summary_flush(sbi, false);
	spin_lock(&sbi->cp_lock);
	if (data_sum_blocks < NR_CURSEG_DATA_TYPE)
		__set_ckpt_flags(ckpt, CP_COMPACT_SUM_FLAG);
	else
		__clear_ckpt_flags(ckpt, CP_COMPACT_SUM_FLAG);
	spin_unlock(&sbi->cp_lock);

	orphan_blocks = GET_ORPHAN_BLOCKS(orphan_num);
	ckpt->cp_pack_start_sum = cpu_to_le32(1 + cp_payload_blks +
			orphan_blocks);

	if (__remain_node_summaries(cpc->reason))
		ckpt->cp_pack_total_block_count = cpu_to_le32(F2FS_CP_PACKS+
				cp_payload_blks + data_sum_blocks +
				orphan_blocks + NR_CURSEG_NODE_TYPE);
	else
		ckpt->cp_pack_total_block_count = cpu_to_le32(F2FS_CP_PACKS +
				cp_payload_blks + data_sum_blocks +
				orphan_blocks);

	/* update ckpt flag for checkpoint */
	update_ckpt_flags(sbi, cpc);
=======
	data_sum_blocks = npages_for_summary_flush(sbi);
	if (data_sum_blocks < 3)
		set_ckpt_flags(ckpt, CP_COMPACT_SUM_FLAG);
	else
		clear_ckpt_flags(ckpt, CP_COMPACT_SUM_FLAG);

	orphan_blocks = (sbi->n_orphans + F2FS_ORPHANS_PER_BLOCK - 1)
					/ F2FS_ORPHANS_PER_BLOCK;
	ckpt->cp_pack_start_sum = cpu_to_le32(1 + orphan_blocks);

	if (is_umount) {
		set_ckpt_flags(ckpt, CP_UMOUNT_FLAG);
		ckpt->cp_pack_total_block_count = cpu_to_le32(2 +
			data_sum_blocks + orphan_blocks + NR_CURSEG_NODE_TYPE);
	} else {
		clear_ckpt_flags(ckpt, CP_UMOUNT_FLAG);
		ckpt->cp_pack_total_block_count = cpu_to_le32(2 +
			data_sum_blocks + orphan_blocks);
	}

	if (sbi->n_orphans)
		set_ckpt_flags(ckpt, CP_ORPHAN_PRESENT_FLAG);
	else
		clear_ckpt_flags(ckpt, CP_ORPHAN_PRESENT_FLAG);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	/* update SIT/NAT bitmap */
	get_sit_bitmap(sbi, __bitmap_ptr(sbi, SIT_BITMAP));
	get_nat_bitmap(sbi, __bitmap_ptr(sbi, NAT_BITMAP));

<<<<<<< HEAD
	crc32 = f2fs_crc32(sbi, ckpt, le32_to_cpu(ckpt->checksum_offset));
	*((__le32 *)((unsigned char *)ckpt +
				le32_to_cpu(ckpt->checksum_offset)))
				= cpu_to_le32(crc32);

	start_blk = __start_cp_next_addr(sbi);

	/* need to wait for end_io results */
	wait_on_all_pages_writeback(sbi);
	if (unlikely(f2fs_cp_error(sbi)))
		return -EIO;

	/* write out checkpoint buffer at block 0 */
	update_meta_page(sbi, ckpt, start_blk++);

	for (i = 1; i < 1 + cp_payload_blks; i++)
		update_meta_page(sbi, (char *)ckpt + i * F2FS_BLKSIZE,
							start_blk++);

	if (orphan_num) {
=======
	crc32 = f2fs_crc32(ckpt, le32_to_cpu(ckpt->checksum_offset));
	*(__le32 *)((unsigned char *)ckpt +
				le32_to_cpu(ckpt->checksum_offset))
				= cpu_to_le32(crc32);

	start_blk = __start_cp_addr(sbi);

	/* write out checkpoint buffer at block 0 */
	cp_page = grab_meta_page(sbi, start_blk++);
	kaddr = page_address(cp_page);
	memcpy(kaddr, ckpt, (1 << sbi->log_blocksize));
	set_page_dirty(cp_page);
	f2fs_put_page(cp_page, 1);

	if (sbi->n_orphans) {
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		write_orphan_inodes(sbi, start_blk);
		start_blk += orphan_blocks;
	}

	write_data_summaries(sbi, start_blk);
	start_blk += data_sum_blocks;
<<<<<<< HEAD

	/* Record write statistics in the hot node summary */
	kbytes_written = sbi->kbytes_written;
	if (sb->s_bdev->bd_part)
		kbytes_written += BD_PART_WRITTEN(sbi);

	seg_i->journal->info.kbytes_written = cpu_to_le64(kbytes_written);

	if (__remain_node_summaries(cpc->reason)) {
=======
	if (is_umount) {
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		write_node_summaries(sbi, start_blk);
		start_blk += NR_CURSEG_NODE_TYPE;
	}

	/* writeout checkpoint block */
<<<<<<< HEAD
	update_meta_page(sbi, ckpt, start_blk);

	/* wait for previous submitted node/meta pages writeback */
	wait_on_all_pages_writeback(sbi);

	if (unlikely(f2fs_cp_error(sbi)))
		return -EIO;

	filemap_fdatawait_range(NODE_MAPPING(sbi), 0, LLONG_MAX);
	filemap_fdatawait_range(META_MAPPING(sbi), 0, LLONG_MAX);

	/* update user_block_counts */
	sbi->last_valid_block_count = sbi->total_valid_block_count;
	percpu_counter_set(&sbi->alloc_valid_block_count, 0);
=======
	cp_page = grab_meta_page(sbi, start_blk);
	kaddr = page_address(cp_page);
	memcpy(kaddr, ckpt, (1 << sbi->log_blocksize));
	set_page_dirty(cp_page);
	f2fs_put_page(cp_page, 1);

	/* wait for previous submitted node/meta pages writeback */
	while (get_pages(sbi, F2FS_WRITEBACK))
		congestion_wait(BLK_RW_ASYNC, HZ / 50);

	filemap_fdatawait_range(sbi->node_inode->i_mapping, 0, LONG_MAX);
	filemap_fdatawait_range(sbi->meta_inode->i_mapping, 0, LONG_MAX);

	/* update user_block_counts */
	sbi->last_valid_block_count = sbi->total_valid_block_count;
	sbi->alloc_valid_block_count = 0;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	/* Here, we only have one bio having CP pack */
	sync_meta_pages(sbi, META_FLUSH, LONG_MAX);

<<<<<<< HEAD
	/* wait for previous submitted meta pages writeback */
	wait_on_all_pages_writeback(sbi);

	release_ino_entry(sbi, false);

	if (unlikely(f2fs_cp_error(sbi)))
		return -EIO;

	clear_sbi_flag(sbi, SBI_IS_DIRTY);
	clear_sbi_flag(sbi, SBI_NEED_CP);
	__set_cp_next_pack(sbi);

	/*
	 * redirty superblock if metadata like node page or inode cache is
	 * updated during writing checkpoint.
	 */
	if (get_pages(sbi, F2FS_DIRTY_NODES) ||
			get_pages(sbi, F2FS_DIRTY_IMETA))
		set_sbi_flag(sbi, SBI_IS_DIRTY);

	f2fs_bug_on(sbi, get_pages(sbi, F2FS_DIRTY_DENTS));

	return 0;
}

/*
 * We guarantee that this checkpoint procedure will not fail.
 */
int write_checkpoint(struct f2fs_sb_info *sbi, struct cp_control *cpc)
{
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);
	unsigned long long ckpt_ver;
	int err = 0;

	mutex_lock(&sbi->cp_mutex);

	if (!is_sbi_flag_set(sbi, SBI_IS_DIRTY) &&
		(cpc->reason == CP_FASTBOOT || cpc->reason == CP_SYNC ||
		(cpc->reason == CP_DISCARD && !sbi->discard_blks)))
		goto out;
	if (unlikely(f2fs_cp_error(sbi))) {
		err = -EIO;
		goto out;
	}
	if (f2fs_readonly(sbi->sb)) {
		err = -EROFS;
		goto out;
	}

	trace_f2fs_write_checkpoint(sbi->sb, cpc->reason, "start block_ops");

	err = block_operations(sbi);
	if (err)
		goto out;

	trace_f2fs_write_checkpoint(sbi->sb, cpc->reason, "finish block_ops");

	f2fs_flush_merged_bios(sbi);

	/* this is the case of multiple fstrims without any changes */
	if (cpc->reason == CP_DISCARD && !is_sbi_flag_set(sbi, SBI_IS_DIRTY)) {
		f2fs_bug_on(sbi, NM_I(sbi)->dirty_nat_cnt);
		f2fs_bug_on(sbi, SIT_I(sbi)->dirty_sentries);
		f2fs_bug_on(sbi, prefree_segments(sbi));
		flush_sit_entries(sbi, cpc);
		clear_prefree_segments(sbi, cpc);
		unblock_operations(sbi);
		goto out;
	}
=======
	if (!is_set_ckpt_flags(ckpt, CP_ERROR_FLAG)) {
		clear_prefree_segments(sbi);
		F2FS_RESET_SB_DIRT(sbi);
	}
}

/*
 * We guarantee that this checkpoint procedure should not fail.
 */
void write_checkpoint(struct f2fs_sb_info *sbi, bool is_umount)
{
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);
	unsigned long long ckpt_ver;

	trace_f2fs_write_checkpoint(sbi->sb, is_umount, "start block_ops");

	mutex_lock(&sbi->cp_mutex);
	block_operations(sbi);

	trace_f2fs_write_checkpoint(sbi->sb, is_umount, "finish block_ops");

	f2fs_submit_bio(sbi, DATA, true);
	f2fs_submit_bio(sbi, NODE, true);
	f2fs_submit_bio(sbi, META, true);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	/*
	 * update checkpoint pack index
	 * Increase the version number so that
	 * SIT entries and seg summaries are written at correct place
	 */
<<<<<<< HEAD
	ckpt_ver = cur_cp_version(ckpt);
=======
	ckpt_ver = le64_to_cpu(ckpt->checkpoint_ver);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	ckpt->checkpoint_ver = cpu_to_le64(++ckpt_ver);

	/* write cached NAT/SIT entries to NAT/SIT area */
	flush_nat_entries(sbi);
<<<<<<< HEAD
	flush_sit_entries(sbi, cpc);

	/* unlock all the fs_lock[] in do_checkpoint() */
	err = do_checkpoint(sbi, cpc);

	if (err)
		release_discard_addrs(sbi);
	else
		clear_prefree_segments(sbi, cpc);

	unblock_operations(sbi);
	stat_inc_cp_count(sbi->stat_info);

	if (cpc->reason == CP_RECOVERY)
		f2fs_msg(sbi->sb, KERN_NOTICE,
			"checkpoint: version = %llx", ckpt_ver);

	/* do checkpoint periodically */
	f2fs_update_time(sbi, CP_TIME);
	trace_f2fs_write_checkpoint(sbi->sb, cpc->reason, "finish checkpoint");
out:
	mutex_unlock(&sbi->cp_mutex);
	return err;
}

void init_ino_entry_info(struct f2fs_sb_info *sbi)
{
	int i;

	for (i = 0; i < MAX_INO_ENTRY; i++) {
		struct inode_management *im = &sbi->im[i];

		INIT_RADIX_TREE(&im->ino_root, GFP_ATOMIC);
		spin_lock_init(&im->ino_lock);
		INIT_LIST_HEAD(&im->ino_list);
		im->ino_num = 0;
	}

	sbi->max_orphans = (sbi->blocks_per_seg - F2FS_CP_PACKS -
			NR_CURSEG_TYPE - __cp_payload(sbi)) *
				F2FS_ORPHANS_PER_BLOCK;
=======
	flush_sit_entries(sbi);

	/* unlock all the fs_lock[] in do_checkpoint() */
	do_checkpoint(sbi, is_umount);

	unblock_operations(sbi);
	mutex_unlock(&sbi->cp_mutex);

	trace_f2fs_write_checkpoint(sbi->sb, is_umount, "finish checkpoint");
}

void init_orphan_info(struct f2fs_sb_info *sbi)
{
	mutex_init(&sbi->orphan_inode_mutex);
	INIT_LIST_HEAD(&sbi->orphan_inode_list);
	sbi->n_orphans = 0;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

int __init create_checkpoint_caches(void)
{
<<<<<<< HEAD
	ino_entry_slab = f2fs_kmem_cache_create("f2fs_ino_entry",
			sizeof(struct ino_entry));
	if (!ino_entry_slab)
		return -ENOMEM;
	inode_entry_slab = f2fs_kmem_cache_create("f2fs_inode_entry",
			sizeof(struct inode_entry));
	if (!inode_entry_slab) {
		kmem_cache_destroy(ino_entry_slab);
=======
	orphan_entry_slab = f2fs_kmem_cache_create("f2fs_orphan_entry",
			sizeof(struct orphan_inode_entry), NULL);
	if (unlikely(!orphan_entry_slab))
		return -ENOMEM;
	inode_entry_slab = f2fs_kmem_cache_create("f2fs_dirty_dir_entry",
			sizeof(struct dir_inode_entry), NULL);
	if (unlikely(!inode_entry_slab)) {
		kmem_cache_destroy(orphan_entry_slab);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		return -ENOMEM;
	}
	return 0;
}

void destroy_checkpoint_caches(void)
{
<<<<<<< HEAD
	kmem_cache_destroy(ino_entry_slab);
=======
	kmem_cache_destroy(orphan_entry_slab);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	kmem_cache_destroy(inode_entry_slab);
}

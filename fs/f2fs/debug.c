/*
 * f2fs debugging statistics
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *             http://www.samsung.com/
 * Copyright (c) 2012 Linux Foundation
 * Copyright (c) 2012 Greg Kroah-Hartman <gregkh@linuxfoundation.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/fs.h>
#include <linux/backing-dev.h>
#include <linux/f2fs_fs.h>
#include <linux/blkdev.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>

#include "f2fs.h"
#include "node.h"
#include "segment.h"
#include "gc.h"

static LIST_HEAD(f2fs_stat_list);
<<<<<<< HEAD
static struct dentry *f2fs_debugfs_root;
=======
static struct dentry *debugfs_root;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
static DEFINE_MUTEX(f2fs_stat_mutex);

static void update_general_status(struct f2fs_sb_info *sbi)
{
<<<<<<< HEAD
	struct f2fs_stat_info *si = F2FS_STAT(sbi);
	int i;

	/* validation check of the segment numbers */
	si->hit_largest = atomic64_read(&sbi->read_hit_largest);
	si->hit_cached = atomic64_read(&sbi->read_hit_cached);
	si->hit_rbtree = atomic64_read(&sbi->read_hit_rbtree);
	si->hit_total = si->hit_largest + si->hit_cached + si->hit_rbtree;
	si->total_ext = atomic64_read(&sbi->total_hit_ext);
	si->ext_tree = atomic_read(&sbi->total_ext_tree);
	si->zombie_tree = atomic_read(&sbi->total_zombie_tree);
	si->ext_node = atomic_read(&sbi->total_ext_node);
	si->ndirty_node = get_pages(sbi, F2FS_DIRTY_NODES);
	si->ndirty_dent = get_pages(sbi, F2FS_DIRTY_DENTS);
	si->ndirty_meta = get_pages(sbi, F2FS_DIRTY_META);
	si->ndirty_data = get_pages(sbi, F2FS_DIRTY_DATA);
	si->ndirty_imeta = get_pages(sbi, F2FS_DIRTY_IMETA);
	si->ndirty_dirs = sbi->ndirty_inode[DIR_INODE];
	si->ndirty_files = sbi->ndirty_inode[FILE_INODE];
	si->ndirty_all = sbi->ndirty_inode[DIRTY_META];
	si->inmem_pages = get_pages(sbi, F2FS_INMEM_PAGES);
	si->nr_wb_cp_data = get_pages(sbi, F2FS_WB_CP_DATA);
	si->nr_wb_data = get_pages(sbi, F2FS_WB_DATA);
=======
	struct f2fs_stat_info *si = sbi->stat_info;
	int i;

	/* valid check of the segment numbers */
	si->hit_ext = sbi->read_hit_ext;
	si->total_ext = sbi->total_hit_ext;
	si->ndirty_node = get_pages(sbi, F2FS_DIRTY_NODES);
	si->ndirty_dent = get_pages(sbi, F2FS_DIRTY_DENTS);
	si->ndirty_dirs = sbi->n_dirty_dirs;
	si->ndirty_meta = get_pages(sbi, F2FS_DIRTY_META);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	si->total_count = (int)sbi->user_block_count / sbi->blocks_per_seg;
	si->rsvd_segs = reserved_segments(sbi);
	si->overp_segs = overprovision_segments(sbi);
	si->valid_count = valid_user_blocks(sbi);
<<<<<<< HEAD
	si->discard_blks = discard_blocks(sbi);
	si->valid_node_count = valid_node_count(sbi);
	si->valid_inode_count = valid_inode_count(sbi);
	si->inline_xattr = atomic_read(&sbi->inline_xattr);
	si->inline_inode = atomic_read(&sbi->inline_inode);
	si->inline_dir = atomic_read(&sbi->inline_dir);
	si->orphans = sbi->im[ORPHAN_INO].ino_num;
=======
	si->valid_node_count = valid_node_count(sbi);
	si->valid_inode_count = valid_inode_count(sbi);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	si->utilization = utilization(sbi);

	si->free_segs = free_segments(sbi);
	si->free_secs = free_sections(sbi);
	si->prefree_count = prefree_segments(sbi);
	si->dirty_count = dirty_segments(sbi);
<<<<<<< HEAD
	si->node_pages = NODE_MAPPING(sbi)->nrpages;
	si->meta_pages = META_MAPPING(sbi)->nrpages;
	si->nats = NM_I(sbi)->nat_cnt;
	si->dirty_nats = NM_I(sbi)->dirty_nat_cnt;
	si->sits = MAIN_SEGS(sbi);
	si->dirty_sits = SIT_I(sbi)->dirty_sentries;
	si->free_nids = NM_I(sbi)->nid_cnt[FREE_NID_LIST];
	si->alloc_nids = NM_I(sbi)->nid_cnt[ALLOC_NID_LIST];
=======
	si->node_pages = sbi->node_inode->i_mapping->nrpages;
	si->meta_pages = sbi->meta_inode->i_mapping->nrpages;
	si->nats = NM_I(sbi)->nat_cnt;
	si->sits = SIT_I(sbi)->dirty_sentries;
	si->fnids = NM_I(sbi)->fcnt;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	si->bg_gc = sbi->bg_gc;
	si->util_free = (int)(free_user_blocks(sbi) >> sbi->log_blocks_per_seg)
		* 100 / (int)(sbi->user_block_count >> sbi->log_blocks_per_seg)
		/ 2;
	si->util_valid = (int)(written_block_count(sbi) >>
						sbi->log_blocks_per_seg)
		* 100 / (int)(sbi->user_block_count >> sbi->log_blocks_per_seg)
		/ 2;
	si->util_invalid = 50 - si->util_free - si->util_valid;
	for (i = CURSEG_HOT_DATA; i <= CURSEG_COLD_NODE; i++) {
		struct curseg_info *curseg = CURSEG_I(sbi, i);
		si->curseg[i] = curseg->segno;
		si->cursec[i] = curseg->segno / sbi->segs_per_sec;
		si->curzone[i] = si->cursec[i] / sbi->secs_per_zone;
	}

	for (i = 0; i < 2; i++) {
		si->segment_count[i] = sbi->segment_count[i];
		si->block_count[i] = sbi->block_count[i];
	}
<<<<<<< HEAD

	si->inplace_count = atomic_read(&sbi->inplace_count);
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

/*
 * This function calculates BDF of every segments
 */
static void update_sit_info(struct f2fs_sb_info *sbi)
{
<<<<<<< HEAD
	struct f2fs_stat_info *si = F2FS_STAT(sbi);
	unsigned long long blks_per_sec, hblks_per_sec, total_vblocks;
	unsigned long long bimodal, dist;
=======
	struct f2fs_stat_info *si = sbi->stat_info;
	unsigned int blks_per_sec, hblks_per_sec, total_vblocks, bimodal, dist;
	struct sit_info *sit_i = SIT_I(sbi);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	unsigned int segno, vblocks;
	int ndirty = 0;

	bimodal = 0;
	total_vblocks = 0;
<<<<<<< HEAD
	blks_per_sec = sbi->segs_per_sec * sbi->blocks_per_seg;
	hblks_per_sec = blks_per_sec / 2;
	for (segno = 0; segno < MAIN_SEGS(sbi); segno += sbi->segs_per_sec) {
=======
	blks_per_sec = sbi->segs_per_sec * (1 << sbi->log_blocks_per_seg);
	hblks_per_sec = blks_per_sec / 2;
	mutex_lock(&sit_i->sentry_lock);
	for (segno = 0; segno < TOTAL_SEGS(sbi); segno += sbi->segs_per_sec) {
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		vblocks = get_valid_blocks(sbi, segno, sbi->segs_per_sec);
		dist = abs(vblocks - hblks_per_sec);
		bimodal += dist * dist;

		if (vblocks > 0 && vblocks < blks_per_sec) {
			total_vblocks += vblocks;
			ndirty++;
		}
	}
<<<<<<< HEAD
	dist = div_u64(MAIN_SECS(sbi) * hblks_per_sec * hblks_per_sec, 100);
	si->bimodal = div64_u64(bimodal, dist);
	if (si->dirty_count)
		si->avg_vblocks = div_u64(total_vblocks, ndirty);
=======
	mutex_unlock(&sit_i->sentry_lock);
	dist = TOTAL_SECS(sbi) * hblks_per_sec * hblks_per_sec / 100;
	si->bimodal = bimodal / dist;
	if (si->dirty_count)
		si->avg_vblocks = total_vblocks / ndirty;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	else
		si->avg_vblocks = 0;
}

/*
 * This function calculates memory footprint.
 */
static void update_mem_info(struct f2fs_sb_info *sbi)
{
<<<<<<< HEAD
	struct f2fs_stat_info *si = F2FS_STAT(sbi);
	unsigned npages;
	int i;
=======
	struct f2fs_stat_info *si = sbi->stat_info;
	unsigned npages;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	if (si->base_mem)
		goto get_cache;

	si->base_mem = sizeof(struct f2fs_sb_info) + sbi->sb->s_blocksize;
	si->base_mem += 2 * sizeof(struct f2fs_inode_info);
	si->base_mem += sizeof(*sbi->ckpt);
<<<<<<< HEAD
	si->base_mem += sizeof(struct percpu_counter) * NR_COUNT_TYPE;
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	/* build sm */
	si->base_mem += sizeof(struct f2fs_sm_info);

	/* build sit */
	si->base_mem += sizeof(struct sit_info);
<<<<<<< HEAD
	si->base_mem += MAIN_SEGS(sbi) * sizeof(struct seg_entry);
	si->base_mem += f2fs_bitmap_size(MAIN_SEGS(sbi));
	si->base_mem += 2 * SIT_VBLOCK_MAP_SIZE * MAIN_SEGS(sbi);
	if (f2fs_discard_en(sbi))
		si->base_mem += SIT_VBLOCK_MAP_SIZE * MAIN_SEGS(sbi);
	si->base_mem += SIT_VBLOCK_MAP_SIZE;
	if (sbi->segs_per_sec > 1)
		si->base_mem += MAIN_SECS(sbi) * sizeof(struct sec_entry);
=======
	si->base_mem += TOTAL_SEGS(sbi) * sizeof(struct seg_entry);
	si->base_mem += f2fs_bitmap_size(TOTAL_SEGS(sbi));
	si->base_mem += 2 * SIT_VBLOCK_MAP_SIZE * TOTAL_SEGS(sbi);
	if (sbi->segs_per_sec > 1)
		si->base_mem += TOTAL_SECS(sbi) * sizeof(struct sec_entry);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	si->base_mem += __bitmap_size(sbi, SIT_BITMAP);

	/* build free segmap */
	si->base_mem += sizeof(struct free_segmap_info);
<<<<<<< HEAD
	si->base_mem += f2fs_bitmap_size(MAIN_SEGS(sbi));
	si->base_mem += f2fs_bitmap_size(MAIN_SECS(sbi));

	/* build curseg */
	si->base_mem += sizeof(struct curseg_info) * NR_CURSEG_TYPE;
	si->base_mem += PAGE_SIZE * NR_CURSEG_TYPE;

	/* build dirty segmap */
	si->base_mem += sizeof(struct dirty_seglist_info);
	si->base_mem += NR_DIRTY_TYPE * f2fs_bitmap_size(MAIN_SEGS(sbi));
	si->base_mem += f2fs_bitmap_size(MAIN_SECS(sbi));

	/* build nm */
	si->base_mem += sizeof(struct f2fs_nm_info);
	si->base_mem += __bitmap_size(sbi, NAT_BITMAP);

get_cache:
	si->cache_mem = 0;

	/* build gc */
	if (sbi->gc_thread)
		si->cache_mem += sizeof(struct f2fs_gc_kthread);

	/* build merge flush thread */
	if (SM_I(sbi)->cmd_control_info)
		si->cache_mem += sizeof(struct flush_cmd_control);

	/* free nids */
	si->cache_mem += (NM_I(sbi)->nid_cnt[FREE_NID_LIST] +
				NM_I(sbi)->nid_cnt[ALLOC_NID_LIST]) *
				sizeof(struct free_nid);
	si->cache_mem += NM_I(sbi)->nat_cnt * sizeof(struct nat_entry);
	si->cache_mem += NM_I(sbi)->dirty_nat_cnt *
					sizeof(struct nat_entry_set);
	si->cache_mem += si->inmem_pages * sizeof(struct inmem_pages);
	for (i = 0; i <= ORPHAN_INO; i++)
		si->cache_mem += sbi->im[i].ino_num * sizeof(struct ino_entry);
	si->cache_mem += atomic_read(&sbi->total_ext_tree) *
						sizeof(struct extent_tree);
	si->cache_mem += atomic_read(&sbi->total_ext_node) *
						sizeof(struct extent_node);

	si->page_mem = 0;
	npages = NODE_MAPPING(sbi)->nrpages;
	si->page_mem += (unsigned long long)npages << PAGE_SHIFT;
	npages = META_MAPPING(sbi)->nrpages;
	si->page_mem += (unsigned long long)npages << PAGE_SHIFT;
=======
	si->base_mem += f2fs_bitmap_size(TOTAL_SEGS(sbi));
	si->base_mem += f2fs_bitmap_size(TOTAL_SECS(sbi));

	/* build curseg */
	si->base_mem += sizeof(struct curseg_info) * NR_CURSEG_TYPE;
	si->base_mem += PAGE_CACHE_SIZE * NR_CURSEG_TYPE;

	/* build dirty segmap */
	si->base_mem += sizeof(struct dirty_seglist_info);
	si->base_mem += NR_DIRTY_TYPE * f2fs_bitmap_size(TOTAL_SEGS(sbi));
	si->base_mem += f2fs_bitmap_size(TOTAL_SECS(sbi));

	/* buld nm */
	si->base_mem += sizeof(struct f2fs_nm_info);
	si->base_mem += __bitmap_size(sbi, NAT_BITMAP);

	/* build gc */
	si->base_mem += sizeof(struct f2fs_gc_kthread);

get_cache:
	/* free nids */
	si->cache_mem = NM_I(sbi)->fcnt;
	si->cache_mem += NM_I(sbi)->nat_cnt;
	npages = sbi->node_inode->i_mapping->nrpages;
	si->cache_mem += npages << PAGE_CACHE_SHIFT;
	npages = sbi->meta_inode->i_mapping->nrpages;
	si->cache_mem += npages << PAGE_CACHE_SHIFT;
	si->cache_mem += sbi->n_orphans * sizeof(struct orphan_inode_entry);
	si->cache_mem += sbi->n_dirty_dirs * sizeof(struct dir_inode_entry);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

static int stat_show(struct seq_file *s, void *v)
{
<<<<<<< HEAD
	struct f2fs_stat_info *si;
=======
	struct f2fs_stat_info *si, *next;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	int i = 0;
	int j;

	mutex_lock(&f2fs_stat_mutex);
<<<<<<< HEAD
	list_for_each_entry(si, &f2fs_stat_list, stat_list) {
=======
	list_for_each_entry_safe(si, next, &f2fs_stat_list, stat_list) {
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		char devname[BDEVNAME_SIZE];

		update_general_status(si->sbi);

<<<<<<< HEAD
		seq_printf(s, "\n=====[ partition info(%s). #%d, %s]=====\n",
			bdevname(si->sbi->sb->s_bdev, devname), i++,
			f2fs_readonly(si->sbi->sb) ? "RO": "RW");
=======
		seq_printf(s, "\n=====[ partition info(%s). #%d ]=====\n",
			bdevname(si->sbi->sb->s_bdev, devname), i++);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		seq_printf(s, "[SB: 1] [CP: 2] [SIT: %d] [NAT: %d] ",
			   si->sit_area_segs, si->nat_area_segs);
		seq_printf(s, "[SSA: %d] [MAIN: %d",
			   si->ssa_area_segs, si->main_area_segs);
		seq_printf(s, "(OverProv:%d Resv:%d)]\n\n",
			   si->overp_segs, si->rsvd_segs);
<<<<<<< HEAD
		if (test_opt(si->sbi, DISCARD))
			seq_printf(s, "Utilization: %u%% (%u valid blocks, %u discard blocks)\n",
				si->utilization, si->valid_count, si->discard_blks);
		else
			seq_printf(s, "Utilization: %u%% (%u valid blocks)\n",
				si->utilization, si->valid_count);

=======
		seq_printf(s, "Utilization: %d%% (%d valid blocks)\n",
			   si->utilization, si->valid_count);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		seq_printf(s, "  - Node: %u (Inode: %u, ",
			   si->valid_node_count, si->valid_inode_count);
		seq_printf(s, "Other: %u)\n  - Data: %u\n",
			   si->valid_node_count - si->valid_inode_count,
			   si->valid_count - si->valid_node_count);
<<<<<<< HEAD
		seq_printf(s, "  - Inline_xattr Inode: %u\n",
			   si->inline_xattr);
		seq_printf(s, "  - Inline_data Inode: %u\n",
			   si->inline_inode);
		seq_printf(s, "  - Inline_dentry Inode: %u\n",
			   si->inline_dir);
		seq_printf(s, "  - Orphan Inode: %u\n",
			   si->orphans);
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		seq_printf(s, "\nMain area: %d segs, %d secs %d zones\n",
			   si->main_area_segs, si->main_area_sections,
			   si->main_area_zones);
		seq_printf(s, "  - COLD  data: %d, %d, %d\n",
			   si->curseg[CURSEG_COLD_DATA],
			   si->cursec[CURSEG_COLD_DATA],
			   si->curzone[CURSEG_COLD_DATA]);
		seq_printf(s, "  - WARM  data: %d, %d, %d\n",
			   si->curseg[CURSEG_WARM_DATA],
			   si->cursec[CURSEG_WARM_DATA],
			   si->curzone[CURSEG_WARM_DATA]);
		seq_printf(s, "  - HOT   data: %d, %d, %d\n",
			   si->curseg[CURSEG_HOT_DATA],
			   si->cursec[CURSEG_HOT_DATA],
			   si->curzone[CURSEG_HOT_DATA]);
		seq_printf(s, "  - Dir   dnode: %d, %d, %d\n",
			   si->curseg[CURSEG_HOT_NODE],
			   si->cursec[CURSEG_HOT_NODE],
			   si->curzone[CURSEG_HOT_NODE]);
		seq_printf(s, "  - File   dnode: %d, %d, %d\n",
			   si->curseg[CURSEG_WARM_NODE],
			   si->cursec[CURSEG_WARM_NODE],
			   si->curzone[CURSEG_WARM_NODE]);
		seq_printf(s, "  - Indir nodes: %d, %d, %d\n",
			   si->curseg[CURSEG_COLD_NODE],
			   si->cursec[CURSEG_COLD_NODE],
			   si->curzone[CURSEG_COLD_NODE]);
		seq_printf(s, "\n  - Valid: %d\n  - Dirty: %d\n",
			   si->main_area_segs - si->dirty_count -
			   si->prefree_count - si->free_segs,
			   si->dirty_count);
		seq_printf(s, "  - Prefree: %d\n  - Free: %d (%d)\n\n",
			   si->prefree_count, si->free_segs, si->free_secs);
<<<<<<< HEAD
		seq_printf(s, "CP calls: %d (BG: %d)\n",
				si->cp_count, si->bg_cp_count);
		seq_printf(s, "GC calls: %d (BG: %d)\n",
			   si->call_count, si->bg_gc);
		seq_printf(s, "  - data segments : %d (%d)\n",
				si->data_segs, si->bg_data_segs);
		seq_printf(s, "  - node segments : %d (%d)\n",
				si->node_segs, si->bg_node_segs);
		seq_printf(s, "Try to move %d blocks (BG: %d)\n", si->tot_blks,
				si->bg_data_blks + si->bg_node_blks);
		seq_printf(s, "  - data blocks : %d (%d)\n", si->data_blks,
				si->bg_data_blks);
		seq_printf(s, "  - node blocks : %d (%d)\n", si->node_blks,
				si->bg_node_blks);
		seq_puts(s, "\nExtent Cache:\n");
		seq_printf(s, "  - Hit Count: L1-1:%llu L1-2:%llu L2:%llu\n",
				si->hit_largest, si->hit_cached,
				si->hit_rbtree);
		seq_printf(s, "  - Hit Ratio: %llu%% (%llu / %llu)\n",
				!si->total_ext ? 0 :
				div64_u64(si->hit_total * 100, si->total_ext),
				si->hit_total, si->total_ext);
		seq_printf(s, "  - Inner Struct Count: tree: %d(%d), node: %d\n",
				si->ext_tree, si->zombie_tree, si->ext_node);
		seq_puts(s, "\nBalancing F2FS Async:\n");
		seq_printf(s, "  - inmem: %4d, wb_cp_data: %4d, wb_data: %4d\n",
			   si->inmem_pages, si->nr_wb_cp_data, si->nr_wb_data);
		seq_printf(s, "  - nodes: %4d in %4d\n",
			   si->ndirty_node, si->node_pages);
		seq_printf(s, "  - dents: %4d in dirs:%4d (%4d)\n",
			   si->ndirty_dent, si->ndirty_dirs, si->ndirty_all);
		seq_printf(s, "  - datas: %4d in files:%4d\n",
			   si->ndirty_data, si->ndirty_files);
		seq_printf(s, "  - meta: %4d in %4d\n",
			   si->ndirty_meta, si->meta_pages);
		seq_printf(s, "  - imeta: %4d\n",
			   si->ndirty_imeta);
		seq_printf(s, "  - NATs: %9d/%9d\n  - SITs: %9d/%9d\n",
			   si->dirty_nats, si->nats, si->dirty_sits, si->sits);
		seq_printf(s, "  - free_nids: %9d, alloc_nids: %9d\n",
			   si->free_nids, si->alloc_nids);
		seq_puts(s, "\nDistribution of User Blocks:");
		seq_puts(s, " [ valid | invalid | free ]\n");
		seq_puts(s, "  [");

		for (j = 0; j < si->util_valid; j++)
			seq_putc(s, '-');
		seq_putc(s, '|');

		for (j = 0; j < si->util_invalid; j++)
			seq_putc(s, '-');
		seq_putc(s, '|');

		for (j = 0; j < si->util_free; j++)
			seq_putc(s, '-');
		seq_puts(s, "]\n\n");
		seq_printf(s, "IPU: %u blocks\n", si->inplace_count);
=======
		seq_printf(s, "GC calls: %d (BG: %d)\n",
			   si->call_count, si->bg_gc);
		seq_printf(s, "  - data segments : %d\n", si->data_segs);
		seq_printf(s, "  - node segments : %d\n", si->node_segs);
		seq_printf(s, "Try to move %d blocks\n", si->tot_blks);
		seq_printf(s, "  - data blocks : %d\n", si->data_blks);
		seq_printf(s, "  - node blocks : %d\n", si->node_blks);
		seq_printf(s, "\nExtent Hit Ratio: %d / %d\n",
			   si->hit_ext, si->total_ext);
		seq_printf(s, "\nBalancing F2FS Async:\n");
		seq_printf(s, "  - nodes %4d in %4d\n",
			   si->ndirty_node, si->node_pages);
		seq_printf(s, "  - dents %4d in dirs:%4d\n",
			   si->ndirty_dent, si->ndirty_dirs);
		seq_printf(s, "  - meta %4d in %4d\n",
			   si->ndirty_meta, si->meta_pages);
		seq_printf(s, "  - NATs %5d > %lu\n",
			   si->nats, NM_WOUT_THRESHOLD);
		seq_printf(s, "  - SITs: %5d\n  - free_nids: %5d\n",
			   si->sits, si->fnids);
		seq_printf(s, "\nDistribution of User Blocks:");
		seq_printf(s, " [ valid | invalid | free ]\n");
		seq_printf(s, "  [");

		for (j = 0; j < si->util_valid; j++)
			seq_printf(s, "-");
		seq_printf(s, "|");

		for (j = 0; j < si->util_invalid; j++)
			seq_printf(s, "-");
		seq_printf(s, "|");

		for (j = 0; j < si->util_free; j++)
			seq_printf(s, "-");
		seq_printf(s, "]\n\n");
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		seq_printf(s, "SSR: %u blocks in %u segments\n",
			   si->block_count[SSR], si->segment_count[SSR]);
		seq_printf(s, "LFS: %u blocks in %u segments\n",
			   si->block_count[LFS], si->segment_count[LFS]);

		/* segment usage info */
		update_sit_info(si->sbi);
		seq_printf(s, "\nBDF: %u, avg. vblocks: %u\n",
			   si->bimodal, si->avg_vblocks);

		/* memory footprint */
		update_mem_info(si->sbi);
<<<<<<< HEAD
		seq_printf(s, "\nMemory: %llu KB\n",
			(si->base_mem + si->cache_mem + si->page_mem) >> 10);
		seq_printf(s, "  - static: %llu KB\n",
				si->base_mem >> 10);
		seq_printf(s, "  - cached: %llu KB\n",
				si->cache_mem >> 10);
		seq_printf(s, "  - paged : %llu KB\n",
				si->page_mem >> 10);
=======
		seq_printf(s, "\nMemory: %u KB = static: %u + cached: %u\n",
				(si->base_mem + si->cache_mem) >> 10,
				si->base_mem >> 10, si->cache_mem >> 10);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	}
	mutex_unlock(&f2fs_stat_mutex);
	return 0;
}

static int stat_open(struct inode *inode, struct file *file)
{
	return single_open(file, stat_show, inode->i_private);
}

static const struct file_operations stat_fops = {
<<<<<<< HEAD
	.owner = THIS_MODULE,
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	.open = stat_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

int f2fs_build_stats(struct f2fs_sb_info *sbi)
{
	struct f2fs_super_block *raw_super = F2FS_RAW_SUPER(sbi);
	struct f2fs_stat_info *si;

<<<<<<< HEAD
	si = kzalloc(sizeof(struct f2fs_stat_info), GFP_KERNEL);
	if (!si)
		return -ENOMEM;

=======
	sbi->stat_info = kzalloc(sizeof(struct f2fs_stat_info), GFP_KERNEL);
	if (!sbi->stat_info)
		return -ENOMEM;

	si = sbi->stat_info;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	si->all_area_segs = le32_to_cpu(raw_super->segment_count);
	si->sit_area_segs = le32_to_cpu(raw_super->segment_count_sit);
	si->nat_area_segs = le32_to_cpu(raw_super->segment_count_nat);
	si->ssa_area_segs = le32_to_cpu(raw_super->segment_count_ssa);
	si->main_area_segs = le32_to_cpu(raw_super->segment_count_main);
	si->main_area_sections = le32_to_cpu(raw_super->section_count);
	si->main_area_zones = si->main_area_sections /
				le32_to_cpu(raw_super->secs_per_zone);
	si->sbi = sbi;
<<<<<<< HEAD
	sbi->stat_info = si;

	atomic64_set(&sbi->total_hit_ext, 0);
	atomic64_set(&sbi->read_hit_rbtree, 0);
	atomic64_set(&sbi->read_hit_largest, 0);
	atomic64_set(&sbi->read_hit_cached, 0);

	atomic_set(&sbi->inline_xattr, 0);
	atomic_set(&sbi->inline_inode, 0);
	atomic_set(&sbi->inline_dir, 0);
	atomic_set(&sbi->inplace_count, 0);
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	mutex_lock(&f2fs_stat_mutex);
	list_add_tail(&si->stat_list, &f2fs_stat_list);
	mutex_unlock(&f2fs_stat_mutex);

	return 0;
}

void f2fs_destroy_stats(struct f2fs_sb_info *sbi)
{
<<<<<<< HEAD
	struct f2fs_stat_info *si = F2FS_STAT(sbi);
=======
	struct f2fs_stat_info *si = sbi->stat_info;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	mutex_lock(&f2fs_stat_mutex);
	list_del(&si->stat_list);
	mutex_unlock(&f2fs_stat_mutex);

<<<<<<< HEAD
	kfree(si);
}

int __init f2fs_create_root_stats(void)
{
	struct dentry *file;

	f2fs_debugfs_root = debugfs_create_dir("f2fs", NULL);
	if (!f2fs_debugfs_root)
		return -ENOMEM;

	file = debugfs_create_file("status", S_IRUGO, f2fs_debugfs_root,
			NULL, &stat_fops);
	if (!file) {
		debugfs_remove(f2fs_debugfs_root);
		f2fs_debugfs_root = NULL;
		return -ENOMEM;
	}

	return 0;
=======
	kfree(sbi->stat_info);
}

void __init f2fs_create_root_stats(void)
{
	debugfs_root = debugfs_create_dir("f2fs", NULL);
	if (debugfs_root)
		debugfs_create_file("status", S_IRUGO, debugfs_root,
					 NULL, &stat_fops);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

void f2fs_destroy_root_stats(void)
{
<<<<<<< HEAD
	if (!f2fs_debugfs_root)
		return;

	debugfs_remove_recursive(f2fs_debugfs_root);
	f2fs_debugfs_root = NULL;
=======
	debugfs_remove_recursive(debugfs_root);
	debugfs_root = NULL;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

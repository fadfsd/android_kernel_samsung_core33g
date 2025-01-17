/*
 * fs/f2fs/dir.c
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *             http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/fs.h>
<<<<<<< HEAD
#include <linux/namei.h>
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#include <linux/f2fs_fs.h>
#include "f2fs.h"
#include "node.h"
#include "acl.h"
<<<<<<< HEAD
#include "xattr.h"

static unsigned long dir_blocks(struct inode *inode)
{
	return ((unsigned long long) (i_size_read(inode) + PAGE_SIZE - 1))
							>> PAGE_SHIFT;
}

static unsigned int dir_buckets(unsigned int level, int dir_level)
{
	if (level + dir_level < MAX_DIR_HASH_DEPTH / 2)
		return 1 << (level + dir_level);
	else
		return MAX_DIR_BUCKETS;
=======

static unsigned long dir_blocks(struct inode *inode)
{
	return ((unsigned long long) (i_size_read(inode) + PAGE_CACHE_SIZE - 1))
							>> PAGE_CACHE_SHIFT;
}

static unsigned int dir_buckets(unsigned int level)
{
	if (level < MAX_DIR_HASH_DEPTH / 2)
		return 1 << level;
	else
		return 1 << ((MAX_DIR_HASH_DEPTH / 2) - 1);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

static unsigned int bucket_blocks(unsigned int level)
{
	if (level < MAX_DIR_HASH_DEPTH / 2)
		return 2;
	else
		return 4;
}

static unsigned char f2fs_filetype_table[F2FS_FT_MAX] = {
	[F2FS_FT_UNKNOWN]	= DT_UNKNOWN,
	[F2FS_FT_REG_FILE]	= DT_REG,
	[F2FS_FT_DIR]		= DT_DIR,
	[F2FS_FT_CHRDEV]	= DT_CHR,
	[F2FS_FT_BLKDEV]	= DT_BLK,
	[F2FS_FT_FIFO]		= DT_FIFO,
	[F2FS_FT_SOCK]		= DT_SOCK,
	[F2FS_FT_SYMLINK]	= DT_LNK,
};

<<<<<<< HEAD
=======
#define S_SHIFT 12
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
static unsigned char f2fs_type_by_mode[S_IFMT >> S_SHIFT] = {
	[S_IFREG >> S_SHIFT]	= F2FS_FT_REG_FILE,
	[S_IFDIR >> S_SHIFT]	= F2FS_FT_DIR,
	[S_IFCHR >> S_SHIFT]	= F2FS_FT_CHRDEV,
	[S_IFBLK >> S_SHIFT]	= F2FS_FT_BLKDEV,
	[S_IFIFO >> S_SHIFT]	= F2FS_FT_FIFO,
	[S_IFSOCK >> S_SHIFT]	= F2FS_FT_SOCK,
	[S_IFLNK >> S_SHIFT]	= F2FS_FT_SYMLINK,
};

<<<<<<< HEAD
void set_de_type(struct f2fs_dir_entry *de, umode_t mode)
{
	de->file_type = f2fs_type_by_mode[(mode & S_IFMT) >> S_SHIFT];
}

unsigned char get_de_type(struct f2fs_dir_entry *de)
{
	if (de->file_type < F2FS_FT_MAX)
		return f2fs_filetype_table[de->file_type];
	return DT_UNKNOWN;
}

static unsigned long dir_block_index(unsigned int level,
				int dir_level, unsigned int idx)
=======
static void set_de_type(struct f2fs_dir_entry *de, struct inode *inode)
{
	umode_t mode = inode->i_mode;
	de->file_type = f2fs_type_by_mode[(mode & S_IFMT) >> S_SHIFT];
}

static unsigned long dir_block_index(unsigned int level, unsigned int idx)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
{
	unsigned long i;
	unsigned long bidx = 0;

	for (i = 0; i < level; i++)
<<<<<<< HEAD
		bidx += dir_buckets(i, dir_level) * bucket_blocks(i);
=======
		bidx += dir_buckets(i) * bucket_blocks(i);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	bidx += idx * bucket_blocks(level);
	return bidx;
}

<<<<<<< HEAD
static struct f2fs_dir_entry *find_in_block(struct page *dentry_page,
				struct fscrypt_name *fname,
				f2fs_hash_t namehash,
				int *max_slots,
				struct page **res_page)
{
	struct f2fs_dentry_block *dentry_blk;
	struct f2fs_dir_entry *de;
	struct f2fs_dentry_ptr d;

	dentry_blk = (struct f2fs_dentry_block *)kmap(dentry_page);

	make_dentry_ptr(NULL, &d, (void *)dentry_blk, 1);
	de = find_target_dentry(fname, namehash, max_slots, &d);
	if (de)
		*res_page = dentry_page;
	else
		kunmap(dentry_page);

	return de;
}

struct f2fs_dir_entry *find_target_dentry(struct fscrypt_name *fname,
			f2fs_hash_t namehash, int *max_slots,
			struct f2fs_dentry_ptr *d)
{
	struct f2fs_dir_entry *de;
	unsigned long bit_pos = 0;
	int max_len = 0;
	struct fscrypt_str de_name = FSTR_INIT(NULL, 0);
	struct fscrypt_str *name = &fname->disk_name;

	if (max_slots)
		*max_slots = 0;
	while (bit_pos < d->max) {
		if (!test_bit_le(bit_pos, d->bitmap)) {
			bit_pos++;
			max_len++;
			continue;
		}

		de = &d->dentry[bit_pos];

		if (unlikely(!de->name_len)) {
			bit_pos++;
			continue;
		}

		/* encrypted case */
		de_name.name = d->filename[bit_pos];
		de_name.len = le16_to_cpu(de->name_len);

		/* show encrypted name */
		if (fname->hash) {
			if (de->hash_code == cpu_to_le32(fname->hash))
				goto found;
		} else if (de_name.len == name->len &&
			de->hash_code == namehash &&
			!memcmp(de_name.name, name->name, name->len))
			goto found;

		if (max_slots && max_len > *max_slots)
			*max_slots = max_len;
		max_len = 0;

		bit_pos += GET_DENTRY_SLOTS(le16_to_cpu(de->name_len));
	}

	de = NULL;
found:
	if (max_slots && max_len > *max_slots)
		*max_slots = max_len;
=======
static bool early_match_name(const char *name, size_t namelen,
			f2fs_hash_t namehash, struct f2fs_dir_entry *de)
{
	if (le16_to_cpu(de->name_len) != namelen)
		return false;

	if (de->hash_code != namehash)
		return false;

	return true;
}

static struct f2fs_dir_entry *find_in_block(struct page *dentry_page,
			const char *name, size_t namelen, int *max_slots,
			f2fs_hash_t namehash, struct page **res_page)
{
	struct f2fs_dir_entry *de;
	unsigned long bit_pos, end_pos, next_pos;
	struct f2fs_dentry_block *dentry_blk = kmap(dentry_page);
	int slots;

	bit_pos = find_next_bit_le(&dentry_blk->dentry_bitmap,
					NR_DENTRY_IN_BLOCK, 0);
	while (bit_pos < NR_DENTRY_IN_BLOCK) {
		de = &dentry_blk->dentry[bit_pos];
		slots = GET_DENTRY_SLOTS(le16_to_cpu(de->name_len));

		if (early_match_name(name, namelen, namehash, de)) {
			if (!memcmp(dentry_blk->filename[bit_pos],
							name, namelen)) {
				*res_page = dentry_page;
				goto found;
			}
		}
		next_pos = bit_pos + slots;
		bit_pos = find_next_bit_le(&dentry_blk->dentry_bitmap,
				NR_DENTRY_IN_BLOCK, next_pos);
		if (bit_pos >= NR_DENTRY_IN_BLOCK)
			end_pos = NR_DENTRY_IN_BLOCK;
		else
			end_pos = bit_pos;
		if (*max_slots < end_pos - next_pos)
			*max_slots = end_pos - next_pos;
	}

	de = NULL;
	kunmap(dentry_page);
found:
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	return de;
}

static struct f2fs_dir_entry *find_in_level(struct inode *dir,
<<<<<<< HEAD
					unsigned int level,
					struct fscrypt_name *fname,
					struct page **res_page)
{
	struct qstr name = FSTR_TO_QSTR(&fname->disk_name);
	int s = GET_DENTRY_SLOTS(name.len);
=======
		unsigned int level, const char *name, size_t namelen,
			f2fs_hash_t namehash, struct page **res_page)
{
	int s = GET_DENTRY_SLOTS(namelen);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	unsigned int nbucket, nblock;
	unsigned int bidx, end_block;
	struct page *dentry_page;
	struct f2fs_dir_entry *de = NULL;
	bool room = false;
<<<<<<< HEAD
	int max_slots;
	f2fs_hash_t namehash;

	if(fname->hash)
		namehash = cpu_to_le32(fname->hash);
	else
		namehash = f2fs_dentry_hash(&name);

	nbucket = dir_buckets(level, F2FS_I(dir)->i_dir_level);
	nblock = bucket_blocks(level);

	bidx = dir_block_index(level, F2FS_I(dir)->i_dir_level,
					le32_to_cpu(namehash) % nbucket);
=======
	int max_slots = 0;

	BUG_ON(level > MAX_DIR_HASH_DEPTH);

	nbucket = dir_buckets(level);
	nblock = bucket_blocks(level);

	bidx = dir_block_index(level, le32_to_cpu(namehash) % nbucket);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	end_block = bidx + nblock;

	for (; bidx < end_block; bidx++) {
		/* no need to allocate new dentry pages to all the indices */
<<<<<<< HEAD
		dentry_page = find_data_page(dir, bidx);
		if (IS_ERR(dentry_page)) {
			if (PTR_ERR(dentry_page) == -ENOENT) {
				room = true;
				continue;
			} else {
				*res_page = dentry_page;
				break;
			}
		}

		de = find_in_block(dentry_page, fname, namehash, &max_slots,
								res_page);
=======
		dentry_page = find_data_page(dir, bidx, true);
		if (IS_ERR(dentry_page)) {
			room = true;
			continue;
		}

		de = find_in_block(dentry_page, name, namelen,
					&max_slots, namehash, res_page);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		if (de)
			break;

		if (max_slots >= s)
			room = true;
		f2fs_put_page(dentry_page, 0);
	}

	if (!de && room && F2FS_I(dir)->chash != namehash) {
		F2FS_I(dir)->chash = namehash;
		F2FS_I(dir)->clevel = level;
	}

	return de;
}

<<<<<<< HEAD
struct f2fs_dir_entry *__f2fs_find_entry(struct inode *dir,
			struct fscrypt_name *fname, struct page **res_page)
{
	unsigned long npages = dir_blocks(dir);
	struct f2fs_dir_entry *de = NULL;
	unsigned int max_depth;
	unsigned int level;

	if (f2fs_has_inline_dentry(dir)) {
		*res_page = NULL;
		de = find_in_inline_dir(dir, fname, res_page);
		goto out;
	}

	if (npages == 0) {
		*res_page = NULL;
		goto out;
	}

	max_depth = F2FS_I(dir)->i_current_depth;
	if (unlikely(max_depth > MAX_DIR_HASH_DEPTH)) {
		f2fs_msg(F2FS_I_SB(dir)->sb, KERN_WARNING,
				"Corrupted max_depth of %lu: %u",
				dir->i_ino, max_depth);
		max_depth = MAX_DIR_HASH_DEPTH;
		f2fs_i_depth_write(dir, max_depth);
	}

	for (level = 0; level < max_depth; level++) {
		*res_page = NULL;
		de = find_in_level(dir, level, fname, res_page);
		if (de || IS_ERR(*res_page))
			break;
	}
out:
	return de;
}

/*
 * Find an entry in the specified directory with the wanted name.
 * It returns the page where the entry was found (as a parameter - res_page),
 * and the entry itself. Page is returned mapped and unlocked.
 * Entry is guaranteed to be valid.
 */
struct f2fs_dir_entry *f2fs_find_entry(struct inode *dir,
			const struct qstr *child, struct page **res_page)
{
	struct f2fs_dir_entry *de = NULL;
	struct fscrypt_name fname;
	int err;

	err = fscrypt_setup_filename(dir, child, 1, &fname);
	if (err) {
		*res_page = ERR_PTR(err);
		return NULL;
	}

	de = __f2fs_find_entry(dir, &fname, res_page);

	fscrypt_free_filename(&fname);
	return de;
}

struct f2fs_dir_entry *f2fs_parent_dir(struct inode *dir, struct page **p)
{
	struct qstr dotdot = QSTR_INIT("..", 2);

	return f2fs_find_entry(dir, &dotdot, p);
}

ino_t f2fs_inode_by_name(struct inode *dir, struct qstr *qstr,
							struct page **page)
{
	ino_t res = 0;
	struct f2fs_dir_entry *de;

	de = f2fs_find_entry(dir, qstr, page);
	if (de) {
		res = le32_to_cpu(de->ino);
		f2fs_dentry_kunmap(dir, *page);
		f2fs_put_page(*page, 0);
=======
/*
 * Find an entry in the specified directory with the wanted name.
 * It returns the page where the entry was found (as a parameter - res_page),
 * and the entry itself. Page is returned mapped and unlocked.
 * Entry is guaranteed to be valid.
 */
struct f2fs_dir_entry *f2fs_find_entry(struct inode *dir,
			struct qstr *child, struct page **res_page)
{
	const char *name = child->name;
	size_t namelen = child->len;
	unsigned long npages = dir_blocks(dir);
	struct f2fs_dir_entry *de = NULL;
	f2fs_hash_t name_hash;
	unsigned int max_depth;
	unsigned int level;

	if (namelen > F2FS_NAME_LEN)
		return NULL;

	if (npages == 0)
		return NULL;

	*res_page = NULL;

	name_hash = f2fs_dentry_hash(name, namelen);
	max_depth = F2FS_I(dir)->i_current_depth;

	for (level = 0; level < max_depth; level++) {
		de = find_in_level(dir, level, name,
				namelen, name_hash, res_page);
		if (de)
			break;
	}
	if (!de && F2FS_I(dir)->chash != name_hash) {
		F2FS_I(dir)->chash = name_hash;
		F2FS_I(dir)->clevel = level - 1;
	}
	return de;
}

struct f2fs_dir_entry *f2fs_parent_dir(struct inode *dir, struct page **p)
{
	struct page *page = NULL;
	struct f2fs_dir_entry *de = NULL;
	struct f2fs_dentry_block *dentry_blk = NULL;

	page = get_lock_data_page(dir, 0);
	if (IS_ERR(page))
		return NULL;

	dentry_blk = kmap(page);
	de = &dentry_blk->dentry[1];
	*p = page;
	unlock_page(page);
	return de;
}

ino_t f2fs_inode_by_name(struct inode *dir, struct qstr *qstr)
{
	ino_t res = 0;
	struct f2fs_dir_entry *de;
	struct page *page;

	de = f2fs_find_entry(dir, qstr, &page);
	if (de) {
		res = le32_to_cpu(de->ino);
		kunmap(page);
		f2fs_put_page(page, 0);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	}

	return res;
}

void f2fs_set_link(struct inode *dir, struct f2fs_dir_entry *de,
		struct page *page, struct inode *inode)
{
<<<<<<< HEAD
	enum page_type type = f2fs_has_inline_dentry(dir) ? NODE : DATA;
	lock_page(page);
	f2fs_wait_on_page_writeback(page, type, true);
	de->ino = cpu_to_le32(inode->i_ino);
	set_de_type(de, inode->i_mode);
	f2fs_dentry_kunmap(dir, page);
	set_page_dirty(page);

	dir->i_mtime = dir->i_ctime = current_time(dir);
	f2fs_mark_inode_dirty_sync(dir, false);
	f2fs_put_page(page, 1);
}

static void init_dent_inode(const struct qstr *name, struct page *ipage)
{
	struct f2fs_inode *ri;

	f2fs_wait_on_page_writeback(ipage, NODE, true);

	/* copy name info. to this inode page */
	ri = F2FS_INODE(ipage);
	ri->i_namelen = cpu_to_le32(name->len);
	memcpy(ri->i_name, name->name, name->len);
	set_page_dirty(ipage);
}

int update_dent_inode(struct inode *inode, struct inode *to,
					const struct qstr *name)
{
	struct page *page;

	if (file_enc_name(to))
		return 0;

	page = get_node_page(F2FS_I_SB(inode), inode->i_ino);
	if (IS_ERR(page))
		return PTR_ERR(page);

	init_dent_inode(name, page);
	f2fs_put_page(page, 1);

	return 0;
}

void do_make_empty_dir(struct inode *inode, struct inode *parent,
					struct f2fs_dentry_ptr *d)
{
	struct qstr dot = QSTR_INIT(".", 1);
	struct qstr dotdot = QSTR_INIT("..", 2);

	/* update dirent of "." */
	f2fs_update_dentry(inode->i_ino, inode->i_mode, d, &dot, 0, 0);

	/* update dirent of ".." */
	f2fs_update_dentry(parent->i_ino, parent->i_mode, d, &dotdot, 0, 1);
}

static int make_empty_dir(struct inode *inode,
		struct inode *parent, struct page *page)
{
	struct page *dentry_page;
	struct f2fs_dentry_block *dentry_blk;
	struct f2fs_dentry_ptr d;

	if (f2fs_has_inline_dentry(inode))
		return make_empty_inline_dir(inode, parent, page);

	dentry_page = get_new_data_page(inode, page, 0, true);
	if (IS_ERR(dentry_page))
		return PTR_ERR(dentry_page);

	dentry_blk = kmap_atomic(dentry_page);

	make_dentry_ptr(NULL, &d, (void *)dentry_blk, 1);
	do_make_empty_dir(inode, parent, &d);

	kunmap_atomic(dentry_blk);
=======
	lock_page(page);
	wait_on_page_writeback(page);
	de->ino = cpu_to_le32(inode->i_ino);
	set_de_type(de, inode);
	kunmap(page);
	set_page_dirty(page);
	dir->i_mtime = dir->i_ctime = CURRENT_TIME;
	mark_inode_dirty(dir);

	/* update parent inode number before releasing dentry page */
	F2FS_I(inode)->i_pino = dir->i_ino;

	f2fs_put_page(page, 1);
}

void init_dent_inode(const struct qstr *name, struct page *ipage)
{
	struct f2fs_node *rn;

	if (IS_ERR(ipage))
		return;

	wait_on_page_writeback(ipage);

	/* copy name info. to this inode page */
	rn = (struct f2fs_node *)page_address(ipage);
	rn->i.i_namelen = cpu_to_le32(name->len);
	memcpy(rn->i.i_name, name->name, name->len);
	set_page_dirty(ipage);
}

static int make_empty_dir(struct inode *inode, struct inode *parent)
{
	struct page *dentry_page;
	struct f2fs_dentry_block *dentry_blk;
	struct f2fs_dir_entry *de;
	void *kaddr;

	dentry_page = get_new_data_page(inode, 0, true);
	if (IS_ERR(dentry_page))
		return PTR_ERR(dentry_page);

	kaddr = kmap_atomic(dentry_page);
	dentry_blk = (struct f2fs_dentry_block *)kaddr;

	de = &dentry_blk->dentry[0];
	de->name_len = cpu_to_le16(1);
	de->hash_code = 0;
	de->ino = cpu_to_le32(inode->i_ino);
	memcpy(dentry_blk->filename[0], ".", 1);
	set_de_type(de, inode);

	de = &dentry_blk->dentry[1];
	de->hash_code = 0;
	de->name_len = cpu_to_le16(2);
	de->ino = cpu_to_le32(parent->i_ino);
	memcpy(dentry_blk->filename[1], "..", 2);
	set_de_type(de, inode);

	test_and_set_bit_le(0, &dentry_blk->dentry_bitmap);
	test_and_set_bit_le(1, &dentry_blk->dentry_bitmap);
	kunmap_atomic(kaddr);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	set_page_dirty(dentry_page);
	f2fs_put_page(dentry_page, 1);
	return 0;
}

<<<<<<< HEAD
struct page *init_inode_metadata(struct inode *inode, struct inode *dir,
			const struct qstr *new_name, const struct qstr *orig_name,
			struct page *dpage)
{
	struct page *page;
	int err;

	if (is_inode_flag_set(inode, FI_NEW_INODE)) {
		page = new_inode_page(inode);
		if (IS_ERR(page))
			return page;

		if (S_ISDIR(inode->i_mode)) {
			/* in order to handle error case */
			get_page(page);
			err = make_empty_dir(inode, dir, page);
			if (err) {
				lock_page(page);
				goto put_error;
			}
			put_page(page);
		}

		err = f2fs_init_acl(inode, dir, page, dpage);
		if (err)
			goto put_error;

		err = f2fs_init_security(inode, dir, orig_name, page);
		if (err)
			goto put_error;

		if (f2fs_encrypted_inode(dir) && f2fs_may_encrypt(inode)) {
			err = fscrypt_inherit_context(dir, inode, page, false);
			if (err)
				goto put_error;
		}
	} else {
		page = get_node_page(F2FS_I_SB(dir), inode->i_ino);
		if (IS_ERR(page))
			return page;

		set_cold_node(inode, page);
	}

	if (new_name)
		init_dent_inode(new_name, page);

	/*
	 * This file should be checkpointed during fsync.
	 * We lost i_pino from now on.
	 */
	if (is_inode_flag_set(inode, FI_INC_LINK)) {
		file_lost_pino(inode);
		/*
		 * If link the tmpfile to alias through linkat path,
		 * we should remove this inode from orphan list.
		 */
		if (inode->i_nlink == 0)
			remove_orphan_inode(F2FS_I_SB(dir), inode->i_ino);
		f2fs_i_links_write(inode, true);
	}
	return page;

put_error:
	clear_nlink(inode);
	update_inode(inode, page);
	f2fs_put_page(page, 1);
	return ERR_PTR(err);
}

void update_parent_metadata(struct inode *dir, struct inode *inode,
						unsigned int current_depth)
{
	if (inode && is_inode_flag_set(inode, FI_NEW_INODE)) {
		if (S_ISDIR(inode->i_mode))
			f2fs_i_links_write(dir, true);
		clear_inode_flag(inode, FI_NEW_INODE);
	}
	dir->i_mtime = dir->i_ctime = current_time(dir);
	f2fs_mark_inode_dirty_sync(dir, false);

	if (F2FS_I(dir)->i_current_depth != current_depth)
		f2fs_i_depth_write(dir, current_depth);

	if (inode && is_inode_flag_set(inode, FI_INC_LINK))
		clear_inode_flag(inode, FI_INC_LINK);
}

int room_for_filename(const void *bitmap, int slots, int max_slots)
=======
static int init_inode_metadata(struct inode *inode,
		struct inode *dir, const struct qstr *name)
{
	if (is_inode_flag_set(F2FS_I(inode), FI_NEW_INODE)) {
		int err;
		err = new_inode_page(inode, name);
		if (err)
			return err;

		if (S_ISDIR(inode->i_mode)) {
			err = make_empty_dir(inode, dir);
			if (err) {
				remove_inode_page(inode);
				return err;
			}
		}

		err = f2fs_init_acl(inode, dir);
		if (err) {
			remove_inode_page(inode);
			return err;
		}
	} else {
		struct page *ipage;
		ipage = get_node_page(F2FS_SB(dir->i_sb), inode->i_ino);
		if (IS_ERR(ipage))
			return PTR_ERR(ipage);
		set_cold_node(inode, ipage);
		init_dent_inode(name, ipage);
		f2fs_put_page(ipage, 1);
	}
	if (is_inode_flag_set(F2FS_I(inode), FI_INC_LINK)) {
		inc_nlink(inode);
		update_inode_page(inode);
	}
	return 0;
}

static void update_parent_metadata(struct inode *dir, struct inode *inode,
						unsigned int current_depth)
{
	bool need_dir_update = false;

	if (is_inode_flag_set(F2FS_I(inode), FI_NEW_INODE)) {
		if (S_ISDIR(inode->i_mode)) {
			inc_nlink(dir);
			need_dir_update = true;
		}
		clear_inode_flag(F2FS_I(inode), FI_NEW_INODE);
	}
	dir->i_mtime = dir->i_ctime = CURRENT_TIME;
	if (F2FS_I(dir)->i_current_depth != current_depth) {
		F2FS_I(dir)->i_current_depth = current_depth;
		need_dir_update = true;
	}

	if (need_dir_update)
		update_inode_page(dir);
	else
		mark_inode_dirty(dir);

	if (is_inode_flag_set(F2FS_I(inode), FI_INC_LINK))
		clear_inode_flag(F2FS_I(inode), FI_INC_LINK);
}

static int room_for_filename(struct f2fs_dentry_block *dentry_blk, int slots)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
{
	int bit_start = 0;
	int zero_start, zero_end;
next:
<<<<<<< HEAD
	zero_start = find_next_zero_bit_le(bitmap, max_slots, bit_start);
	if (zero_start >= max_slots)
		return max_slots;

	zero_end = find_next_bit_le(bitmap, max_slots, zero_start);
=======
	zero_start = find_next_zero_bit_le(&dentry_blk->dentry_bitmap,
						NR_DENTRY_IN_BLOCK,
						bit_start);
	if (zero_start >= NR_DENTRY_IN_BLOCK)
		return NR_DENTRY_IN_BLOCK;

	zero_end = find_next_bit_le(&dentry_blk->dentry_bitmap,
						NR_DENTRY_IN_BLOCK,
						zero_start);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	if (zero_end - zero_start >= slots)
		return zero_start;

	bit_start = zero_end + 1;

<<<<<<< HEAD
	if (zero_end + 1 >= max_slots)
		return max_slots;
	goto next;
}

void f2fs_update_dentry(nid_t ino, umode_t mode, struct f2fs_dentry_ptr *d,
				const struct qstr *name, f2fs_hash_t name_hash,
				unsigned int bit_pos)
{
	struct f2fs_dir_entry *de;
	int slots = GET_DENTRY_SLOTS(name->len);
	int i;

	de = &d->dentry[bit_pos];
	de->hash_code = name_hash;
	de->name_len = cpu_to_le16(name->len);
	memcpy(d->filename[bit_pos], name->name, name->len);
	de->ino = cpu_to_le32(ino);
	set_de_type(de, mode);
	for (i = 0; i < slots; i++) {
		__set_bit_le(bit_pos + i, (void *)d->bitmap);
		/* avoid wrong garbage data for readdir */
		if (i)
			(de + i)->name_len = 0;
	}
}

int f2fs_add_regular_entry(struct inode *dir, const struct qstr *new_name,
				const struct qstr *orig_name,
				struct inode *inode, nid_t ino, umode_t mode)
=======
	if (zero_end + 1 >= NR_DENTRY_IN_BLOCK)
		return NR_DENTRY_IN_BLOCK;
	goto next;
}

/*
 * Caller should grab and release a mutex by calling mutex_lock_op() and
 * mutex_unlock_op().
 */
int __f2fs_add_link(struct inode *dir, const struct qstr *name, struct inode *inode)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
{
	unsigned int bit_pos;
	unsigned int level;
	unsigned int current_depth;
	unsigned long bidx, block;
	f2fs_hash_t dentry_hash;
<<<<<<< HEAD
	unsigned int nbucket, nblock;
	struct page *dentry_page = NULL;
	struct f2fs_dentry_block *dentry_blk = NULL;
	struct f2fs_dentry_ptr d;
	struct page *page = NULL;
	int slots, err = 0;

	level = 0;
	slots = GET_DENTRY_SLOTS(new_name->len);
	dentry_hash = f2fs_dentry_hash(new_name);

=======
	struct f2fs_dir_entry *de;
	unsigned int nbucket, nblock;
	size_t namelen = name->len;
	struct page *dentry_page = NULL;
	struct f2fs_dentry_block *dentry_blk = NULL;
	int slots = GET_DENTRY_SLOTS(namelen);
	int err = 0;
	int i;

	dentry_hash = f2fs_dentry_hash(name->name, name->len);
	level = 0;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	current_depth = F2FS_I(dir)->i_current_depth;
	if (F2FS_I(dir)->chash == dentry_hash) {
		level = F2FS_I(dir)->clevel;
		F2FS_I(dir)->chash = 0;
	}

start:
<<<<<<< HEAD
#ifdef CONFIG_F2FS_FAULT_INJECTION
	if (time_to_inject(F2FS_I_SB(dir), FAULT_DIR_DEPTH))
		return -ENOSPC;
#endif
	if (unlikely(current_depth == MAX_DIR_HASH_DEPTH))
=======
	if (current_depth == MAX_DIR_HASH_DEPTH)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		return -ENOSPC;

	/* Increase the depth, if required */
	if (level == current_depth)
		++current_depth;

<<<<<<< HEAD
	nbucket = dir_buckets(level, F2FS_I(dir)->i_dir_level);
	nblock = bucket_blocks(level);

	bidx = dir_block_index(level, F2FS_I(dir)->i_dir_level,
				(le32_to_cpu(dentry_hash) % nbucket));

	for (block = bidx; block <= (bidx + nblock - 1); block++) {
		dentry_page = get_new_data_page(dir, NULL, block, true);
=======
	nbucket = dir_buckets(level);
	nblock = bucket_blocks(level);

	bidx = dir_block_index(level, (le32_to_cpu(dentry_hash) % nbucket));

	for (block = bidx; block <= (bidx + nblock - 1); block++) {
		dentry_page = get_new_data_page(dir, block, true);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		if (IS_ERR(dentry_page))
			return PTR_ERR(dentry_page);

		dentry_blk = kmap(dentry_page);
<<<<<<< HEAD
		bit_pos = room_for_filename(&dentry_blk->dentry_bitmap,
						slots, NR_DENTRY_IN_BLOCK);
=======
		bit_pos = room_for_filename(dentry_blk, slots);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		if (bit_pos < NR_DENTRY_IN_BLOCK)
			goto add_dentry;

		kunmap(dentry_page);
		f2fs_put_page(dentry_page, 1);
	}

	/* Move to next level to find the empty slot for new dentry */
	++level;
	goto start;
add_dentry:
<<<<<<< HEAD
	f2fs_wait_on_page_writeback(dentry_page, DATA, true);

	if (inode) {
		down_write(&F2FS_I(inode)->i_sem);
		page = init_inode_metadata(inode, dir, new_name,
						orig_name, NULL);
		if (IS_ERR(page)) {
			err = PTR_ERR(page);
			goto fail;
		}
		if (f2fs_encrypted_inode(dir))
			file_set_enc_name(inode);
	}

	make_dentry_ptr(NULL, &d, (void *)dentry_blk, 1);
	f2fs_update_dentry(ino, mode, &d, new_name, dentry_hash, bit_pos);

	set_page_dirty(dentry_page);

	if (inode) {
		f2fs_i_pino_write(inode, dir->i_ino);
		f2fs_put_page(page, 1);
	}

	update_parent_metadata(dir, inode, current_depth);
fail:
	if (inode)
		up_write(&F2FS_I(inode)->i_sem);

	kunmap(dentry_page);
	f2fs_put_page(dentry_page, 1);

	return err;
}

int __f2fs_do_add_link(struct inode *dir, struct fscrypt_name *fname,
				struct inode *inode, nid_t ino, umode_t mode)
{
	struct qstr new_name;
	int err = -EAGAIN;

	new_name.name = fname_name(fname);
	new_name.len = fname_len(fname);

	if (f2fs_has_inline_dentry(dir))
		err = f2fs_add_inline_entry(dir, &new_name, fname->usr_fname,
							inode, ino, mode);
	if (err == -EAGAIN)
		err = f2fs_add_regular_entry(dir, &new_name, fname->usr_fname,
							inode, ino, mode);

	f2fs_update_time(F2FS_I_SB(dir), REQ_TIME);
	return err;
}

/*
 * Caller should grab and release a rwsem by calling f2fs_lock_op() and
 * f2fs_unlock_op().
 */
int __f2fs_add_link(struct inode *dir, const struct qstr *name,
				struct inode *inode, nid_t ino, umode_t mode)
{
	struct fscrypt_name fname;
	int err;

	err = fscrypt_setup_filename(dir, name, 0, &fname);
	if (err)
		return err;

	err = __f2fs_do_add_link(dir, &fname, inode, ino, mode);

	fscrypt_free_filename(&fname);
	return err;
}

int f2fs_do_tmpfile(struct inode *inode, struct inode *dir)
{
	struct page *page;
	int err = 0;

	down_write(&F2FS_I(inode)->i_sem);
	page = init_inode_metadata(inode, dir, NULL, NULL, NULL);
	if (IS_ERR(page)) {
		err = PTR_ERR(page);
		goto fail;
	}
	f2fs_put_page(page, 1);

	clear_inode_flag(inode, FI_NEW_INODE);
fail:
	up_write(&F2FS_I(inode)->i_sem);
	f2fs_update_time(F2FS_I_SB(inode), REQ_TIME);
	return err;
}

void f2fs_drop_nlink(struct inode *dir, struct inode *inode)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(dir);

	down_write(&F2FS_I(inode)->i_sem);

	if (S_ISDIR(inode->i_mode))
		f2fs_i_links_write(dir, false);
	inode->i_ctime = current_time(inode);

	f2fs_i_links_write(inode, false);
	if (S_ISDIR(inode->i_mode)) {
		f2fs_i_links_write(inode, false);
		f2fs_i_size_write(inode, 0);
	}
	up_write(&F2FS_I(inode)->i_sem);

	if (inode->i_nlink == 0)
		add_orphan_inode(inode);
	else
		release_orphan_inode(sbi);
}

/*
 * It only removes the dentry from the dentry page, corresponding name
 * entry in name page does not need to be touched during deletion.
 */
void f2fs_delete_entry(struct f2fs_dir_entry *dentry, struct page *page,
					struct inode *dir, struct inode *inode)
{
	struct	f2fs_dentry_block *dentry_blk;
	unsigned int bit_pos;
	int slots = GET_DENTRY_SLOTS(le16_to_cpu(dentry->name_len));
	int i;

	f2fs_update_time(F2FS_I_SB(dir), REQ_TIME);

	if (f2fs_has_inline_dentry(dir))
		return f2fs_delete_inline_entry(dentry, page, dir, inode);

	lock_page(page);
	f2fs_wait_on_page_writeback(page, DATA, true);

	dentry_blk = page_address(page);
	bit_pos = dentry - dentry_blk->dentry;
	for (i = 0; i < slots; i++)
		clear_bit_le(bit_pos + i, &dentry_blk->dentry_bitmap);
=======
	err = init_inode_metadata(inode, dir, name);
	if (err)
		goto fail;

	wait_on_page_writeback(dentry_page);

	de = &dentry_blk->dentry[bit_pos];
	de->hash_code = dentry_hash;
	de->name_len = cpu_to_le16(namelen);
	memcpy(dentry_blk->filename[bit_pos], name->name, name->len);
	de->ino = cpu_to_le32(inode->i_ino);
	set_de_type(de, inode);
	for (i = 0; i < slots; i++)
		test_and_set_bit_le(bit_pos + i, &dentry_blk->dentry_bitmap);
	set_page_dirty(dentry_page);

	update_parent_metadata(dir, inode, current_depth);

	/* update parent inode number before releasing dentry page */
	F2FS_I(inode)->i_pino = dir->i_ino;
fail:
	kunmap(dentry_page);
	f2fs_put_page(dentry_page, 1);
	return err;
}

/*
 * It only removes the dentry from the dentry page,corresponding name
 * entry in name page does not need to be touched during deletion.
 */
void f2fs_delete_entry(struct f2fs_dir_entry *dentry, struct page *page,
						struct inode *inode)
{
	struct	f2fs_dentry_block *dentry_blk;
	unsigned int bit_pos;
	struct address_space *mapping = page->mapping;
	struct inode *dir = mapping->host;
	struct f2fs_sb_info *sbi = F2FS_SB(dir->i_sb);
	int slots = GET_DENTRY_SLOTS(le16_to_cpu(dentry->name_len));
	void *kaddr = page_address(page);
	int i;

	lock_page(page);
	wait_on_page_writeback(page);

	dentry_blk = (struct f2fs_dentry_block *)kaddr;
	bit_pos = dentry - (struct f2fs_dir_entry *)dentry_blk->dentry;
	for (i = 0; i < slots; i++)
		test_and_clear_bit_le(bit_pos + i, &dentry_blk->dentry_bitmap);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	/* Let's check and deallocate this dentry page */
	bit_pos = find_next_bit_le(&dentry_blk->dentry_bitmap,
			NR_DENTRY_IN_BLOCK,
			0);
	kunmap(page); /* kunmap - pair of f2fs_find_entry */
	set_page_dirty(page);

<<<<<<< HEAD
	dir->i_ctime = dir->i_mtime = current_time(dir);
	f2fs_mark_inode_dirty_sync(dir, false);

	if (inode)
		f2fs_drop_nlink(dir, inode);

	if (bit_pos == NR_DENTRY_IN_BLOCK &&
			!truncate_hole(dir, page->index, page->index + 1)) {
		clear_page_dirty_for_io(page);
		ClearPagePrivate(page);
		ClearPageUptodate(page);
		inode_dec_dirty_pages(dir);
		remove_dirty_inode(dir);
=======
	dir->i_ctime = dir->i_mtime = CURRENT_TIME;

	if (inode && S_ISDIR(inode->i_mode)) {
		drop_nlink(dir);
		update_inode_page(dir);
	} else {
		mark_inode_dirty(dir);
	}

	if (inode) {
		inode->i_ctime = CURRENT_TIME;
		drop_nlink(inode);
		if (S_ISDIR(inode->i_mode)) {
			drop_nlink(inode);
			i_size_write(inode, 0);
		}
		update_inode_page(inode);

		if (inode->i_nlink == 0)
			add_orphan_inode(sbi, inode->i_ino);
	}

	if (bit_pos == NR_DENTRY_IN_BLOCK) {
		truncate_hole(dir, page->index, page->index + 1);
		clear_page_dirty_for_io(page);
		ClearPageUptodate(page);
		dec_page_count(sbi, F2FS_DIRTY_DENTS);
		inode_dec_dirty_dents(dir);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	}
	f2fs_put_page(page, 1);
}

bool f2fs_empty_dir(struct inode *dir)
{
	unsigned long bidx;
	struct page *dentry_page;
	unsigned int bit_pos;
<<<<<<< HEAD
	struct f2fs_dentry_block *dentry_blk;
	unsigned long nblock = dir_blocks(dir);

	if (f2fs_has_inline_dentry(dir))
		return f2fs_empty_inline_dir(dir);

	for (bidx = 0; bidx < nblock; bidx++) {
		dentry_page = get_lock_data_page(dir, bidx, false);
=======
	struct	f2fs_dentry_block *dentry_blk;
	unsigned long nblock = dir_blocks(dir);

	for (bidx = 0; bidx < nblock; bidx++) {
		void *kaddr;
		dentry_page = get_lock_data_page(dir, bidx);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		if (IS_ERR(dentry_page)) {
			if (PTR_ERR(dentry_page) == -ENOENT)
				continue;
			else
				return false;
		}

<<<<<<< HEAD
		dentry_blk = kmap_atomic(dentry_page);
=======
		kaddr = kmap_atomic(dentry_page);
		dentry_blk = (struct f2fs_dentry_block *)kaddr;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		if (bidx == 0)
			bit_pos = 2;
		else
			bit_pos = 0;
		bit_pos = find_next_bit_le(&dentry_blk->dentry_bitmap,
						NR_DENTRY_IN_BLOCK,
						bit_pos);
<<<<<<< HEAD
		kunmap_atomic(dentry_blk);
=======
		kunmap_atomic(kaddr);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

		f2fs_put_page(dentry_page, 1);

		if (bit_pos < NR_DENTRY_IN_BLOCK)
			return false;
	}
	return true;
}

<<<<<<< HEAD
int f2fs_fill_dentries(struct file *file, void *dirent, filldir_t filldir,
		struct f2fs_dentry_ptr *d, unsigned int n, unsigned int bit_pos,
		struct fscrypt_str *fstr)
{
	unsigned int start_bit_pos = bit_pos;
	unsigned char d_type;
	struct f2fs_dir_entry *de = NULL;
	struct fscrypt_str de_name = FSTR_INIT(NULL, 0);
	int over;

	while (bit_pos < d->max) {
		d_type = DT_UNKNOWN;
		bit_pos = find_next_bit_le(d->bitmap, d->max, bit_pos);
		if (bit_pos >= d->max)
			break;

		de = &d->dentry[bit_pos];

		if (de->name_len == 0) {
			bit_pos++;
			continue;
		}

		d_type = get_de_type(de);

		de_name.name = d->filename[bit_pos];
		de_name.len = le16_to_cpu(de->name_len);

		if (f2fs_encrypted_inode(d->inode)) {
			int save_len = fstr->len;
			int err;

			err = fscrypt_fname_disk_to_usr(d->inode,
						(u32)de->hash_code, 0,
						&de_name, fstr);
			if (err)
				return err;

			de_name = *fstr;
			fstr->len = save_len;
		}

		over = filldir(dirent, de_name.name, de_name.len,
					(n * d->max) + bit_pos,
					le32_to_cpu(de->ino), d_type);
		if (over) {
			file->f_pos += bit_pos - start_bit_pos;
			return 1;
		}

		bit_pos += GET_DENTRY_SLOTS(le16_to_cpu(de->name_len));
	}
	return 0;
}

static int f2fs_readdir(struct file *file, void *dirent, filldir_t filldir)
{
	unsigned long pos = file->f_pos;
	unsigned int bit_pos = 0;
	struct inode *inode = file_inode(file);
	unsigned long npages = dir_blocks(inode);
	struct f2fs_dentry_block *dentry_blk = NULL;
	struct page *dentry_page = NULL;
	struct file_ra_state *ra = &file->f_ra;
	struct f2fs_dentry_ptr d;
	struct fscrypt_str fstr = FSTR_INIT(NULL, 0);
	unsigned int n = 0;
	int err = 0;

	if (f2fs_encrypted_inode(inode)) {
		err = fscrypt_get_encryption_info(inode);
		if (err && err != -ENOKEY)
			return err;

		err = fscrypt_fname_alloc_buffer(inode, F2FS_NAME_LEN, &fstr);
		if (err < 0)
			return err;
	}

	if (f2fs_has_inline_dentry(inode)) {
		err = f2fs_read_inline_dir(file, dirent, filldir, &fstr);
		goto out;
	}

	bit_pos = (pos % NR_DENTRY_IN_BLOCK);
	n = (pos / NR_DENTRY_IN_BLOCK);

	/* readahead for multi pages of dir */
	if (npages - n > 1 && !ra_has_index(ra, n))
		page_cache_sync_readahead(inode->i_mapping, ra, file, n,
				min(npages - n, (pgoff_t)MAX_DIR_RA_PAGES));

	for (; n < npages; n++) {
		dentry_page = get_lock_data_page(inode, n, false);
		if (IS_ERR(dentry_page)) {
			err = PTR_ERR(dentry_page);
			if (err == -ENOENT) {
				err = 0;
				continue;
			} else {
				goto out;
			}
		}

		dentry_blk = kmap(dentry_page);

		make_dentry_ptr(inode, &d, (void *)dentry_blk, 1);

		err = f2fs_fill_dentries(file, dirent, filldir, &d, n,
							bit_pos, &fstr);
		if (err) {
			kunmap(dentry_page);
			f2fs_put_page(dentry_page, 1);
			break;
		}

=======
static int f2fs_readdir(struct file *file, void *dirent, filldir_t filldir)
{
	unsigned long pos = file->f_pos;
	struct inode *inode = file_inode(file);
	unsigned long npages = dir_blocks(inode);
	unsigned char *types = NULL;
	unsigned int bit_pos = 0, start_bit_pos = 0;
	int over = 0;
	struct f2fs_dentry_block *dentry_blk = NULL;
	struct f2fs_dir_entry *de = NULL;
	struct page *dentry_page = NULL;
	unsigned int n = 0;
	unsigned char d_type = DT_UNKNOWN;
	int slots;

	types = f2fs_filetype_table;
	bit_pos = (pos % NR_DENTRY_IN_BLOCK);
	n = (pos / NR_DENTRY_IN_BLOCK);

	for ( ; n < npages; n++) {
		dentry_page = get_lock_data_page(inode, n);
		if (IS_ERR(dentry_page))
			continue;

		start_bit_pos = bit_pos;
		dentry_blk = kmap(dentry_page);
		while (bit_pos < NR_DENTRY_IN_BLOCK) {
			d_type = DT_UNKNOWN;
			bit_pos = find_next_bit_le(&dentry_blk->dentry_bitmap,
							NR_DENTRY_IN_BLOCK,
							bit_pos);
			if (bit_pos >= NR_DENTRY_IN_BLOCK)
				break;

			de = &dentry_blk->dentry[bit_pos];
			if (types && de->file_type < F2FS_FT_MAX)
				d_type = types[de->file_type];

			over = filldir(dirent,
					dentry_blk->filename[bit_pos],
					le16_to_cpu(de->name_len),
					(n * NR_DENTRY_IN_BLOCK) + bit_pos,
					le32_to_cpu(de->ino), d_type);
			if (over) {
				file->f_pos += bit_pos - start_bit_pos;
				goto success;
			}
			slots = GET_DENTRY_SLOTS(le16_to_cpu(de->name_len));
			bit_pos += slots;
		}
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		bit_pos = 0;
		file->f_pos = (n + 1) * NR_DENTRY_IN_BLOCK;
		kunmap(dentry_page);
		f2fs_put_page(dentry_page, 1);
<<<<<<< HEAD
	}
out:
	fscrypt_fname_free_buffer(&fstr);
	return err < 0 ? err : 0;
}

static int f2fs_dir_open(struct inode *inode, struct file *filp)
{
	if (f2fs_encrypted_inode(inode))
		return fscrypt_get_encryption_info(inode) ? -EACCES : 0;
=======
		dentry_page = NULL;
	}
success:
	if (dentry_page && !IS_ERR(dentry_page)) {
		kunmap(dentry_page);
		f2fs_put_page(dentry_page, 1);
	}

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	return 0;
}

const struct file_operations f2fs_dir_operations = {
	.llseek		= generic_file_llseek,
	.read		= generic_read_dir,
	.readdir	= f2fs_readdir,
	.fsync		= f2fs_sync_file,
<<<<<<< HEAD
	.open		= f2fs_dir_open,
	.unlocked_ioctl	= f2fs_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl   = f2fs_compat_ioctl,
#endif
=======
	.unlocked_ioctl	= f2fs_ioctl,
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
};

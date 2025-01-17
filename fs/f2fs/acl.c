/*
 * fs/f2fs/acl.c
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *             http://www.samsung.com/
 *
 * Portions of this code from linux/fs/ext2/acl.c
 *
 * Copyright (C) 2001-2003 Andreas Gruenbacher, <agruen@suse.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/f2fs_fs.h>
#include "f2fs.h"
#include "xattr.h"
#include "acl.h"

<<<<<<< HEAD
=======
#define get_inode_mode(i)	((is_inode_flag_set(F2FS_I(i), FI_ACL_MODE)) ? \
					(F2FS_I(i)->i_acl_mode) : ((i)->i_mode))

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
static inline size_t f2fs_acl_size(int count)
{
	if (count <= 4) {
		return sizeof(struct f2fs_acl_header) +
			count * sizeof(struct f2fs_acl_entry_short);
	} else {
		return sizeof(struct f2fs_acl_header) +
			4 * sizeof(struct f2fs_acl_entry_short) +
			(count - 4) * sizeof(struct f2fs_acl_entry);
	}
}

static inline int f2fs_acl_count(size_t size)
{
	ssize_t s;
	size -= sizeof(struct f2fs_acl_header);
	s = size - 4 * sizeof(struct f2fs_acl_entry_short);
	if (s < 0) {
		if (size % sizeof(struct f2fs_acl_entry_short))
			return -1;
		return size / sizeof(struct f2fs_acl_entry_short);
	} else {
		if (s % sizeof(struct f2fs_acl_entry))
			return -1;
		return s / sizeof(struct f2fs_acl_entry) + 4;
	}
}

static struct posix_acl *f2fs_acl_from_disk(const char *value, size_t size)
{
	int i, count;
	struct posix_acl *acl;
	struct f2fs_acl_header *hdr = (struct f2fs_acl_header *)value;
	struct f2fs_acl_entry *entry = (struct f2fs_acl_entry *)(hdr + 1);
	const char *end = value + size;

	if (hdr->a_version != cpu_to_le32(F2FS_ACL_VERSION))
		return ERR_PTR(-EINVAL);

	count = f2fs_acl_count(size);
	if (count < 0)
		return ERR_PTR(-EINVAL);
	if (count == 0)
		return NULL;

<<<<<<< HEAD
	acl = posix_acl_alloc(count, GFP_NOFS);
=======
	acl = posix_acl_alloc(count, GFP_KERNEL);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	if (!acl)
		return ERR_PTR(-ENOMEM);

	for (i = 0; i < count; i++) {

		if ((char *)entry > end)
			goto fail;

		acl->a_entries[i].e_tag  = le16_to_cpu(entry->e_tag);
		acl->a_entries[i].e_perm = le16_to_cpu(entry->e_perm);

		switch (acl->a_entries[i].e_tag) {
		case ACL_USER_OBJ:
		case ACL_GROUP_OBJ:
		case ACL_MASK:
		case ACL_OTHER:
			entry = (struct f2fs_acl_entry *)((char *)entry +
					sizeof(struct f2fs_acl_entry_short));
			break;

		case ACL_USER:
			acl->a_entries[i].e_uid =
				make_kuid(&init_user_ns,
						le32_to_cpu(entry->e_id));
			entry = (struct f2fs_acl_entry *)((char *)entry +
					sizeof(struct f2fs_acl_entry));
			break;
		case ACL_GROUP:
			acl->a_entries[i].e_gid =
				make_kgid(&init_user_ns,
						le32_to_cpu(entry->e_id));
			entry = (struct f2fs_acl_entry *)((char *)entry +
					sizeof(struct f2fs_acl_entry));
			break;
		default:
			goto fail;
		}
	}
	if ((char *)entry != end)
		goto fail;
	return acl;
fail:
	posix_acl_release(acl);
	return ERR_PTR(-EINVAL);
}

<<<<<<< HEAD
static void *f2fs_acl_to_disk(struct f2fs_sb_info *sbi,
				const struct posix_acl *acl, size_t *size)
=======
static void *f2fs_acl_to_disk(const struct posix_acl *acl, size_t *size)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
{
	struct f2fs_acl_header *f2fs_acl;
	struct f2fs_acl_entry *entry;
	int i;

<<<<<<< HEAD
	f2fs_acl = f2fs_kmalloc(sbi, sizeof(struct f2fs_acl_header) +
			acl->a_count * sizeof(struct f2fs_acl_entry),
			GFP_NOFS);
=======
	f2fs_acl = kmalloc(sizeof(struct f2fs_acl_header) + acl->a_count *
			sizeof(struct f2fs_acl_entry), GFP_KERNEL);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	if (!f2fs_acl)
		return ERR_PTR(-ENOMEM);

	f2fs_acl->a_version = cpu_to_le32(F2FS_ACL_VERSION);
	entry = (struct f2fs_acl_entry *)(f2fs_acl + 1);

	for (i = 0; i < acl->a_count; i++) {

		entry->e_tag  = cpu_to_le16(acl->a_entries[i].e_tag);
		entry->e_perm = cpu_to_le16(acl->a_entries[i].e_perm);

		switch (acl->a_entries[i].e_tag) {
		case ACL_USER:
			entry->e_id = cpu_to_le32(
					from_kuid(&init_user_ns,
						acl->a_entries[i].e_uid));
			entry = (struct f2fs_acl_entry *)((char *)entry +
					sizeof(struct f2fs_acl_entry));
			break;
		case ACL_GROUP:
			entry->e_id = cpu_to_le32(
					from_kgid(&init_user_ns,
						acl->a_entries[i].e_gid));
			entry = (struct f2fs_acl_entry *)((char *)entry +
					sizeof(struct f2fs_acl_entry));
			break;
		case ACL_USER_OBJ:
		case ACL_GROUP_OBJ:
		case ACL_MASK:
		case ACL_OTHER:
			entry = (struct f2fs_acl_entry *)((char *)entry +
					sizeof(struct f2fs_acl_entry_short));
			break;
		default:
			goto fail;
		}
	}
	*size = f2fs_acl_size(acl->a_count);
	return (void *)f2fs_acl;

fail:
	kfree(f2fs_acl);
	return ERR_PTR(-EINVAL);
}

<<<<<<< HEAD
static struct posix_acl *__f2fs_get_acl(struct inode *inode, int type,
						struct page *dpage)
=======
struct posix_acl *f2fs_get_acl(struct inode *inode, int type)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
{
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
	int name_index = F2FS_XATTR_INDEX_POSIX_ACL_DEFAULT;
	void *value = NULL;
	struct posix_acl *acl;
	int retval;

	if (!test_opt(sbi, POSIX_ACL))
		return NULL;

	acl = get_cached_acl(inode, type);
	if (acl != ACL_NOT_CACHED)
		return acl;

	if (type == ACL_TYPE_ACCESS)
		name_index = F2FS_XATTR_INDEX_POSIX_ACL_ACCESS;

<<<<<<< HEAD
	retval = f2fs_getxattr(inode, name_index, "", NULL, 0, dpage);
	if (retval > 0) {
		value = f2fs_kmalloc(F2FS_I_SB(inode), retval, GFP_F2FS_ZERO);
		if (!value)
			return ERR_PTR(-ENOMEM);
		retval = f2fs_getxattr(inode, name_index, "", value,
							retval, dpage);
=======
	retval = f2fs_getxattr(inode, name_index, "", NULL, 0);
	if (retval > 0) {
		value = kmalloc(retval, GFP_KERNEL);
		if (!value)
			return ERR_PTR(-ENOMEM);
		retval = f2fs_getxattr(inode, name_index, "", value, retval);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	}

	if (retval > 0)
		acl = f2fs_acl_from_disk(value, retval);
	else if (retval == -ENODATA)
		acl = NULL;
	else
		acl = ERR_PTR(retval);
	kfree(value);

	if (!IS_ERR(acl))
		set_cached_acl(inode, type, acl);

	return acl;
}

<<<<<<< HEAD
struct posix_acl *f2fs_get_acl(struct inode *inode, int type)
{
	return __f2fs_get_acl(inode, type, NULL);
}

static int f2fs_set_acl(struct inode *inode, int type,
			struct posix_acl *acl, struct page *ipage)
{
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
=======
static int f2fs_set_acl(struct inode *inode, int type, struct posix_acl *acl)
{
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
	struct f2fs_inode_info *fi = F2FS_I(inode);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	int name_index;
	void *value = NULL;
	size_t size = 0;
	int error;

	if (!test_opt(sbi, POSIX_ACL))
		return 0;
	if (S_ISLNK(inode->i_mode))
		return -EOPNOTSUPP;

	switch (type) {
	case ACL_TYPE_ACCESS:
		name_index = F2FS_XATTR_INDEX_POSIX_ACL_ACCESS;
		if (acl) {
			error = posix_acl_equiv_mode(acl, &inode->i_mode);
			if (error < 0)
				return error;
<<<<<<< HEAD
			set_acl_inode(inode, inode->i_mode);
=======
			set_acl_inode(fi, inode->i_mode);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
			if (error == 0)
				acl = NULL;
		}
		break;

	case ACL_TYPE_DEFAULT:
		name_index = F2FS_XATTR_INDEX_POSIX_ACL_DEFAULT;
		if (!S_ISDIR(inode->i_mode))
			return acl ? -EACCES : 0;
		break;

	default:
		return -EINVAL;
	}

<<<<<<< HEAD
	f2fs_mark_inode_dirty_sync(inode, true);

	if (acl) {
		value = f2fs_acl_to_disk(F2FS_I_SB(inode), acl, &size);
		if (IS_ERR(value)) {
			clear_inode_flag(inode, FI_ACL_MODE);
=======
	if (acl) {
		value = f2fs_acl_to_disk(acl, &size);
		if (IS_ERR(value)) {
			cond_clear_inode_flag(fi, FI_ACL_MODE);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
			return (int)PTR_ERR(value);
		}
	}

<<<<<<< HEAD
	error = f2fs_setxattr(inode, name_index, "", value, size, ipage, 0);
=======
	error = f2fs_setxattr(inode, name_index, "", value, size);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

	kfree(value);
	if (!error)
		set_cached_acl(inode, type, acl);

<<<<<<< HEAD
	clear_inode_flag(inode, FI_ACL_MODE);
	return error;
}

int f2fs_init_acl(struct inode *inode, struct inode *dir, struct page *ipage,
							struct page *dpage)
{
	struct f2fs_sb_info *sbi = F2FS_SB(dir->i_sb);
	struct posix_acl *acl = NULL;
=======
	cond_clear_inode_flag(fi, FI_ACL_MODE);
	return error;
}

int f2fs_init_acl(struct inode *inode, struct inode *dir)
{
	struct posix_acl *acl = NULL;
	struct f2fs_sb_info *sbi = F2FS_SB(dir->i_sb);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	int error = 0;

	if (!S_ISLNK(inode->i_mode)) {
		if (test_opt(sbi, POSIX_ACL)) {
<<<<<<< HEAD
			acl = __f2fs_get_acl(dir, ACL_TYPE_DEFAULT, dpage);
=======
			acl = f2fs_get_acl(dir, ACL_TYPE_DEFAULT);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
			if (IS_ERR(acl))
				return PTR_ERR(acl);
		}
		if (!acl)
			inode->i_mode &= ~current_umask();
	}

<<<<<<< HEAD
	if (!test_opt(sbi, POSIX_ACL) || !acl)
		goto cleanup;

	if (S_ISDIR(inode->i_mode)) {
		error = f2fs_set_acl(inode, ACL_TYPE_DEFAULT, acl, ipage);
		if (error)
			goto cleanup;
	}
	error = posix_acl_create(&acl, GFP_KERNEL, &inode->i_mode);
	if (error < 0)
		return error;
	if (error > 0)
		error = f2fs_set_acl(inode, ACL_TYPE_ACCESS, acl, ipage);

	f2fs_mark_inode_dirty_sync(inode, true);
=======
	if (test_opt(sbi, POSIX_ACL) && acl) {

		if (S_ISDIR(inode->i_mode)) {
			error = f2fs_set_acl(inode, ACL_TYPE_DEFAULT, acl);
			if (error)
				goto cleanup;
		}
		error = posix_acl_create(&acl, GFP_KERNEL, &inode->i_mode);
		if (error < 0)
			return error;
		if (error > 0)
			error = f2fs_set_acl(inode, ACL_TYPE_ACCESS, acl);
	}
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
cleanup:
	posix_acl_release(acl);
	return error;
}

int f2fs_acl_chmod(struct inode *inode)
{
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
	struct posix_acl *acl;
	int error;
	umode_t mode = get_inode_mode(inode);

	if (!test_opt(sbi, POSIX_ACL))
		return 0;
	if (S_ISLNK(mode))
		return -EOPNOTSUPP;

	acl = f2fs_get_acl(inode, ACL_TYPE_ACCESS);
	if (IS_ERR(acl) || !acl)
		return PTR_ERR(acl);

	error = posix_acl_chmod(&acl, GFP_KERNEL, mode);
	if (error)
		return error;
<<<<<<< HEAD

	error = f2fs_set_acl(inode, ACL_TYPE_ACCESS, acl, NULL);
=======
	error = f2fs_set_acl(inode, ACL_TYPE_ACCESS, acl);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	posix_acl_release(acl);
	return error;
}

static size_t f2fs_xattr_list_acl(struct dentry *dentry, char *list,
		size_t list_size, const char *name, size_t name_len, int type)
{
	struct f2fs_sb_info *sbi = F2FS_SB(dentry->d_sb);
	const char *xname = POSIX_ACL_XATTR_DEFAULT;
	size_t size;

	if (!test_opt(sbi, POSIX_ACL))
		return 0;

	if (type == ACL_TYPE_ACCESS)
		xname = POSIX_ACL_XATTR_ACCESS;

	size = strlen(xname) + 1;
	if (list && size <= list_size)
		memcpy(list, xname, size);
	return size;
}

static int f2fs_xattr_get_acl(struct dentry *dentry, const char *name,
		void *buffer, size_t size, int type)
{
	struct f2fs_sb_info *sbi = F2FS_SB(dentry->d_sb);
	struct posix_acl *acl;
	int error;

	if (strcmp(name, "") != 0)
		return -EINVAL;
	if (!test_opt(sbi, POSIX_ACL))
		return -EOPNOTSUPP;

<<<<<<< HEAD
	acl = f2fs_get_acl(d_inode(dentry), type);
=======
	acl = f2fs_get_acl(dentry->d_inode, type);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	if (IS_ERR(acl))
		return PTR_ERR(acl);
	if (!acl)
		return -ENODATA;
	error = posix_acl_to_xattr(&init_user_ns, acl, buffer, size);
	posix_acl_release(acl);

	return error;
}

static int f2fs_xattr_set_acl(struct dentry *dentry, const char *name,
		const void *value, size_t size, int flags, int type)
{
	struct f2fs_sb_info *sbi = F2FS_SB(dentry->d_sb);
<<<<<<< HEAD
	struct inode *inode = d_inode(dentry);
=======
	struct inode *inode = dentry->d_inode;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	struct posix_acl *acl = NULL;
	int error;

	if (strcmp(name, "") != 0)
		return -EINVAL;
	if (!test_opt(sbi, POSIX_ACL))
		return -EOPNOTSUPP;
	if (!inode_owner_or_capable(inode))
		return -EPERM;

	if (value) {
		acl = posix_acl_from_xattr(&init_user_ns, value, size);
		if (IS_ERR(acl))
			return PTR_ERR(acl);
		if (acl) {
			error = posix_acl_valid(acl);
			if (error)
				goto release_and_out;
		}
	} else {
		acl = NULL;
	}

<<<<<<< HEAD
	error = f2fs_set_acl(inode, type, acl, NULL);
=======
	error = f2fs_set_acl(inode, type, acl);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

release_and_out:
	posix_acl_release(acl);
	return error;
}

const struct xattr_handler f2fs_xattr_acl_default_handler = {
	.prefix = POSIX_ACL_XATTR_DEFAULT,
	.flags = ACL_TYPE_DEFAULT,
	.list = f2fs_xattr_list_acl,
	.get = f2fs_xattr_get_acl,
	.set = f2fs_xattr_set_acl,
};

const struct xattr_handler f2fs_xattr_acl_access_handler = {
	.prefix = POSIX_ACL_XATTR_ACCESS,
	.flags = ACL_TYPE_ACCESS,
	.list = f2fs_xattr_list_acl,
	.get = f2fs_xattr_get_acl,
	.set = f2fs_xattr_set_acl,
};

/*
 * Workqueue for crypto subsystem
 *
 * Copyright (c) 2009 Intel Corp.
 *   Author: Huang Ying <ying.huang@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 */

#include <linux/workqueue.h>
#include <linux/module.h>
#include <crypto/algapi.h>
#include <crypto/crypto_wq.h>

struct workqueue_struct *kcrypto_wq;
EXPORT_SYMBOL_GPL(kcrypto_wq);

static int __init crypto_wq_init(void)
{
	kcrypto_wq = alloc_workqueue("crypto",
				     WQ_MEM_RECLAIM | WQ_CPU_INTENSIVE, 1);
	if (unlikely(!kcrypto_wq))
		return -ENOMEM;
	return 0;
}

static void __exit crypto_wq_exit(void)
{
	destroy_workqueue(kcrypto_wq);
}

<<<<<<< HEAD
subsys_initcall(crypto_wq_init);
=======
module_init(crypto_wq_init);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
module_exit(crypto_wq_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Workqueue for crypto subsystem");

/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __MACH_COMMON_H
#define __MACH_COMMON_H

extern void msm7x01_timer_init(void);
extern void msm7x30_timer_init(void);
extern void msm_dt_timer_init(void);
extern void qsd8x50_timer_init(void);

extern void msm_map_common_io(void);
extern void msm_map_msm7x30_io(void);
extern void msm_map_msm8x60_io(void);
extern void msm_map_msm8960_io(void);
extern void msm_map_qsd8x50_io(void);

<<<<<<< HEAD
extern void __iomem *__msm_ioremap_caller(phys_addr_t phys_addr, size_t size,
=======
extern void __iomem *__msm_ioremap_caller(unsigned long phys_addr, size_t size,
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
					  unsigned int mtype, void *caller);

extern struct smp_operations msm_smp_ops;
extern void msm_cpu_die(unsigned int cpu);

#endif

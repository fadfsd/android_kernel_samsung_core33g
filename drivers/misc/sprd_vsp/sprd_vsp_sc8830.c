/*
 * Copyright (C) 2012 Spreadtrum Communications Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/clk.h>
#include <linux/debugfs.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/clk.h>
<<<<<<< HEAD
#ifdef CONFIG_OF
#include <linux/clk-provider.h>
#endif
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#include <linux/semaphore.h>
#include <linux/slab.h>
#include <linux/wakelock.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

#include <video/sprd_vsp.h>

<<<<<<< HEAD
#include <mach/sci.h>
#include <mach/sci_glb_regs.h>
#include <mach/globalregs.h>
=======
#include <mach/hardware.h>
#include <mach/irqs.h>
#include <mach/globalregs.h>
#include <mach/sci.h>

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#include <linux/sprd_iommu.h>

#define VSP_MINOR MISC_DYNAMIC_MINOR
#define VSP_AQUIRE_TIMEOUT_MS 500
#define VSP_INIT_TIMEOUT_MS 200

#define USE_INTERRUPT
/*#define RT_VSP_THREAD*/

#define DEFAULT_FREQ_DIV 0x0

#define ARM_ACCESS_CTRL_OFF         0x0
#define ARM_ACCESS_STATUS_OFF   0x04
#define MCU_CTRL_SET_OFF                0x08
#define ARM_INT_STS_OFF                     0x10        //from OPENRISC
#define ARM_INT_MASK_OFF                0x14
#define ARM_INT_CLR_OFF                     0x18
#define ARM_INT_RAW_OFF                 0x1C
#define WB_ADDR_SET0_OFF                0x20
#define WB_ADDR_SET1_OFF                0x24

<<<<<<< HEAD
#define SCI_IOMAP_BASE  0xF5000000
#define SCI_IOMAP(x)    (SCI_IOMAP_BASE + (x))
#define SPRD_MMAHB_BASE                        SCI_IOMAP(0x340000)
#define SPRD_AONAPB_BASE                SCI_IOMAP(0x250000)

static unsigned long SPRD_VSP_PHYS = 0;
static unsigned long SPRD_VSP_BASE = 0;
static unsigned long VSP_GLB_REG_BASE = 0;
=======
#ifndef CONFIG_OF
#define VSP_GLB_REG_BASE        (SPRD_VSP_BASE+0x1000)
#define SPRD_VSP_BASE_DT SPRD_VSP_BASE
#define SPRD_MMAHB_BASE_DT SPRD_MMAHB_BASE
#define SPRD_AONAPB_BASE_DT SPRD_AONAPB_BASE
#else
#define         clk_enable      clk_prepare_enable
#define         clk_disable     clk_disable_unprepare
static unsigned int SPRD_VSP_BASE_DT;
static unsigned int VSP_GLB_REG_BASE;

//will be removed later
#define SPRD_MMAHB_BASE_DT		SPRD_MMAHB_BASE
#define SPRD_AONAPB_BASE_DT		SPRD_AONAPB_BASE
#endif
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

#define VSP_INT_STS_OFF            0x0             //from VSP
#define VSP_INT_MASK_OFF        0x04
#define VSP_INT_CLR_OFF           0x08
#define VSP_INT_RAW_OFF         0x0c

struct vsp_fh {
    int is_vsp_aquired;
    int is_clock_enabled;

    wait_queue_head_t wait_queue_work;
    int condition_work;
    int vsp_int_status;
};

struct vsp_dev {
    unsigned int freq_div;

    struct semaphore vsp_mutex;

    struct clk *vsp_clk;
    struct clk *vsp_parent_clk;
    struct clk *mm_clk;
<<<<<<< HEAD
    struct clk *mm_clk_axi;

    unsigned int irq;
    unsigned int version;

    struct vsp_fh *vsp_fp;
    struct device_node *dev_np;
    bool  light_sleep_en;
=======

    unsigned int irq;

    struct vsp_fh *vsp_fp;
    struct device_node *dev_np;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
};

static struct vsp_dev vsp_hw_dev;
static struct wake_lock vsp_wakelock;
static atomic_t vsp_instance_cnt = ATOMIC_INIT(0);

struct clock_name_map_t {
    unsigned long freq;
    char *name;
};

<<<<<<< HEAD
#ifdef CONFIG_OF
static struct clock_name_map_t clock_name_map[SPRD_VSP_CLK_LEVEL_NUM];
#else
static struct clock_name_map_t clock_name_map[] = {
#if defined(CONFIG_ARCH_SCX35LT8)
    {96000000,"clk_96m"},
    {128000000,"clk_128m"},
    {256000000,"clk_256m"},
    {307200000,"clk_307m2"}
#elif defined(CONFIG_ARCH_SCX35L)
     {76800000,"clk_76m8"},
    {128000000,"clk_128m"},
    {256000000,"clk_256m"},
    {312000000,"clk_312m"}
#elif defined(CONFIG_ARCH_SCX15)
    {76800000,"clk_76m8"}
    {128000000,"clk_128m"},
    {153600000,"clk_153m6"},
    {192000000,"clk_192m"}
#elif defined(CONFIG_ARCH_SCX20)
    {76800000,"clk_76m8"},
    {128000000,"clk_128m"},
    {192000000,"clk_192m"}
#else
    {76800000,"clk_76m8"},
    {128000000,"clk_128m"},
    {192000000,"clk_192m"},
    {256000000,"clk_256m"}
#endif
};
#endif

static int max_freq_level = SPRD_VSP_CLK_LEVEL_NUM;
=======
#if defined(CONFIG_ARCH_SCX15)
static struct clock_name_map_t clock_name_map[] = {
    {192000000,"clk_192m"},
    {153600000,"clk_153m6"},
    {128000000,"clk_128m"},
    {76800000,"clk_76m8"}
};
#else
static struct clock_name_map_t clock_name_map[] = {
    {256000000,"clk_256m"},
    {192000000,"clk_192m"},
    {128000000,"clk_128m"},
    {76800000,"clk_76m8"}
};
#endif

static int max_freq_level = ARRAY_SIZE(clock_name_map);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

static char *vsp_get_clk_src_name(unsigned int freq_level)
{
    if (freq_level >= max_freq_level ) {
        printk(KERN_INFO "set freq_level to 0");
        freq_level = 0;
    }

    return clock_name_map[freq_level].name;
}

static int find_vsp_freq_level(unsigned long freq)
{
    int level = 0;
    int i;
    for (i = 0; i < max_freq_level; i++) {
        if (clock_name_map[i].freq == freq) {
            level = i;
            break;
        }
    }
    return level;
}

#if defined(CONFIG_ARCH_SCX35)
#ifdef USE_INTERRUPT
static irqreturn_t vsp_isr(int irq, void *data);
#endif
#endif
static long vsp_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret;
    struct clk *clk_parent;
    char *name_parent;
    unsigned long frequency;
    struct vsp_fh *vsp_fp = filp->private_data;

    if (vsp_fp == NULL) {
        printk(KERN_ERR "vsp_ioctl error occured, vsp_fp == NULL\n");
        return  -EINVAL;
    }

    switch (cmd) {
    case VSP_CONFIG_FREQ:
        get_user(vsp_hw_dev.freq_div, (int __user *)arg);
        name_parent = vsp_get_clk_src_name(vsp_hw_dev.freq_div);
        clk_parent = clk_get(NULL, name_parent);
        if ((!clk_parent )|| IS_ERR(clk_parent)) {
            printk(KERN_ERR "clock[%s]: failed to get parent [%s] by clk_get()!\n", "clk_vsp", name_parent);
            return -EINVAL;
        }
        ret = clk_set_parent(vsp_hw_dev.vsp_clk, clk_parent);
        if (ret) {
            printk(KERN_ERR "clock[%s]: clk_set_parent() failed!","clk_vsp");
            return -EINVAL;
        } else {
            clk_put(vsp_hw_dev.vsp_parent_clk);
            vsp_hw_dev.vsp_parent_clk = clk_parent;
        }
        pr_debug(KERN_INFO "VSP_CONFIG_FREQ %d\n", vsp_hw_dev.freq_div);
        break;
    case VSP_GET_FREQ:
        frequency = clk_get_rate(vsp_hw_dev.vsp_clk);
        ret = find_vsp_freq_level(frequency);
        put_user(ret, (int __user *)arg);
        pr_debug(KERN_INFO "vsp ioctl VSP_GET_FREQ %d\n", ret);
        break;
    case VSP_ENABLE:
        pr_debug("vsp ioctl VSP_ENABLE\n");
        wake_lock(&vsp_wakelock);
<<<<<<< HEAD
        ret = clk_prepare_enable(vsp_hw_dev.vsp_clk);
=======
        ret = clk_enable(vsp_hw_dev.vsp_clk);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
        if (ret) {
            printk(KERN_ERR "###:vsp_hw_dev.vsp_clk: clk_enable() failed!\n");
            return ret;
        } else {
            pr_debug("###vsp_hw_dev.vsp_clk: clk_enable() ok.\n");
        }
#ifdef CONFIG_OF
<<<<<<< HEAD
        sci_glb_set(SPRD_MMAHB_BASE+0x08, BIT(5));
=======
        sci_glb_set(SPRD_MMAHB_BASE_DT+0x08, BIT(5));
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#endif
        vsp_fp->is_clock_enabled= 1;
        break;
    case VSP_DISABLE:
        pr_debug("vsp ioctl VSP_DISABLE\n");
<<<<<<< HEAD
        if(1 == vsp_fp->is_clock_enabled)
        {
            clk_disable_unprepare(vsp_hw_dev.vsp_clk);
        }

#ifdef CONFIG_OF
        sci_glb_clr(SPRD_MMAHB_BASE+0x08, BIT(5));
=======
        clk_disable(vsp_hw_dev.vsp_clk);
#ifdef CONFIG_OF
        sci_glb_clr(SPRD_MMAHB_BASE_DT+0x08, BIT(5));
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#endif
        vsp_fp->is_clock_enabled = 0;
        wake_unlock(&vsp_wakelock);
        break;
    case VSP_ACQUAIRE:
        pr_debug("vsp ioctl VSP_ACQUAIRE begin\n");
        ret = down_timeout(&vsp_hw_dev.vsp_mutex,
                           msecs_to_jiffies(VSP_AQUIRE_TIMEOUT_MS));
        if (ret) {
            printk(KERN_ERR "vsp error timeout\n");
            //up(&vsp_hw_dev.vsp_mutex);
            return ret;
        }
#ifdef RT_VSP_THREAD
        if (!rt_task(current)) {
            struct sched_param schedpar;
            int ret;
            struct cred *new = prepare_creds();
            cap_raise(new->cap_effective, CAP_SYS_NICE);
            commit_creds(new);
            schedpar.sched_priority = 1;
            ret = sched_setscheduler(current, SCHED_RR, &schedpar);
            if (ret!=0)
                printk(KERN_ERR "vsp change pri fail a\n");
        }
#endif
<<<<<<< HEAD

        vsp_hw_dev.vsp_fp = vsp_fp;

        if (vsp_hw_dev.light_sleep_en) {
            pr_debug("VSP mmi_clk open\n");
            ret = clk_prepare_enable(vsp_hw_dev.mm_clk);
            if (ret) {
                printk(KERN_ERR "###:vsp_hw_dev.mm_clk: clk_prepare_enable() failed!\n");
                return ret;
            } else {
                pr_debug("###vsp_hw_dev.mm_clk: clk_prepare_enable() ok.\n");
            }
#if defined(CONFIG_SPRD_IOMMU)
            sprd_iommu_module_enable(IOMMU_MM);
#endif
        }
        vsp_fp->is_vsp_aquired = 1;
=======
        vsp_fp->is_vsp_aquired = 1;
        vsp_hw_dev.vsp_fp = vsp_fp;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
        pr_debug("vsp ioctl VSP_ACQUAIRE end\n");
        break;
    case VSP_RELEASE:
        pr_debug("vsp ioctl VSP_RELEASE\n");
<<<<<<< HEAD

        if (vsp_hw_dev.light_sleep_en) {
#if defined(CONFIG_SPRD_IOMMU)
            sprd_iommu_module_disable(IOMMU_MM);
#endif
            if(1 == vsp_fp->is_vsp_aquired)
            {
                clk_disable_unprepare(vsp_hw_dev.mm_clk);
            }
            pr_debug("VSP mmi_clk close\n");
        }
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
        vsp_fp->is_vsp_aquired = 0;
        vsp_hw_dev.vsp_fp = NULL;
        up(&vsp_hw_dev.vsp_mutex);
        break;
#ifdef USE_INTERRUPT
    case VSP_COMPLETE:
        pr_debug("vsp ioctl VSP_COMPLETE\n");
        ret = wait_event_interruptible_timeout(
                  vsp_fp->wait_queue_work,
                  vsp_fp->condition_work,
                  msecs_to_jiffies(VSP_INIT_TIMEOUT_MS));
        if (ret == -ERESTARTSYS) {
            printk(KERN_INFO "vsp complete -ERESTARTSYS\n");
            vsp_fp->vsp_int_status |= (1<<30);
            put_user(vsp_fp->vsp_int_status, (int __user *)arg);
            ret = -EINVAL;
        } else
        {
            vsp_fp->vsp_int_status &= (~ (1<<30));
            if (ret == 0) {
                printk(KERN_ERR "vsp complete  timeout\n");
                vsp_fp->vsp_int_status |= (1<<31);
                ret = -ETIMEDOUT;
                /*clear vsp int*/
<<<<<<< HEAD
                __raw_writel((1<<1) |(1<<2)|(1<<4)|(1<<5), (void *)(VSP_GLB_REG_BASE+VSP_INT_CLR_OFF));
                __raw_writel((1<<0)|(1<<1)|(1<<2), (void *)(SPRD_VSP_BASE+ARM_INT_CLR_OFF));
=======
                __raw_writel((1<<1) |(1<<2)|(1<<4)|(1<<5), VSP_GLB_REG_BASE+VSP_INT_CLR_OFF);
                __raw_writel((1<<0)|(1<<1)|(1<<2), SPRD_VSP_BASE_DT+ARM_INT_CLR_OFF);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
            } else {
                ret = 0;
            }
            put_user(vsp_fp->vsp_int_status, (int __user *)arg);
            vsp_fp->vsp_int_status = 0;
            vsp_fp->condition_work = 0;
        }
        pr_debug("vsp ioctl VSP_COMPLETE end\n");
        return ret;
        break;
#endif
    case VSP_RESET:
        pr_debug("vsp ioctl VSP_RESET\n");
<<<<<<< HEAD
        sci_glb_set(SPRD_MMAHB_BASE+0x04, BIT(4));
        sci_glb_clr(SPRD_MMAHB_BASE+0x04, BIT(4));
=======
        sci_glb_set(SPRD_MMAHB_BASE_DT+0x04, BIT(4));
        sci_glb_clr(SPRD_MMAHB_BASE_DT+0x04, BIT(4));
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
        break;
    case VSP_HW_INFO:
    {
        u32 mm_eb_reg;

        pr_debug("vsp ioctl VSP_HW_INFO\n");
<<<<<<< HEAD
        mm_eb_reg = sci_glb_read(SPRD_AONAPB_BASE, 0xFFFFFFFF);
=======
        mm_eb_reg = sci_glb_read(SPRD_AONAPB_BASE_DT, 0xFFFFFFFF);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
        put_user(mm_eb_reg, (int __user *)arg);
    }
    break;

<<<<<<< HEAD
    case VSP_VERSION:
    {
        printk(KERN_INFO "vsp version -enter\n");
        put_user(vsp_hw_dev.version, (int __user *)arg);
=======
    case VSP_CAPABILITY:
    {
        u32 vsp_capability = 3;

        printk(KERN_INFO "vsp capability -enter\n");
#if defined(CONFIG_ARCH_SCX30G)
        vsp_capability =   2;
#elif defined(CONFIG_ARCH_SCX15)
        vsp_capability = 0;
#else
        vsp_capability = 1;
#endif
        put_user(vsp_capability, (int __user *)arg);
        printk(KERN_INFO "vsp capability -%d\n", vsp_capability);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
    }
    break;

    default:
        return -EINVAL;
    }
    return 0;
}

#ifdef USE_INTERRUPT
static irqreturn_t vsp_isr(int irq, void *data)
{
    int int_status;
    int ret = 0xff; // 0xff : invalid
    struct vsp_fh *vsp_fp = vsp_hw_dev.vsp_fp;

    if (vsp_fp == NULL) {
<<<<<<< HEAD
        //printk(KERN_ERR "vsp_isr error occured, vsp_fp == NULL\n");
=======
        printk(KERN_ERR "vsp_isr error occured, vsp_fp == NULL\n");
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
        return  IRQ_NONE;
    }

    //check which module occur interrupt and clear coresponding bit
<<<<<<< HEAD
    int_status =  __raw_readl((void *)(VSP_GLB_REG_BASE+VSP_INT_STS_OFF));
    if((int_status >> 0) & 0x1) //BSM_BUF_OVF DONE
    {
        __raw_writel((1<<0), (void *)(VSP_GLB_REG_BASE+VSP_INT_CLR_OFF));
        ret = (1 << 0);
    } else if((int_status >> 1) & 0x1) // VLC SLICE DONE
    {
        __raw_writel((1<<1), (void *)(VSP_GLB_REG_BASE+VSP_INT_CLR_OFF));
        ret = (1<<1);
    } else if((int_status >> 2) & 0x1) // MBW SLICE DONE
    {
        __raw_writel((1<<2), (void *)(VSP_GLB_REG_BASE+VSP_INT_CLR_OFF));
        ret = (1<<2);
    } else if((int_status >> 4) & 0x1) // VLD ERR
    {
        __raw_writel((1<<4), (void *)(VSP_GLB_REG_BASE+VSP_INT_CLR_OFF));
        ret = (1<<4);
    } else if((int_status >> 5) & 0x1) // TIMEOUT ERR
    {
        __raw_writel((1<<5), (void *)(VSP_GLB_REG_BASE+VSP_INT_CLR_OFF));
        ret = (1<<5);
    } else
    {
        return IRQ_NONE;
    }

    //clear VSP accelerator interrupt bit
    int_status =  __raw_readl((void *)(SPRD_VSP_BASE+ARM_INT_STS_OFF));
    if ((int_status >> 2) & 0x1) //VSP ACC INT
    {
        __raw_writel((1<<2), (void *)(SPRD_VSP_BASE+ARM_INT_CLR_OFF));
=======
    int_status =  __raw_readl(VSP_GLB_REG_BASE+VSP_INT_STS_OFF);
    if((int_status >> 1) & 0x1) // VLC SLICE DONE
    {
        __raw_writel((1<<1), VSP_GLB_REG_BASE+VSP_INT_CLR_OFF);
        ret = (1<<1);
    } else if((int_status >> 2) & 0x1) // MBW SLICE DONE
    {
        __raw_writel((1<<2), VSP_GLB_REG_BASE+VSP_INT_CLR_OFF);
        ret = (1<<2);
    } else if((int_status >> 4) & 0x1) // VLD ERR
    {
        __raw_writel((1<<4), VSP_GLB_REG_BASE+VSP_INT_CLR_OFF);
        ret = (1<<4);
    } else if((int_status >> 5) & 0x1) // TIMEOUT ERR
    {
        __raw_writel((1<<5), VSP_GLB_REG_BASE+VSP_INT_CLR_OFF);
        ret = (1<<5);
    }

    //clear VSP accelerator interrupt bit
    int_status =  __raw_readl(SPRD_VSP_BASE_DT+ARM_INT_STS_OFF);
    if ((int_status >> 2) & 0x1) //VSP ACC INT
    {
        __raw_writel((1<<2), SPRD_VSP_BASE_DT+ARM_INT_CLR_OFF);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
    }

    vsp_fp->vsp_int_status = ret;
    vsp_fp->condition_work = 1;
    wake_up_interruptible(&vsp_fp->wait_queue_work);

    return IRQ_HANDLED;
}
#endif

#ifdef CONFIG_OF
static const struct of_device_id  of_match_table_vsp[] = {
    { .compatible = "sprd,sprd_vsp", },
    { },
};

static int vsp_parse_dt(struct device *dev)
{
    struct device_node *np = dev->of_node;
<<<<<<< HEAD
    struct device_node *vsp_clk_np = NULL;
    char *vsp_clk_node_name = NULL;
    struct resource res;
    int i, ret, clk_count = 0;
=======
    struct resource res;
    int ret;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

    printk(KERN_INFO "vsp_parse_dt called !\n");

    ret = of_address_to_resource(np, 0, &res);
    if(ret < 0) {
        dev_err(dev, "no reg of property specified\n");
        printk(KERN_ERR "vsp: failed to parse_dt!\n");
        return -EINVAL;
    }
<<<<<<< HEAD

    SPRD_VSP_PHYS = res.start;
    SPRD_VSP_BASE = (unsigned long)ioremap_nocache(res.start,
                    resource_size(&res));
    if(!SPRD_VSP_BASE)
        BUG();

    VSP_GLB_REG_BASE = SPRD_VSP_BASE + 0x1000;

    printk(KERN_INFO "sprd_vsp_phys = %lx\n", SPRD_VSP_PHYS);
    printk(KERN_INFO "sprd_vsp_base = %lx\n", SPRD_VSP_BASE);
    printk(KERN_INFO "vsp_glb_reg_base = %lx\n", VSP_GLB_REG_BASE);

    ret = of_property_read_u32(np, "version", &(vsp_hw_dev.version));
    if(0 != ret) {
        printk(KERN_ERR "vsp: read version fail (%d)\n", ret);
        return -EINVAL;
    }

    if (vsp_hw_dev.version == PIKE){
        max_freq_level = SPRD_VSP_CLK_LEVEL_NUM_PIKE;
    }
=======
    SPRD_VSP_BASE_DT = SPRD_VSP_BASE;//res.start;
    VSP_GLB_REG_BASE = SPRD_VSP_BASE_DT + 0x1000;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

    vsp_hw_dev.irq = irq_of_parse_and_map(np, 0);
    vsp_hw_dev.dev_np = np;

<<<<<<< HEAD
    printk(KERN_INFO "vsp: irq = 0x%x, version = 0x%0x\n", vsp_hw_dev.irq, vsp_hw_dev.version);

    vsp_clk_node_name = of_clk_get_parent_name(np, 1); //This position is based on related dts file
    printk(KERN_INFO "vsp_node_name = %s\n", vsp_clk_node_name);

    vsp_clk_np = of_find_node_by_name(NULL, vsp_clk_node_name);
    if (!vsp_clk_np) {
        printk(KERN_ERR "failed to get clk_vsp device node\n");
        return -EINVAL;
    }

    clk_count = of_clk_get_parent_count(vsp_clk_np);
    if(clk_count != max_freq_level) {
        printk(KERN_ERR "failed to get vsp clock count\n");
        return -EINVAL;
    }

    for(i = 0; i < clk_count; i++) {
        struct clk *clk_parent;
        char *name_parent;
        unsigned long frequency;

        name_parent = of_clk_get_parent_name(vsp_clk_np,  i);
        clk_parent = clk_get(NULL, name_parent);
        frequency = clk_get_rate(clk_parent);
        printk(KERN_INFO "vsp clk order in dts file: clk[%d] = (%d, %s)\n", i, frequency, name_parent);

        clock_name_map[i].name = name_parent;
        clock_name_map[i].freq = frequency;
    }

=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
    return 0;
}
#else
static int  vsp_parse_dt(
    struct device *dev)
{
    vsp_hw_dev.irq = IRQ_VSP_INT;
<<<<<<< HEAD
    vsp_hw_dev.version = 0;
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
    return 0;
}
#endif

static int vsp_nocache_mmap(struct file *filp, struct vm_area_struct *vma)
{
    printk(KERN_INFO "@vsp[%s]\n", __FUNCTION__);
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    vma->vm_pgoff     = (SPRD_VSP_PHYS>>PAGE_SHIFT);
    if (remap_pfn_range(vma,vma->vm_start, vma->vm_pgoff,
                        vma->vm_end - vma->vm_start, vma->vm_page_prot))
        return -EAGAIN;
<<<<<<< HEAD
    printk(KERN_INFO "@vsp mmap %x,%lx,%x\n", (unsigned int)PAGE_SHIFT,
           (unsigned long)vma->vm_start,
=======
    printk(KERN_INFO "@vsp mmap %x,%x,%x\n", (unsigned int)PAGE_SHIFT,
           (unsigned int)vma->vm_start,
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
           (unsigned int)(vma->vm_end - vma->vm_start));
    return 0;
}

static int vsp_set_mm_clk(void)
{
    int ret =0;
<<<<<<< HEAD
    struct clk *clk_mm_axi;
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
    struct clk *clk_mm_i;
    struct clk *clk_vsp;
    struct clk *clk_parent;
    char *name_parent;
    int instance_cnt = atomic_read(&vsp_instance_cnt);

    printk(KERN_INFO "vsp_set_mm_clk: vsp_instance_cnt %d\n", instance_cnt);

#if defined(CONFIG_ARCH_SCX35)

#ifdef CONFIG_OF
<<<<<<< HEAD
    clk_mm_axi = of_clk_get_by_name(vsp_hw_dev.dev_np, "clk_mm_axi");
    if (IS_ERR(clk_mm_axi) || (!clk_mm_axi)) {
        printk(KERN_ERR "###: Failed : Can't get clock [%s}!\n",
               "clk_mm_axi");
        printk(KERN_ERR "###: clk_mm_axi =  %p\n", clk_mm_axi);
        vsp_hw_dev.mm_clk_axi = NULL;
        vsp_hw_dev.light_sleep_en = false;
    } else {
        vsp_hw_dev.mm_clk_axi= clk_mm_axi;
        vsp_hw_dev.light_sleep_en = true;
    }
#endif

    if (vsp_hw_dev.mm_clk_axi) {
        printk(KERN_INFO "VSP mmi_clk_axi open");
        ret = clk_prepare(vsp_hw_dev.mm_clk_axi);
        if (ret) {
            printk(KERN_ERR "###:vsp_hw_dev.mm_clk_axi: clk_prepare() failed!\n");
            goto errout0;
        } else {
            pr_debug("###vsp_hw_dev.mm_clk_axi: clk_prepare() ok.\n");
        }
    }

#ifdef CONFIG_OF
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
    clk_mm_i = of_clk_get_by_name(vsp_hw_dev.dev_np, "clk_mm_i");
#else
    clk_mm_i = clk_get(NULL, "clk_mm_i");
#endif
    if (IS_ERR(clk_mm_i) || (!clk_mm_i)) {
        printk(KERN_ERR "###: Failed : Can't get clock [%s}!\n",
               "clk_mm_i");
        printk(KERN_ERR "###: clk_mm_i =  %p\n", clk_mm_i);
        ret = -EINVAL;
<<<<<<< HEAD
        goto errout0;
=======
        goto errout;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
    } else {
        vsp_hw_dev.mm_clk= clk_mm_i;
    }
#endif

<<<<<<< HEAD
    printk(KERN_INFO "VSP mmi_clk open\n");
    ret = clk_prepare_enable(vsp_hw_dev.mm_clk);
    if (ret) {
        printk(KERN_ERR "###:vsp_hw_dev.mm_clk: clk_prepare_enable() failed!\n");
        goto errout1;
    } else {
        pr_debug("###vsp_hw_dev.mm_clk: clk_prepare_enable() ok.\n");
    }

=======

    printk(KERN_INFO "VSP mmi_clk open");
    ret = clk_enable(vsp_hw_dev.mm_clk);
    if (ret) {
        printk(KERN_ERR "###:vsp_hw_dev.mm_clk: clk_enable() failed!\n");
        return ret;
    } else {
        pr_debug("###vsp_hw_dev.mm_clk: clk_enable() ok.\n");
    }



>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#ifdef CONFIG_OF
    clk_vsp = of_clk_get_by_name(vsp_hw_dev.dev_np, "clk_vsp");
#else
    clk_vsp = clk_get(NULL, "clk_vsp");
#endif
    if (IS_ERR(clk_vsp) || (!clk_vsp)) {
        printk(KERN_ERR "###: Failed : Can't get clock [%s}!\n",
               "clk_vsp");
        printk(KERN_ERR "###: vsp_clk =  %p\n", clk_vsp);
        ret = -EINVAL;
<<<<<<< HEAD
        goto errout1;
=======
        goto errout;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
    } else {
        vsp_hw_dev.vsp_clk = clk_vsp;
    }

    name_parent = vsp_get_clk_src_name(vsp_hw_dev.freq_div);
    clk_parent = clk_get(NULL, name_parent);
    if ((!clk_parent )|| IS_ERR(clk_parent) ) {
        printk(KERN_ERR "clock[%s]: failed to get parent in probe[%s] \
by clk_get()!\n", "clk_vsp", name_parent);
        ret = -EINVAL;
<<<<<<< HEAD
        goto errout2;
=======
        goto errout;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
    } else {
        vsp_hw_dev.vsp_parent_clk = clk_parent;
    }

    ret = clk_set_parent(vsp_hw_dev.vsp_clk, vsp_hw_dev.vsp_parent_clk);
    if (ret) {
        printk(KERN_ERR "clock[%s]: clk_set_parent() failed in probe!",
               "clk_vsp");
        ret = -EINVAL;
<<<<<<< HEAD
        goto errout3;
    }

    printk("vsp parent clock name %s\n", name_parent);
    printk("vsp_freq %d Hz\n",
           (int)clk_get_rate(vsp_hw_dev.vsp_clk));

    if (vsp_hw_dev.light_sleep_en) {
        clk_disable_unprepare(vsp_hw_dev.mm_clk);
        pr_debug("VSP mmi_clk close\n");
    }

    return 0;

errout3:
    if (vsp_hw_dev.vsp_parent_clk) {
        clk_put(vsp_hw_dev.vsp_parent_clk);
    }

errout2:
    if (vsp_hw_dev.vsp_clk) {
        clk_put(vsp_hw_dev.vsp_clk);
    }

errout1:
=======
        goto errout;
    }

    printk("vsp parent clock name %s\n", name_parent);
    printk("vsp_freq %d Hz",
           (int)clk_get_rate(vsp_hw_dev.vsp_clk));

#if defined(CONFIG_SPRD_IOMMU)
    sprd_iommu_module_enable(IOMMU_MM);
#endif

    return 0;
errout:
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#if defined(CONFIG_ARCH_SCX35)
    if (vsp_hw_dev.mm_clk) {
        clk_put(vsp_hw_dev.mm_clk);
    }
#endif

<<<<<<< HEAD
errout0:
#if defined(CONFIG_ARCH_SCX35)
    if (vsp_hw_dev.mm_clk_axi) {
        clk_put(vsp_hw_dev.mm_clk_axi);
    }
#endif

    return ret;
}

=======
    if (vsp_hw_dev.vsp_clk) {
        clk_put(vsp_hw_dev.vsp_clk);
    }

    if (vsp_hw_dev.vsp_parent_clk) {
        clk_put(vsp_hw_dev.vsp_parent_clk);
    }
    return ret;
}
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
static int vsp_open(struct inode *inode, struct file *filp)
{
    int ret;
    struct vsp_fh *vsp_fp = kmalloc(sizeof(struct vsp_fh), GFP_KERNEL);

    printk(KERN_INFO "vsp_open called %p\n", vsp_fp);

    if (vsp_fp == NULL) {
        printk(KERN_ERR "vsp open error occured\n");
        return  -EINVAL;
    }
    filp->private_data = vsp_fp;
    vsp_fp->is_clock_enabled = 0;
    vsp_fp->is_vsp_aquired = 0;

    init_waitqueue_head(&vsp_fp->wait_queue_work);
    vsp_fp->vsp_int_status = 0;
    vsp_fp->condition_work = 0;

    ret = vsp_set_mm_clk();

    atomic_inc_return(&vsp_instance_cnt);

    printk(KERN_INFO "vsp_open: ret %d\n", ret);

    return ret;
<<<<<<< HEAD
=======


>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

static int vsp_release (struct inode *inode, struct file *filp)
{
<<<<<<< HEAD
=======
    int ret;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
    struct vsp_fh *vsp_fp = filp->private_data;
    int instance_cnt = atomic_read(&vsp_instance_cnt);

    if (vsp_fp == NULL) {
        printk(KERN_ERR "vsp_release error occured, vsp_fp == NULL\n");
        return  -EINVAL;
    }

    printk(KERN_INFO "vsp_release: instance_cnt %d\n", instance_cnt);

    atomic_dec_return(&vsp_instance_cnt);

<<<<<<< HEAD
    if (vsp_fp->is_clock_enabled) {
        printk(KERN_ERR "error occured and close clock \n");
        clk_disable_unprepare(vsp_hw_dev.vsp_clk);
=======
#if defined(CONFIG_SPRD_IOMMU)
    sprd_iommu_module_disable(IOMMU_MM);
#endif

    if (vsp_fp->is_clock_enabled) {
        printk(KERN_ERR "error occured and close clock \n");
        clk_disable(vsp_hw_dev.vsp_clk);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
        vsp_fp->is_clock_enabled = 0;
    }

    if (vsp_fp->is_vsp_aquired) {
        printk(KERN_ERR "error occured and up vsp_mutex \n");
        up(&vsp_hw_dev.vsp_mutex);
    }

    printk(KERN_INFO "vsp_release %p\n", vsp_fp);
    kfree(filp->private_data);
    filp->private_data=NULL;

<<<<<<< HEAD
    if (!vsp_hw_dev.light_sleep_en) {
        if(vsp_hw_dev.mm_clk)
        {
            clk_disable_unprepare(vsp_hw_dev.mm_clk);
        }
        printk(KERN_INFO "VSP mmi_clk close!\n");
    }

    if (vsp_hw_dev.mm_clk_axi) {
        clk_unprepare(vsp_hw_dev.mm_clk_axi);
        printk(KERN_INFO "VSP mm_clk_axi close!\n");
    }
=======
    clk_disable(vsp_hw_dev.mm_clk);

    printk(KERN_INFO "VSP mmi_clk close!!");
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

    return 0;
}

static const struct file_operations vsp_fops =
{
    .owner = THIS_MODULE,
    .unlocked_ioctl = vsp_ioctl,
    .mmap  = vsp_nocache_mmap,
    .open = vsp_open,
    .release = vsp_release,
<<<<<<< HEAD
#ifdef CONFIG_COMPAT
    .compat_ioctl   = vsp_ioctl,
#endif
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
};

static struct miscdevice vsp_dev = {
    .minor   = VSP_MINOR,
    .name   = "sprd_vsp",
    .fops   = &vsp_fops,
};

static int vsp_suspend(struct platform_device *pdev, pm_message_t state)
{
    int ret=-1;
    int cnt;
    int instance_cnt = atomic_read(&vsp_instance_cnt);

    for (cnt = 0; cnt < instance_cnt; cnt++) {
<<<<<<< HEAD
        if (!vsp_hw_dev.light_sleep_en) {
            clk_disable_unprepare(vsp_hw_dev.mm_clk);
            pr_debug("VSP mm_clk close\n");
        } else {
            clk_unprepare(vsp_hw_dev.mm_clk_axi);
            pr_debug("VSP mm_clk_axi close\n");
        }
=======
#if defined(CONFIG_SPRD_IOMMU)
        sprd_iommu_module_disable(IOMMU_MM);
#endif

        clk_disable(vsp_hw_dev.mm_clk);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

        printk(KERN_INFO "vsp_suspend, cnt: %d\n", cnt);
    }

<<<<<<< HEAD
=======

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
    return 0;
}

static int vsp_resume(struct platform_device *pdev)
{
<<<<<<< HEAD
=======

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
    int ret = 0;
    int cnt;
    int instance_cnt = atomic_read(&vsp_instance_cnt);

    for (cnt = 0; cnt < instance_cnt; cnt++) {
        ret = vsp_set_mm_clk();
        printk(KERN_INFO "vsp_resume, cnt: %d\n", cnt);
    }

    return ret;
}

<<<<<<< HEAD
=======

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
static int vsp_probe(struct platform_device *pdev)
{
    int ret;

    printk(KERN_INFO "vsp_probe called !\n");

#ifdef CONFIG_OF
    if (pdev->dev.of_node) {
<<<<<<< HEAD
        if(vsp_parse_dt(&pdev->dev)) {
            printk(KERN_ERR "vsp_parse_dt failed\n");
            return -EINVAL;
        }
=======
        ret = vsp_parse_dt(&pdev->dev);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
    }
#else
    ret = vsp_parse_dt(&pdev->dev);
#endif

<<<<<<< HEAD
    wake_lock_init(&vsp_wakelock, WAKE_LOCK_SUSPEND, "pm_message_wakelock_vsp");

    sema_init(&vsp_hw_dev.vsp_mutex, 1);

    vsp_hw_dev.freq_div = max_freq_level;
=======
    wake_lock_init(&vsp_wakelock, WAKE_LOCK_SUSPEND,
                   "pm_message_wakelock_vsp");

    sema_init(&vsp_hw_dev.vsp_mutex, 1);

    vsp_hw_dev.freq_div = DEFAULT_FREQ_DIV;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

    vsp_hw_dev.vsp_clk = NULL;
    vsp_hw_dev.vsp_parent_clk = NULL;
    vsp_hw_dev.mm_clk= NULL;
<<<<<<< HEAD
    vsp_hw_dev.mm_clk_axi = NULL;
    vsp_hw_dev.vsp_fp = NULL;
    vsp_hw_dev.light_sleep_en = false;

    ret = misc_register(&vsp_dev);
    if (ret) {
        printk(KERN_ERR "cannot register miscdev on minor=%d (%d)\n", VSP_MINOR, ret);
=======
    vsp_hw_dev.vsp_fp = NULL;

    ret = misc_register(&vsp_dev);
    if (ret) {
        printk(KERN_ERR "cannot register miscdev on minor=%d (%d)\n",
               VSP_MINOR, ret);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
        goto errout;
    }

#ifdef USE_INTERRUPT
    /* register isr */
<<<<<<< HEAD
    ret = request_irq(vsp_hw_dev.irq, vsp_isr, IRQF_DISABLED|IRQF_SHARED, "VSP", &vsp_hw_dev);
=======
    ret = request_irq(vsp_hw_dev.irq, vsp_isr, IRQF_DISABLED/*0*/, "VSP", &vsp_hw_dev);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
    if (ret) {
        printk(KERN_ERR "vsp: failed to request irq!\n");
        ret = -EINVAL;
        goto errout;
    }
#endif

    return 0;

errout:
    misc_deregister(&vsp_dev);

    return ret;
}

static int vsp_remove(struct platform_device *pdev)
{
    printk(KERN_INFO "vsp_remove called !\n");

    misc_deregister(&vsp_dev);

#ifdef USE_INTERRUPT
    free_irq(vsp_hw_dev.irq, &vsp_hw_dev);
#endif

    if (vsp_hw_dev.vsp_clk) {
        clk_put(vsp_hw_dev.vsp_clk);
    }

    if (vsp_hw_dev.vsp_parent_clk) {
        clk_put(vsp_hw_dev.vsp_parent_clk);
    }

    printk(KERN_INFO "vsp_remove Success !\n");
    return 0;
}

static struct platform_driver vsp_driver = {
    .probe    = vsp_probe,
    .remove   = vsp_remove,
<<<<<<< HEAD
#if !defined(CONFIG_ARCH_SCX35L)
    .suspend = vsp_suspend,
    .resume = vsp_resume,
#endif
=======
    .suspend = vsp_suspend,
    .resume = vsp_resume,
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
    .driver   = {
        .owner = THIS_MODULE,
        .name = "sprd_vsp",
#ifdef CONFIG_OF
        .of_match_table = of_match_ptr(of_match_table_vsp) ,
#endif
    },
};

static int __init vsp_init(void)
{
    printk(KERN_INFO "vsp_init called !\n");
    if (platform_driver_register(&vsp_driver) != 0) {
        printk(KERN_ERR "platform device vsp drv register Failed \n");
        return -1;
    }
    return 0;
}

static void __exit vsp_exit(void)
{
    printk(KERN_INFO "vsp_exit called !\n");
    platform_driver_unregister(&vsp_driver);
}

module_init(vsp_init);
module_exit(vsp_exit);

MODULE_DESCRIPTION("SPRD VSP Driver");
MODULE_LICENSE("GPL");

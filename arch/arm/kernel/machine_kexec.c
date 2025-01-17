/*
 * machine_kexec.c - handle transition of Linux booting another kernel
 */

#include <linux/mm.h>
#include <linux/kexec.h>
#include <linux/delay.h>
#include <linux/reboot.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/memblock.h>
#include <asm/pgtable.h>
#include <linux/of_fdt.h>
#include <asm/pgalloc.h>
#include <asm/mmu_context.h>
#include <asm/cacheflush.h>
<<<<<<< HEAD
#include <asm/fncpy.h>
#include <asm/mach-types.h>
#include <asm/system_misc.h>

extern void relocate_new_kernel(void);
=======
#include <asm/mach-types.h>
#include <asm/system_misc.h>

extern const unsigned char relocate_new_kernel[];
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
extern const unsigned int relocate_new_kernel_size;

extern unsigned long kexec_start_address;
extern unsigned long kexec_indirection_page;
extern unsigned long kexec_mach_type;
extern unsigned long kexec_boot_atags;

static atomic_t waiting_for_crash_ipi;

/*
 * Provide a dummy crash_notes definition while crash dump arrives to arm.
 * This prevents breakage of crash_notes attribute in kernel/ksysfs.c.
 */

int machine_kexec_prepare(struct kimage *image)
{
	struct kexec_segment *current_segment;
	__be32 header;
	int i, err;

	/*
	 * No segment at default ATAGs address. try to locate
	 * a dtb using magic.
	 */
	for (i = 0; i < image->nr_segments; i++) {
		current_segment = &image->segment[i];

		if (!memblock_is_region_memory(current_segment->mem,
					       current_segment->memsz))
			return -EINVAL;

		err = get_user(header, (__be32*)current_segment->buf);
		if (err)
			return err;

		if (be32_to_cpu(header) == OF_DT_HEADER)
			kexec_boot_atags = current_segment->mem;
	}
	return 0;
}

void machine_kexec_cleanup(struct kimage *image)
{
}

void machine_crash_nonpanic_core(void *unused)
{
	struct pt_regs regs;

	crash_setup_regs(&regs, NULL);
	printk(KERN_DEBUG "CPU %u will stop doing anything useful since another CPU has crashed\n",
	       smp_processor_id());
	crash_save_cpu(&regs, smp_processor_id());
	flush_cache_all();

<<<<<<< HEAD
	set_cpu_online(smp_processor_id(), false);
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	atomic_dec(&waiting_for_crash_ipi);
	while (1)
		cpu_relax();
}

static void machine_kexec_mask_interrupts(void)
{
	unsigned int i;
	struct irq_desc *desc;

	for_each_irq_desc(i, desc) {
		struct irq_chip *chip;

		chip = irq_desc_get_chip(desc);
		if (!chip)
			continue;

		if (chip->irq_eoi && irqd_irq_inprogress(&desc->irq_data))
			chip->irq_eoi(&desc->irq_data);

		if (chip->irq_mask)
			chip->irq_mask(&desc->irq_data);

		if (chip->irq_disable && !irqd_irq_disabled(&desc->irq_data))
			chip->irq_disable(&desc->irq_data);
	}
}

void machine_crash_shutdown(struct pt_regs *regs)
{
	unsigned long msecs;

	local_irq_disable();

	atomic_set(&waiting_for_crash_ipi, num_online_cpus() - 1);
	smp_call_function(machine_crash_nonpanic_core, NULL, false);
	msecs = 1000; /* Wait at most a second for the other cpus to stop */
	while ((atomic_read(&waiting_for_crash_ipi) > 0) && msecs) {
		mdelay(1);
		msecs--;
	}
	if (atomic_read(&waiting_for_crash_ipi) > 0)
		printk(KERN_WARNING "Non-crashing CPUs did not react to IPI\n");

	crash_save_cpu(regs, smp_processor_id());
	machine_kexec_mask_interrupts();

	printk(KERN_INFO "Loading crashdump kernel...\n");
}

/*
 * Function pointer to optional machine-specific reinitialization
 */
void (*kexec_reinit)(void);

void machine_kexec(struct kimage *image)
{
	unsigned long page_list;
	unsigned long reboot_code_buffer_phys;
<<<<<<< HEAD
	unsigned long reboot_entry = (unsigned long)relocate_new_kernel;
	unsigned long reboot_entry_phys;
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	void *reboot_code_buffer;

	if (num_online_cpus() > 1) {
		pr_err("kexec: error: multiple CPUs still online\n");
		return;
	}

	page_list = image->head & PAGE_MASK;

	/* we need both effective and real address here */
	reboot_code_buffer_phys =
	    page_to_pfn(image->control_code_page) << PAGE_SHIFT;
	reboot_code_buffer = page_address(image->control_code_page);

	/* Prepare parameters for reboot_code_buffer*/
	kexec_start_address = image->start;
	kexec_indirection_page = page_list;
	kexec_mach_type = machine_arch_type;
	if (!kexec_boot_atags)
		kexec_boot_atags = image->start - KEXEC_ARM_ZIMAGE_OFFSET + KEXEC_ARM_ATAGS_OFFSET;


	/* copy our kernel relocation code to the control code page */
<<<<<<< HEAD
	reboot_entry = fncpy(reboot_code_buffer,
			     reboot_entry,
			     relocate_new_kernel_size);
	reboot_entry_phys = (unsigned long)reboot_entry +
		(reboot_code_buffer_phys - (unsigned long)reboot_code_buffer);

=======
	memcpy(reboot_code_buffer,
	       relocate_new_kernel, relocate_new_kernel_size);


	flush_icache_range((unsigned long) reboot_code_buffer,
			   (unsigned long) reboot_code_buffer + KEXEC_CONTROL_PAGE_SIZE);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	printk(KERN_INFO "Bye!\n");

	if (kexec_reinit)
		kexec_reinit();

<<<<<<< HEAD
	soft_restart(reboot_entry_phys);
}

void arch_crash_save_vmcoreinfo(void)
{
#ifdef CONFIG_ARM_LPAE
	VMCOREINFO_CONFIG(ARM_LPAE);
#endif
=======
	soft_restart(reboot_code_buffer_phys);
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
}

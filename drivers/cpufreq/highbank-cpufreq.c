/*
 * Copyright (C) 2012 Calxeda, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This driver provides the clk notifier callbacks that are used when
 * the cpufreq-cpu0 driver changes to frequency to alert the highbank
 * EnergyCore Management Engine (ECME) about the need to change
 * voltage. The ECME interfaces with the actual voltage regulators.
 */

#define pr_fmt(fmt)	KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/cpu.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/mailbox.h>
#include <linux/platform_device.h>

#define HB_CPUFREQ_CHANGE_NOTE	0x80000001
#define HB_CPUFREQ_IPC_LEN	7
#define HB_CPUFREQ_VOLT_RETRIES	15

static int hb_voltage_change(unsigned int freq)
{
	u32 msg[HB_CPUFREQ_IPC_LEN] = {HB_CPUFREQ_CHANGE_NOTE, freq / 1000000};

	return pl320_ipc_transmit(msg);
}

static int hb_cpufreq_clk_notify(struct notifier_block *nb,
				unsigned long action, void *hclk)
{
	struct clk_notifier_data *clk_data = hclk;
	int i = 0;

	if (action == PRE_RATE_CHANGE) {
		if (clk_data->new_rate > clk_data->old_rate)
			while (hb_voltage_change(clk_data->new_rate))
				if (i++ > HB_CPUFREQ_VOLT_RETRIES)
					return NOTIFY_BAD;
	} else if (action == POST_RATE_CHANGE) {
		if (clk_data->new_rate < clk_data->old_rate)
			while (hb_voltage_change(clk_data->new_rate))
				if (i++ > HB_CPUFREQ_VOLT_RETRIES)
					return NOTIFY_BAD;
	}

	return NOTIFY_DONE;
}

static struct notifier_block hb_cpufreq_clk_nb = {
	.notifier_call = hb_cpufreq_clk_notify,
};

static int hb_cpufreq_driver_init(void)
{
	struct platform_device_info devinfo = { .name = "cpufreq-cpu0", };
	struct device *cpu_dev;
	struct clk *cpu_clk;
	struct device_node *np;
	int ret;

<<<<<<< HEAD
	if ((!of_machine_is_compatible("calxeda,highbank")) &&
		(!of_machine_is_compatible("calxeda,ecx-2000")))
=======
	if (!of_machine_is_compatible("calxeda,highbank"))
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		return -ENODEV;

	for_each_child_of_node(of_find_node_by_path("/cpus"), np)
		if (of_get_property(np, "operating-points", NULL))
			break;

	if (!np) {
		pr_err("failed to find highbank cpufreq node\n");
		return -ENOENT;
	}

	cpu_dev = get_cpu_device(0);
	if (!cpu_dev) {
		pr_err("failed to get highbank cpufreq device\n");
		ret = -ENODEV;
		goto out_put_node;
	}

	cpu_dev->of_node = np;

	cpu_clk = clk_get(cpu_dev, NULL);
	if (IS_ERR(cpu_clk)) {
		ret = PTR_ERR(cpu_clk);
		pr_err("failed to get cpu0 clock: %d\n", ret);
		goto out_put_node;
	}

	ret = clk_notifier_register(cpu_clk, &hb_cpufreq_clk_nb);
	if (ret) {
		pr_err("failed to register clk notifier: %d\n", ret);
		goto out_put_node;
	}

	/* Instantiate cpufreq-cpu0 */
	platform_device_register_full(&devinfo);

out_put_node:
	of_node_put(np);
	return ret;
}
module_init(hb_cpufreq_driver_init);

MODULE_AUTHOR("Mark Langsdorf <mark.langsdorf@calxeda.com>");
MODULE_DESCRIPTION("Calxeda Highbank cpufreq driver");
MODULE_LICENSE("GPL");

/*
 * This file provides /sys/class/ieee80211/<wiphy name>/
 * and some default attributes.
 *
 * Copyright 2005-2006	Jiri Benc <jbenc@suse.cz>
 * Copyright 2006	Johannes Berg <johannes@sipsolutions.net>
 *
 * This file is GPLv2 as found in COPYING.
 */

#include <linux/device.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/nl80211.h>
#include <linux/rtnetlink.h>
#include <net/cfg80211.h>
#include "sysfs.h"
#include "core.h"
#include "rdev-ops.h"

static inline struct cfg80211_registered_device *dev_to_rdev(
	struct device *dev)
{
	return container_of(dev, struct cfg80211_registered_device, wiphy.dev);
}

#define SHOW_FMT(name, fmt, member)					\
static ssize_t name ## _show(struct device *dev,			\
			      struct device_attribute *attr,		\
			      char *buf)				\
{									\
	return sprintf(buf, fmt "\n", dev_to_rdev(dev)->member);	\
}

SHOW_FMT(index, "%d", wiphy_idx);
SHOW_FMT(macaddress, "%pM", wiphy.perm_addr);
SHOW_FMT(address_mask, "%pM", wiphy.addr_mask);

static ssize_t name_show(struct device *dev,
			 struct device_attribute *attr,
			 char *buf) {
	struct wiphy *wiphy = &dev_to_rdev(dev)->wiphy;
	return sprintf(buf, "%s\n", dev_name(&wiphy->dev));
}


static ssize_t addresses_show(struct device *dev,
			      struct device_attribute *attr,
			      char *buf)
{
	struct wiphy *wiphy = &dev_to_rdev(dev)->wiphy;
	char *start = buf;
	int i;

	if (!wiphy->addresses)
		return sprintf(buf, "%pM\n", wiphy->perm_addr);

	for (i = 0; i < wiphy->n_addresses; i++)
		buf += sprintf(buf, "%pM\n", &wiphy->addresses[i].addr);

	return buf - start;
}

static struct device_attribute ieee80211_dev_attrs[] = {
	__ATTR_RO(index),
	__ATTR_RO(macaddress),
	__ATTR_RO(address_mask),
	__ATTR_RO(addresses),
	__ATTR_RO(name),
	{}
};

static void wiphy_dev_release(struct device *dev)
{
	struct cfg80211_registered_device *rdev = dev_to_rdev(dev);

	cfg80211_dev_free(rdev);
}

static int wiphy_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	/* TODO, we probably need stuff here */
	return 0;
}

static void cfg80211_leave_all(struct cfg80211_registered_device *rdev)
{
	struct wireless_dev *wdev;

	list_for_each_entry(wdev, &rdev->wdev_list, list)
		cfg80211_leave(rdev, wdev);
}

static int wiphy_suspend(struct device *dev, pm_message_t state)
{
	struct cfg80211_registered_device *rdev = dev_to_rdev(dev);
	int ret = 0;

	rdev->suspend_at = get_seconds();

	rtnl_lock();
	if (rdev->wiphy.registered) {
<<<<<<< HEAD
		if (!rdev->wowlan)
=======
		if (!rdev->wowlan && !rdev->wiphy.wowlan.flags)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
			cfg80211_leave_all(rdev);
		if (rdev->ops->suspend)
			ret = rdev_suspend(rdev, rdev->wowlan);
		if (ret == 1) {
			/* Driver refuse to configure wowlan */
			cfg80211_leave_all(rdev);
			ret = rdev_suspend(rdev, NULL);
		}
	}
	rtnl_unlock();

	return ret;
}

static int wiphy_resume(struct device *dev)
{
	struct cfg80211_registered_device *rdev = dev_to_rdev(dev);
	int ret = 0;

	/* Age scan results with time spent in suspend */
	cfg80211_bss_age(rdev, get_seconds() - rdev->suspend_at);

	if (rdev->ops->resume) {
		rtnl_lock();
		if (rdev->wiphy.registered)
			ret = rdev_resume(rdev);
		rtnl_unlock();
	}

	return ret;
}

static const void *wiphy_namespace(struct device *d)
{
	struct wiphy *wiphy = container_of(d, struct wiphy, dev);

	return wiphy_net(wiphy);
}

struct class ieee80211_class = {
	.name = "ieee80211",
	.owner = THIS_MODULE,
	.dev_release = wiphy_dev_release,
	.dev_attrs = ieee80211_dev_attrs,
	.dev_uevent = wiphy_uevent,
	.suspend = wiphy_suspend,
	.resume = wiphy_resume,
	.ns_type = &net_ns_type_operations,
	.namespace = wiphy_namespace,
};

int wiphy_sysfs_init(void)
{
	return class_register(&ieee80211_class);
}

void wiphy_sysfs_exit(void)
{
	class_unregister(&ieee80211_class);
}

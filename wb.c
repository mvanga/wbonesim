#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/list.h>

#include "wb.h"

#define PFX "wb: "

static struct device wb_dev;
static struct bus_type wb_bus_type;

static void wb_dev_release(struct device *dev)
{
	struct wb_device *wb_dev;
	wb_dev = to_wb_device(dev);
	printk(KERN_DEBUG PFX "release %llx\n", wb_dev->wbd.vendor);
}

/*
 * Register a Wishbone peripheral on the bus.
 */
int wb_register_device(struct wb_device *wbdev)
{
	/* TODO: race condition with devno possible */
	static unsigned int devno = 1;
	wbdev->dev.bus = &wb_bus_type;
	wbdev->dev.parent = &wb_dev;
	wbdev->dev.release = wb_dev_release;
	dev_set_name(&wbdev->dev, "wb%d", devno++);
	INIT_LIST_HEAD(&wbdev->list);
	return device_register(&wbdev->dev);
}
EXPORT_SYMBOL(wb_register_device);

/*
 * Unregister a previously registered Wishbone device
 */
void wb_unregister_device(struct wb_device *wbdev)
{
	if (!wbdev)
		return;
	device_unregister(&wbdev->dev);
}
EXPORT_SYMBOL(wb_unregister_device);

/*
 * Register a Wishbone driver
 */
int wb_register_driver(struct wb_driver *driver)
{
	int ret;
	driver->driver.bus = &wb_bus_type;
	driver->driver.name = driver->name;
	ret = driver_register(&driver->driver);
	INIT_LIST_HEAD(&driver->list);
	return ret;
}
EXPORT_SYMBOL(wb_register_driver);

/* Unregister a Wishbone driver */
void wb_unregister_driver(struct wb_driver *driver)
{
	driver_unregister(&driver->driver);
}
EXPORT_SYMBOL(wb_unregister_driver);

/*
 * Match a single Wishbone driver and Wishbone device. An ID of
 * WBONE_ANY_ID can be used as a match-all value for any of the
 * fields (vendor, device, subdevice).
 */
static struct wb_device_id *wb_match_device(struct wb_driver *drv,
						struct wb_device *dev)
{
	struct wb_device_id *ids;
	ids = drv->id_table;
	if (ids) {
		while (ids->vendor || ids->device) {
			if ((ids->vendor == WBONE_ANY_ID
					|| ids->vendor == dev->wbd.vendor) &&
			    (ids->device == (__u16)WBONE_ANY_ID
					|| ids->device == dev->wbd.device))
				return ids;
			ids++;
		}
	}
	return NULL;
}

static int wb_bus_match(struct device *dev, struct device_driver *drv)
{
	struct wb_device *wb_dev;
	struct wb_driver *wb_drv;
	struct wb_device_id *found;
	wb_dev = to_wb_device(dev);
	wb_drv = to_wb_driver(drv);

	found = wb_match_device(wb_drv, wb_dev);
	if (found) {
		/* set this so we can access it later */
		wb_dev->driver = wb_drv;
		return 1;
	}
	return 0;
}

static int wb_bus_probe(struct device *dev)
{
	struct wb_driver *wb_drv;
	struct wb_device *wb_dev;
	wb_dev = to_wb_device(dev);
	wb_drv = wb_dev->driver;
	if (wb_drv && wb_drv->probe)
		return wb_drv->probe(wb_dev);
	return 0;
}

static int wb_bus_remove(struct device *dev)
{
	struct wb_driver *wb_drv;
	struct wb_device *wb_dev;
	wb_dev = to_wb_device(dev);
	wb_drv = wb_dev->driver;
	if (wb_drv && wb_drv->remove)
		return wb_drv->remove(wb_dev);
	return 0;
}

static void wb_bus_shutdown(struct device *dev)
{
	return;
}

static int wb_bus_suspend(struct device *dev, pm_message_t state)
{
	return 0;
}

static int wb_bus_resume(struct device *dev)
{
	return 0;
}

static struct device wb_dev = {
	.init_name = "wb0",
	.release = wb_dev_release,
};

static struct bus_type wb_bus_type = {
	.name = "wb",
	.match = wb_bus_match,
	.probe = wb_bus_probe,
	.remove = wb_bus_remove,
	.shutdown = wb_bus_shutdown,
	.suspend = wb_bus_suspend,
	.resume = wb_bus_resume,
};

static int wb_init(void)
{
	int ret;
	ret = bus_register(&wb_bus_type);
	if (ret)
		return ret;
	ret = device_register(&wb_dev);
	if (ret) {
		bus_unregister(&wb_bus_type);
		return ret;
	}
	return 0;
}

static void wb_exit(void)
{
	device_unregister(&wb_dev);
	bus_unregister(&wb_bus_type);
}

module_init(wb_init)
module_exit(wb_exit)

MODULE_AUTHOR("Manohar Vanga");
MODULE_DESCRIPTION("Wishbone bus driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("Apr 2011");

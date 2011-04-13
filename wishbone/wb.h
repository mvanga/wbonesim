#ifndef LINUX_BUS_VBUS_H
#define LINUX_BUS_VBUS_H

#include <linux/device.h>
#include <linux/pm.h>
#include <linux/types.h>

#define WBONE_ANY_ID (~0)

struct wb_device;

struct wb_device_id {
	__u32 vendor;		/* Vendor or WBONE_ANY_ID */
	__u16 device;		/* Device ID or WBONE_ANY_ID */
	__u16 subdevice;	/* Device ID or WBONE_ANY_ID */
};

struct wb_driver {
	char *version;
	struct module *owner;
	struct wb_device_id *id_table;
	int (*probe)(struct wb_device *);
	int (*remove)(struct wb_device *);
	void (*shutdown)(struct wb_device *);
	struct dev_pm_ops ops;
	struct device_driver driver;
	struct driver_attribute version_attr;
};
#define to_wb_driver(drv) container_of(drv, struct wb_driver, driver);

struct wb_device {
	char *name;
	unsigned int vendor;
	unsigned short device;
	unsigned short subdevice;
	unsigned int flags; /* LSB to MSB: 4 bits priority, 12 bits class, 
	                       16 bits version */
	struct wb_driver *driver;
	struct device dev;
};
#define to_wb_device(dev) container_of(dev, struct wb_device, dev);

//int wb_register_device(struct wb_device *wbdev); /* not public */
int wb_register_driver(struct wb_driver *driver);
void wb_unregister_driver(struct wb_driver *driver);

#endif
#include "wb.h"

#define PFX "fakedac: "

#define WB_CERN_VENDOR 	0x0
#define WB_DAC_DEVICE	0x2

static struct wb_device_id fakedac_ids[] = {
	{ WB_CERN_VENDOR, WB_DAC_DEVICE},
	{ 0, },
};

static int fakedac_probe(struct wb_device *dev)
{
	printk(KERN_INFO PFX "found a fake DAC device!\n");
	return 0;
}

static int fakedac_remove(struct wb_device *dev)
{
	printk(KERN_INFO PFX "removed a fake DAC device!\n");
	return 0;
}

static struct wb_driver fakedac_driver = {
	.name = "fakedac",
	.owner = THIS_MODULE,
	.id_table = fakedac_ids,
	.probe = fakedac_probe,
	.remove = fakedac_remove,
};

static int fakedac_init(void)
{
	int ret;
	ret = wb_register_driver(&fakedac_driver);
	if (ret < 0)
		return ret;
	return 0;
}

static void fakedac_exit(void)
{
	wb_unregister_driver(&fakedac_driver);
}

module_init(fakedac_init);
module_exit(fakedac_exit);

MODULE_AUTHOR("Manohar Vanga");
MODULE_DESCRIPTION("Fake DAC wishbone driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("Apr 2011");

#ifndef SDWB_H
#define SDWB_H

#define SDWB_HEAD_MAGIC (('s'<<7)|('d'<<6)|('w'<<5)|('b'<<4)|('h'<<3)|('e'<<2)|('a'<<1)|'d')
#define SDWB_WBD_MAGIC	(('w'<<8)|'b')

struct sdwb_head {
	uint64_t	unused;
	uint64_t	magic;
	uint64_t	wbid_address;
	uint64_t	wbd_address;
};

struct sdwb_wbid {
	/* To be defined */
	uint32_t dummy;
};

struct sdwb_wbd {
	uint16_t wbd_magic;
	uint16_t wbd_version;

	uint64_t vendor;
	uint32_t device;

	uint64_t hdl_base;
	uint64_t hdl_size;

	uint32_t wbd_flags;
	uint32_t hdl_class;
	uint32_t hdl_version;
	uint32_t hdl_date;

	char vendor_name[16];
	char device_name[16];
};

#endif

/*
 * firmware.h
 *
 * Created: 3/2/2016 4:38:46 PM
 *  Author: pitter.liao
 */ 


#ifndef FIRMWARE_H_
#define FIRMWARE_H_

#include "device.h"

struct firmware {
	size_t size;
	const u8 *data;
};


int request_firmware(const struct firmware **, const char *,struct device *);
void release_firmware(const struct firmware *);

#endif /* FIRMWARE_H_ */
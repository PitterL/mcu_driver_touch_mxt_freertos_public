/*
 * device.c
 *
 * Created: 3/6/2016 10:59:22 AM
 *  Author: pitter.liao
 */ 

#include <errno.h>
#include <common/types.h>
#include <common/slab.h>
#include <common/device.h>

/**
 * driver_register - register driver with bus
 * @drv: driver to register
 *
 * We pass off most of the work to the bus_add_driver() call,
 * since most of the things we have to do deal with the bus
 * structures.
 */
int driver_register(struct device_driver *drv)
{
	return 0;
}

int device_suspend(struct device *dev)
{
	return dev->pm->suspend(dev);
}

/**
 * device_resume - Execute "resume" callbacks for given device.
 * @dev: Device to handle.
 */
int device_resume(struct device *dev)
{
	return dev->pm->resume(dev);
}
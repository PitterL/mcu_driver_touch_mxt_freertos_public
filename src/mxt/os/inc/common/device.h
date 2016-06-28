/*
 * device.h
 *
 * Created: 2/28/2016 11:07:42 AM
 *  Author: pitter.liao
 */ 


#ifndef DEVICE_H_
#define DEVICE_H_

struct device_private {
	void *driver_data;
};

struct device {
	struct device_private priv;
	void * platform_data;

	const struct dev_pm_ops *pm;
};

/**
 * struct dev_pm_ops - device PM callbacks
 *
 * Several device power state transitions are externally visible, affecting
 * the state of pending I/O queues and (for drivers that touch hardware)
 * interrupts, wakeups, DMA, and other hardware state.  There may also be
 * internal transitions to various low-power modes which are transparent
 * to the rest of the driver stack (such as a driver that's ON gating off
 * clocks which are not in active use).
 *
 * The externally visible transitions are handled with the help of callbacks
 * included in this structure in such a way that two levels of callbacks are
 * involved.  First, the PM core executes callbacks provided by PM domains,
 * device types, classes and bus types.  They are the subsystem-level callbacks
 * supposed to execute callbacks provided by device drivers, although they may
 * choose not to do that.  If the driver callbacks are executed, they have to
 * collaborate with the subsystem-level callbacks to achieve the goals
 * appropriate for the given system transition, given transition phase and the
 * subsystem the device belongs to.
 *
 * @suspend: Executed before putting the system into a sleep state in which the
 *	contents of main memory are preserved.  The exact action to perform
 *	depends on the device's subsystem (PM domain, device type, class or bus
 *	type), but generally the device must be quiescent after subsystem-level
 *	@suspend() has returned, so that it doesn't do any I/O or DMA.
 *	Subsystem-level @suspend() is executed for all devices after invoking
 *	subsystem-level @prepare() for all of them.
 *
 * @resume: Executed after waking the system up from a sleep state in which the
 *	contents of main memory were preserved.  The exact action to perform
 *	depends on the device's subsystem, but generally the driver is expected
 *	to start working again, responding to hardware events and software
 *	requests (the device itself may be left in a low-power state, waiting
 *	for a runtime resume to occur).  The state of the device at the time its
 *	driver's @resume() callback is run depends on the platform and subsystem
 *	the device belongs to.  On most platforms, there are no restrictions on
 *	availability of resources like clocks during @resume().
 *	Subsystem-level @resume() is executed for all devices after invoking
 *	subsystem-level @resume_noirq() for all of them.
 
 * Refer to Documentation/power/runtime_pm.txt for more information about the
 * role of the above callbacks in device runtime power management.
 *
 */

struct dev_pm_ops {
	int (*suspend)(struct device *dev);
	int (*resume)(struct device *dev);
};

/**
 * struct device_driver - The basic device driver structure
 * @name:	Name of the device driver.
 * @bus:	The bus which the device of this driver belongs to.
 * @owner:	The module owner.
 * @mod_name:	Used for built-in modules.
 * @suppress_bind_attrs: Disables bind/unbind via sysfs.
 * @of_match_table: The open firmware table.
 * @acpi_match_table: The ACPI match table.
 * @probe:	Called to query the existence of a specific device,
 *		whether this driver can work with it, and bind the driver
 *		to a specific device.
 * @remove:	Called when the device is removed from the system to
 *		unbind a device from this driver.
 * @shutdown:	Called at shut-down time to quiesce the device.
 * @suspend:	Called to put the device to sleep mode. Usually to a
 *		low power state.
 * @resume:	Called to bring a device from sleep mode.
 * @groups:	Default attributes that get created by the driver core
 *		automatically.
 * @pm:		Power management operations of the device which matched
 *		this driver.
 * @p:		Driver core's private data, no one other than the driver
 *		core can touch this.
 *
 * The device driver-model tracks all of the drivers known to the system.
 * The main reason for this tracking is to enable the driver core to match
 * up drivers with new devices. Once drivers are known objects within the
 * system, however, a number of other things become possible. Device drivers
 * can export information and configuration variables that are independent
 * of any specific device.
 */
struct device_driver {
	const struct dev_pm_ops *pm;
};

/*
 * These exports can't be _GPL due to .h files using this within them, and it
 * might break something that was previously working...
 */
static inline void *dev_get_drvdata(const struct device *dev)
{
	if (dev)
		return dev->priv.driver_data;
	return NULL;
}

static inline int dev_set_drvdata(struct device *dev, void *data)
{
	dev->priv.driver_data = data;

	return 0;
}

static inline void *dev_get_platdata(const struct device *dev)
{
	return dev->platform_data;
}

int driver_register(struct device_driver *drv);
int device_suspend(struct device *dev);
int device_resume(struct device *dev);

#endif /* DEVICE_H_ */
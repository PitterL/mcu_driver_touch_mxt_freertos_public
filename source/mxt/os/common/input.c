/*
 * input.c
 *
 * Created: 3/2/2016 4:42:55 PM
 *  Author: pitter.liao
 */ 

#include <errno.h>
#include <common/types.h>
#include <common/slab.h>
#include <common/debug.h>
#include <common/input.h>

struct input_dev *input_allocate_device(void)
{
	//dummy
	return (struct input_dev *)0x4f4f4f4f;	
	//return (struct input_dev *)kzalloc(sizeof(struct input_dev), GFP_KERNEL);
}

/**
 * input_free_device - free memory occupied by input_dev structure
 * @dev: input device to free
 *
 * This function should only be used if input_register_device()
 * was not called yet or if it failed. Once device was registered
 * use input_unregister_device() and memory will be freed once last
 * reference to the device is dropped.
 *
 * Device should be allocated by input_allocate_device().
 *
 * NOTE: If there are references to the input device then memory
 * will not be freed until last reference is dropped.
 */
void input_free_device(struct input_dev *dev)
{
	//dummy
	/*
	if (dev)
		kfree(dev);
	*/
}

/**
 * input_register_device - register device with input core
 * @dev: device to be registered
 *
 * This function registers device with input core. The device must be
 * allocated with input_allocate_device() and all it's capabilities
 * set up before registering.
 * If function fails the device must be freed with input_free_device().
 * Once device has been successfully registered it can be unregistered
 * with input_unregister_device(); input_free_device() should not be
 * called in this case.
 *
 * Note that this function is also used to register managed input devices
 * (ones allocated with devm_input_allocate_device()). Such managed input
 * devices need not be explicitly unregistered or freed, their tear down
 * is controlled by the devres infrastructure. It is also worth noting
 * that tear down of managed input devices is internally a 2-step process:
 * registered managed input device is first unregistered, but stays in
 * memory and can still handle input_event() calls (although events will
 * not be delivered anywhere). The freeing of managed input device will
 * happen later, when devres stack is unwound to the point where device
 * allocation was made.
 */
int input_register_device(struct input_dev *dev)
{
	return 0;
}

/**
 * input_unregister_device - unregister previously registered device
 * @dev: device to be unregistered
 *
 * This function unregisters an input device. Once device is unregistered
 * the caller should not try to access it as it may get freed at any moment.
 */
void input_unregister_device(struct input_dev *dev)
{
	//dummy
}

/**
 * input_alloc_absinfo - allocates array of input_absinfo structs
 * @dev: the input device emitting absolute events
 *
 * If the absinfo struct the caller asked for is already allocated, this
 * functions will not do anything.
 */
void input_alloc_absinfo(struct input_dev *dev)
{
	//dummy

	/*
	if (!dev->absinfo)
		dev->absinfo = kcalloc(ABS_CNT, sizeof(struct input_absinfo),
					GFP_KERNEL);

	WARN(!dev->absinfo, "kcalloc() failed?\n");
	*/
}

void input_set_abs_params(struct input_dev *dev, unsigned int axis,
		int min, int max, int fuzz, int flat)
{
	//dummy

	/*
	struct input_absinfo *absinfo;

	input_alloc_absinfo(dev);
	if (!dev->absinfo)
		return;

	absinfo = &dev->absinfo[axis];
	absinfo->minimum = min;
	absinfo->maximum = max;
	absinfo->fuzz = fuzz;
	absinfo->flat = flat;

	dev->absbit[BIT_WORD(axis)] |= BIT_MASK(axis);
	*/
}

extern void input_event_hook(unsigned int type, unsigned int code, int value);
void input_event(struct input_dev *dev,
			unsigned int type, unsigned int code, int value)
{
	input_event_hook(type, code, value);
}
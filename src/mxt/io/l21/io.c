/*
 * Atmel maXTouch Touchscreen driver Hardware interface
 *
 * Copyright (C) 2010 Samsung Electronics Co.Ltd
 * Copyright (C) 2011-2012 Atmel Corporation
 * Copyright (C) 2012 Google, Inc.
 *
 * Author: Pitter.liao <pitter.liao@atmel.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

/*-----------------------------------------------------------------*/
#define DRIVER_VERSION 0x0001
/*----------------------------------------------------------------
*	write version log here
*/

#include <extint.h>

#include <include/config.h>
#include <os_if.h> 
#include "io.h"
#include <common/input.h>
#include <common/i2c.h>
#include "asf_wrapper/hal_gpio.h"
#include "asf_wrapper/hal_ext_irq.h"

static void *mxt_g_data;
struct device *t_dev;

int device_wait_irq_state(struct device *dev, int pin_level, long interval)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);
	int state;
	unsigned long start_wait_jiffies = jiffies;
	unsigned long timeout;

	if (!pdata)
		return -ENODEV;

	/* Reset completion indicated by asserting CHG  */
	/* Wait for CHG asserted or timeout after 200ms */
	timeout = start_wait_jiffies + msecs_to_jiffies(interval);
	do {
		state = gpio_get_pin_level(pdata->gpio_irq);
		if (state == pin_level)
			break;

		//usleep_range(1000, 1000); 
		msleep(1);

		if (time_after_eq(jiffies, timeout)) {
			state = gpio_get_pin_level(pdata->gpio_irq);
			break;
		}
	} while (1);

	if (state == pin_level) {
		dev_dbg(dev, "irq took (%ld) %ums\n",
			interval, jiffies_to_msecs(jiffies - start_wait_jiffies));
		return 0;
	}else {
		dev_warn(dev, "timeout waiting(%ld) for idle %ums\n",
			interval, jiffies_to_msecs(jiffies - start_wait_jiffies));
		return -ETIME;
	}
}

void device_regulator_enable(struct device *dev)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);
	int error;

	if (!pdata)
		return;

	dev_info(dev, "Regulator on\n");

	if (gpio_is_valid(pdata->gpio_reset))
		gpio_set_pin_level(pdata->gpio_reset, 0);

	//vdd power on

	if (pdata->reg_vdd) {
		error = regulator_enable(pdata->reg_vdd);
		if (error) {
			dev_err(dev, "Error %d enabling vdd regulator\n", error);
			return;
		}
	}

	if (pdata->common_vdd_supply == 0) {
		if (pdata->reg_avdd) {
			error = regulator_enable(pdata->reg_avdd);
			if (error) {
				regulator_disable(pdata->reg_vdd);
				dev_err(dev, "Error %d enabling avdd regulator\n",
					error);
				return;
			}
		}
	}

	msleep(20);

	if (gpio_is_valid(pdata->gpio_reset))
		gpio_set_pin_level(pdata->gpio_reset, 1);

	msleep(20);
	
	device_wait_irq_state(dev, 0, 100);
}

void device_regulator_disable(struct device *dev)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);

	if (!pdata)
		return;

	dev_info(dev, "Regulator off\n");

	if (gpio_is_valid(pdata->gpio_reset))
		gpio_set_pin_level(pdata->gpio_reset, 0);

	if (pdata->reg_vdd)
		regulator_disable(pdata->reg_vdd);
	if (pdata->common_vdd_supply == 0) {
		if (pdata->reg_avdd)
			regulator_disable(pdata->reg_avdd);
	}
}

int device_gpio_configure(struct device *dev)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);

	int error = -EINVAL;

	if (!pdata)
		return error;

	/* According to maXTouch power sequencing specification, RESET line
	 * must be kept low until some time after regulators come up to
	 * voltage */

	dev_info(dev, "config gpio: irq[%d] reset[%d]\n", 
		pdata->gpio_irq, pdata->gpio_reset);
	 
	if (gpio_is_valid(pdata->gpio_reset)) {
		/* configure touchscreen reset out gpio */
		gpio_set_pin_direction(pdata->gpio_reset, GPIO_DIRECTION_OUT);
	} else {
		dev_err(dev, "reset gpio not provided\n");
		error = -EIO;
		goto fail;
	}

	if (gpio_is_valid(pdata->gpio_irq)) {
		/* configure touchscreen irq gpio */
		gpio_set_pin_direction(pdata->gpio_reset, GPIO_DIRECTION_IN);
		//may set pullup ...
		gpio_set_pin_pull_mode(pdata->gpio_reset, GPIO_PULL_UP);
	} else {
		dev_err(dev, "irq gpio not provided\n");
		error = -EIO;
		goto fail_release_reset_gpio;
	}
	dev_err(dev, "irq pin config success\n");

	return 0;

fail_release_reset_gpio:
fail:
	dev_err(dev, "gpio config failed\n");

	return error;
}

void device_gpio_free(struct device *dev)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);

	if (!pdata)
		return;

	dev_info(dev, "device gpio free\n");
}

int device_power_init(struct device *dev)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);

	if (!pdata)
		return -ENODEV;

	/*
		VDD: for T series: VDD 3.1~3.3v, for U series VDD 2.7~3.3v
		AVDD: suggest 3.3v for higer SNR
		suggest default value 3.3v both VDD and AVDD
	*/
	
	device_regulator_enable(dev);
	
	return 0;
}

void device_power_deinit(struct device *dev)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);

	if (!pdata)
		return;
}

int device_hw_reset(struct device *dev)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);

	if (!pdata)
		return -EACCES;

	gpio_set_pin_level(pdata->gpio_reset, 0);
	udelay(1500);
	gpio_set_pin_level(pdata->gpio_reset, 1);

	device_wait_irq_state(dev, 0, 100);

	return 0;

}

int device_por_reset(struct device *dev)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);

	if (!pdata)
		return -EACCES;

	device_regulator_disable(dev);
	msleep(100);
	device_regulator_enable(dev);

	device_wait_irq_state(dev, 0, 100);

	return 0;
}

void device_disable_irq(struct device *dev, const char * name_str)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);

	dev_dbg(dev, "irq disabled ++, depth %d, %s\n", pdata->depth, name_str);

	ext_irq_disable(pdata->irq);

	pdata->depth--;

	WARN_ON(pdata->depth < -1);

	dev_dbg(dev, "irq disabled --, depth %d, %s\n", pdata->depth, name_str);
}

void device_disable_irq_nosync(struct device *dev, const char * name_str)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);

	dev_dbg(dev, "irq disabled nosync ++, depth %d, %s\n", pdata->depth, name_str);

	ext_irq_disable(pdata->irq);
	pdata->depth--;

	WARN_ON(pdata->depth < -1);

	dev_dbg(dev, "irq disabled nosync --, depth %d, %s\n", pdata->depth, name_str);
}

void device_disable_irq_wake(struct device *dev)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);

	dev_info(dev, "irq wake disable, depth %d \n", pdata->depth);
}

void device_enable_irq(struct device *dev, const char * name_str)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);

	dev_dbg(dev, "irq enabled ++, depth %d, %s\n", pdata->depth, name_str);

	pdata->depth++;
	WARN_ON(pdata->depth > 0);

	ext_irq_enable(pdata->irq);

	dev_dbg(dev, "irq enabled --, depth %d, %s\n", pdata->depth, name_str);
}

void device_enable_irq_wake(struct device *dev)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);

	dev_info(dev, "irq wake enable, depth %d \n", pdata->depth);
}

void device_free_irq(struct device *dev, void *dev_id, const char * name_str)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);

	if (pdata && pdata->irq) {
		dev_info(dev, "irq free, %d %s\n", pdata->irq, name_str);
		free_irq(pdata->irq, dev_id);
		pdata->depth = 0;
	}
}

static void irq_cb_handler(void)
{
	struct device *dev = t_dev;
	struct mxt_platform_data *pdata = dev_get_platdata(dev);
	
	pdata->irq_fn(pdata->irq, mxt_g_data);
}

int device_register_irq(struct device *dev, irq_handler_t handler,
			irq_handler_t thread_fn, const char *devname, void *dev_id)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);
	int ret;
	
	dev_info(dev, "register irq %d hanlder %p thread_fn %p\n",pdata->irq, handler, thread_fn);

	if (handler)
		pdata->irq_fn = handler;
	else
		pdata->irq_fn = thread_fn;

	ret = ext_irq_register(pdata->irq, irq_cb_handler);	

	return ret;
}

unsigned int t15_key_array[] = {KEY_HOME,KEY_BACK,KEY_MENU};
static bool device_parse_platform(struct device *dev)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);
	int error;

	if (!pdata)
		return NULL;

	pdata->common_vdd_supply = 1;
	pdata->use_regulator = 0;
	if (pdata->use_regulator)
		dev_info(dev, "using suspend method: power off\n");

	/* key list */
	pdata->keymap = t15_key_array;
	pdata->num_keys = ARRAY_SIZE(t15_key_array);
	/* reset, irq gpio info */

	pdata->gpio_irq = GPIO(PORT_PA21, PIN_PA21);	//----- here write irq io number
	//transfer to IRQ
	pdata->irq = 5/*gpio_to_irq(pdata->gpio_irq)*/;

	pdata->irqflags = IRQF_TRIGGER_LOW;

	pdata->gpio_reset = GPIO(PORT_PA20, PIN_PA20);  //---- here write reset io number

	error = device_gpio_configure(dev);
	if (error) {
		dev_err(dev, "failed to config gpio");
		return error;
	}

	error = device_power_init(dev);
	if (error) {
		dev_err(dev, "failed to init power\n");
		goto exit_parser;
	}

	dev_err(dev, "device parse dt successful\n");
	return 0;

exit_parser:
	dev_err(dev, "device parse dt failed\n");
	device_gpio_free(dev);

	return error;
}

static void device_release_platform(struct device *dev)
{
	device_regulator_disable(dev);
	device_power_deinit(dev);
	device_gpio_free(dev);
}

/**
 *	device_parse_default_dts - parse device tree for all hardware resource
 *	@dev: current device of driver
 *
 *	This is a packet of device_parse_dt()
 *
 */

int device_parse_default_chip(void *dev_id, struct device *dev)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);
	int error = 0;

	if (!pdata)
		return -ENODEV;

	error = device_parse_platform(dev);

	if (!error) {
		mxt_g_data = dev_id;
		t_dev = dev;
	}

	return error;
}

void device_release_chip(struct device *dev)
{
	struct mxt_platform_data *pdata = dev_get_platdata(dev);

	if (!pdata)
		return;

	dev_err(dev, "device release dt\n");

	device_release_platform(dev);
	
	mxt_g_data =NULL;
	t_dev = NULL;
}

extern int mxt_probe(struct i2c_client *, const struct i2c_device_id *);
extern int mxt_remove(struct i2c_client *);
extern void mxt_shutdown(struct i2c_client *);

struct i2c_driver mxt_driver = {
	.driver = {
		#ifdef CONFIG_PM_SLEEP
		.pm	= &mxt_pm_ops,
		#endif
	},
	.probe		= mxt_probe,
	.remove		= mxt_remove,
	.shutdown	= mxt_shutdown,
};
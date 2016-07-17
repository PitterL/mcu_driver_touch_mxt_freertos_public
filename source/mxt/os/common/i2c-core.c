/*
 * i2c_core.c
 *
 * Created: 2/27/2016 10:29:40 PM
 *  Author: pitter.liao
 */ 
#include <errno.h>
#include <common/types.h>
#include <common/slab.h>
#include <common/i2c.h>
#include <common/mutex.h>
#include <common/jiffies.h>
#include <common/debug.h>

#include <os_pq.h>

/**
 * i2c_lock_adapter - Get exclusive access to an I2C bus segment
 * @adapter: Target I2C bus segment
 */
static void i2c_lock_adapter(struct i2c_adapter *adapter)
{
	mutex_lock(adapter->bus_lock);
}

/**
 * i2c_trylock_adapter - Try to get exclusive access to an I2C bus segment
 * @adapter: Target I2C bus segment
 */
static int i2c_trylock_adapter(struct i2c_adapter *adapter)
{
	return mutex_trylock(adapter->bus_lock);
}

/**
 * i2c_unlock_adapter - Release exclusive access to an I2C bus segment
 * @adapter: Target I2C bus segment
 */
static void i2c_unlock_adapter(struct i2c_adapter *adapter)
{
	mutex_unlock(adapter->bus_lock);
}

/* ----------------------------------------------------
 * the functional interface to the i2c busses.
 * ----------------------------------------------------
 */

/**
 * __i2c_transfer - unlocked flavor of i2c_transfer
 * @adap: Handle to I2C bus
 * @msgs: One or more messages to execute before STOP is issued to
 *	terminate the operation; each message begins with a START.
 * @num: Number of messages to be executed.
 *
 * Returns negative errno, else the number of messages executed.
 *
 * Adapter lock must be held when calling this function. No debug logging
 * takes place. adap->algo->master_xfer existence isn't checked.
 */
static int __i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
{
	unsigned long orig_jiffies;
	int ret, try;

	/* Retry automatically on arbitration loss */
	orig_jiffies = jiffies;
	for (ret = 0, try = 0; try <= adap->retries; try++) {
		ret = adap->algo->master_xfer(adap, msgs, num);
		if (ret != -EAGAIN)
			break;
		if (time_after(jiffies, orig_jiffies + adap->timeout))
			break;
	}

	return ret;
}

/**
 * i2c_transfer - execute a single or combined I2C message
 * @adap: Handle to I2C bus
 * @msgs: One or more messages to execute before STOP is issued to
 *	terminate the operation; each message begins with a START.
 * @num: Number of messages to be executed.
 *
 * Returns negative errno, else the number of messages executed.
 *
 * Note that there is no requirement that each message be sent to
 * the same slave address, although that is the most common model.
 */
int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
{
	int ret;

	/* REVISIT the fault reporting model here is weak:
	 *
	 *  - When we get an error after receiving N bytes from a slave,
	 *    there is no way to report "N".
	 *
	 *  - When we get a NAK after transmitting N bytes to a slave,
	 *    there is no way to report "N" ... or to let the master
	 *    continue executing the rest of this combined message, if
	 *    that's the appropriate response.
	 *
	 *  - When for example "num" is two and we successfully complete
	 *    the first message but get an error part way through the
	 *    second, it's unclear whether that should be reported as
	 *    one (discarding status on the second message) or errno
	 *    (discarding status on the first one).
	 */

	if (adap->algo->master_xfer) {
#ifdef DEBUG
		for (ret = 0; ret < num; ret++) {
			dev_dbg(NULL, "master_xfer[%d] %c, addr=0x%02x, "
				"len=%d%s\n", ret, (msgs[ret].flags & I2C_M_RD)
				? 'R' : 'W', msgs[ret].addr, msgs[ret].len,
				(msgs[ret].flags & I2C_M_RECV_LEN) ? "+" : "");
		}
#endif

		if (/*in_atomic() || irqs_disabled()*/1) {
			ret = i2c_trylock_adapter(adap);
			if (ret)
				/* I2C activity is ongoing. */
				return -EBUSY;
		} else {
			i2c_lock_adapter(adap);
		}

		ret = __i2c_transfer(adap, msgs, num);
		i2c_unlock_adapter(adap);

		return ret;
	} else {
		dev_dbg(NULL, "I2C level transfers not supported\n");
		return -EOPNOTSUPP;
	}
}

/*
 * An i2c_driver is used with one or more i2c_client (device) nodes to access
 * i2c slave chips, on a bus instance associated with some i2c_adapter.
 */

static int i2c_register_driver(struct i2c_driver *driver)
{
	int res;

	/* When registration returns, the driver core
	 * will have called probe() for all matching-but-unbound devices.
	 */
	res = driver_register(&driver->driver);
	if (res)
		return res;

	pr_debug("i2c-core: driver registered\n");

	return 0;
}

struct i2c_client *i2c_dev_init(struct i2c_driver *driver, unsigned short addr)
{
	struct i2c_adapter *adap;
	struct i2c_client *client;
	int ret;
	/* This creates an anonymous i2c_client, which may later be
	 * pointed to some address using I2C_SLAVE or I2C_SLAVE_FORCE.
	 *
	 * This client is ** NEVER REGISTERED ** with the driver model
	 * or I2C core code!!  It just holds private copies of addressing
	 * information and maybe a PEC flag.
	 */
	client = kzalloc(sizeof(*client), GFP_KERNEL);
	if (!client) {
		return NULL;
	}
	client->addr = addr;
	snprintf(client->name, I2C_NAME_SIZE, "i2c-dev %x", addr);

	adap = kzalloc(sizeof(*adap), GFP_KERNEL);
	if (!adap) {
		kfree(adap);
		return NULL;
	}
	i2c_add_adapter(adap);
	client->adapter = adap;

	ret = i2c_register_driver(driver);
	if (ret) {
		kfree(client);
		return NULL;	
	}
	client->dev.pm = driver->driver.pm;
	client->driver = &driver->driver;

	driver->driver.dev = &client->dev;

	return client;
}

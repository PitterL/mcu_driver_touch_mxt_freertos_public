/*
 * os_i2c.c
 *
 * Created: 3/11/2016 11:05:39 AM
 *  Author: pitter.liao
 */ 
#include <errno.h>
#include <string.h>
#include <status_codes.h>
#include <i2c_master.h>

#include <common/types.h>
#include <common/slab.h>
#include <common/i2c.h>
#include <common/mutex.h>
#include <common/jiffies.h>
#include <common/debug.h>
#include <common/atomic_op.h>
#include <os_pq.h>
#include <os_kthread.h>
#include <os_i2c.h>

static int i2c_master_xfer(void *adp, struct i2c_msg *msgs, int num)
{
	struct i2c_adapter *adap = (struct i2c_adapter *)adp;
	int i;
	enum status_code result;
	
	struct i2c_master_packet packet;

	memset(&packet, 0, sizeof(packet));
	for (i = 0; i < num; i++) {
		packet.address = msgs[i].addr;
		packet.data_length = msgs[i].len;
		packet.data = msgs[i].buf;
		
		if (test_flag(I2C_M_RD, &msgs[i].flags))
			result = i2c_master_read_packet_wait(adap->algo_data, &packet);
		else
			result = i2c_master_write_packet_wait(adap->algo_data, &packet);
			
		if (result != STATUS_OK)
			return -EIO;
	}
	
	return num;
}

struct i2c_algorithm i2c_algo_func = {
	.master_xfer = i2c_master_xfer,
};

struct i2c_master_config i2c_algo_adapter_desc;

int i2c_add_adapter(struct i2c_adapter *adapter)
{
	void *lock;

	lock = mutex_init();
	if (!lock)
		return -ENOMEM;
	
	adapter->algo = &i2c_algo_func;
	adapter->algo_data = &i2c_algo_adapter_desc;

	adapter->bus_lock = lock;
	adapter->retries = 3;
	adapter->timeout = 3 * HZ;
	
	return 0;
}

static void i2c_dev_probe(void *dev)
{
	struct i2c_client *client = dev;
	struct i2c_driver *driver;
	
	if (client) {
		driver = container_of(client->driver, struct i2c_driver, driver);
		driver->probe(client, NULL);
	}

	//vTaskDelete(NULL);
}

int i2c_bus_init(struct i2c_driver *driver, void *hw)
{
	struct i2c_client *client;
	struct i2c_master_config config_i2c_master;
	task_handle_p task;

	client = i2c_dev_init(driver, 0x4a);
	if (!client)
		return -ENOMEM;

	/* master init testing */
	i2c_master_get_config_defaults(&config_i2c_master);
	config_i2c_master.buffer_timeout = 10000;
	switch((unsigned long)hw){	//format: SDA / SCL
		case (unsigned long)SERCOM0:
			config_i2c_master.pinmux_pad0    = PINMUX_PA08C_SERCOM0_PAD0;
			config_i2c_master.pinmux_pad1    = PINMUX_PA09C_SERCOM0_PAD1;
			break;
		case (unsigned long)SERCOM2:
			config_i2c_master.pinmux_pad0    = PINMUX_PA08D_SERCOM2_PAD0;
			config_i2c_master.pinmux_pad1    = PINMUX_PA09D_SERCOM2_PAD1;
			break;
		case (unsigned long)SERCOM4:
			config_i2c_master.pinmux_pad0    = PINMUX_PA12D_SERCOM4_PAD0;
			config_i2c_master.pinmux_pad1    = PINMUX_PA13D_SERCOM4_PAD1;
			break;
		default:
		return -EIO;
	}	
	config_i2c_master.unknown_bus_state_timeout = 10;
	i2c_master_init(client->adapter->algo_data, SERCOM4, &config_i2c_master);
	/* Check for successful initialization */
	i2c_master_enable(client->adapter->algo_data);

	task = kthread_run(i2c_dev_probe, (void *)client,
		(const signed char * const)"Atmel mxt probe");

	if (task)
		return 0;
	
	return -ENOMEM;
}
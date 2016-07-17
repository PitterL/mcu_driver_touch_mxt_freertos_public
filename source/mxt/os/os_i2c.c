/*
 * os_i2c.c
 *
 * Created: 3/11/2016 11:05:39 AM
 *  Author: pitter.liao
 */ 
#include <errno.h>
#include <string.h>
#include <fsl_i2c.h>

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

struct i2c_master_config {
	void *hw;
};

static int i2c_master_xfer(void *adp, struct i2c_msg *msgs, int num)
{
	struct i2c_adapter *adap = (struct i2c_adapter *)adp;
	struct i2c_master_config *desc = (struct i2c_master_config *)adap->algo_data;
	int i;
	int result;

	i2c_master_transfer_t packet;

#define I2C_MASTER_SLAVE_ADDR_7BIT 0x7EU
	packet.subaddress = 0;
	packet.subaddressSize = 0;
	packet.flags = kI2C_TransferDefaultFlag;

	memset(&packet, 0, sizeof(packet));
	for (i = 0; i < num; i++) {
		packet.slaveAddress = msgs[i].addr & I2C_MASTER_SLAVE_ADDR_7BIT;
		packet.dataSize = msgs[i].len;
		packet.data = msgs[i].buf;
		
		if (test_flag(I2C_M_RD, &msgs[i].flags))
			packet.direction = kI2C_Read;
		else
			packet.direction = kI2C_Write;

		result = I2C_MasterTransferBlocking((I2C_Type *)desc->hw, &packet);
			
		if (result)
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

struct i2c_client *i2c_bus_init(struct i2c_driver *driver, void *hw)
{
	struct i2c_client *client;
    i2c_master_config_t masterConfig;
    uint32_t sourceClock;
    task_handle_p task;
    clock_name_t clock;
    struct i2c_master_config *desc;

	client = i2c_dev_init(driver, 0x4a);
	if (!client)
		return NULL;

	/* master init testing */
	switch((unsigned long)hw){	//format: SDA / SCL
		case (unsigned long)I2C0:
			clock = I2C0_CLK_SRC;
			break;
		default:
		case (unsigned long)I2C1:
			clock = I2C0_CLK_SRC;
			break;
	}
#define I2C_BAUDRATE 100000U
	/*
	 * masterConfig.baudRate_Bps = 100000U;
	 * masterConfig.enableHighDrive = false;
	 * masterConfig.enableStopHold = false;
	 * masterConfig.glitchFilterWidth = 0U;
	 * masterConfig.enableMaster = true;
	 */
	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = I2C_BAUDRATE;
	sourceClock = CLOCK_GetFreq(clock);

	desc = (struct i2c_master_config *)client->adapter->algo_data;
	desc->hw = hw;
	I2C_MasterInit(desc->hw, &masterConfig, sourceClock);

	task = kthread_run(i2c_dev_probe, (void *)client,
		(const char * const)"Atmel mxt probe");

	if (task)
		return client;
	
	return NULL;
}

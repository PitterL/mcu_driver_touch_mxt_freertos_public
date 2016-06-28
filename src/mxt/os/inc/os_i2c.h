/*
 * os_i2c.h
 *
 * Created: 3/12/2016 10:51:38 AM
 *  Author: pitter.liao
 */ 


#ifndef OS_I2C_H_
#define OS_I2C_H_

#include <common/i2c.h>

int i2c_bus_init(struct i2c_driver *driver, void *hw);

#endif /* OS_I2C_H_ */
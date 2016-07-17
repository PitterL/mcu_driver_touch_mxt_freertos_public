/*
 * io_asf_wrapper.c
 *
 * Created: 3/11/2016 3:51:14 PM
 *  Author: pitter.liao
 */ 

#include <fsl_port.h>
#include <fsl_gpio.h>
#include <fsl_common.h>
#include "hal_ext_irq.h"
#include "hpl_gpio.h"
//#include <status_codes.h>

int32_t ext_irq_register(const uint32_t irq, const uint8_t gpio, ext_irq_cb_t cb)
{
    /* Init input switch GPIO. */
	//GPIO_PinInit(BOARD_CHG_GPIO, BOARD_CHG_GPIO_PIN, &sw_config);
	//PORT_SetPinInterruptConfig(BOARD_CHG_PORT, BOARD_CHG_GPIO_PIN, kPORT_InterruptLogicZero);
	_gpio_set_pin_irq(GPIO_PORT(gpio), GPIO_PIN(gpio), kPORT_InterruptLogicZero);
	EnableIRQ(irq);

	return 0;
}

int32_t ext_irq_enable(const uint32_t irq)
{
	EnableIRQ(irq);

	return 0;
}

int32_t ext_irq_disable(const uint32_t irq)
{
	DisableIRQ(irq);

	return 0;
}

void ext_irq_clear(const uint8_t gpio)
{
	_gpio_clr_pin_irq(GPIO_PORT(gpio), GPIO_PIN(gpio));
}

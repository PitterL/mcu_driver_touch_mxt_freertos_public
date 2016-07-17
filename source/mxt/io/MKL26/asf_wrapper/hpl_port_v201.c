/**
 * \file
 *
 * \brief SAM PORT.
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <fsl_common.h>
#include <fsl_gpio.h>
#include <fsl_port.h>
//#include <compiler.h>
#include <stdint.h>

#include "hpl_gpio.h"
#include "hpl_port_v201.h"
//#include <utils_assert.h>

#define CRITICAL_SECTION_ENTER();
#define CRITICAL_SECTION_LEAVE();

static GPIO_Type *gpio_port_group[NUM_GPIO_PORTS] = GPIO_BASE_PTRS;
static clock_ip_name_t gpio_clock_group[NUM_GPIO_PORTS] = {
		kCLOCK_PortA,
		kCLOCK_PortB,
		kCLOCK_PortC,
		kCLOCK_PortD,
		kCLOCK_PortE,
};
/**
 * \brief Set direction on port with mask
 */
void _gpio_set_direction(const enum gpio_port port,
		const uint32_t pin,
		const enum gpio_direction direction)
{
    gpio_pin_config_t config = {
        kGPIO_DigitalOutput, 0,
    };

    ASSERT(port < NUM_GPIO_PORTS);

	switch (direction) {
	case GPIO_DIRECTION_OFF:
		break;

	case GPIO_DIRECTION_IN:
		config.pinDirection = kGPIO_DigitalInput;
		GPIO_PinInit(gpio_port_group[port], pin, &config);
		break;

	case GPIO_DIRECTION_OUT:
		GPIO_PinInit(gpio_port_group[port], pin, &config);
		break;

	default:
		ASSERT(false);
	}
}

/**
 * \brief Set output level on port with mask
 */
void _gpio_set_level(const enum gpio_port port,
		const uint32_t pin,
		const bool level)
{
	ASSERT(port < NUM_GPIO_PORTS);

	GPIO_WritePinOutput(gpio_port_group[port], pin, level);
}

/**
 * \brief Change output level to the opposite with mask
 */
void _gpio_toggle_level(const enum gpio_port port, const uint32_t pin)
{
	ASSERT(port < NUM_GPIO_PORTS);

	GPIO_TogglePinsOutput(gpio_port_group[port], pin);
}

/**
 * \brief Get input levels on all port pins
 */
uint32_t _gpio_get_level(const enum gpio_port port)
{
	ASSERT(port < NUM_GPIO_PORTS);

	return GPIO_ReadPortInput(gpio_port_group[port]);
}

/**
 * \brief Set pin pull mode
 */
void _gpio_set_pin_pull_mode(const enum gpio_port port,
		const uint8_t pin,
		const enum gpio_pull_mode pull_mode)
{
	port_pin_config_t config;

	ASSERT(port < NUM_GPIO_PORTS);

	PORT_GetPinConfig((PORT_Type *)gpio_port_group[port], pin, &config);

	switch(pull_mode) {
	case GPIO_PULL_OFF:
		config.pullSelect = kPORT_PullDisable;
		break;

	case GPIO_PULL_UP:
		config.pullSelect = kPORT_PullUp;
		break;

	case GPIO_PULL_DOWN:
		config.pullSelect = kPORT_PullDown;
		break;

	default:
		ASSERT(false);
		break;
	}

	PORT_SetPinConfig((PORT_Type *)gpio_port_group[port], pin, &config);
}

/**
 * \brief Set gpio pin function
 */
void _gpio_set_pin_function(const uint8_t gpio, const uint32_t function)
{
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	ASSERT(port < NUM_GPIO_PORTS);

	if(function == GPIO_PIN_FUNCTION_OFF) {
		CLOCK_DisableClock(gpio_clock_group[port]);
	} else {
	    /* Enable GPIO port clock */
	    CLOCK_EnableClock(gpio_clock_group[port]);
	    /* Led pin mux Configuration */
	    PORT_SetPinMux((PORT_Type *)gpio_port_group[port], pin, function);
	}
}

/**
 * \brief Set gpio pin irq type
*/
void _gpio_set_pin_irq(const enum gpio_port port,
		const uint8_t pin, port_interrupt_t type)
{
	ASSERT(port < NUM_GPIO_PORTS);

	PORT_SetPinInterruptConfig((PORT_Type *)gpio_port_group[port], pin, type);
}


/**
 * \brief clear gpio pin irq status
*/
void _gpio_clr_pin_irq(const enum gpio_port port,
		const uint8_t pin)
{
	ASSERT(port < NUM_GPIO_PORTS);

	GPIO_ClearPinsInterruptFlags(gpio_port_group[port], 1 << pin);
}

#ifndef __LINUX_ATMEL_MXT_IO
#define __LINUX_ATMEL_MXT_IO

#include <common/types.h>
#include <common/interrupt.h>

#define MXT_I2C_DMA_ADDR_FLAG	0

#define MXT_RESET_TIME		200	/* msec */

/* The platform data for the Atmel maXTouch touchscreen driver */
struct mxt_platform_data {
	unsigned long irqflags;
	int depth;		/* irq nested counter*/
	int irq;			/* irq issued by device 	*/

	irq_handler_t irq_fn;

	unsigned gpio_reset;
	unsigned gpio_irq;

	bool use_retrigen_workaround;
	bool use_regulator;
	bool common_vdd_supply;

	int reg_vdd;
	int reg_avdd;

#if defined(CONFIG_MXT_T15_KEYS_SUPPORT)
	int num_keys;
	const unsigned int *keymap;
#endif

	bool mem_allocated;
};

static inline bool gpio_is_valid(const uint8_t gpio)
{
	return !!gpio;
}

static inline int gpio_to_irq(const uint8_t gpio)
{
	return gpio;
}

static inline void free_irq(int irq, void *dev_id)
{
	
}

static inline int regulator_enable(int regulator)
{
	return 0;
}

static inline int regulator_disable(int regulator)
{
	return 0;
}

int device_wait_irq_state(struct device *dev, int val, long interval);
void device_regulator_enable(struct device *dev);
void device_regulator_disable(struct device *dev);
int device_gpio_configure(struct device *dev);
void device_gpio_free(struct device *dev);
int device_power_init(struct device *dev);
void device_power_deinit(struct device *dev);
int device_hw_reset(struct device *dev);
int device_por_reset(struct device *dev);
void device_disable_irq(struct device *dev, const char * name_str);
void device_disable_irq_nosync(struct device *dev, const char * name_str);
void device_disable_irq_wake(struct device *dev);
void device_enable_irq(struct device *dev, const char * name_str);
void device_enable_irq_wake(struct device *dev);
void device_free_irq(struct device *dev, void *dev_id, const char * name_str);
int device_register_irq(struct device *dev, irq_handler_t handler,
			irq_handler_t thread_fn, const char *devname, void *dev_id);
int device_parse_default_chip(void *dev_id, struct device *dev);
void device_release_chip(struct device *dev);

#endif


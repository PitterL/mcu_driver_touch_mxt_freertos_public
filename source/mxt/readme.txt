Folder PATH listing
Volume serial number is 1E2D-BBC9
D:.
©¦  mxt.c	//main driver file
©¦  
©À©¤include
©¦      config.h	//global driver config file
©¦      
©À©¤io
©¦  ©¸©¤MKL26
©¦      ©¦  io.c	//driver io interface (porting)
©¦      ©¦  io.h
©¦      ©¦  
©¦      ©¸©¤asf_wrapper
©¦              hal_ext_irq.h	
©¦              hal_gpio.h
©¦              hpl_ext_irq.c	//bsp irq wrapper(porting)
©¦              hpl_ext_irq.h
©¦              hpl_gpio.h
©¦              hpl_port_v201.c	//bsp gpio wrapper(porting)
©¦              hpl_port_v201.h
©¦              
©À©¤os
©¦  ©¦  os_debug.c
©¦  ©¦  os_i2c.c	//bsp i2c interface(porting)
©¦  ©¦  os_if.c	//bsp i2c os interface(porting)
©¦  ©¦  os_input.c	//bsp input event i/o(adapt to your upleve)
©¦  ©¦  
©¦  ©À©¤common
©¦  ©¦      device.c
©¦  ©¦      firmware.c
©¦  ©¦      i2c-core.c
©¦  ©¦      input.c
©¦  ©¦      
©¦  ©¸©¤inc
©¦      ©¦  os_dma_mapping.h
©¦      ©¦  os_gpio.h	//gpio definition(porting)
©¦      ©¦  os_i2c.h
©¦      ©¦  os_if.h
©¦      ©¦  os_irqflags.h
©¦      ©¦  os_kthread.h	//os thread parameter(priority/stack depth)
©¦      ©¦  os_pq.h
©¦      ©¦  
©¦      ©¸©¤common
©¦              atomic_op.h
©¦              big_endian.h
©¦              bitops.h
©¦              completion.h
©¦              debug.h
©¦              device.h
©¦              errno2.h
©¦              firmware.h
©¦              generic.h	//little endian
©¦              i2c.h
©¦              input.h
©¦              interrupt.h
©¦              irqreturn.h
©¦              jiffies.h
©¦              kernel.h
©¦              list.h
©¦              little_endian.h
©¦              mt.h
©¦              mutex.h
©¦              non-atomic.h
©¦              poison.h
©¦              sched.h
©¦              slab.h
©¦              swab.h
©¦              types.h
©¦              vector.h
©¦              wait.h
©¦              
©¸©¤plugin
        plug.a
        plug.h
 
-----------------------
Driver probe route:
-----------------------

main()->
	sys_platform_init()-> 		//driver probe(os_if.c)
		i2c_bus_init()->		//i2c bus initialize(os_i2c.c)
			i2c_dev_init()->		//i2c device initialize (i2c-core.c)
				i2c_dev_probe()->		//i2c device probe (os_i2c.c)
					mxt_probe()->			//TP probe (mxt.c)
						mxt_handle_pdata()->		//gpio,power,irq initialize£¨call io.c device_parse_default_chip()£©
							mxt_initialize()->		//TP chip infomation initialize (mxt.c)
								mxt_acquire_irq()->		//irq register (call io.c device_register_irq())
									mxt_process_message_thread()	//Touch event thread
									mxt_process_message_async_thread()	//extra event thread
->vTaskStartScheduler()

-----------------------
FreeRTOSConfig.h
-----------------------
set heap for memory use in driver

#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 20000 ) )
#define configENABLE_BACKWARD_COMPATIBILITY     1

-----------------------
interface
-----------------------
message:
mxt_proc_t100_message()
ABS_MT_SLOT	:  select slot
ABS_MT_TRACKING_ID  :  tracking id
SYN_REPORT£º frame sync
ABS_MT_POSITION_X	x axis
ABS_MT_POSITION_Y	y axis
ABS_HAT0X	£º point relative position (left/right)
ABS_MT_ORIENTATION	:	point vector information

message to up-layer
OS_input.c
void input_event_hook(int type, unsigned int code, int value);

-----------------------

plugin parameters
-----------------------
command: plugin <id> <op> <rotation> <algorithm>";
<id>:	6	--- finger relative position algorithm
<op>:
	set
	clr	--- clear all algorithm, no need following parameters
<rotation>:
	portrait
	landscape
<algorithm>:
	bit[0]:	vector check
	bit[1]: finger relative poistion calculate

e.g.
cmd = "plugin 6 set landscape 3";	//landscape, enable both vector and finger
cmd = "plugin 6 clr";			//clean all algorithm
mxt_store(mxt_driver.driver.dev, NULL, cmd, strlen(cmd));



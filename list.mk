BSP_ROOT = BSP
APP_ROOT = APP
COMPONENTS_ROOT = Components

BSP_C_SOURCES = \
bsp_uart.c \
bsp_i2c.c \
bsp_motor.c \
bsp_led.c \
bsp_init.c \
bsp_scan.c \
bsp_chasis.c

APP_C_SOURCES = \
app_entry.c \
line.c \
command.c

Components_C_SOURCES = \
algs/fifo.c \
algs/pid.c \
openocd_rtos_helper/FreeRTOS-openocd.c

Components_C_INCLUDES = \
algs/Inc \
gw_gray \

User_C_Sources = \
$(addprefix $(BSP_ROOT)/, $(BSP_C_SOURCES)) \
$(addprefix $(COMPONENTS_ROOT)/, $(Components_C_SOURCES)) \
$(addprefix $(APP_ROOT)/, $(APP_C_SOURCES))

USER_C_INCLUDES = Drivers/CMSIS/DSP/Include\
-IAPP/Inc \
-IAPP/Entries \
-IBSP/Inc \
$(addprefix -I$(COMPONENTS_ROOT)/, $(Components_C_INCLUDES))

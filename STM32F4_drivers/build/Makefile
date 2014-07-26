LIB = libSTM32F4xx_drivers.a

CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
RANLIB = arm-none-eabi-ranlib

CFLAGS = -Wall -O2 -mthumb
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard -falign-functions=16
CFLAGS += -mcpu=cortex-m4 -fsingle-precision-constant -funsigned-char -Wundef -Wsign-compare -Wunreachable-code -Wstrict-prototypes
CFLAGS += -I../inc -I../../core -DARM_MATH_CM4 -DUSE_STDPERIPH_DRIVER

SRCS = \
../src/misc.c \
../src/stm32f4xx_adc.c \
../src/stm32f4xx_can.c \
../src/stm32f4xx_crc.c \
../src/stm32f4xx_cryp.c \
../src/stm32f4xx_cryp_aes.c \
../src/stm32f4xx_cryp_des.c \
../src/stm32f4xx_cryp_tdes.c \
../src/stm32f4xx_dac.c \
../src/stm32f4xx_dbgmcu.c \
../src/stm32f4xx_dcmi.c \
../src/stm32f4xx_dma.c \
../src/stm32f4xx_exti.c \
../src/stm32f4xx_flash.c \
../src/stm32f4xx_fsmc.c \
../src/stm32f4xx_gpio.c \
../src/stm32f4xx_hash.c \
../src/stm32f4xx_hash_md5.c \
../src/stm32f4xx_hash_sha1.c \
../src/stm32f4xx_i2c.c \
../src/stm32f4xx_iwdg.c \
../src/stm32f4xx_pwr.c \
../src/stm32f4xx_rcc.c \
../src/stm32f4xx_rng.c \
../src/stm32f4xx_rtc.c \
../src/stm32f4xx_sdio.c \
../src/stm32f4xx_spi.c \
../src/stm32f4xx_syscfg.c \
../src/stm32f4xx_tim.c \
../src/stm32f4xx_usart.c \
../src/stm32f4xx_wwdg.c 

OBJS = $(SRCS:.c=.o)

all: $(LIB)

$(LIB): $(OBJS)
	@$(AR) -r $(LIB) $(OBJS)
	@$(RANLIB) $(LIB)

%.o : %.c
	@echo "  GCC $^"
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	-rm -f $(OBJS)
	-rm -f $(LIB)

.PHONY: all clean

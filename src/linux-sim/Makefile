# Additional linux build rules

dirs-y += src/linux-sim src/generic

src-y += linux-sim/main.c linux-sim/timer.c linux-sim/console.c linux-sim/watchdog.c
src-y += linux-sim/pca9685.c linux-sim/spidev.c linux-sim/analog.c linux-sim/hard_pwm.c
src-y += linux-sim/i2c.c linux-sim/gpio.c generic/crc16_ccitt.c generic/alloc.c
src-y += linux-sim/sensor_ds18b20.c

CFLAGS_klipper.elf += -lutil -lrt -lpthread

flash: $(OUT)klipper.elf
	@echo "  Flashing"
	$(Q)sudo ./scripts/flash-linux.sh $(OUT)

#ifndef __LINUX_SIM_GPIO_H
#define __LINUX_SIM_GPIO_H

#include <stdint.h> // uint8_t
#include "internal.h" // MAX_GPIO_LINES

enum pin_mode {
    PM_NONE = 0,
    PM_BIN_IN = 1,
    PM_BIN_OUT = 2,
    PM_ANALOG_IN = 3,
    PM_PWM_OUT = 4
};

enum pin_action {
    PA_NONE = 0,
    PA_GPIO_OUT_SETUP,
    PA_GPIO_OUT_RESET,
    PA_GPIO_OUT_TOGGLE_NOIRQ,
    PA_GPIO_OUT_TOGGLE,
    PA_GPIO_OUT_WRITE,
    PA_GPIO_IN_SETUP,
    PA_GPIO_IN_RESET,
    PA_GPIO_IN_READ,
    PA_PWM_SETUP,
    PA_PWM_WRITE,
    PA_ADC_SETUP,
    PA_ADC_SAMPLE,
    PA_ADC_READ,
    PA_ADC_CANCEL_SAMPLE
};

enum device_type {
    DT_NONE = 0,
    DT_STEPPER_X,
    DT_STEPPER_Y,
    DT_STEPPER_Z,
    DT_EXTRUDER,
    DT_FUN,
    DT_BED,
    DT_LED
};

struct gpio_line {
//////    int chipid;
//////    int offset;
//////    int fd;
    int state;
    int num;
    enum pin_mode pinMode;
    enum pin_action pinAction;
    enum device_type deviceType;
};

// gpio.c
extern struct gpio_line gpio_lines[9 * MAX_GPIO_LINES];

struct gpio_out {
    struct gpio_line* line;
};
struct gpio_out gpio_out_setup(uint32_t pin, uint8_t val);
void gpio_out_reset(struct gpio_out g, uint8_t val);
void gpio_out_toggle_noirq(struct gpio_out g);
void gpio_out_toggle(struct gpio_out g);
void gpio_out_write(struct gpio_out g, uint8_t val);

struct gpio_in {
    struct gpio_line* line;
};
struct gpio_in gpio_in_setup(uint32_t pin, int8_t pull_up);
void gpio_in_reset(struct gpio_in g, int8_t pull_up);
uint8_t gpio_in_read(struct gpio_in g);

struct gpio_adc {
    int fd;
};
struct gpio_adc gpio_adc_setup(uint32_t pin);
uint32_t gpio_adc_sample(struct gpio_adc g);
uint16_t gpio_adc_read(struct gpio_adc g);
void gpio_adc_cancel_sample(struct gpio_adc g);

struct spi_config {
    int fd;
    int rate;
};
struct spi_config spi_setup(uint32_t bus, uint8_t mode, uint32_t rate);
void spi_prepare(struct spi_config config);
void spi_transfer(struct spi_config config, uint8_t receive_data
                  , uint8_t len, uint8_t *data);

struct gpio_pwm {
    int duty_fd, enable_fd;
    uint32_t period;
};
struct gpio_pwm gpio_pwm_setup(uint32_t pin, uint32_t cycle_time, uint16_t val);
void gpio_pwm_write(struct gpio_pwm g, uint16_t val);

struct i2c_config {
    int fd;
    uint8_t addr;
};

struct i2c_config i2c_setup(uint32_t bus, uint32_t rate, uint8_t addr);
int i2c_write(struct i2c_config config, uint8_t write_len, uint8_t *write);
int i2c_read(struct i2c_config config, uint8_t reg_len, uint8_t *reg
              , uint8_t read_len, uint8_t *read);

#endif // gpio.h

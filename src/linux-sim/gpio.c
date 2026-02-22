// Very basic support via a Linux gpiod device
//
// Copyright (C) 2017-2021  Kevin O'Connor <kevin@koconnor.net>
//
// This file may be distributed under the terms of the GNU GPLv3 license.

#include <fcntl.h> // open
#include <stdio.h> // snprintf
#include <stdlib.h> // atexit
#include <string.h> // memset
////// #include <sys/ioctl.h> // ioctl
#include <unistd.h> // close
#include <linux/gpio.h> // GPIOHANDLE_REQUEST_OUTPUT
#include "command.h" // shutdown
#include "gpio.h" // gpio_out_write
#include "internal.h" // report_errno
#include "sched.h" // sched_shutdown
#include "debug.h"
////// #define GPIO_CONSUMER "klipper"

DECL_ENUMERATION_RANGE("pin", "PA0", GPIO('A', 0), MAX_GPIO_LINES);
DECL_ENUMERATION_RANGE("pin", "PB0", GPIO('B', 0), MAX_GPIO_LINES);
DECL_ENUMERATION_RANGE("pin", "PC0", GPIO('C', 0), MAX_GPIO_LINES);
DECL_ENUMERATION_RANGE("pin", "PD0", GPIO('D', 0), MAX_GPIO_LINES);
DECL_ENUMERATION_RANGE("pin", "PE0", GPIO('E', 0), MAX_GPIO_LINES);
DECL_ENUMERATION_RANGE("pin", "PF0", GPIO('F', 0), MAX_GPIO_LINES);
DECL_ENUMERATION_RANGE("pin", "PG0", GPIO('G', 0), MAX_GPIO_LINES);
DECL_ENUMERATION_RANGE("pin", "PH0", GPIO('H', 0), MAX_GPIO_LINES);
DECL_ENUMERATION_RANGE("pin", "PI0", GPIO('I', 0), MAX_GPIO_LINES);

//enum pin_mode {
//    PM_NONE = 0,
//    PM_BIN_IN = 1,
//    PM_BIN_OUT = 2,
//    PM_ANALOG_IN = 3,
//    PM_PWM_OUT = 4
//};
//
//enum pin_action {
//    PA_NONE = 0,
//    PA_GPIO_OUT_SETUP,
//    PA_GPIO_OUT_RESET,
//    PA_GPIO_OUT_TOGGLE_NOIRQ,
//    PA_GPIO_OUT_TOGGLE,
//    PA_GPIO_OUT_WRITE,
//    PA_GPIO_IN_SETUP,
//    PA_GPIO_IN_RESET,
//    PA_GPIO_IN_READ,
//    PA_PWM_SETUP,
//    PA_PWM_WRITE,
//    PA_ADC_SETUP,
//    PA_ADC_SAMPLE,
//    PA_ADC_READ,
//    PA_ADC_CANCEL_SAMPLE
//};
//
//enum device_type {
//    DT_NONE = 0,
//    DT_STEPPER_X,
//    DT_STEPPER_Y,
//    DT_STEPPER_Z,
//    DT_EXTRUDER,
//    DT_FUN,
//    DT_BED,
//    DT_LED
//};
//
//struct gpio_line {
////////    int chipid;
////////    int offset;
////////    int fd;
//    int state;
//    int num;
//    enum pin_mode pinMode;
//    enum pin_action pinAction;
//    enum device_type deviceType;
//};

struct gpio_line gpio_lines[9 * MAX_GPIO_LINES];
////// static int gpio_chip_fd[9] = { -1, -1, -1, -1, -1, -1, -1, -1, -1 };

//////static int
//////get_chip_fd(uint8_t chipId)
//////{
//////    if (gpio_chip_fd[chipId] >= 0)
//////        return gpio_chip_fd[chipId];
//////    char chipFilename[64];
//////    snprintf(chipFilename, sizeof(chipFilename), "/dev/gpiochip%u", chipId);
//////    int ret = access(chipFilename, F_OK);
//////    if (ret < 0) {
//////        report_errno("gpio access", ret);
//////        shutdown("GPIO chip device not found");
//////    }
//////    int fd = open(chipFilename, O_RDWR | O_CLOEXEC);
//////    if (fd < 0) {
//////        report_errno("gpio open", fd);
//////        shutdown("Unable to open GPIO chip device");
//////    }
//////    gpio_chip_fd[chipId] = fd;
//////    int i;
//////    for (i=0; i<MAX_GPIO_LINES; i++) {
//////        gpio_lines[GPIO(chipId, i)].offset = i;
//////        gpio_lines[GPIO(chipId, i)].fd = -1;
//////        gpio_lines[GPIO(chipId, i)].chipid = chipId;
//////    }
//////    return fd;
//////}

struct gpio_out
gpio_out_setup(uint32_t pin, uint8_t val)
{
    if (pin >= ARRAY_SIZE(gpio_lines)) shutdown("Not an output pin");
//////    struct gpio_line *line = &gpio_lines[pin];
//////    line->offset = GPIO2PIN(pin);
//////    line->chipid = GPIO2PORT(pin);
//////    struct gpio_out g = { .line = line };
    debug_log_gpio_out_setup(pin, val);
    gpio_lines[pin].num = pin;
    struct gpio_out g = { .line = &gpio_lines[pin] };
    gpio_out_reset(g,val);
    return g;
}

static void
gpio_release_line(struct gpio_line *line)
{
    line->pinMode = PM_NONE;
    line->pinAction = PA_NONE;
    line->deviceType = DT_NONE;
//////    line->state = 0;
//////    if (line->fd > 0) {
//////        close(line->fd);
//////        line->fd = -1;
//////    }
}

void
gpio_out_reset(struct gpio_out g, uint8_t val)
{
    debug_log_gpio_out_reset(g, val);
    gpio_release_line(g.line);
    g.line->pinMode = PM_BIN_OUT;
    g.line->state = !!val;

//////     struct gpiohandle_request req;
//////     memset(&req, 0, sizeof(req));
//////     req.lines = 1;
//////     req.flags = GPIOHANDLE_REQUEST_OUTPUT;
//////     req.lineoffsets[0] = g.line->offset;
//////     req.default_values[0] = !!val;
//////     strncpy(req.consumer_label, GPIO_CONSUMER, sizeof(req.consumer_label) - 1);
//////     int fd = 0; ////// get_chip_fd(g.line->chipid);
//////     int ret = 0; ////// ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
//////     if (ret < 0) {
//////         report_errno("gpio_out_reset get line", ret);
//////         shutdown("Unable to open out GPIO chip line");
//////     }
//////     set_close_on_exec(req.fd);
//////     g.line->fd = req.fd;
//////     g.line->state = !!val;
}

void
gpio_out_write(struct gpio_out g, uint8_t val)
{
    debug_log_gpio_out_write(g, val);
    g.line->state = !!val;
//////     struct gpiohandle_data data;
//////     memset(&data, 0, sizeof(data));
//////     data.values[0] = !!val;
//////     ioctl(g.line->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
//////     g.line->state = !!val;
}

void
gpio_out_toggle_noirq(struct gpio_out g)
{
    debug_log_gpio_out_toggle_noirq(g);
    gpio_out_write(g, !g.line->state);
}

void
gpio_out_toggle(struct gpio_out g)
{
    debug_log_gpio_out_toggle(g);
    gpio_out_toggle_noirq(g);
}

struct gpio_in
gpio_in_setup(uint32_t pin, int8_t pull_up)
{
    if (pin >= ARRAY_SIZE(gpio_lines)) shutdown("Not an input pin");
    gpio_lines[pin].pinMode = PM_BIN_IN;
    gpio_lines[pin].num = pin;
    gpio_lines[pin].state = 1;
    struct gpio_in g = { .line = &gpio_lines[pin] };
    debug_log_gpio_in_setup(pin, pull_up);
    gpio_in_reset(g, pull_up);
    return g;
//////     if (pin >= ARRAY_SIZE(gpio_lines)) shutdown("Not an input pin");
//////     struct gpio_line *line = &gpio_lines[pin];
//////     line->offset = GPIO2PIN(pin);
//////     line->chipid = GPIO2PORT(pin);
//////     struct gpio_in g = { .line = line };
//////     gpio_in_reset(g, pull_up);
//////     return g;
}

void
gpio_in_reset(struct gpio_in g, int8_t pull_up)
{
    gpio_release_line(g.line);
    debug_log_gpio_in_reset(g, pull_up);

//////     struct gpiohandle_request req;
//////     memset(&req, 0, sizeof(req));
//////     req.lines = 1;
//////     req.flags = GPIOHANDLE_REQUEST_INPUT;
////// #if defined(GPIOHANDLE_REQUEST_BIAS_PULL_UP)
//////     if (pull_up > 0) {
//////         req.flags |= GPIOHANDLE_REQUEST_BIAS_PULL_UP;
//////     } else if (pull_up < 0) {
//////         req.flags |= GPIOHANDLE_REQUEST_BIAS_PULL_DOWN;
//////     }
////// #endif
//////     req.lineoffsets[0] = g.line->offset;
//////     strncpy(req.consumer_label, GPIO_CONSUMER, sizeof(req.consumer_label) - 1);
//////     int fd = 0;////// get_chip_fd(g.line->chipid);
//////     int ret = 0; ////// ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
//////     if (ret < 0) {
//////         report_errno("gpio_in_reset get line", ret);
//////         shutdown("Unable to open in GPIO chip line");
//////     }
//////     set_close_on_exec(req.fd);
//////     g.line->fd = req.fd;
}

uint8_t
gpio_in_read(struct gpio_in g)
{
    g.line->state = gpio_lines[9].state;
    //debug_log_gpio_in_read(g);
    return g.line->state;
//////     struct gpiohandle_data data;
//////     memset(&data, 0, sizeof(data));
//////     ioctl(g.line->fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
//////     return data.values[0];
}

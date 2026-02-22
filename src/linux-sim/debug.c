#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include "debug.h"

static int d_fd = -1;
static uint64_t base_time = 0;

void log_write(const char *text, int len)
{
    if (d_fd < 0) return;

    (void)write(d_fd, text, len);
}

const char* pin_to_text(uint32_t pin)
{
    static char text[5];
    text[0] = 'P';
    text[1] = (pin / 8) + 'A';
    text[2] = (pin % 8) + '0';
    text[3] = '\0';
    return text;
}

uint64_t now_us(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t time = (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
    return time - base_time;
}

int debug_log_open(const char *path)
{
    d_fd = open(path, O_CREAT|O_WRONLY|O_TRUNC|O_CLOEXEC, 0644);
    base_time = 0;
    base_time = now_us();
///    for (int i = 0; i < 9*8; i++)
///      log_write(pin_to_text(i), 5);
    return (d_fd < 0) ? 1 : 0;
}

void debug_log_close(void)
{
    if (d_fd >= 0)
    {
        close(d_fd);
        d_fd = -1;
    }
}

void debug_log_gpio_out_setup(uint32_t pin, uint8_t val)
{
    char text[96];
    int n = snprintf(text, sizeof(text),
        "%012lluus gpio_out_setup(pin = %u(%s), val = %u)\n",
        (unsigned long long)now_us(), pin, pin_to_text(pin), val);
    log_write(text, n);
}

void debug_log_gpio_out_reset(struct gpio_out g, uint8_t val)
{
    char text[96];
    int state = g.line->state;
    int num = g.line->num;
    int pm = g.line->pinMode;
    int pa = g.line->pinAction;
    int dt = g.line->deviceType;

    int n = snprintf(text, sizeof(text),
        "%012lluus gpio_out_reset(pin = %u(%s), val = %u)\n",
        (unsigned long long)now_us(), num, pin_to_text(num), val);
    log_write(text, n);
}

void debug_log_gpio_out_toggle_noirq(struct gpio_out g)
{
    char text[96];
    int state = g.line->state;
    int num = g.line->num;
    int pm = g.line->pinMode;
    int pa = g.line->pinAction;
    int dt = g.line->deviceType;

    int n = snprintf(text, sizeof(text),
        "%012lluus gpio_out_toggle_noirq(pin = %u(%s), oldval = %u)\n",
        (unsigned long long)now_us(), num, pin_to_text(num), state);
    log_write(text, n);
}

void debug_log_gpio_out_toggle(struct gpio_out g)
{
    char text[96];
    int state = g.line->state;
    int num = g.line->num;
    int pm = g.line->pinMode;
    int pa = g.line->pinAction;
    int dt = g.line->deviceType;

    int n = snprintf(text, sizeof(text),
        "%012lluus gpio_out_toggle(pin = %u(%s), oldval = %u)\n",
        (unsigned long long)now_us(), num, pin_to_text(num), state);
    log_write(text, n);
}

void debug_log_gpio_out_write(struct gpio_out g, uint8_t val)
{
    char text[96];
    int state = g.line->state;
    int num = g.line->num;
    int pm = g.line->pinMode;
    int pa = g.line->pinAction;
    int dt = g.line->deviceType;

    int n = snprintf(text, sizeof(text),
        "%012lluus gpio_out_write(pin = %u(%s), val = %u)\n",
        (unsigned long long)now_us(), num, pin_to_text(num), val);
    log_write(text, n);
}

void debug_log_gpio_in_setup(uint32_t pin, int8_t pull_up)
{
    char text[96];
    int n = snprintf(text, sizeof(text),
        "%012lluus gpio_in_setup(pin = %u(%s), pull_up = %u)\n",
        (unsigned long long)now_us(), pin, pin_to_text(pin), pull_up);
    log_write(text, n);
}

void debug_log_gpio_in_reset(struct gpio_in g, int8_t pull_up)
{
    char text[96];
    int state = g.line->state;
    int num = g.line->num;
    int pm = g.line->pinMode;
    int pa = g.line->pinAction;
    int dt = g.line->deviceType;

    int n = snprintf(text, sizeof(text),
        "%012lluus gpio_in_reset(pin = %u(%s), pull_up = %u)\n",
        (unsigned long long)now_us(), num, pin_to_text(num), pull_up);
    log_write(text, n);
}

void debug_log_gpio_in_read(struct gpio_in g)
{
    char text[96];
    int state = g.line->state;
    int num = g.line->num;
    int pm = g.line->pinMode;
    int pa = g.line->pinAction;
    int dt = g.line->deviceType;

    int n = snprintf(text, sizeof(text),
        "%012lluus gpio_in_read(pin = %u(%s)) = %u)\n",
        (unsigned long long)now_us(), num, pin_to_text(num), state);
    log_write(text, n);
}

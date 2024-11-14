#include "esp_common.h"
#include "gpio.h"
#include "freertos/task.h"
#include "math.h"
#include "init.h"

struct BlinkParams
{
    uint32 channel_id;
    uint32 start;
    uint32 end;
    uint32 delay_time;
    uint32 duration;
};

int long_duration = 2000;
int short_duration = 500;

struct BlinkParams params = {
    .start = 0,
    .channel_id = 0,
    .end = 1023,
    .delay_time = 50,
    .duration = 500,
};

void delay(int ms)
{
    vTaskDelay(ms / portTICK_RATE_MS);
}

void blink_task(void *pvParameters)
{
    struct BlinkParams *params = (struct BlinkParams *)pvParameters;
    int now = params->start;

    int channel_id;
    int start;
    int end;
    int delay_time;
    int duration;

    while (1)
    {
        channel_id = params->channel_id;
        start = params->start;
        end = params->end;
        delay_time = params->delay_time;
        duration = params->duration;

        int step = floor(2 * (end - start) / (duration / delay_time));

        while (now < end)
        {
            pwm_set_duty(now, channel_id);
            pwm_start();
            delay(delay_time);
            now += step;
        }

        while (now > start)
        {
            pwm_set_duty(now, channel_id);
            pwm_start();
            delay(delay_time);
            now -= step;
        }
    }
}

void button_down(bool is_long, uint32_t gpio_status_last, uint32_t gpio_status)
{
    if (is_long)
    {
        is_long = false;
    }
    else
    {
        if (params.duration == long_duration)
            params.duration = short_duration;
        else
            params.duration = long_duration;
        is_long = true;
    }
    gpio_status_last = gpio_status;
    printf("button down");
}

void button_up()
{
    printf("button up");
}

void button_daemon()
{
    // 0 - pressed, 16 - released

    uint32_t gpio_status_last = gpio_input_get() & inp_bit_1;
    bool is_long = false;

    while (1)
    {
        uint32_t gpio_status = gpio_input_get() & inp_bit_1;

        if (gpio_status_last != 0 && gpio_status == 0)
            button_down(is_long, gpio_status_last, gpio_status);

        else if (gpio_status_last == 0 && gpio_status != 0)
            button_up();

        gpio_status_last = gpio_status;
        delay(100);
    }
}

void user_init(void)
{
    init_uart();
    init_pwm();
    init_inp();

    xTaskCreate(&button_daemon, "button_daemon", 2048, NULL, 1, NULL);
    xTaskCreate(&blink_task, "blink_task_1", 2048, (void *)&params, 1, NULL);

    // while (1)
    // {
    //     delay(100);
    // }
}
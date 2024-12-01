#include "esp_common.h"
#include "gpio.h"
#include "freertos/task.h"
#include "math.h"
#include "init.h"
#include "funcs.h"

struct BlinkParams
{
    uint32 channel_id;
    uint32 start;
    uint32 end;
    uint32 delay_time;
    uint32 duration;
    bool is_pwm;
};

int time_to_long_pressed = 500;
int long_duration = 2000;
int short_duration = 500;

struct BlinkParams params1 = {
    .start = 0,
    .channel_id = 0,
    .end = 1023,
    .delay_time = 200,
    .duration = 1000,
    .is_pwm = false,
};

struct BlinkParams params2 = {
    .start = 0,
    .channel_id = 1,
    .end = 1023,
    .delay_time = 200,
    .duration = 500,
    .is_pwm = false,
};

struct BlinkParams params3 = {
    .start = 0,
    .channel_id = 2,
    .end = 1023,
    .delay_time = 50,
    .duration = 1000,
    .is_pwm = true,
};

struct BlinkParams params4 = {
    .start = 0,
    .channel_id = 3,
    .end = 1023,
    .delay_time = 50,
    .duration = 500,
    .is_pwm = true,
};

void blink_task(void *pvParameters)
{
    struct BlinkParams *params = (struct BlinkParams *)pvParameters;
    int now = params->start;

    int channel_id;
    int start;
    int end;
    int delay_time;
    int duration;

    bool is_pwm = params->is_pwm;

    if (is_pwm)
    {
        while (1)
        {
            channel_id = params->channel_id;
            start = params->start;
            end = params->end;
            delay_time = params->delay_time;
            duration = params->duration;

            int step = floor((end - start) / (duration / delay_time));

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
    else {
        while (1)
        {
            channel_id = params->channel_id;
            start = params->start;
            end = params->end;
            delay_time = params->delay_time;
            duration = params->duration;

            pwm_set_duty(end, channel_id);
            pwm_start();

            delay(delay_time);

            pwm_set_duty(start, channel_id);
            pwm_start();

            delay(duration);
        }
    }
}

void button_up_short(uint32_t gpio_status_last, uint32_t gpio_status)
{
    if (params1.duration == long_duration)
        params1.duration = short_duration;
    else
        params1.duration = long_duration;

    gpio_status_last = gpio_status;
    printf("button up short\n");
}

void button_up_long(uint32_t gpio_status_last, uint32_t gpio_status)
{
    if (params1.duration == long_duration)
        params1.duration = short_duration;
    else
        params1.duration = long_duration;

    gpio_status_last = gpio_status;
    printf("button up long\n");
}

void button_daemon()
{
    // 0 - pressed, 16 - released

    uint32_t gpio_status_last = gpio_input_get() & inp_bit_1;
    uint32_t time = get_time_ms();

    while (1)
    {
        uint32_t gpio_status = gpio_input_get() & inp_bit_1;

        if (gpio_status_last != 0 && gpio_status == 0)
        {
            time = get_time_ms();
        }

        else if (gpio_status_last == 0 && gpio_status != 0)
        {
            uint32_t time_diff = get_time_ms() - time;

            printf("time diff: %d\n", time_diff);

            if (time_diff > time_to_long_pressed)
                button_up_long(gpio_status_last, gpio_status);
            else
                button_up_short(gpio_status_last, gpio_status);
        }

        gpio_status_last = gpio_status;
        delay(50);
    }
}

void user_init(void)
{
    init_uart();
    init_pwm();
    init_inp();

    xTaskCreate(&button_daemon, "button_daemon", 2048, NULL, 1, NULL);
    xTaskCreate(&blink_task, "blink_task_1", 2048, (void *)&params1, 2, NULL);
    xTaskCreate(&blink_task, "blink_task_2", 2048, (void *)&params2, 2, NULL);
    xTaskCreate(&blink_task, "blink_task_3", 2048, (void *)&params3, 2, NULL);
    xTaskCreate(&blink_task, "blink_task_4", 2048, (void *)&params4, 2, NULL);

    // while (1)
    // {
    //     pwm_set_duty(0, 0);
    //     pwm_start();

    //     delay(500);

    //     pwm_set_duty(1023, 0);
    //     pwm_start();

    //     delay(500);
    // }
}
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
int MODE = 0;

xTaskHandle blink_wave_handle;
xTaskHandle blink_1;
xTaskHandle blink_2;
xTaskHandle blink_3;
xTaskHandle blink_4;

struct BlinkParams params1 = {
    .start = 0,
    .channel_id = 0,
    .end = 300,
    .delay_time = 100,
    .duration = 1000,
    .is_pwm = false,
};

struct BlinkParams params2 = {
    .start = 0,
    .channel_id = 1,
    .end = 300,
    .delay_time = 100,
    .duration = 500,
    .is_pwm = false,
};

struct BlinkParams params3 = {
    .start = 0,
    .channel_id = 2,
    .end = 300,
    .delay_time = 50,
    .duration = 1000,
    .is_pwm = true,
};

struct BlinkParams params4 = {
    .start = 0,
    .channel_id = 3,
    .end = 300,
    .delay_time = 50,
    .duration = 500,
    .is_pwm = true,
};

struct BlinkParams params_wave = {
    .start = 0,
    .channel_id = -1,
    .end = 300,
    .delay_time = 50,
    .duration = 1000,
    .is_pwm = true,
};

void blink_pwm(void *pvParameters)
{
    struct BlinkParams *params = (struct BlinkParams *)pvParameters;
    int now = params->start;

    int channel_id = params->channel_id;
    int start = params->start;
    int end = params->end;
    int delay_time = params->delay_time;
    int duration = params->duration;

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

void blink_wave()
{
    int channel_id = 0;

    while (1)
    {
        params_wave.channel_id = channel_id;
        blink_pwm(&params_wave);
        channel_id = (channel_id + 1) % 4;
    }
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

    bool is_pwm = params->is_pwm;

    if (is_pwm)
    {
        while (1)
            blink_pwm(pvParameters);
    }
    else
    {
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

void button_up_short()
{
    if (MODE == 0)
    {
        params1.channel_id = (params1.channel_id + 1) % 4;
        params2.channel_id = (params2.channel_id + 1) % 4;
        params3.channel_id = (params3.channel_id + 1) % 4;
        params4.channel_id = (params4.channel_id + 1) % 4;
    }
    else
    {
        params_wave.duration = (params_wave.duration + 100) % 2000; // should be: 1, 0.5, 0.3, 1, ...
    }
    printf("button up short\n");
}

void button_up_long()
{
    MODE = (MODE + 1) % 2;
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
                button_up_long();
            else
                button_up_short();
        }

        gpio_status_last = gpio_status;
        delay(50);
    }
}

void blink_daemon()
{
    int MODE_PAST = MODE;

    xTaskCreate(&blink_wave, "blink_wave", 2048, (void *)&params_wave, 7, &blink_wave_handle);
    vTaskSuspend(blink_wave_handle);

    xTaskCreate(&blink_task, "blink_task_1", 2048, (void *)&params1, 3, &blink_1);
    xTaskCreate(&blink_task, "blink_task_2", 2048, (void *)&params2, 4, &blink_2);
    xTaskCreate(&blink_task, "blink_task_3", 2048, (void *)&params3, 5, &blink_3);
    xTaskCreate(&blink_task, "blink_task_4", 2048, (void *)&params4, 6, &blink_4);

    while (1)
    {
        printf("MODE: %d\n", MODE);

        if (MODE_PAST == MODE)
        {
            delay(50);
            continue;
        }

        if (MODE == 0)
        {
            vTaskSuspend(blink_wave_handle);

            params1.channel_id = 0;
            params2.channel_id = 1;
            params3.channel_id = 2;
            params4.channel_id = 3;

            vTaskResume(blink_1);
            vTaskResume(blink_2);
            vTaskResume(blink_3);
            vTaskResume(blink_4);
        }
        else
        {
            vTaskSuspend(blink_1);
            vTaskSuspend(blink_2);
            vTaskSuspend(blink_3);
            vTaskSuspend(blink_4);

            vTaskResume(blink_wave_handle);
        }

        MODE_PAST = MODE;
    }
}

void user_init(void)
{
    init_uart();
    init_pwm();
    init_inp();

    xTaskCreate(&button_daemon, "button_daemon", 2048, NULL, 1, NULL);
    // xTaskCreate(&blink_daemon, "blink_daemon", 2048, NULL, 2, NULL);

    int channel_id = 0;

    while (1)
    {
        params_wave.channel_id = channel_id;
        blink_pwm(&params_wave);
        channel_id = (channel_id + 1) % 4;
    }

    // xTaskHandle xHandle;

    // xTaskCreate(&blink_task, "blink_task_1", 2048, (void *)&params1, 2, NULL);

    // delay(5000);

    // vTaskDelete(xHandle);

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
// #define configTOTAL_HEAP_SIZE       16384
// #define configSUPPORT_STATIC_ALLOCATION 1
// #define configSUPPORT_DYNAMIC_ALLOCATION 0

#include "esp_common.h"
#include "gpio.h"
#include "freertos/task.h"
#include "math.h"

#include "common.h"
#include "init.h"
#include "conf.h"
#include "blink_funcs.h"

int MODE = 0;

void button_up_short()
{
    if (is_blinking)
    {
        params1.channel_id = (params1.channel_id + 1) % 3;
        params2.channel_id = (params2.channel_id + 1) % 3;
        params3.channel_id = (params3.channel_id + 1) % 3;
    }
    else if (is_waving)
    {
        if (params_wave.duration == 1000)
            params_wave.duration = 500;
        else if (params_wave.duration == 500)
            params_wave.duration = 300;
        else if (params_wave.duration == 300)
            params_wave.duration = 100;
        else if (params_wave.duration == 100)
            params_wave.duration = 1000;
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

            if (time_diff > 2500)
            {
                if (is_blinking)
                    stop_blink_tasks();
                else if (is_waving)
                    stop_blink_wave_task();

                set_all((int[]){0, 1, 2}, 3, 0);
            }
            else if (time_diff > time_to_long_pressed)
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

    create_blink_tasks();
    create_blink_wave_task();

    while (1)
    {
        if (MODE_PAST == MODE)
        {
            delay(50);
            continue;
        }

        MODE_PAST = MODE;
        printf("MODE: %d\n", MODE);

        if (is_blinking)
        {
            stop_blink_tasks();
            set_all((int[]){0, 1, 2}, 3, 0);
            resume_blink_wave_task();

            is_blinking = false;
            is_waving = true;
        }
        else if (is_waving)
        {
            stop_blink_wave_task();
            set_all((int[]){0, 1, 2}, 3, 0);
            resume_blink_tasks();

            is_blinking = true;
            is_waving = false;
        }
        else
        {
            resume_blink_tasks();
            is_blinking = true;
        }
    }
}

void user_init(void)
{
    init_uart();
    init_pwm();
    init_inp();

    xTaskCreate(&button_daemon, "button_daemon", 2048, NULL, 1, NULL);
    xTaskCreate(&blink_daemon, "blink_daemon", 2048, NULL, 2, NULL);

    // int channel_id = 0;

    // while (1)
    // {
    //     params_wave.channel_id = channel_id;
    //     blink_pwm(&params_wave);
    //     channel_id = (channel_id + 1) % 4;
    // }
}
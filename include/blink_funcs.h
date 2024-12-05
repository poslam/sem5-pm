bool is_blinking = false;
bool is_waving = false;

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

    pwm_set_duty(end, channel_id);
    pwm_start();
    delay(delay_time);

    while (now > start)
    {
        pwm_set_duty(now, channel_id);
        pwm_start();
        delay(delay_time);
        now -= step;
    }

    pwm_set_duty(start, channel_id);
    pwm_start();
    delay(delay_time);
}

void blink_wave()
{
    int channel_id = 0;

    while (1)
    {
        params_wave.channel_id = channel_id;
        blink_pwm(&params_wave);
        channel_id = (channel_id + 1) % channels_size;
    }
}

void blink_task(void *pvParameters)
{
    struct BlinkParams *params = (struct BlinkParams *)pvParameters;
    
}

void stop_blink_tasks()
{
    vTaskSuspend(blink_handle);
    is_blinking = false;
}

void create_blink_tasks()
{
    xTaskCreate(&blink_task, "blink_task", 128, (void *)&params_list, 3, &blink_handle);
    stop_blink_tasks();
    set_all(channels, channels_size, 0);
}

void resume_blink_tasks()
{
    vTaskResume(blink_handle);
    is_blinking = true;
}

void stop_blink_wave_task()
{
    vTaskSuspend(blink_wave_handle);
    is_waving = false;
}

void create_blink_wave_task()
{
    xTaskCreate(&blink_wave, "blink_wave", 128, NULL, 3, &blink_wave_handle);
    stop_blink_wave_task();
    set_all(channels, channels_size, 0);
}

void resume_blink_wave_task()
{
    vTaskResume(blink_wave_handle);
    is_waving = true;
}
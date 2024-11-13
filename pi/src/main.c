#include "esp_common.h"
#include "gpio.h"
#include "freertos/task.h"
#include "math.h"

#define UART_BAUD_RATE 9600

void init_uart()
{
    UART_SetBaudrate(0, UART_BAUD_RATE);
}

void init_pwm()
{
    uint32 io_info[][3] = {
        {PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO5, 5},
    };

    u32 dutys[] = {0};
    pwm_init(1000, dutys, 1, io_info);
}

uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;
    switch (size_map)
    {
    case FLASH_SIZE_4M_MAP_256_256:
        rf_cal_sec = 128 - 5;
        break;

    case FLASH_SIZE_8M_MAP_512_512:
        rf_cal_sec = 256 - 5;
        break;

    case FLASH_SIZE_16M_MAP_512_512:
    case FLASH_SIZE_16M_MAP_1024_1024:
        rf_cal_sec = 512 - 5;
        break;

    case FLASH_SIZE_32M_MAP_512_512:
    case FLASH_SIZE_32M_MAP_1024_1024:
        rf_cal_sec = 1024 - 5;
        break;

    default:
        rf_cal_sec = 0;
        break;
    }

    return rf_cal_sec;
}

void delay(int ms)
{
    vTaskDelay(ms / portTICK_RATE_MS);
}

struct BlinkParams
{
    uint32 start;
    uint32 channel_id;
    uint32 end;
    uint32 delay_time;
    uint32 duration;
};

void blink_task(void *pvParameters)
{
    struct BlinkParams params = *(struct BlinkParams *)pvParameters;
    ;

    int step = floor(
        (params.end - params.start) /
        (params.duration / params.delay_time));
    int now = params.start;

    while (1)
    {
        while (now < params.end)
        {
            pwm_set_duty(now, params.channel_id);
            pwm_start();
            delay(params.delay_time);
            now += step;

            printf("+ Duty: %d\n", now);
        }

        while (now > params.start)
        {
            pwm_set_duty(now, params.channel_id);
            pwm_start();
            delay(params.delay_time);
            now -= step;

            printf("- Duty: %d\n", now);
        }
    }
}

struct BlinkParams params = {
    .start = 50,
    .channel_id = 0,
    .end = 500,
    .delay_time = 50,
    .duration = 500,
};

void user_init(void)
{
    init_uart();
    init_pwm();

    // gpio_enable(GPIO_Pin_5);

    GPIO_AS_INPUT(GPIO_Pin_5);

    // xTaskCreate(&blink_task, "blink_task_1", 2048, (void *)&params, 1, NULL);
    
    while (1)
    {
        // printf("asd\n");
        delay(100);
    }
}
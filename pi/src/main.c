#include "esp_common.h"
#include "gpio.h"
#include "freertos/task.h"

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

#include <math.h>

void user_init(void)
{
    init_uart();
    init_pwm();

    GPIO_AS_OUTPUT(GPIO_Pin_5);

    int start = 10;
    int end = 1023;
    int steps = 100;

    int step = floor((end - start) / steps);

    while (1) {

        for (int i = 0; i < steps; i++) {
            pwm_set_duty(0, 0);
            delay(10);
        }
    }
}
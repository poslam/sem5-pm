#include "esp_common.h"

uint32_t get_time_ms(void) {
    return system_get_time() / 1000;  // Convert microseconds to milliseconds
}

void delay(int ms)
{
    vTaskDelay(ms / portTICK_RATE_MS);
}
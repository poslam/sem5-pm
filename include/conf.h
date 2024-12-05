struct BlinkParams
{
    uint32 channel_id;
    uint32 start;
    uint32 now;
    uint32 end;
    uint32 cycle_time;
    uint32 delay_time;
    uint32 duration;
    bool is_pwm;
};

const int time_to_long_pressed = 500;

int channels[4] = {0, 1, 2, 3};
int channels_size = sizeof(channels) / sizeof(channels[0]);

int delay_time = 50;

xTaskHandle blink_wave_handle;
xTaskHandle blink_handle;

// detailed params

struct BlinkParams params1 = {
    .start = 0,
    .end = 1000,
    .now = 0,
    .cycle_time = 0,
    .channel_id = 0,
    .delay_time = 100,
    .duration = 1000,
    .is_pwm = false,
};

struct BlinkParams params2 = {
    .start = 0,
    .now = 0,
    .cycle_time = 0,
    .channel_id = 1,
    .end = 1000,
    .delay_time = 50,
    .duration = 500,
    .is_pwm = true,
};

struct BlinkParams params3 = {
    .start = 0,
    .now = 0,
    .cycle_time = 0,
    .channel_id = 2,
    .end = 1000,
    .delay_time = 50,
    .duration = 1000,
    .is_pwm = true,
};

struct BlinkParams params4 = {
    .start = 0,
    .now = 0,
    .cycle_time = 0,
    .channel_id = 3,
    .end = 1000,
    .delay_time = 100,
    .duration = 500,
    .is_pwm = false,
};

// all

struct BlinkParams params_wave = {
    .start = 0,
    .channel_id = -1,
    .end = 300,
    .delay_time = 50,
    .duration = 1000,
    .is_pwm = true,
};

struct BlinkParams* params_list[4] = {
    &params1,
    &params2,
    &params3,
    &params4,
};

int params_list_size = sizeof(params_list) / sizeof(params_list[0]);
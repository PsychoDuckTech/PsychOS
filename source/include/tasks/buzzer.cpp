#include "buzzer.h"
#include "driver/ledc.h"

#define BUZZER_PIN 8
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE // Changed to LOW_SPEED_MODE
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY 5000

QueueHandle_t buzzerQueue = NULL;

const Melody predefinedMelodies[] = {
    {{{1000, 200}, {1500, 200}}, 2}, // ID 1: Connect sound
    {{{1500, 200}, {1000, 200}}, 2}, // ID 2: Disconnect sound
};

void setupBuzzerPWM()
{
    ledc_timer_config_t timer_conf;
    timer_conf.speed_mode = LEDC_MODE;
    timer_conf.duty_resolution = LEDC_DUTY_RES;
    timer_conf.timer_num = LEDC_TIMER;
    timer_conf.freq_hz = LEDC_FREQUENCY;
    timer_conf.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf;
    channel_conf.gpio_num = BUZZER_PIN;
    channel_conf.speed_mode = LEDC_MODE;
    channel_conf.channel = LEDC_CHANNEL;
    channel_conf.timer_sel = LEDC_TIMER;
    channel_conf.duty = 0;
    channel_conf.hpoint = 0;
    ledc_channel_config(&channel_conf);
}

void playTone(uint16_t frequency, uint16_t duration)
{
    if (frequency == 0)
    {
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        return;
    }

    ledc_set_freq(LEDC_MODE, LEDC_TIMER, frequency);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, (1 << (LEDC_DUTY_RES - 1)));
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
    vTaskDelay(duration / portTICK_PERIOD_MS);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void buzzerTask(void *parameters)
{
    setupBuzzerPWM();
    BuzzerCommand cmd;

    for (;;)
    {
        if (xQueueReceive(buzzerQueue, &cmd, portMAX_DELAY))
        {
            switch (cmd.type)
            {
            case BUZZER_SINGLE_TONE:
                playTone(cmd.data.tone.frequency, cmd.data.tone.duration);
                break;
            case BUZZER_PREDEFINED_SOUND:
                if (cmd.data.predefined_id >= 0 && cmd.data.predefined_id < sizeof(predefinedMelodies) / sizeof(Melody))
                {
                    const Melody *melody = &predefinedMelodies[cmd.data.predefined_id];
                    for (int i = 0; i < melody->length; i++)
                    {
                        playTone(melody->notes[i].frequency, melody->notes[i].duration);
                        vTaskDelay(10 / portTICK_PERIOD_MS);
                    }
                }
                break;
            case BUZZER_MELODY:
                for (int i = 0; i < cmd.data.melody.length; i++)
                {
                    playTone(cmd.data.melody.notes[i].frequency, cmd.data.melody.notes[i].duration);
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                }
                break;
            }
        }
    }
}

void startBuzzerTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    buzzerQueue = xQueueCreate(10, sizeof(BuzzerCommand));
    xTaskCreatePinnedToCore(
        buzzerTask,
        "Buzzer Task",
        stackDepth,
        NULL,
        priority,
        NULL,
        core);
}

void buzzerPlayTone(uint16_t frequency, uint16_t duration)
{
    BuzzerCommand cmd;
    cmd.type = BUZZER_SINGLE_TONE;
    cmd.data.tone.frequency = frequency;
    cmd.data.tone.duration = duration;
    xQueueSend(buzzerQueue, &cmd, portMAX_DELAY);
}

void buzzerPlayPredefined(int id)
{
    BuzzerCommand cmd;
    cmd.type = BUZZER_PREDEFINED_SOUND;
    cmd.data.predefined_id = id;
    xQueueSend(buzzerQueue, &cmd, portMAX_DELAY);
}

void buzzerPlayMelody(Tone *notes, uint8_t length)
{
    BuzzerCommand cmd;
    cmd.type = BUZZER_MELODY;
    cmd.data.melody.length = length;
    memcpy(cmd.data.melody.notes, notes, length * sizeof(Tone));
    xQueueSend(buzzerQueue, &cmd, portMAX_DELAY);
}
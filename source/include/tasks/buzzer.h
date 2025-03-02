#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#define MAX_MELODY_NOTES 10

typedef enum
{
    BUZZER_SINGLE_TONE,
    BUZZER_PREDEFINED_SOUND,
    BUZZER_MELODY
} BuzzerCommandType;

typedef struct
{
    uint16_t frequency;
    uint16_t duration;
} Tone;

typedef struct
{
    Tone notes[MAX_MELODY_NOTES];
    uint8_t length;
} Melody;

typedef struct
{
    BuzzerCommandType type;
    union
    {
        Tone tone;
        int predefined_id;
        Melody melody;
    } data;
} BuzzerCommand;

extern QueueHandle_t buzzerQueue;

enum PredefinedSounds
{
    SOUND_CONNECT = 0,
    SOUND_DISCONNECT = 1,
};

void buzzerTask(void *parameters);
void startBuzzerTask(UBaseType_t core = 1, uint32_t stackDepth = 2048, UBaseType_t priority = 1);
void buzzerPlayTone(uint16_t frequency, uint16_t duration);
void buzzerPlayPredefined(int id);
void buzzerPlayMelody(Tone *notes, uint8_t length);
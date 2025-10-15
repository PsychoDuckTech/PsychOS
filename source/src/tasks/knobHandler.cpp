#include <Arduino.h>
#include "tasks/knobHandler.h"
#include "drivers/rotaryEncoder/EncoderHandler.h"
#include "tasks/hostCommunicationBridge.h"
#include "tasks/displayHandler.h"
#include "main.h"
#include "display/screens.h"
#include "display/pixelFlushScreen.h"
#include "display/components/uiComponents.h"  // For menu system
#include "freertos/queue.h"
#include "freertos/timers.h"

extern void displayPixelFlushScreen();
extern bool needsFullRedraw;

constexpr unsigned long POLLING_RATE_MS = 20;
// CORREÇÃO: Reduzido de 50ms para 5ms. 
// 50ms é demasiado lento e descarta o primeiro passo na inversão de direção.
constexpr unsigned long ROTATION_DEBOUNCE_MS = 5; 
constexpr unsigned long BUTTON_DEBOUNCE_MS = 10; // Debounce for button press/release 
constexpr int NUM_RGB_EFFECTS = 5;
constexpr int NUM_RGB_SELECTIONS = 3;
constexpr int NUM_CLOCK_SELECTIONS = 3;
constexpr int NUM_SETTINGS_OPTIONS = 4;

// Definição da fila de botões (deve ser externa em knobHandler.h)
QueueHandle_t knobButtonQueue = NULL; 

EncoderHandler knob(KNOB_DT_PIN, KNOB_CLK_PIN, KNOB_SW_PIN);

// Variáveis estáticas e semáforo para o Long Press
static volatile unsigned long pressStartTime = 0;
static volatile bool buttonIsPressed = false;
static volatile unsigned long lastButtonTime = 0;
static TimerHandle_t longPressTimer = NULL;

// Enum para tipos de eventos de botão
enum ButtonEventType {
    SHORT_PRESS,
    LONG_PRESS_DETECTED,
    // DOUBLE_PRESS pode ser calculado no task handler
};

// Callback do temporizador para Long Press
void longPressTimerCallback(TimerHandle_t xTimer) {
    if (buttonIsPressed) {
        ButtonEventType event = LONG_PRESS_DETECTED;
        // Enviar o evento do timer para a fila
        xQueueSendFromISR(knobButtonQueue, &event, NULL);
    }
}

// Funções de ISR para detetar pressionamentos
void IRAM_ATTR knobButtonISR() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    unsigned long currentTime = millis();
    
    // Debounce check
    if (currentTime - lastButtonTime < BUTTON_DEBOUNCE_MS) {
        return;
    }
    lastButtonTime = currentTime;
    
    ButtonEventType event;
    
    // Ler o estado atual (LOW = Pressionado, HIGH = Solto)
    if (digitalRead(KNOB_SW_PIN) == LOW) {
        // Pressionamento detectado (Início)
        pressStartTime = currentTime;
        buttonIsPressed = true;
        // Iniciar o temporizador de Long Press (225ms definido no EncoderHandler.h)
        xTimerStartFromISR(longPressTimer, &xHigherPriorityTaskWoken);
    } else {
        // Soltura detectada (Fim)
        if (buttonIsPressed) {
            unsigned long duration = currentTime - pressStartTime;
            xTimerStopFromISR(longPressTimer, &xHigherPriorityTaskWoken);
            
            // Se o temporizador não detetou um Long Press (i.e., duração < 225ms)
            if (duration < 350) {
                event = SHORT_PRESS;
                xQueueSendFromISR(knobButtonQueue, &event, &xHigherPriorityTaskWoken);
            }
            buttonIsPressed = false;
        }
    }
    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

// Inicialização do Timer e da ISR (para ser chamada em startKnobHandlerTask)
static void setupKnobISR() {
    knobButtonQueue = xQueueCreate(10, sizeof(ButtonEventType));
    
    // Configurar o Timer para o Long Press
    longPressTimer = xTimerCreate(
        "LongPressTimer",
        pdMS_TO_TICKS(350), // LONG_PRESS_TIME do EncoderHandler.h
        pdFALSE,            // Não repetir
        (void *)0,
        longPressTimerCallback
    );

    // Anexar a ISR ao pino do botão
    pinMode(KNOB_SW_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(KNOB_SW_PIN), knobButtonISR, CHANGE);
}

void knobHandler(void *parameters)
{
    knob.begin();
    setupKnobISR(); // Configurar ISR
    Serial.println("Knob Handler started with ISR");

    unsigned long lastRotationTime = 0;
    ButtonEventType buttonEvent;
    
    for (;;)
    {
        knob.update();

        // 1. Processar Rotação (Polling)
        int rotation = knob.getEncoderDelta();
        unsigned long currentTime = millis();
        
        // A otimização está aqui: ROTATION_DEBOUNCE_MS agora é 5ms.
        if (rotation != 0 && (currentTime - lastRotationTime) > ROTATION_DEBOUNCE_MS)
        {
            lastRotationTime = currentTime;

            // --- Lógica de rotação completa (Volume, Menu, RGB) ---
            switch (currentScreen)
            {
            case MainScreen:
                if (rotation != 0)
                {
                    HostMessage msg;
                    msg.type = VOLUME_CHANGE;
                    msg.data = rotation; // Natural direction for volume
                    xQueueSend(hostMessageQueue, &msg, 0); 
                }
                break;

            case SettingsScreen:
            {
                SettingsRotationEvent event;
                event.totalSteps = rotation; // Natural direction
                xQueueSend(settingsRotationQueue, &event, 0);
            }
            break;

            case PixelFlushScreen:
                handlePixelFlushKnobRotation(rotation);
                break;

            case ClockSubmenu:
            {
                SettingsRotationEvent event;
                event.totalSteps = rotation;
                xQueueSend(settingsRotationQueue, &event, 0);
            }
            break;

            case RGBLightingSubmenu:
            {
                // Tratamento detalhado para navegação e ajuste de parâmetros RGB
                if (rgbState.currentSelection == 0)
                {
                    // Seleção de Efeito (cicla através dos tipos de efeito)
                    int oldEffect = rgbState.effect;
                    int newEffect = rgbState.effect + rotation;
                    
                    if (newEffect < 0) {
                        newEffect = NUM_RGB_EFFECTS - 1; // Volta para o último
                    } else if (newEffect >= NUM_RGB_EFFECTS) {
                        newEffect = 0; // Volta para o primeiro
                    }
                    rgbState.effect = newEffect;

                    if (oldEffect != rgbState.effect)
                    {
                        // Envia o comando de mudança de efeito
                        RGBCommand cmd;
                        cmd.type = RGB_CMD_SET_EFFECT;

                        // Configurações baseadas no efeito selecionado
                        switch (rgbState.effect)
                        {
                        case RGB_EFFECT_STATIC:
                            cmd.data.effect.config = {RGB_EFFECT_STATIC,
                                                      static_cast<uint8_t>(map(rgbState.speed, 1, 20, 1, 255)), 255};
                            strncpy(cmd.data.effect.colors[0], "#FFA500", HEX_COLOR_LENGTH); // Orange
                            cmd.data.effect.num_colors = 1;
                            break;

                        case RGB_EFFECT_RUNNER:
                            cmd.data.effect.config = {RGB_EFFECT_RUNNER,
                                                      static_cast<uint8_t>(map(rgbState.speed, 1, 20, 1, 255)), 255};
                            strncpy(cmd.data.effect.colors[0], "#FFA500", HEX_COLOR_LENGTH); // Orange
                            strncpy(cmd.data.effect.colors[1], "#FF69B4", HEX_COLOR_LENGTH); // Hot Pink
                            strncpy(cmd.data.effect.colors[2], "#800080", HEX_COLOR_LENGTH); // Purple
                            cmd.data.effect.num_colors = 3;
                            break;

                        case RGB_EFFECT_SCROLL:
                            cmd.data.effect.config = {RGB_EFFECT_SCROLL,
                                                      static_cast<uint8_t>(map(rgbState.speed, 1, 20, 1, 255)), 255};
                            strncpy(cmd.data.effect.colors[0], "#1d1d1d", HEX_COLOR_LENGTH); // Dark Brown
                            strncpy(cmd.data.effect.colors[1], "#674de0", HEX_COLOR_LENGTH); // Light Brown
                            cmd.data.effect.num_colors = 2;
                            break;

                        case RGB_EFFECT_BREATHE:
                            cmd.data.effect.config = {RGB_EFFECT_BREATHE,
                                                      static_cast<uint8_t>(map(rgbState.speed, 1, 20, 1, 255)), 255};
                            strncpy(cmd.data.effect.colors[0], "#51ff00ff", HEX_COLOR_LENGTH); // Deep Sky Blue
                            strncpy(cmd.data.effect.colors[1], "#590082ff", HEX_COLOR_LENGTH); // Indigo
                            cmd.data.effect.num_colors = 2;
                            break;

                        default: // RGB_EFFECT_WAVE (custom effect for demo)
                            cmd.data.effect.config = {RGB_EFFECT_BREATHE, // Usando breathe como base
                                                      static_cast<uint8_t>(map(rgbState.speed, 1, 20, 1, 255)), 255};
                            strncpy(cmd.data.effect.colors[0], "#fffb00ff", HEX_COLOR_LENGTH); // Green
                            strncpy(cmd.data.effect.colors[1], "#ff00ddff", HEX_COLOR_LENGTH); // Blue
                            strncpy(cmd.data.effect.colors[2], "#00f7ffff", HEX_COLOR_LENGTH); // Red
                            cmd.data.effect.num_colors = 3;
                            break;
                        }

                        cmd.data.effect.set_colors = true;
                        cmd.data.effect.temporary = false;
                        cmd.data.effect.duration_ms = 0;

                        // Envia o comando RGB
                        if (xQueueSend(rgbCommandQueue, &cmd, pdMS_TO_TICKS(50)) != pdPASS)
                        {
                            Serial.println("RGB queue full (effect)");
                        }
                        rgbState.needsRefresh = true;
                    }
                }
                else if (rgbState.currentSelection == 1)
                {
                    // Ajuste de Brilho (escala 0-100)
                    int oldBrightness = rgbState.brightness;
                    rgbState.brightness = constrain(rgbState.brightness + rotation, 0, 100);

                    if (oldBrightness != rgbState.brightness)
                    {
                        // Envia apenas o comando de brilho
                        RGBCommand cmd;
                        cmd.type = RGB_CMD_SET_BRIGHTNESS;
                        cmd.data.brightness = rgbState.brightness;

                        if (xQueueSend(rgbCommandQueue, &cmd, pdMS_TO_TICKS(50)) != pdPASS)
                        {
                            Serial.println("RGB queue full (brightness)");
                        }
                        rgbState.needsRefresh = true;
                    }
                }
                else
                {
                    // Ajuste de Velocidade (escala 1-20)
                    int oldSpeed = rgbState.speed;
                    rgbState.speed = constrain(rgbState.speed + rotation, 1, 20);

                    if (oldSpeed != rgbState.speed)
                    {
                        // Envia apenas o comando de velocidade
                        RGBCommand cmd;
                        cmd.type = RGB_CMD_SET_SPEED;
                        cmd.data.speed = rgbState.speed;

                        if (xQueueSend(rgbCommandQueue, &cmd, pdMS_TO_TICKS(50)) != pdPASS)
                        {
                            Serial.println("RGB queue full (speed)");
                        }
                        rgbState.needsRefresh = true;
                    }
                }
            }
            break;
            } // Fim do switch (currentScreen)
            // --- Fim da Lógica de rotação completa ---
        }

        // 2. Processar Eventos de Botão (Queue)
        if (xQueueReceive(knobButtonQueue, &buttonEvent, 0) == pdTRUE)
        {
            if (buttonEvent == SHORT_PRESS)
            {
                // Lógica de Short Press
                switch (currentScreen)
                {
                case MainScreen:
                {
                    HostMessage msg;
                    msg.type = VOLUME_MUTE;
                    msg.data = 0;
                    xQueueSend(hostMessageQueue, &msg, 0);
                    break;
                }
                case SettingsScreen:
                    // Use centralized menu actions
                    extern const ScrollableMenuItem settingsMenuItems[];
                    if (settingsSelectedOption >= 0 && settingsSelectedOption < 6) {
                        settingsMenuItems[settingsSelectedOption].action();
                    }
                    break;
                case RGBLightingSubmenu:
                    rgbState.currentSelection = (rgbState.currentSelection + 1) % NUM_RGB_SELECTIONS;
                    rgbState.needsRefresh = true;
                    break;
                case ClockSubmenu:
                    settingsSelectedOption = (settingsSelectedOption + 1) % NUM_CLOCK_SELECTIONS;
                    displayClockSubmenu(nullptr);
                    break;
                case PixelFlushScreen:
                    handlePixelFlushKnobPress();
                    break;
                }
            } 
            else if (buttonEvent == LONG_PRESS_DETECTED)
            {
                // Lógica de Long Press
                if (currentScreen == MainScreen)
                {
                    switchScreen(SettingsScreen);
                }
                else if (currentScreen == PixelFlushScreen)
                {
                    handlePixelFlushLongPress();
                }
                else if (currentScreen == ModulesSubmenu ||
                         currentScreen == RGBLightingSubmenu ||
                         currentScreen == ClockSubmenu)
                {
                    switchScreen(MainScreen);
                }
                else
                {
                    switchScreen(MainScreen);
                }
            }
        }
        
        // A lógica doublePress (capsLockStatus) requer uma lógica de queue/timer mais avançada.
        // O código original de doublePress/longPress/shortPress (knob.check...) foi removido para esta otimização baseada em ISR.

        vTaskDelay(pdMS_TO_TICKS(POLLING_RATE_MS));
    }
}

void startKnobHandlerTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    TaskHandle_t knobTaskHandle;
    xTaskCreatePinnedToCore(
        knobHandler,
        "Knob Handler",
        stackDepth,
        NULL,
        priority,
        &knobTaskHandle,
        core);
}

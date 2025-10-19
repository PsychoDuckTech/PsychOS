#pragma once

#include "arduino_compat.h"
#include "driver/pulse_cnt.h"

// Encoder pull resistor configuration
typedef enum {
    NONE = 0,
    UP = 1,
    DOWN = 2
} EncoderPullMode;

class ESP32Encoder {
private:
    pcnt_unit_handle_t _pcnt_unit;
    int _clk_pin;
    int _dt_pin;
    bool _initialized;
    int32_t _count;
    
public:
    static EncoderPullMode useInternalWeakPullResistors;
    
    ESP32Encoder() : _pcnt_unit(nullptr), _initialized(false), _count(0) {}
    
    ~ESP32Encoder() {
        if (_pcnt_unit) {
            pcnt_del_unit(_pcnt_unit);
        }
    }
    
    void attachFullQuad(int clk_pin, int dt_pin) {
        _clk_pin = clk_pin;
        _dt_pin = dt_pin;
        
        // Configure PCNT unit
        pcnt_unit_config_t unit_config = {};
        unit_config.high_limit = 32767;
        unit_config.low_limit = -32768;
        
        ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &_pcnt_unit));
        
        // Configure channel 0 (CLK)
        pcnt_chan_config_t chan_a_config = {};
        chan_a_config.edge_gpio_num = clk_pin;
        chan_a_config.level_gpio_num = dt_pin;
        
        pcnt_channel_handle_t pcnt_chan_a = nullptr;
        ESP_ERROR_CHECK(pcnt_new_channel(_pcnt_unit, &chan_a_config, &pcnt_chan_a));
        ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a,
                                                      PCNT_CHANNEL_EDGE_ACTION_INCREASE,
                                                      PCNT_CHANNEL_EDGE_ACTION_DECREASE));
        ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a,
                                                       PCNT_CHANNEL_LEVEL_ACTION_KEEP,
                                                       PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
        
        // Configure channel 1 (DT)
        pcnt_chan_config_t chan_b_config = {};
        chan_b_config.edge_gpio_num = dt_pin;
        chan_b_config.level_gpio_num = clk_pin;
        
        pcnt_channel_handle_t pcnt_chan_b = nullptr;
        ESP_ERROR_CHECK(pcnt_new_channel(_pcnt_unit, &chan_b_config, &pcnt_chan_b));
        ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b,
                                                      PCNT_CHANNEL_EDGE_ACTION_DECREASE,
                                                      PCNT_CHANNEL_EDGE_ACTION_INCREASE));
        ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b,
                                                       PCNT_CHANNEL_LEVEL_ACTION_KEEP,
                                                       PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
        
        // Configure pull resistors if requested
        if (useInternalWeakPullResistors == UP) {
            gpio_set_pull_mode((gpio_num_t)clk_pin, GPIO_PULLUP_ONLY);
            gpio_set_pull_mode((gpio_num_t)dt_pin, GPIO_PULLUP_ONLY);
        } else if (useInternalWeakPullResistors == DOWN) {
            gpio_set_pull_mode((gpio_num_t)clk_pin, GPIO_PULLDOWN_ONLY);
            gpio_set_pull_mode((gpio_num_t)dt_pin, GPIO_PULLDOWN_ONLY);
        }
        
        ESP_ERROR_CHECK(pcnt_unit_enable(_pcnt_unit));
        ESP_ERROR_CHECK(pcnt_unit_start(_pcnt_unit));
        
        _initialized = true;
    }
    
    int32_t getCount() {
        if (!_initialized) return 0;
        
        int count;
        pcnt_unit_get_count(_pcnt_unit, &count);
        return count;
    }
    
    void setCount(int32_t value) {
        if (!_initialized) return;
        pcnt_unit_clear_count(_pcnt_unit);
        // Note: ESP-IDF PCNT can only be cleared, not set to arbitrary value
        _count = value;
    }
    
    void clearCount() {
        if (!_initialized) return;
        pcnt_unit_clear_count(_pcnt_unit);
        _count = 0;
    }
    
    void setFilter(uint16_t value) {
        if (!_initialized) return;
        // Configure glitch filter (value in APB_CLK cycles)
        pcnt_glitch_filter_config_t filter_config = {};
        filter_config.max_glitch_ns = value * 12.5; // Convert to nanoseconds (80MHz APB clock)
        pcnt_unit_set_glitch_filter(_pcnt_unit, &filter_config);
    }
};

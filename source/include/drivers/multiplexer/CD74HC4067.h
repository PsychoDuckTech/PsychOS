#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H

#include <Arduino.h>

class Multiplexer
{
private: // Private variables (attributes)
    const uint8_t S0_PIN;
    const uint8_t S1_PIN;
    const uint8_t S2_PIN;
    const uint8_t S3_PIN;
    const uint8_t SIG_PIN;

public: // Public methods (functions)
    Multiplexer(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3, uint8_t sig)
        : S0_PIN(s0), S1_PIN(s1), S2_PIN(s2), S3_PIN(s3), SIG_PIN(sig)
    {
        pinMode(S0_PIN, OUTPUT);
        pinMode(S1_PIN, OUTPUT);
        pinMode(S2_PIN, OUTPUT);
        pinMode(S3_PIN, OUTPUT);
        pinMode(SIG_PIN, INPUT_PULLUP);
    }

    // Select a channel (0-15)
    void fastSelect(uint8_t channel)
    {
        GPIO.out_w1tc = ((1ULL << S0_PIN) | (1ULL << S1_PIN) | (1ULL << S2_PIN) | (1ULL << S3_PIN));
        GPIO.out_w1ts = (((channel & 0x01) ? (1ULL << S0_PIN) : 0) |
                         ((channel & 0x02) ? (1ULL << S1_PIN) : 0) |
                         ((channel & 0x04) ? (1ULL << S2_PIN) : 0) |
                         ((channel & 0x08) ? (1ULL << S3_PIN) : 0));
    }

    // Old method for selecting a channel, literally 1000 times slower than fastSelect
    /*void selectChannel(uint8_t channel) {
        digitalWrite(S0_PIN, channel & 0x01);
        digitalWrite(S1_PIN, (channel >> 1) & 0x01);
        digitalWrite(S2_PIN, (channel >> 2) & 0x01);
        digitalWrite(S3_PIN, (channel >> 3) & 0x01);
        delay(15); // Allow the multiplexer to stabilize, this is actually needed
    }*/

    // Read the selected channel
    bool readChannel()
    {
        return digitalRead(SIG_PIN); // Old method, same in speed as the new one
        // return (REG_READ(GPIO_IN_REG) >> SIG_PIN) & 0x1; // just looks cooler :) [CORRECTION: doesn't work with input only pins]
    }
};

#endif

// GabiBrawl != human
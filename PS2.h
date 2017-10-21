#ifndef PS2_h
#define PS2_h

#include "Arduino.h"
#include "PS2Enums.h"

#define REPORT_MODE 0
#define CONFIG_MODE 1
#define DIGITAL_MODE 0
#define ANALOGUE_MODE 1

#define PS2_COMMAND_SIZE 21
#define PS2_DIGITAL_REPORT_SIZE 9
#define PS2_ANALOGUE_REPORT_SIZE 21
#define PS2_CONFIG_SIZE 9

class PS2
{
  public:
    PS2(uint8_t clockPin = 0,
    uint8_t attenPin = 0,
    uint8_t commandPin = 0,
    uint8_t dataPin = 0,
    uint8_t ackPin = 0);

    void handleTick();
    void handleAttention();
    
    void setButton(ButtonEnum button, bool b);
    void setAnalogueHat(AnalogHatEnum hat, uint8_t b);
    void setAnalogueButton(ButtonEnum button, uint8_t b);

    byte getRumbleLeft();
    byte getRumbleRight();
    void toggleContMode();

  private:

    uint8_t clockPin = 0;
    uint8_t attenPin = 0;
    uint8_t commandPin = 0;
    uint8_t dataPin = 0;
    uint8_t ackPin = 0;

    // keep track of which
    // byte and bit we are on
    uint8_t by = 0;
    uint8_t bi = 0;
    uint8_t tick = 0;
    uint8_t maxTicks = PS2_CONFIG_SIZE;
    uint8_t commMode = REPORT_MODE;
    uint8_t contMode = DIGITAL_MODE;

    uint8_t commandBuffer[PS2_COMMAND_SIZE];
    uint8_t reportBuffer[PS2_ANALOGUE_REPORT_SIZE] = { 0x01 };

    uint8_t data41[PS2_CONFIG_SIZE];
    uint8_t data44[PS2_CONFIG_SIZE];
    uint8_t data45[PS2_CONFIG_SIZE];
    uint8_t data4D[PS2_CONFIG_SIZE];
    uint8_t data4F[PS2_CONFIG_SIZE];

    uint8_t rumble[2];

    uint8_t data46, data4C;

    void tickFalling();
    void tickRising();
    void attentionFalling();
    void attentionRising();
    void setCommMode(uint8_t m);
    void setContMode(uint8_t m);
    void setMode();
};

#endif

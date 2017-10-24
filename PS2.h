#ifndef PS2_h
#define PS2_h

#include "Arduino.h"
#include "PS2Enums.h"

#define REPORT_MODE 0
#define CONFIG_MODE 1
#define DIGITAL_MODE 0
#define ANALOG_MODE 1

#define PS2_CONFIG_SIZE 9
#define PS2_COMMAND_SIZE 21
#define PS2_DIGITAL_REPORT_SIZE 5
#define PS2_ANALOG_REPORT_SIZE 21

#define PS2_SMALL_MOTOR 0
#define PS2_LARGE_MOTOR 1

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

    void toggleContMode();
    
    uint8_t getSmallMotor();
    uint8_t getLargeMotor();
    
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
    bool contModeLocked = false;

    uint32_t responseMask = 0;

    // hold the data we are reading from the ps2
    uint8_t commandBuffer[PS2_COMMAND_SIZE];
    // hold the values of all analogue and digital buttons
    // a button press reports as low
    uint8_t reportBuffer[PS2_ANALOG_REPORT_SIZE] = { 0x01 };
    // holt the report config for the controller
    // report config starts out only reporting digial buttons
    uint8_t reportConfig[PS2_CONFIG_SIZE] = {0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x5A};
    
    uint8_t motor0;
    uint8_t motor1;

    uint8_t smallMotor;
    uint8_t largeMotor;

    uint8_t data46, data4C;

    void tickFalling();
    void tickRising();
    void attentionFalling();
    void attentionRising();
    void setCommMode(uint8_t m);
    void setContMode(uint8_t m);
    void setMaxTicks();
};

#endif

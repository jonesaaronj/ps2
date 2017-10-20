#ifndef PS2_h
#define PS2_h

#include "Arduino.h"
#include "PS2Enums.h"

#define EP_MAXPKTSIZE 29

#define REPORT 0
#define CONFIG 1
#define PS2_REPORT_SIZE 21
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
    
    void setButton(ButtonEnum button, byte b);
    void setAnalogueButton(ButtonEnum button, byte b);

    byte getRumbleLeft();
    byte getRumbleRight();

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
    uint8_t mode = REPORT;
    
    uint8_t commandBuffer[EP_MAXPKTSIZE];
    uint8_t reportBuffer[PS2_REPORT_SIZE];

    uint8_t data41[PS2_CONFIG_SIZE];
    uint8_t data44[PS2_CONFIG_SIZE];
    uint8_t data45[PS2_CONFIG_SIZE];
    uint8_t data4D[PS2_CONFIG_SIZE];
    uint8_t data4F[PS2_CONFIG_SIZE];

    uint8_t data46, data4C;

  	void tickFalling();
  	void tickRising();
  	void attentionFalling();
    void attentionRising();
    void switchMode();
};

#endif
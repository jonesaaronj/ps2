#include "Arduino.h"
#include "PS2.h"

PS2::PS2(uint8_t clockPin = 0,
  uint8_t attenPin = 0,
  uint8_t commandPin = 0,
  uint8_t dataPin = 0,
  uint8_t ackPin = 0) {

  pinMode(clockPin, INPUT_PULLUP);
  pinMode(attenPin, INPUT_PULLUP);
  pinMode(commandPin, INPUT_PULLUP);

  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, LOW);
  
  pinMode(ackPin, OUTPUT);
  digitalWrite(ackPin, HIGH);
}

void PS2::handleTick(){
  digitalRead(clockPin) == HIGH ? tickRising() : tickFalling();
}

// writing should happen on falling
void PS2::tickFalling() {
  
  //if we are not at attention do nothing
  //attention is low when sending/receiving
  if (digitalRead(attenPin) || tick >= maxTicks) {
    return;
  }

  //which byte are we on
  by = tick / 8;
  //which bit are we on
  bi = tick % 8;

  if (by < 3) {
    // we are in the header, write header bytes
    switch (mode) {
      case REPORT_MODE:
        digitalWrite(dataPin, ((reportDataHeader[by] & (1 << bi)) >> bi));
        break;
      case CONFIG_MODE:
        digitalWrite(dataPin, ((configDataHeader[by] & (1 << bi)) >> bi));
        break;
    }
  } else {
    // check header to see what we are doing 
    switch (commandBuffer[1]) {
      
      case 0x41:
        digitalWrite(dataPin, ((data41[by] & (1 << bi)) >> bi));
        break;
      
      // Main polling command
      case 0x42:
        digitalWrite(dataPin, ((reportBuffer[by] & (1 << bi)) >> bi));
        break;
        
      // Config mode
      case 0x43:
        digitalWrite(dataPin, ((reportBuffer[by] & (1 << bi)) >> bi));
        break;

      case 0x44:
        digitalWrite(dataPin, ((data44[by] & (1 << bi)) >> bi));
        break;
        
      case 0x45:
        digitalWrite(dataPin, ((data45[by] & (1 << bi)) >> bi));
        break;
        
      case 0x46:
        switch (data46) {
          case 0:
            digitalWrite(dataPin, ((data46_a[by] & (1 << bi)) >> bi));
            break;
          case 1:
            digitalWrite(dataPin, ((data46_b[by] & (1 << bi)) >> bi));
            break;
        }
        data46 = !data46;
        break;
        
      case 0x47:
        digitalWrite(dataPin, ((pgm_read_byte(&data47[(uint8_t)by])& (1 << bi)) >> bi));
        break;
        
      case 0x4C:
        switch (data4C) {
          case 0:
            digitalWrite(dataPin, ((pgm_read_byte(&data4C_a[(uint8_t)by]) & (1 << bi)) >> bi));
            break;
          case 1:
            digitalWrite(dataPin, ((pgm_read_byte(&data4C_a[(uint8_t)by]) & (1 << bi)) >> bi));
            break;
        }
        data4C = !data4C;
        break;
        
      case 0x4D:
        digitalWrite(dataPin, ((data4D[by] & (1 << bi)) >> bi));
        break;
        
      case 0x4F:
        digitalWrite(dataPin, ((data4F[by] & (1 << bi)) >> bi));
        break;
    }
  }

  // if we are at the end of a byte send an ack for a few us
  if (bi == 7 && tick < maxTicks){
    delayMicroseconds(10);   
    digitalWrite(ackPin, LOW);
    delayMicroseconds(20);
    digitalWrite(ackPin, HIGH); 
  }
  
  tick++;
}

// read data on rising to allow settling
void PS2::tickRising() {

  //if we are not at attention do nothing
  //attention is low when sending/receiving
  if (digitalRead(attenPin) || tick >= maxTicks) {
    return;
  }

  commandBuffer[by] |= (digitalRead(commandPin) << bi);

  if (by > 3) {
    switch (commandBuffer[1]) {
      case 0x43:
        // togle config mode
        switchMode();
        break;
    }
  }

  // if we are at the end of a byte send an ack for a few us
  if (bi == 7 && tick < maxTicks){
    delayMicroseconds(10);   
    digitalWrite(ackPin, LOW);
    delayMicroseconds(20);
    digitalWrite(ackPin, HIGH); 
  }

  tick++;
}

void PS2::handleAttention() {
  digitalRead(clockPin) == HIGH ? 
  attentionRising() : 
  attentionFalling();
}

void PS2::attentionFalling() {
  tick = 0;

  // reset all our command arrays
  memset(commandBuffer, 0x00, sizeof commandBuffer);
}

void PS2::attentionRising() {

}

void PS2::switchMode() {
  mode = !mode;
  // set maxTicks for the mode
  switch (mode) {
    case REPORT_MODE:
      maxTicks = PS2_REPORT_SIZE;
      break;
    case CONFIG_MODE:
      maxTicks = PS2_CONFIG_SIZE;
      break;
  }
}

void PS2::setButton(ButtonEnum button, bool b) {
  // digital buttons are reported in the 4th and 5th byte of the report
  // when button is clicked signal is low
  uint8_t by = (uint8_t)(pgm_read_word(&PS2_BUTTONS[(uint8_t)button]) >> 8);
  uint8_t bi = (uint8_t)(pgm_read_word(&PS2_BUTTONS[(uint8_t)button]) & 0x00FF);

  b ?
  reportBuffer[3 + by] &= ~bi:
  reportBuffer[3 + by] |= bi;
}

void PS2::setAnalogueHat(AnalogHatEnum hat, uint8_t b) {
  reportBuffer[pgm_read_byte(&PS2_ANALOG_HATS[(uint8_t)hat])] = b;
}

void PS2::setAnalogueButton(ButtonEnum button, uint8_t b) {
  reportBuffer[pgm_read_byte(&PS2_ANALOG_BUTTONS[(uint8_t)button])] = b;
}


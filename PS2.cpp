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

  //digitalWrite(ackPin, LOW);

  switch (by) {
    case 0:
      // first byte is always 0xFF
      digitalWrite(dataPin, ((0xFF & (1 << bi)) >> bi));
      break;
    case 1:
      // second byte
      // first nibble is 0x4 for digital, 0x7 for analog, 0xF for config
      // second nibble is for the length of the report
      switch (commMode) {
        case CONFIG_MODE:
          digitalWrite(dataPin, ((0xF3 & (1 << bi)) >> bi));
          break;
        case REPORT_MODE:
          switch (contMode) {
            case DIGITAL_MODE:
              digitalWrite(dataPin, ((0x41 & (1 << bi)) >> bi));
              break;
            case ANALOGUE_MODE:
              digitalWrite(dataPin, ((0x73 & (1 << bi)) >> bi));
              break;
            case ANALOGUE_EX_MODE:
              digitalWrite(dataPin, ((0x79 & (1 << bi)) >> bi));
              break;
          }
          break;
      }
    case 2:
      // third byte is always 0x5A
      digitalWrite(dataPin, ((0x5A & (1 << bi)) >> bi));
      break;
    default:
      // check header to see what we are doing 
      switch (commandBuffer[1]) {
       
        case 0x41:
          switch (contMode) {
            case DIGITAL_MODE:
              digitalWrite(dataPin, ((pgm_read_byte(&data41_digital[(uint8_t)by]) & (1 << bi)) >> bi));
              break;
            case ANALOGUE_MODE:
              digitalWrite(dataPin, ((pgm_read_byte(&data41_analogue[(uint8_t)by]) & (1 << bi)) >> bi));
              break;
            case ANALOGUE_EX_MODE:
              digitalWrite(dataPin, ((pgm_read_byte(&data41_analogue_ex[(uint8_t)by]) & (1 << bi)) >> bi));
              break;
          }
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
          // for this mode data is always 0
          digitalWrite(dataPin, LOW);
          break;
          
        case 0x45:
          digitalWrite(dataPin, ((pgm_read_byte(&data45[(uint8_t)by]) & (1 << bi)) >> bi));
          break;
          
        case 0x46:
          switch (data46) {
            case 0:
              digitalWrite(dataPin, ((pgm_read_byte(&data46_a[(uint8_t)by]) & (1 << bi)) >> bi));
              break;
            case 1:
              digitalWrite(dataPin, ((pgm_read_byte(&data46_b[(uint8_t)by]) & (1 << bi)) >> bi));
              break;
          }
          break;
          
        case 0x47:
          digitalWrite(dataPin, ((pgm_read_byte(&data47[(uint8_t)by]) & (1 << bi)) >> bi));
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
          break;
          
        case 0x4D:
          digitalWrite(dataPin, ((data4D[by] & (1 << bi)) >> bi));
          break;
          
        case 0x4F:
          digitalWrite(dataPin, ((pgm_read_byte(&data4F[(uint8_t)by]) & (1 << bi)) >> bi));
          break;
          
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

  if (by == 5 && bi == 0) {
    switch (commandBuffer[1]) {
      case 0x43:
        // set config mode
        setCommMode(commandBuffer[3]);
        break;

      case 0x46:
        data46 = commandBuffer[3];
        break;

      case 0x4C:
        data4C = commandBuffer[3];
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
  digitalRead(clockPin) == HIGH ? attentionRising() : attentionFalling();
}

void PS2::attentionFalling() {
  tick = 0;

  // pull data from command buffer
  switch (commandBuffer[1]) {
    case 0x42:
      // pull rumble data
      rumble[0] = commandBuffer[4];
      rumble[1] = commandBuffer[5];
      break;

    case 0x44:
      setContMode(commandBuffer[3]);
      contModeLocked = (commandBuffer[4] == 0x03);
      break;
  }
    
  // reset our commandBuffer
  memset(commandBuffer, 0x00, sizeof commandBuffer);
}

void PS2::attentionRising() {
  
}

void PS2::setCommMode(uint8_t m) {
  commMode = m;
  setMaxTicks();
}

void PS2::setContMode(uint8_t m) {
  contMode = m;
  setMaxTicks();
}

void PS2::setMaxTicks() {
  // set maxTicks for the mode
  commMode == CONFIG_MODE ? maxTicks = PS2_CONFIG_SIZE:
    contMode == DIGITAL_MODE ? maxTicks = PS2_DIGITAL_REPORT_SIZE:
      maxTicks = PS2_ANALOGUE_REPORT_SIZE;
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

void PS2::toggleContMode() {
  if (!contModeLocked) {
    contMode = contMode++ % 3;
  }
}


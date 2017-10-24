#include "Arduino.h"
#include "PS2.h"

PS2::PS2(uint8_t clockPin = 0,
  uint8_t attenPin = 0,
  uint8_t commandPin = 0,
  uint8_t dataPin = 0,
  uint8_t ackPin = 0) {

  // set clock, atten, and command as inputs
  pinMode(clockPin, INPUT_PULLUP);
  pinMode(attenPin, INPUT_PULLUP);
  pinMode(commandPin, INPUT_PULLUP);

  // set data and ack as outputs
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
  if (digitalRead(attenPin) || tick > maxTicks) {
    return;
  }

  //which byte are we on
  by = tick / 8;
  //which bit are we on
  bi = tick % 8;

  // first three bytes are part of the header
  // the rest are determined by 2nd byte of the command header
  switch (by) {
    case 0:
      // first byte is always 0xFF
      digitalWrite(dataPin, ((0xFF & (1 << bi)) >> bi));
      break;
    case 1:
      // second byte
      // first nibble is 0x4 for digital, 0x7 for analog, 0xF for config
      // second nibble is for the length of the report in words
      switch (commMode) {
        case CONFIG_MODE:
          digitalWrite(dataPin, ((0xF3 & (1 << bi)) >> bi));
          break;
        case REPORT_MODE:
          switch (contMode) {
            case DIGITAL_MODE:
              digitalWrite(dataPin, ((0x41 & (1 << bi)) >> bi));
              break;
            case ANALOG_MODE:
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

        // 
        // send our report config
        case 0x41:
          digitalWrite(dataPin, ((reportConfig[(uint8_t)by] & (1 << bi)) >> bi));
          break;
        
        // Main polling command.  send our button report
        // fix to take report config into account
        case 0x42:
          digitalWrite(dataPin, ((reportBuffer[by] & (1 << bi)) >> bi));
          break;
          
        // Enter/Exit config mode.  send our button report
        // fix to take report config into account
        case 0x43:
          digitalWrite(dataPin, ((reportBuffer[by] & (1 << bi)) >> bi));
          break;

        // Switch Mode between digital and analog.
        case 0x44:
          // for this mode data is always 0
          digitalWrite(dataPin, LOW);
          break;

        // Get more status info
        case 0x45:
          digitalWrite(dataPin, ((pgm_read_byte(&data45[(uint8_t)by]) & (1 << bi)) >> bi));
          break;

        // Read an unknown constant value from controller
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

        // Read an unknown constant value from controller
        case 0x47:
          digitalWrite(dataPin, ((pgm_read_byte(&data47[(uint8_t)by]) & (1 << bi)) >> bi));
          break;

        // Read an unknown constant value from controller
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

        // Map bytes in the 0x42 command to actuate the vibration motors
        case 0x4D:
          digitalWrite(dataPin, ((pgm_read_byte(&data4D[(uint8_t)by]) & (1 << bi)) >> bi));
          break;

        // Add or remove analog response bytes from the main polling command (0x42)
        case 0x4F:
          digitalWrite(dataPin, ((pgm_read_byte(&data4F[(uint8_t)by]) & (1 << bi)) >> bi));
          break;
          
      break;
    }
  }
}

// read data on rising to allow settling
void PS2::tickRising() {

  //if we are not at attention do nothing
  //attention is low when sending/receiving
  if (digitalRead(attenPin) || tick >= maxTicks) {
    return;
  }

  commandBuffer[by] |= (digitalRead(commandPin) << bi);

  if (by == 4 && bi == 0) {
    switch (commandBuffer[1]) {
      case 0x43:
        setCommMode(commandBuffer[3]);
        break;

      case 0x46:
        data46 = commandBuffer[3];
        break;

      case 0x4C:
        data4C = commandBuffer[3];
        break;
    }
  } else if (by == 5 && bi == 0) {
    switch (commandBuffer[1]) {
      case 0x4D:
        motor0 = commandBuffer[3];
        motor1 = commandBuffer[4];
        break;
    }
      
  } else if (by == 6 && bi == 0) {
    switch (commandBuffer[1]) {
      case 0x4F:
      reportConfig[3] = commandBuffer[3];
      reportConfig[4] = commandBuffer[4];
      reportConfig[5] = commandBuffer[5];
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
  
}

void PS2::attentionRising() {
  tick = 0;
  by = 0;
  bi = 0;

  // pull data from command buffer
  switch (commandBuffer[1]) {
    case 0x42:
      // pull rumble data
      switch (motor0) {
        case PS2_SMALL_MOTOR:
          smallMotor = commandBuffer[3];
          break;
        case PS2_LARGE_MOTOR:
          largeMotor = commandBuffer[3];
          break;
      }
      switch (motor1) {
        case PS2_SMALL_MOTOR:
          smallMotor = commandBuffer[4];
          break;
        case PS2_LARGE_MOTOR:
          largeMotor = commandBuffer[4];
          break;
      }
      break;

    case 0x44:
      setContMode(commandBuffer[3]);
      contModeLocked = (commandBuffer[4] == 0x03);
      break;
  }
    
  // reset our commandBuffer
  memset(commandBuffer, 0x00, sizeof commandBuffer);
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
  switch (commMode) {
    case CONFIG_MODE:
      maxTicks = PS2_CONFIG_SIZE;
      break;
    case REPORT_MODE:
      switch (contMode) {
        case DIGITAL_MODE:
          maxTicks = PS2_DIGITAL_REPORT_SIZE;
          break;
        case ANALOG_MODE:
          maxTicks = PS2_ANALOG_REPORT_SIZE;
          break;
      }
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
  reportBuffer[pgm_read_byte(&PS2_ANALOG_HATS[hat])] = b;
}

void PS2::setAnalogueButton(ButtonEnum button, uint8_t b) {
  reportBuffer[pgm_read_byte(&PS2_ANALOG_BUTTONS[button])] = b;
}

void PS2::toggleContMode() {
  if (!contModeLocked) {
    contMode = contMode++ % 3;
  }
}

uint8_t PS2::getSmallMotor() {
  return smallMotor;
}

uint8_t PS2::getLargeMotor() {
  return largeMotor;
}


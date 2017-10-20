#include "PS2.h"

#include <PS3BT.h>
#include <usbhub.h>
#include <SPI.h>

#define CLOCK_PIN 1
#define ATTEN_PIN 2
#define COMMAND_PIN 3
#define DATA_PIN 4
#define ACK_PIN 5

USB Usb;
BTD Btd(&Usb);
PS3BT PS3(&Btd);

PS2 PS2(CLOCK_PIN, ATTEN_PIN, COMMAND_PIN, DATA_PIN, ACK_PIN);

void setup() {
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nPS3 Bluetooth Library Started"));

  // interrupts can only be attached to global functions
  attachInterrupt(CLOCK_PIN, handleTick, CHANGE);
  attachInterrupt(ATTEN_PIN, handleAttention, CHANGE);
}

void loop() {
  Usb.Task();

  if (PS3.PS3Connected || PS3.PS3NavigationConnected) {

    // set digital buttons
    PS2.setButton(UP, PS3.getButtonPress(UP));
    PS2.setButton(DOWN, PS3.getButtonPress(DOWN));
    PS2.setButton(LEFT, PS3.getButtonPress(LEFT));
    PS2.setButton(RIGHT, PS3.getButtonPress(RIGHT));
    
    PS2.setButton(SELECT, PS3.getButtonPress(SELECT));
    PS2.setButton(START, PS3.getButtonPress(START));

    PS2.setButton(L1, PS3.getButtonPress(L1));
    PS2.setButton(L2, PS3.getButtonPress(L2));
    PS2.setButton(L3, PS3.getButtonPress(L3));
    PS2.setButton(R1, PS3.getButtonPress(R1));
    PS2.setButton(R2, PS3.getButtonPress(R2));
    PS2.setButton(R3, PS3.getButtonPress(R3));
    
    PS2.setButton(TRIANGLE, PS3.getButtonPress(TRIANGLE));
    PS2.setButton(CIRCLE, PS3.getButtonPress(CIRCLE));
    PS2.setButton(CROSS, PS3.getButtonPress(CROSS));
    PS2.setButton(SQUARE, PS3.getButtonPress(SQUARE));

    // set analogue buttons
    PS2.setAnalogueButton(L2, PS3.getAnalogButton(L2));
    
    //getAnalogButton(ButtonEnum a);
    //getAnalogHat();
    
    if (PS3.getAnalogHat(LeftHatX) > 137 || PS3.getAnalogHat(LeftHatX) < 117 || PS3.getAnalogHat(LeftHatY) > 137 || PS3.getAnalogHat(LeftHatY) < 117 || PS3.getAnalogHat(RightHatX) > 137 || PS3.getAnalogHat(RightHatX) < 117 || PS3.getAnalogHat(RightHatY) > 137 || PS3.getAnalogHat(RightHatY) < 117) {
      Serial.print(F("\r\nLeftHatX: "));
      Serial.print(PS3.getAnalogHat(LeftHatX));
      Serial.print(F("\tLeftHatY: "));
      Serial.print(PS3.getAnalogHat(LeftHatY));
      if (PS3.PS3Connected) { // The Navigation controller only have one joystick
        Serial.print(F("\tRightHatX: "));
        Serial.print(PS3.getAnalogHat(RightHatX));
        Serial.print(F("\tRightHatY: "));
        Serial.print(PS3.getAnalogHat(RightHatY));
      }
    }

    // Analog button values can be read from almost all buttons
    if (PS3.getAnalogButton(L2) || PS3.getAnalogButton(R2)) {
      Serial.print(F("\r\nL2: "));
      Serial.print(PS3.getAnalogButton(L2));
      if (PS3.PS3Connected) {
        Serial.print(F("\tR2: "));
        Serial.print(PS3.getAnalogButton(R2));
      }
    }

    if (PS3.getButtonClick(PS)) {
      Serial.print(F("\r\nPS"));
      PS3.disconnect();
    }
    else {
      if (PS3.getButtonClick(TRIANGLE)) {
        Serial.print(F("\r\nTraingle"));
        PS3.setRumbleOn(RumbleLow);
      }
      if (PS3.getButtonClick(CIRCLE)) {
        Serial.print(F("\r\nCircle"));
        PS3.setRumbleOn(RumbleHigh);
      }
      if (PS3.getButtonClick(CROSS))
        Serial.print(F("\r\nCross"));
      if (PS3.getButtonClick(SQUARE))
        Serial.print(F("\r\nSquare"));

      if (PS3.getButtonClick(UP)) {
        Serial.print(F("\r\nUp"));
        if (PS3.PS3Connected) {
          PS3.setLedOff();
          PS3.setLedOn(LED4);
        }
      }
      if (PS3.getButtonClick(RIGHT)) {
        Serial.print(F("\r\nRight"));
        if (PS3.PS3Connected) {
          PS3.setLedOff();
          PS3.setLedOn(LED1);
        }
      }
      if (PS3.getButtonClick(DOWN)) {
        Serial.print(F("\r\nDown"));
        if (PS3.PS3Connected) {
          PS3.setLedOff();
          PS3.setLedOn(LED2);
        }
      }
      if (PS3.getButtonClick(LEFT)) {
        Serial.print(F("\r\nLeft"));
        if (PS3.PS3Connected) {
          PS3.setLedOff();
          PS3.setLedOn(LED3);
        }
      }

      if (PS3.getButtonClick(L1))
        Serial.print(F("\r\nL1"));
      if (PS3.getButtonClick(L3))
        Serial.print(F("\r\nL3"));
      if (PS3.getButtonClick(R1))
        Serial.print(F("\r\nR1"));
      if (PS3.getButtonClick(R3))
        Serial.print(F("\r\nR3"));

      
    }
  }
}

void handleTick() {
  PS2.handleTick();
}

void handleAttention() {
  PS2.handleAttention();
}





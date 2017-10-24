#include <PS3BT.h>
#include <usbhub.h>
#include <SPI.h>
#include "PS2.h"

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
  // Wait for serial port to connect - used on Leonardo, 
  //Teensy and other boards with built-in USB CDC serial connection
  while (!Serial); 
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
    PS2.setButton(SELECT, PS3.getButtonPress(SELECT));
    PS2.setButton(L3, PS3.getButtonPress(L3));
    PS2.setButton(R3, PS3.getButtonPress(R3));
    PS2.setButton(START, PS3.getButtonPress(START));
   
    PS2.setButton(UP, PS3.getButtonPress(UP));
    PS2.setButton(RIGHT, PS3.getButtonPress(RIGHT));
    PS2.setButton(DOWN, PS3.getButtonPress(DOWN));
    PS2.setButton(LEFT, PS3.getButtonPress(LEFT));
    
    PS2.setButton(L2, PS3.getButtonPress(L2));
    PS2.setButton(R2, PS3.getButtonPress(R2));
    PS2.setButton(L1, PS3.getButtonPress(L1));
    PS2.setButton(R1, PS3.getButtonPress(R1));
    
    PS2.setButton(TRIANGLE, PS3.getButtonPress(TRIANGLE));
    PS2.setButton(CIRCLE, PS3.getButtonPress(CIRCLE));
    PS2.setButton(CROSS, PS3.getButtonPress(CROSS));
    PS2.setButton(SQUARE, PS3.getButtonPress(SQUARE));

    // set analogue buttons
    PS2.setAnalogueHat(RightHatX, PS3.getAnalogHat(RightHatX));
    PS2.setAnalogueHat(RightHatY, PS3.getAnalogHat(RightHatY));
    PS2.setAnalogueHat(LeftHatX, PS3.getAnalogHat(LeftHatX));
    PS2.setAnalogueHat(LeftHatY, PS3.getAnalogHat(LeftHatY));
    
    PS2.setAnalogueButton(RIGHT, PS3.getAnalogButton(RIGHT));
    PS2.setAnalogueButton(LEFT, PS3.getAnalogButton(LEFT));
    PS2.setAnalogueButton(UP, PS3.getAnalogButton(UP));
    PS2.setAnalogueButton(DOWN, PS3.getAnalogButton(DOWN));

    PS2.setAnalogueButton(TRIANGLE, PS3.getAnalogButton(TRIANGLE));
    PS2.setAnalogueButton(CIRCLE, PS3.getAnalogButton(CIRCLE));
    PS2.setAnalogueButton(CROSS, PS3.getAnalogButton(CROSS));
    PS2.setAnalogueButton(SQUARE, PS3.getAnalogButton(SQUARE));
    
    PS2.setAnalogueButton(L1, PS3.getAnalogButton(L1));
    PS2.setAnalogueButton(R1, PS3.getAnalogButton(R1));
    PS2.setAnalogueButton(L2, PS3.getAnalogButton(L2));
    PS2.setAnalogueButton(R2, PS3.getAnalogButton(R2));

    if (PS3.getButtonClick(PS)) {
      PS2.toggleContMode();
    }

    PS2.getSmallMotor();
    PS2.getLargeMotor();
  }
}

void handleTick() {
  PS2.handleTick();
}

void handleAttention() {
  PS2.handleAttention();
}





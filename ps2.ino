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
  // put your main code here, to run repeatedly:
}

void handleTick() {
  PS2.handleTick();
}

void handleAttention() {
  PS2.handleAttention();
}





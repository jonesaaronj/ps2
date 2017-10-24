#ifndef _ps2enums_h
#define _ps2enums_h

#include "PS3Enums.h"

#define PS2_HEADER_SIZE 3
#define PS2_CONFIG_SIZE 9

// the ordering should match the PS3 library
const uint32_t PS2_BUTTONS[] PROGMEM = {
        0x0010, // UP
        0x0020, // RIGHT
        0x0040, // DOWN
        0x0080, // LEFT

        0x0001, // SELECT
        0x0008, // START
        0x0002, // L3
        0x0004, // R3

        0x0101, // L2
        0x0102, // R2
        0x0104, // L1
        0x0108, // R1

        0x0110, // TRIANGLE
        0x0120, // CIRCLE
        0x0140, // CROSS
        0x0180, // SQUARE

        //0x010000, // PS
};

// the byte location of the analog button
// the ordering should match the PS3 library
const uint8_t PS2_ANALOG_BUTTONS[] PROGMEM = {
        11, // UP_ANALOG
         9, // RIGHT_ANALOG
        12, // DOWN_ANALOG
        10, // LEFT_ANALOG
        
        0, 0, 0, 0, // Skip SELECT, START, L3, R3

        19, // L2_ANALOG
        20, // R2_ANALOG
        17, // L1_ANALOG
        18, // R1_ANALOG
        
        13, // TRIANGLE_ANALOG
        14, // CIRCLE_ANALOG
        15, // CROSS_ANALOG
        16, // SQUARE_ANALOG
        
        0, 0, // Skip PS and MOVE
};

// the byte location of the analoge hat
// the ordering should match the PS3 library
const uint8_t PS2_ANALOG_HATS[] PROGMEM = {
        7, // LeftHatX
        8, // LeftHatY
        5, // RightHatX
        6, // RightHatY
};

// these arrays are oversized
// to make the const uint8_t math easier

// 0x45: Get more status info
const uint8_t data45[PS2_CONFIG_SIZE] PROGMEM = {
    0x00, 0x00, 0x00, 0x03/*dualshock*/, 0x02, 0x01/*led*/, 0x02, 0x01, 0x00
};

// 0x46: Read an unknown constant value from controller
// these two packets are always sent in consecutive order
const uint8_t data46_a[PS2_CONFIG_SIZE] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x0A
};
const uint8_t data46_b[PS2_CONFIG_SIZE] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14
};

// 0x47: Read an unknown constant value from controller
const uint8_t data47[PS2_CONFIG_SIZE] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00
};

// 0x4C: Read an unknown constant value from controller
// these two packets are always sent in consecutive order
const uint8_t data4C_a[PS2_CONFIG_SIZE] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00
};
const uint8_t data4C_b[PS2_CONFIG_SIZE] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00
};

const uint8_t data4D[PS2_CONFIG_SIZE] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF
};

const uint8_t data4F[PS2_CONFIG_SIZE] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5A
};
#endif

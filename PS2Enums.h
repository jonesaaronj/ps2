#ifndef _ps2enums_h
#define _ps2enums_h

#include "PS3Enums.h"

#define PS2_HEADER_SIZE 3
#define PS2_CONFIG_SIZE 9

// if we are in REPORT mode our header data is always the same
// 79 indicates we are analogue(F) with (9) words following the header
const uint8_t reportDataHeader[PS2_HEADER_SIZE] = {
    0xFF, 0xF9, 0x5A
};

// if we are in CONFIG mode our header data is always the same
// F3 indicates we are analogue(F) with (3) words following the header
const uint8_t configDataHeader[PS2_HEADER_SIZE] = {
    0xFF, 0xF3, 0x5A
};

// these arrays are oversized
// to make the const uint8_t math easier

// 0x41: Find out what buttons are included in poll responses.
// will contain rumble data
const uint8_t data41[PS2_CONFIG_SIZE] = {
    0x00, 0x00, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x5A
};

// 0x45: Get more status info
const uint8_t data45[PS2_CONFIG_SIZE] = {
    0x00, 0x00, 0x00, 0x03/*dualshock*/, 0x02, 0x01/*led*/, 0x02, 0x01, 0x00
};

// 0x46: Read an unknown constant value from controller
// these two packets are always sent in consecutive order
const uint8_t data46_a[PS2_CONFIG_SIZE] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x0A
};
const uint8_t data46_b[PS2_CONFIG_SIZE] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14
};

// 0x47: Read an unknown constant value from controller
const uint8_t data47[PS2_CONFIG_SIZE] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00
};

// 0x4C: Read an unknown constant value from controller
// these two packets are always sent in consecutive order
const uint8_t data4C_a[PS2_CONFIG_SIZE] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00
};
const uint8_t data4C_b[PS2_CONFIG_SIZE] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00
};

#endif
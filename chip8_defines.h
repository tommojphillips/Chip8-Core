// chip8_defines.h
//
// GitHub: https:\\github.com\tommojphillips

#ifndef CHIP8_DEFINES_H
#define CHIP8_DEFINES_H

/* Shrink video ram to 256 bytes from 2048 bytes
 Originally a pixel takes up 1 byte but only uses 1 bit of data,
 there is 8 bits in a byte, so if dynamic memory is tight you can
 enable this to save 1792 bytes! */
//#define CHIP8_SHRINK_VIDEO_RAM

#define CYCLE_COUNT

#endif

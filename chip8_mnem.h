// chip8_mnem.h
//
// GitHub: https:\\github.com\tommojphillips

#ifndef CHIP8_MNEM_H
#define CHIP8_MNEM_H

#include <stdint.h>

#include "chip8_defines.h"
#include "chip8.h"

#ifdef CHIP8_MNEMONICS

#ifdef __cplusplus
extern "C" {
#endif

/* Disassemble instruction at pc into str, if passed in pc is 0, gets pc from cpu struct */
int chip8_mnem(CHIP8* chip8, uint16_t pc, char* str);

/* Find next instruction
 * Returns the next program counter in pc */
void chip8_mnem_find_next(CHIP8* chip8, uint16_t* pc);

#ifdef __cplusplus
};
#endif
#endif
#endif

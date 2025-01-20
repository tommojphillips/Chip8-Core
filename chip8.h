// chip8.h
//
// GitHub: https:\\github.com\tommojphillips

#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#include "chip8_defines.h"

#define CHIP8_REGISTER_COUNT	0x10
#define CHIP8_STACK_SIZE		0x10
#define CHIP8_MEMORY_BYTES		0x1000
#define CHIP8_PROGRAM_ADDR		0x200
#define CHIP8_FONT_BYTES		0x50

#define CHIP8_DISPLAY_WIDTH		64
#define CHIP8_DISPLAY_HEIGHT	32

#define CHIP8_NUM_PIXELS (CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT)

#ifdef CHIP8_SHRINK_DISPLAY_RAM
#define CHIP8_DISPLAY_BYTES (CHIP8_NUM_PIXELS >> 3) /* divide by 8 */
#define CHIP8_DISPLAY_GET_PX(s, i) (s[(i) >> 3] & (1 << ((i) & 7)))
#define CHIP8_DISPLAY_SET_PX(s, i) (s[(i) >> 3] |= (1 << ((i) & 7)))
#define CHIP8_DISPLAY_CLR_PX(s, i) (s[(i) >> 3] &= ~(1 << ((i) & 7)))
#define CHIP8_DISPLAY_TOGGLE_PX(s, i) (s[(i) >> 3] ^= (1 << ((i) & 7)))
#else
#define CHIP8_DISPLAY_BYTES (CHIP8_NUM_PIXELS)
#define CHIP8_DISPLAY_GET_PX(s, i) (s[i])
#define CHIP8_DISPLAY_SET_PX(s, i) (s[i] = 0x1)
#define CHIP8_DISPLAY_CLR_PX(s, i) (s[i] = 0x0)
#define CHIP8_DISPLAY_TOGGLE_PX(s, i) (s[i] ^= 0x1)
#endif

#define CHIP8_KEYPAD_SET(s, n, v) s = (s & ~(0x1U << (n))) | ((v) << (n))
#define CHIP8_KEYPAD_GET(s, n) ((s >> (n)) & 0x1U)

#define READ_BYTE(address)			chip8->ram[(address) & (CHIP8_MEMORY_BYTES - 1)]
#define WRITE_BYTE(address, value)	chip8->ram[(address) & (CHIP8_MEMORY_BYTES - 1)] = (value)
#define GET_OPCODE(address)			(READ_BYTE(address) << 8) | READ_BYTE(address + 1)

 /* Chip8 cpu state */
typedef enum {
	CHIP8_STATE_EXE = 0,
	CHIP8_STATE_HLT = 1,
	CHIP8_STATE_ERROR_OPCODE = 2,
} CHIP8_CPU_STATE;

/* Chip8 key state */
typedef enum {
	CHIP8_KEY_STATE_KEY_UP = 0,
	CHIP8_KEY_STATE_KEY_DOWN = 1
} CHIP8_KEY_STATE;

/* Chip8 quirk flags */
typedef enum {
	CHIP8_QUIRK_NONE = 0,
	CHIP8_QUIRK_SHIFT_X_REGISTER = 2,
	CHIP8_QUIRK_ZERO_VF_REGISTER = 4,
	CHIP8_QUIRK_CLS_ON_RESET = 8,
	CHIP8_QUIRK_INCREMENT_I_REGISTER = 16,
	CHIP8_QUIRK_JUMP_VX = 32,
	CHIP8_QUIRK_DISPLAY_CLIPPING = 64,
	CHIP8_QUIRK_DISPLAY_WAIT = 128,
} CHIP8_QUIRKS; 

/* Chip8 state structure*/
typedef struct {
	uint16_t i;				// I register
	uint16_t pc;			// program counter
	uint16_t sp;			// stack pointer
	uint16_t opcode;		// current opcode
	uint16_t keypad;		// keypad state; 1 bit per key
	uint16_t fxoa_state;	// FX0A keypad state; 1 bit per key
	
	uint8_t delay_timer;
	uint8_t sound_timer;
	uint8_t draw_display;
	uint8_t cpu_state;

	uint8_t v[CHIP8_REGISTER_COUNT]; // general registers
	uint16_t stack[CHIP8_STACK_SIZE]; 
	uint8_t ram[CHIP8_MEMORY_BYTES];
	uint8_t display[CHIP8_DISPLAY_BYTES];

	uint32_t quirks;

#ifdef CYCLE_COUNT
	uint64_t cycles;
#endif

} CHIP8;

#ifdef __cplusplus
extern "C" {
#endif

// Initialize chip8 cpu state
void chip8_init_cpu(CHIP8* chip8);

// Reset chip8 cpu state
void chip8_reset_cpu(CHIP8* chip8);

// Load font into chip8 memory space
void chip8_load_font(CHIP8* chip8, const uint8_t* font);

// Zero chip8 entire memory space
void chip8_zero_memory(CHIP8* chip8);

// Zero chip8 program space
void chip8_zero_program_memory(CHIP8* chip8);

// Zero chip8 video space
void chip8_zero_video_memory(CHIP8* chip8);

// Step timers
void chip8_step_timers(CHIP8* chip8);

// Decode and execute next instruction
void chip8_execute(CHIP8* chip8);

/*
 * Implementation dependent functions
 */

 /* Chip8 Render implementation */
void chip8_render(CHIP8* chip8);

/* Chip8 Beep implementation */
void chip8_beep(CHIP8* chip8);

/* Chip8 Random byte implementation */
uint8_t chip8_random();

#ifdef __cplusplus
};
#endif

#endif

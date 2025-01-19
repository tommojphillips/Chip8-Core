// chip8.c
// 
// GitHub: https:\\github.com\tommojphillips
// 
// src: 
// https:\\www.laurencescotford.net\2020\07\25\chip-8-on-the-cosmac-vip-instruction-index\

#include <stdint.h>

#include "chip8.h"
#include "chip8_defines.h"

#define X ((chip8->opcode >> 8) & 0x0F) // X register index
#define Y ((chip8->opcode >> 4) & 0x00F) // Y register index
#define NNN (chip8->opcode & 0x0FFF)
#define NN (chip8->opcode & 0x00FF)
#define N (chip8->opcode & 0x000F)

#define VX chip8->v[X]
#define VY chip8->v[Y]
#define VF chip8->v[0xF]
#define I chip8->i
#define PC chip8->pc
#define SP chip8->sp

#ifdef CYCLE_COUNT
#define CYCLES(x) (chip8->cycles += (x))
#else
#define CYCLES(x)
#endif

/* Builtin font */
static const uint8_t chip8_font[CHIP8_FONT_BYTES] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

/* Note: Opcodes that affect the VF register need to cache the value 
 * for VF prior to operation. Then after operation, assign VF to the
 * cached value. This is because VF can be used in the operation itself.
 * Setting VF early will result in an incorrect operation. */

/* OPCODES*/

static void chip8_00E0(CHIP8* chip8) {
	// CLS
	chip8_zero_video_memory(chip8);
	CYCLES(24);
	PC += 2;
}
static void chip8_00EE(CHIP8* chip8) {
	// RET
	PC = chip8->stack[SP];
	SP -= 1;
	CYCLES(10);
	PC += 2;
}
static void chip8_1NNN(CHIP8* chip8) {
	// JMP NNN
	CYCLES(12);
	PC = NNN;
}
static void chip8_2NNN(CHIP8* chip8) {
	// CALL NNN
	SP += 1;
	chip8->stack[SP] = PC;
	CYCLES(26);
	PC = NNN;
}
static void chip8_3XNN(CHIP8* chip8) {
	// SE VX, NN
	if (VX == NN) {
		CYCLES(4);
		PC += 2;
	}
	CYCLES(10);
	PC += 2;
}
static void chip8_4XNN(CHIP8* chip8) {
	// SNE VX, NN
	if (VX != NN) {
		CYCLES(4);
		PC += 2;
	}
	CYCLES(10);
	PC += 2;
}
static void chip8_5XY0(CHIP8* chip8) {
	// SE VX, VY
	if (VX == VY) {
		CYCLES(4);
		PC += 2;
	}
	CYCLES(14);
	PC += 2;
}
static void chip8_6XNN(CHIP8* chip8) {
	// LD VX, NN
	VX = NN;
	CYCLES(6);
	PC += 2;
}
static void chip8_7XNN(CHIP8* chip8) {
	// ADD VX, NN
	VX += NN;
	CYCLES(10);
	PC += 2;
}
static void chip8_8XY0(CHIP8* chip8) {
	// LD VX, VY
	VX = VY;
	CYCLES(12);
	PC += 2;
}
static void chip8_8XY1(CHIP8* chip8) {
	// OR VX, VY
	VX |= VY;
	if (chip8->quirks & CHIP8_QUIRK_ZERO_VF_REGISTER) {
		VF = 0;
	}
	CYCLES(44);
	PC += 2;
}
static void chip8_8XY2(CHIP8* chip8) {
	// AND VX, VY
	VX &= VY;
	if (chip8->quirks & CHIP8_QUIRK_ZERO_VF_REGISTER) {
		VF = 0;
	}
	CYCLES(44);
	PC += 2;
}
static void chip8_8XY3(CHIP8* chip8) {
	// XOR VX, VY
	VX ^= VY;
	if (chip8->quirks & CHIP8_QUIRK_ZERO_VF_REGISTER) {
		VF = 0;
	}
	CYCLES(44);
	PC += 2;
}
static void chip8_8XY4(CHIP8* chip8) {
	// ADD VX, VY ; set VF = carry.
	uint16_t r = VX + VY;
	VX = r & 0xFF;
	VF = r > 0xFF ? 0x1 : 0x0;
	CYCLES(44);
	PC += 2;
}
static void chip8_8XY5(CHIP8* chip8) {
	// SUB VX, VY
	uint8_t vf = VX < VY ? 0x0 : 0x1;
	VX = VX - VY;
	VF = vf;
	CYCLES(44);
	PC += 2;
}
static void chip8_8XY6(CHIP8* chip8) {
	// SHR VX, VY
	uint8_t vf = VX & 0x1;
	if (chip8->quirks & CHIP8_QUIRK_SHIFT_X_REGISTER) {
		VX >>= 1;
	}
	else {
		VX = VY >> 0x1;
	}
	VF = vf;
	CYCLES(44);
	PC += 2;
}
static void chip8_8XY7(CHIP8* chip8) {
	// SUBN VX, VY
	uint8_t vf = VY < VX ? 0x0 : 0x1;
	VX = VY - VX;
	VF = vf;
	CYCLES(44);
	PC += 2;
}
static void chip8_8XYE(CHIP8* chip8) {
	// SHL VX, VY
	uint8_t vf = (VX >> 7) & 0x1;
	if (chip8->quirks & CHIP8_QUIRK_SHIFT_X_REGISTER) {
		VX <<= 1;
	}
	else {
		VX = VY << 0x1;
	}
	VF = vf;
	CYCLES(44);
	PC += 2;
}
static void chip8_9XY0(CHIP8* chip8) {
	// SNE VX, VY
	if (VX != VY) {
		PC += 2;
		CYCLES(4);
	}
	CYCLES(14);
	PC += 2;
}
static void chip8_ANNN(CHIP8* chip8) {
	// LD I, NNN
	I = NNN;
	CYCLES(12);
	PC += 2;
}
static void chip8_BNNN(CHIP8* chip8) {
	// JMP NNN
	if (chip8->quirks & CHIP8_QUIRK_JUMP_VX) {
		// JMP NNN + VX
		PC = NNN + chip8->v[X];
	}
	else {
		// JMP NNN + V0
		PC = NNN + chip8->v[0];
	}

	CYCLES(24); // on page boundary crossed
	//CYCLES(22); // on not page boundary crossed
}
static void chip8_CXNN(CHIP8* chip8) {
	// RND VX, NN
	VX = (chip8_random() & NN);
	CYCLES(36);
	PC += 2;
}
static void chip8_DXYN(CHIP8* chip8) {
	// DRW VX, VY, N
	uint16_t i, vx, vy;
	VF = 0;
	for (int y = 0; y < N; ++y) {
		for (int x = 0; x < 8; ++x) {
			if ((chip8->ram[I + y] & (0x80 >> x)) != 0) {
				if (chip8->quirks & CHIP8_QUIRK_DISPLAY_CLIPPING) {
					vx = (VX & (CHIP8_DISPLAY_WIDTH - 1)) + x;
					vy = (VY & (CHIP8_DISPLAY_HEIGHT - 1)) + y;
				}
				else {
					vx = ((VX + x) & (CHIP8_DISPLAY_WIDTH - 1));
					vy = ((VY + y) & (CHIP8_DISPLAY_HEIGHT - 1));
				}

				if (vx < CHIP8_DISPLAY_WIDTH && vy < CHIP8_DISPLAY_HEIGHT) {
					i = vx + vy * CHIP8_DISPLAY_WIDTH;
					if (CHIP8_DISPLAY_GET_PX(chip8->display, i)) {
						VF = 1;
					}
					CHIP8_DISPLAY_TOGGLE_PX(chip8->display, i);
				}
			}

		}
	}
	CYCLES(170);
	PC += 2;
}
static void chip8_EX9E(CHIP8* chip8) {
	// SKP VX
	if (CHIP8_KEYPAD_GET(chip8->keypad, VX) == 0x1) {
		PC += 2;
		CYCLES(4);
	}
	CYCLES(14);
	PC += 2;
}
static void chip8_EXA1(CHIP8* chip8) {
	// SKNP VX
	if (CHIP8_KEYPAD_GET(chip8->keypad, VX) == 0x0) {
		PC += 2;
		CYCLES(4);
	}
	CYCLES(14);
	PC += 2;
}
static void chip8_FX07(CHIP8* chip8) {
	// LD VX, DT
	VX = chip8->delay_timer;
	CYCLES(14);
	PC += 2;
}
static void chip8_FX0A(CHIP8* chip8) {
	// LD VX, KEY
	
	for (int i = 0; i < 16; ++i) {
		/* Set key state */
		if (CHIP8_KEYPAD_GET(chip8->keypad, i) == CHIP8_KEY_STATE_KEY_DOWN) {
			CHIP8_KEYPAD_SET(chip8->fxoa_state, i, CHIP8_KEY_STATE_KEY_DOWN);
		}

		/* Check key state */
		if (CHIP8_KEYPAD_GET(chip8->fxoa_state, i) == CHIP8_KEY_STATE_KEY_DOWN) {
			if (CHIP8_KEYPAD_GET(chip8->keypad, i) == CHIP8_KEY_STATE_KEY_UP) {
				VX = i;
				PC += 2;

				/* Reset FX0A state */
				chip8->fxoa_state = 0;
				break;
			}
		}
	}

	//CYCLES(FXXX_CYCLES);
	//CYCLES(18900);
}
static void chip8_FX15(CHIP8* chip8) {
	// LD DT, VX
	chip8->delay_timer = VX;
	CYCLES(14);
	PC += 2;
}
static void chip8_FX18(CHIP8* chip8) {
	// LD ST, VX
	chip8->sound_timer = VX;
	CYCLES(10);
	PC += 2;
}
static void chip8_FX1E(CHIP8* chip8) {
	// ADD I, VX
	I += VX;
	CYCLES(16);
	PC += 2;
}
static void chip8_FX29(CHIP8* chip8) {
	// LD B, VX
	I = VX * 5;
	CYCLES(20);
	PC += 2;
}
static void chip8_FX33(CHIP8* chip8) {
	// LD B, VX
	uint8_t a, b, c;
	a = (VX % 1000) / 100;
	b = (VX % 100) / 10;
	c = (VX % 10);
	WRITE_BYTE(I, a);
	WRITE_BYTE(I + 1, b);
	WRITE_BYTE(I + 2, c);
	CYCLES(80+(a+b+c)*16);
	PC += 2;
} 
static void chip8_FX55(CHIP8* chip8) {
	//LD [I], VX
	for (int i = 0; i <= X; ++i) {
		WRITE_BYTE(I + i, chip8->v[i]);
		CYCLES(14);
	}

	if (chip8->quirks & CHIP8_QUIRK_INCREMENT_I_REGISTER) {
		I += X + 1;
	}

	CYCLES(18);
	PC += 2;
}
static void chip8_FX65(CHIP8* chip8) {
	// LD VX, [I]
	for (int i = 0; i <= X; ++i) {
		chip8->v[i] = READ_BYTE(I + i);
		CYCLES(14);
	}

	if (chip8->quirks & CHIP8_QUIRK_INCREMENT_I_REGISTER) {
		I += X + 1;
	}

	CYCLES(18);
	PC += 2;
}

void chip8_init_cpu(CHIP8* chip8) {

	chip8->quirks = 0;
	chip8_reset_cpu(chip8);
	chip8_zero_memory(chip8);
	chip8_zero_video_memory(chip8);
	chip8_load_font(chip8, chip8_font);
}
void chip8_reset_cpu(CHIP8* chip8) {

	I = 0;
	SP = 0;
	PC = CHIP8_PROGRAM_ADDR;

	chip8->opcode = 0;
	chip8->cpu_state = CHIP8_STATE_EXE;

	for (int i = 0; i < CHIP8_REGISTER_COUNT; ++i) {
		chip8->v[i] = 0;
	}

	for (int i = 0; i < CHIP8_STACK_SIZE; ++i) {
		chip8->stack[i] = 0;
	}

	chip8->keypad = 0;
	chip8->fxoa_state = 0;

	chip8->delay_timer = 0;
	chip8->sound_timer = 0;

	if (chip8->quirks & CHIP8_QUIRK_CLS_ON_RESET) {
		chip8_zero_video_memory(chip8);
	}

#ifdef CYCLE_COUNT
	chip8->cycles = 0;
#endif
}

void chip8_zero_memory(CHIP8* chip8) {
	for (int i = 0; i < CHIP8_MEMORY_BYTES; ++i) {
		chip8->ram[i] = 0;
	}
}
void chip8_zero_program_memory(CHIP8* chip8) {
	for (int i = CHIP8_PROGRAM_ADDR; i < CHIP8_MEMORY_BYTES - CHIP8_PROGRAM_ADDR; ++i) {
		chip8->ram[i] = 0;
	}
}
void chip8_zero_video_memory(CHIP8* chip8) {
	for (int i = 0; i < CHIP8_DISPLAY_BYTES; ++i) {
		chip8->display[i] = 0;
	}
}
void chip8_load_font(CHIP8* chip8, const uint8_t* font) {
	for (int i = 0; i < CHIP8_FONT_BYTES; ++i) {
		chip8->ram[i] = font[i];
	}
}
void chip8_step_timers(CHIP8* chip8) {

	if (chip8->delay_timer > 0) {
		chip8->delay_timer -= 1;
	}

	if (chip8->sound_timer > 0) {
		chip8->sound_timer -= 1;
		chip8_beep(chip8);
	}
}

void chip8_execute(CHIP8* chip8) {
	/* Decode and execute the next instruction */

	chip8->opcode = GET_OPCODE(chip8->pc); // chip8 is big endian

#ifdef CYCLE_COUNT
	chip8->cycles = 0;
#endif

	switch (chip8->opcode >> 12) {

		case 0x0: {
			switch (chip8->opcode & 0x00FF) {

				case 0xE0: // CLS
					chip8_00E0(chip8);
					break;
				case 0xEE: // RET
					chip8_00EE(chip8);
					break;
				default:
					chip8->cpu_state = CHIP8_STATE_ERROR_OPCODE;
					break;
			}
		} break;

		case 0x1: // JMP NNN
			chip8_1NNN(chip8);
			break;
		case 0x2: // CALL NNN
			chip8_2NNN(chip8);
			break;
		case 0x3: // SE VX, NN
			chip8_3XNN(chip8);
			break;
		case 0x4: // SNE VX, NN
			chip8_4XNN(chip8);
			break;
		case 0x5: // SE VX, VY
			chip8_5XY0(chip8);
			break;
		case 0x6: // LD VX, NN
			chip8_6XNN(chip8);
			break;
		case 0x7: // ADD VX, NN
			chip8_7XNN(chip8);
			break;

		case 0x8: {
			switch (chip8->opcode & 0x000F) {

				case 0x00: // LD VX, VY
					chip8_8XY0(chip8);
					break;
				case 0x01: // OR VX, VY
					chip8_8XY1(chip8);
					break;
				case 0x02: // AND VX, VY
					chip8_8XY2(chip8);
					break;
				case 0x03: // XOR VX, VY
					chip8_8XY3(chip8);
					break;
				case 0x04: // ADD VX, VY
					chip8_8XY4(chip8);
					break;
				case 0x05: // SUB VX, VY
					chip8_8XY5(chip8);
					break;
				case 0x06: // SHL VX, VY
					chip8_8XY6(chip8);
					break;
				case 0x07: // SUBN VX, VY
					chip8_8XY7(chip8);
					break;
				case 0x0E: // SHR VX, VY
					chip8_8XYE(chip8);
					break;
				default:
					chip8->cpu_state = CHIP8_STATE_ERROR_OPCODE;
					break;
			}
		} break;

		case 0x9: // SNE VX, VY
			chip8_9XY0(chip8);
			break;
		case 0xA: // LD I, NNN 
			chip8_ANNN(chip8);
			break;
		case 0xB: // JMP NNN, V0
			chip8_BNNN(chip8);
			break;
		case 0xC: // RND VX, NN
			chip8_CXNN(chip8);
			break;
		case 0xD: // DSP VX, VY, N
			chip8_DXYN(chip8);
			break;

		case 0xE: {
			switch (chip8->opcode & 0x00FF) {

				case 0x9E: // SKP VX
					chip8_EX9E(chip8);
					break;
				case 0xA1: // SKNP VX 
					chip8_EXA1(chip8);
					break;
				default:
					chip8->cpu_state = CHIP8_STATE_ERROR_OPCODE;
					break;
			}
		} break;

		case 0xF: {
			switch (chip8->opcode & 0x00FF) {

				case 0x07: // LD VX, DT
					chip8_FX07(chip8);
					break;
				case 0x0A: // LD VX, KEY
					chip8_FX0A(chip8);
					break;
				case 0x15: // LD DT, VX
					chip8_FX15(chip8);
					break;
				case 0x18: // LD ST, VX
					chip8_FX18(chip8);
					break;
				case 0x1E: // ADD I, VX
					chip8_FX1E(chip8);
					break;
				case 0x29: // LD F, VX 
					chip8_FX29(chip8);
					break;
				case 0x33: // LD B, VX
					chip8_FX33(chip8);
					break;
				case 0x55: // LD [I], VX 
					chip8_FX55(chip8);
					break;
				case 0x65: // LD VX, [I] 
					chip8_FX65(chip8);
					break;
				default:
					chip8->cpu_state = CHIP8_STATE_ERROR_OPCODE;
					break;
			}
		} break;

		default:
			chip8->cpu_state = CHIP8_STATE_ERROR_OPCODE;
			break;
	}
}

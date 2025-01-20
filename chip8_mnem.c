// chip8_mnem.c
//
// GitHub: https:\\github.com\tommojphillips

#include <stdio.h>

#include "chip8_defines.h"
#include "chip8.h"

#ifdef CHIP8_MNEMONICS

#include "chip8_mnem.h"

#define X ((opcode >> 8) & 0x0F)
#define Y ((opcode >> 4) & 0x00F)
#define NNN (opcode & 0x0FFF)
#define NN (opcode & 0x00FF)
#define N (opcode & 0x000F)

#define VX chip8->v[X]
#define VY chip8->v[Y]
#define VF chip8->v[0xF]
#define I chip8->i
#define PC chip8->pc
#define SP chip8->sp

#define MNEM_0(x) sprintf_s(str, 32, x)
#define MNEM_1(x, x1) sprintf_s(str, 32, x, x1)
#define MNEM_2(x, x1, x2) sprintf_s(str, 32, x, x1, x2)
#define MNEM_3(x, x1, x2, x3) sprintf_s(str, 32, x, x1, x2, x3)

void chip8_mnem_00E0(uint16_t opcode, char* str) {
	// CLS
	MNEM_0("CLS");
}
void chip8_mnem_00EE(uint16_t opcode, char* str) {
	// RET
	MNEM_0("RET");
}
void chip8_mnem_1NNN(uint16_t opcode, char* str) {
	// JMP NNN
	MNEM_1("JMP 0x%x", NNN);
}
void chip8_mnem_2NNN(uint16_t opcode, char* str) {
	// CALL NNN
	MNEM_1("CALL 0x%x", NNN);
}
void chip8_mnem_3XNN(uint16_t opcode, char* str) {
	// SE VX, NN
	MNEM_2("SE V%X, 0x%x", X, NN);
}
void chip8_mnem_4XNN(uint16_t opcode, char* str) {
	// SNE VX, NN
	MNEM_2("SNE V%X, 0x%x", X, NN);
}
void chip8_mnem_5XY0(uint16_t opcode, char* str) {
	// SE VX, VY
	MNEM_2("SE V%X, V%X", X, Y);
}
void chip8_mnem_6XNN(uint16_t opcode, char* str) {
	// LD VX, NN
	MNEM_2("LD V%X, 0x%x", X, NN);
}
void chip8_mnem_7XNN(uint16_t opcode, char* str) {
	// ADD VX, NN
	MNEM_2("ADD V%X, 0x%x", X, NN);
}
void chip8_mnem_8XY0(uint16_t opcode, char* str) {
	// LD VX, VY
	MNEM_2("LD V%X, V%X", X, Y);
}
void chip8_mnem_8XY1(uint16_t opcode, char* str) {
	// VX=VX│VY
	MNEM_2("OR V%X, V%X", X, Y);
}
void chip8_mnem_8XY2(uint16_t opcode, char* str) {
	// VX=VX&VY
	MNEM_2("AND V%X, V%X", X, Y);
}
void chip8_mnem_8XY3(uint16_t opcode, char* str) {
	// VX=VX^VY
	MNEM_2("XOR V%X, V%X", X, Y);
}
void chip8_mnem_8XY4(uint16_t opcode, char* str) {
	// Set Vx = Vx + Vy, set VF = carry.
	MNEM_2("ADD V%X, V%X", X, Y);
}
void chip8_mnem_8XY5(uint16_t opcode, char* str) {
	// SUB VX, VY	
	MNEM_2("SUB V%X, V%X", X, Y);
}
void chip8_mnem_8XY6(uint16_t opcode, char* str) {
	// SHR VX
	MNEM_1("SHR V%X", X);
}
void chip8_mnem_8XY7(uint16_t opcode, char* str) {
	// SUBN VX, VY
	MNEM_2("SUBN V%X, V%X", X, Y);
}
void chip8_mnem_8XYE(uint16_t opcode, char* str) {
	// SHL VX
	MNEM_1("SHL V%X", X);
}
void chip8_mnem_9XY0(uint16_t opcode, char* str) {
	// SNE VX, VY
	MNEM_2("SNE V%X, V%X", X, Y);
}
void chip8_mnem_ANNN(uint16_t opcode, char* str) {
	// LD I, NNN
	MNEM_1("LD I, 0x%x", NNN);
}
void chip8_mnem_BNNN(uint16_t opcode, char* str) {
	// JMP NNN, V0
	MNEM_1("JMP 0x%x, V0", NNN);
}
void chip8_mnem_CXNN(uint16_t opcode, char* str) {
	// RND VX, NN
	MNEM_2("RND V%X, 0x%x", X, NN);
}
void chip8_mnem_DXYN(uint16_t opcode, char* str) {
	// DRW VX, VY, N
	MNEM_3("DRW V%X, V%X, 0x%x", X, Y, N);
}
void chip8_mnem_EX9E(uint16_t opcode, char* str) {
	// SKP VX
	MNEM_1("SKP V%X", X);
}
void chip8_mnem_EXA1(uint16_t opcode, char* str) {
	// SKNP VX
	MNEM_1("SKNP V%X", X);
}
void chip8_mnem_FX07(uint16_t opcode, char* str) {
	// LD VX, DT
	MNEM_1("LD V%X, DT", X);
}
void chip8_mnem_FX0A(uint16_t opcode, char* str) {
	// LD VX, KEY
	MNEM_1("LD V%X, KEY", X);
}
void chip8_mnem_FX15(uint16_t opcode, char* str) {
	// LD DT, VX
	MNEM_1("LD DT, V%X", X);
}
void chip8_mnem_FX18(uint16_t opcode, char* str) {
	// LD ST, VX
	MNEM_1("LD ST, V%X", X);
}
void chip8_mnem_FX1E(uint16_t opcode, char* str) {
	// ADD I, VX
	MNEM_1("ADD I, V%X", X);
}
void chip8_mnem_FX29(uint16_t opcode, char* str) {
	// LD B, VX
	MNEM_1("LD F, V%X", X);
}
void chip8_mnem_FX33(uint16_t opcode, char* str) {
	// LD B, VX
	MNEM_1("LD B, V%X", X);
}
void chip8_mnem_FX55(uint16_t opcode, char* str) {
	//LD [I], VX
	MNEM_1("LD [I], V%X", X);
}
void chip8_mnem_FX65(uint16_t opcode, char* str) {
	// LD VX, [I]
	MNEM_1("LD V%X, [I]", X);
}

void chip8_mnem_find_next(CHIP8* chip8, uint16_t* pc) {

	uint16_t opcode = GET_OPCODE(PC);
	*pc = PC;

	switch (opcode >> 12) {

		case 0x0: {
			switch (opcode & 0x00FF) {

				case 0xEE: // RET
					*pc = chip8->stack[SP];
					break;

				default:
					*pc += 2;
					break;
			}
		} break;

		case 0x1: // JMP NNN
			*pc = NNN;
			break;

		case 0x2: // CALL NNN
			*pc = NNN;
			break;

		case 0x3: // SE VX, NN
			if (VX == NN)
				*pc += 2;
			*pc += 2;
			break;

		case 0x4: // SNE VX, NN
			if (VX != NN)
				*pc += 2;
			*pc += 2;
			break;

		case 0x5: // SE VX, VY
			if (VX == VY)
				*pc += 2;
			*pc += 2;
			break;

		case 0x9: // SNE VX, VY
			if (VX != VY)
				*pc += 2;
			*pc += 2;
			break;

		case 0xB: // JMP NNN, V0
			if (chip8->quirks & CHIP8_QUIRK_JUMP_VX) {
				// JMP NNN + VX
				*pc = NNN + VX;
			}
			else {
				// JMP NNN + V0
				*pc = NNN + chip8->v[0];
			}
			break;

		case 0xE: {
			switch (opcode & 0x00FF) {

				case 0x9E: // SKP VX
					if (CHIP8_KEYPAD_GET(chip8->keypad, VX) == 1) {
						*pc += 2;
					}
					*pc += 2;
					break;

				case 0xA1: // SKNP VX 
					if (CHIP8_KEYPAD_GET(chip8->keypad, VX) == 0) {
						*pc += 2;
					}
					*pc += 2;
					break;

				default:
					*pc += 2; 
					break;
			}
		} break;

		default:
			*pc += 2; 
			break;
	}
}

int chip8_mnem(CHIP8* chip8, uint16_t pc, char* str) {

	if (pc == 0) {
		pc = PC;
	}
	
	uint16_t opcode = GET_OPCODE(pc);
	
	str[0] = '\0';

	switch (opcode >> 12) {

		case 0x0: {
			switch (opcode & 0x00FF) {

				case 0xE0: // CLS
					chip8_mnem_00E0(opcode, str);
					break;
				case 0xEE: // RET
					chip8_mnem_00EE(opcode, str);
					break;
				default:
					return 1;
			}
		} break;

		case 0x1: // JMP NNN
			chip8_mnem_1NNN(opcode, str);
			break;

		case 0x2: // CALL NNN
			chip8_mnem_2NNN(opcode, str);
			break;

		case 0x3: // SE VX, NN
			chip8_mnem_3XNN(opcode, str);
			break;

		case 0x4: // SNE VX, NN
			chip8_mnem_4XNN(opcode, str);
			break;

		case 0x5: // SE VX, VY
			chip8_mnem_5XY0(opcode, str);
			break;

		case 0x6: // LD VX, NN
			chip8_mnem_6XNN(opcode, str);
			break;

		case 0x7: // ADD VX, NN
			chip8_mnem_7XNN(opcode, str);
			break;

		case 0x8: {
			switch (opcode & 0x000F) {

				case 0x00: // LD VX, VY
					chip8_mnem_8XY0(opcode, str);
					break;
				case 0x01: // OR VX, VY
					chip8_mnem_8XY1(opcode, str);
					break;
				case 0x02: // AND VX, VY
					chip8_mnem_8XY2(opcode, str);
					break;
				case 0x03: // XOR VX, VY
					chip8_mnem_8XY3(opcode, str);
					break;
				case 0x04: // ADD VX, VY
					chip8_mnem_8XY4(opcode, str);
					break;
				case 0x05: // SUB VX, VY
					chip8_mnem_8XY5(opcode, str);
					break;
				case 0x06: // SHL VX, VY
					chip8_mnem_8XY6(opcode, str);
					break;
				case 0x07: // SUBN VX, VY
					chip8_mnem_8XY7(opcode, str);
					break;
				case 0x0E: // SHR VX, VY
					chip8_mnem_8XYE(opcode, str);
					break;
				default:
					return 1;
			}
		} break;

		case 0x9: // SNE VX, VY
			chip8_mnem_9XY0(opcode, str);
			break;
		case 0xA: // LD I, NNN 
			chip8_mnem_ANNN(opcode, str);
			break;
		case 0xB: // JMP NNN, V0
			chip8_mnem_BNNN(opcode, str);
			break;
		case 0xC: // RND VX, NN
			chip8_mnem_CXNN(opcode, str);
			break;
		case 0xD: // DSP VX, VY, N
			chip8_mnem_DXYN(opcode, str);
			break;

		case 0xE: {
			switch (opcode & 0x00FF) {

				case 0x9E: // SKP VX
					chip8_mnem_EX9E(opcode, str);
					break;
				case 0xA1: // SKNP VX 
					chip8_mnem_EXA1(opcode, str);
					break;
				default:
					return 1;
			}
		} break;

		case 0xF: {
			switch (opcode & 0x00FF) {

				case 0x07: // LD VX, DT
					chip8_mnem_FX07(opcode, str);
					break;
				case 0x0A: // LD VX, KEY
					chip8_mnem_FX0A(opcode, str);
					break;
				case 0x15: // LD DT, VX
					chip8_mnem_FX15(opcode, str);
					break;
				case 0x18: // LD ST, VX
					chip8_mnem_FX18(opcode, str);
					break;
				case 0x1E: // ADD I, VX
					chip8_mnem_FX1E(opcode, str);
					break;
				case 0x29: // LD F, VX 
					chip8_mnem_FX29(opcode, str);
					break;
				case 0x33: // LD B, VX
					chip8_mnem_FX33(opcode, str);
					break;
				case 0x55: // LD [I], VX 
					chip8_mnem_FX55(opcode, str);
					break;
				case 0x65: // LD VX, [I] 
					chip8_mnem_FX65(opcode, str);
					break;
				default:
					return 1;
			}
		} break;

		default:
			return 1;
	}

	return 0;
}
#endif

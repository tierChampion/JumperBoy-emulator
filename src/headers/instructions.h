#pragma once

#include <headers/common.h>

// Opcodes: https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html

namespace jmpr {

	/**
	* Type of the instruction.
	*/
	enum class InstrType {
		NONE,		// Unknown Instruction
		NOP,		// No operation
		LD,			// Load
		INC,		// Increment
		DEC,		// Decrement
		RLCA,		// ?
		ADD,		// Addition
		RRCA,		// ?
		STOP,		// ?
		RLA,		// ?
		JR,			// Jump ?
		RRA,		// ?
		DAA,		// ?
		CPL,		// ?
		SCF,		// ?
		CCF,		// ?
		HALT,		// Halt
		ADC,		// Addition ?
		SUB,		// Substraction
		SBC,		// Sub ?
		AND,		// Logical AND
		XOR,		// Logical XOR
		OR,			// Logical OR
		CP,			// ?
		RET,		// Return from a subroutine
		POP,		// Pop from the stack ?
		JP,			// Jump ?
		CALL,		// Call a subroutine ?
		PUSH,		// Push on the stack ?
		RST,		// Reset ?
		PREFIX,		// Call a CB
		RETI,		// ?
		// JPHL?
		LDH,		// ?
		DI,			// ?
		EI,			// ?
		CB_ERR,		// Unknown CB Instruction
		CB_RLC,		// ?
		CB_RRC,		// ?
		CB_RL,		// ?
		CB_RR,		// ?
		CB_SLA,		// ?
		CB_SRA,		// ?
		CB_SWAP,	// ?
		CB_SRL,		// ?
		CB_BIT,		// ?
		CB_RES,		// ?
		CB_SET,		// ?
	};

	/**
	* Addressing mode of bits used.
	*
	* REG =				Register
	* REGADD =			Address pointed to by register
	* D8, D16 =			1 or 2 bytes following the instruction
	* A8, A16 =			Address pointed to by the 1 or 2 bytes following the instruction
	* R8 =				1 byte signed integer following the instruction
	* HLINC, HLDEC =	Use the HL register and either increment or decrement it afterwards
	* HL_SPR8 =			HL register and stack at location SP + r8
	*/
	enum class Mode {
		NONE,
		REG_D16,
		REGADD_REG,
		REG,
		REG_D8,
		A16_REG,
		REG_REG,
		REG_REGADD,
		R8,
		HLINC_REG,
		REG_HLINC,
		HLDEC_REG,
		REGADD,
		REGADD_D8,
		REG_HLDEC,
		A16,
		D8,
		A8_REG,
		REG_R8,
		REG_A8,
		HL_SPR8,
		REG_A16,
		// D16
		// D16 Reg
	};

	/**
	* Possible registers referenced in the instruction.
	*/
	enum class Register {
		NONE,
		A,
		F,
		B,
		C,
		D,
		E,
		H,
		L,
		AF,
		BC,
		DE,
		HL,
		SP,
		PC,
	};

	/**
	* Possible conditions for an instruction to execute.
	*/
	enum class Condition {
		NONE,	// No conditions
		Z,		// Zero Flag high
		NZ,		// Zero Flag low
		C,		// Carry Flag high
		NC		// Carry Flag low
	};

	struct Instruction {

		InstrType _type;
		Condition _cond;

		Mode _mode;
		Register _reg1;
		Register _reg2;

		u8 _length;
		u8 _cycles;
		u8 _extra_cycles;

		u8 _affected_flags;

	};
}
#pragma once

#include <common.h>

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
		JP,			// Jump
		CALL,		// Call a subroutine ?
		PUSH,		// Push on the stack ?
		RST,		// Reset ?
		PREFIX,		// Call a CB
		RETI,		// ?
		LDH,		// Loading with 8 bit values and the Accumulator
		DI,			// Disable interrupts
		EI,			// Enable interrupts
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
	* Addressing mode of bits used. If two components are referenced, it means that the first
	* is where the result is stored and the second is the data.
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
		HLINC_REG,
		REG_HLINC,
		HLDEC_REG,
		REGADD,
		REGADD_D8,
		REG_HLDEC,
		A16,
		D8,
		A8_REG,
		REG_A8,
		HL_SPR8,
		REG_A16,
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
		NONE,
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

		u8 _affected_flags;
	};

	bool is16Bits(Register reg);

	const Instruction* fromOpcode(u8 opcode);
	const char* instructionName(const Instruction instr);
}

// Hashing function for the instruction type.
namespace std {
	template<>
	struct hash<jmpr::InstrType> {
		std::size_t operator()(const jmpr::InstrType& it) const {
			return static_cast<std::size_t>(it);
		}
	};
}


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
		RLCA,		// Rotate bits left
		ADD,		// Addition
		RRCA,		// Rotate bits right
		STOP,		// Stop CPU until joypad interrupt
		RLA,		// Rotate bits left through carry
		JR,			// Jump to a relative address
		RRA,		// Rotate bits right through carry
		DAA,		// Decimal adjust after addition for BCD addition / substraction
		CPL,		// Negate the accumulator
		SCF,		// Set the carry flag
		CCF,		// Negate the carry flag
		HALT,		// Stop CPU until next interrupt
		ADC,		// Addition with carry
		SUB,		// Substraction
		SBC,		// Substraction with carry
		AND,		// Logical AND
		XOR,		// Logical XOR
		OR,			// Logical OR
		CP,			// Logical XNOR (Compare)
		RET,		// Return from a function
		POP,		// Pop from the stack
		JP,			// Jump
		CALL,		// Call a function
		PUSH,		// Push on the stack
		RST,		// Reset the gameboy
		PREFIX,		// Call a CB
		RETI,		// Return from a function and enable interrupts
		LDH,		// Load to the HRAM 
		DI,			// Disable interrupts
		EI,			// Enable interrupts

		BIT_ERR,		// Unknown BIT Instruction
		BIT_RLC,		// Rotate left
		BIT_RRC,		// Rotate right
		BIT_RL,		// Rotate left through carry
		BIT_RR,		// Rotate right through carry
		BIT_SLA,		// Shift left
		BIT_SRA,		// Shift right
		BIT_SWAP,	// Exchange the low and high nibbles
		BIT_SRL,		// Shift right logical
		BIT_BIT,		// Test bit
		BIT_RES,		// Reset bit
		BIT_SET,		// Set bit
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
		D16,
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

		friend std::ostream& operator<<(std::ostream& os, const Instruction& instruction);
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


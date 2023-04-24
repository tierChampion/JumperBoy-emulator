#include <instructions.h>

#include <unordered_map>
#include <array>

namespace jmpr {

	// Opcodes: https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
	static const std::unordered_map<u16, Instruction> INSTRUCTIONS_FROM_OPCODES = {
		// HEX		TYPE	CONDITION	ADDR_MODE	REG1	REG2	FLAGS
		{0x00, {InstrType::NOP, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
		{0x01, {InstrType::LD, Condition::NONE, Mode::REG_D16, Register::BC, Register::NONE, 0}},

		{0x05, {InstrType::DEC, Condition::NONE, Mode::REG, Register::B, Register::NONE, 0b1110000}},

		{0x0E, {InstrType::LD, Condition::NONE, Mode::REG_D8, Register::C, Register::NONE, 0}},

		{0xAF, {InstrType::XOR, Condition::NONE, Mode::REG, Register::A, Register::NONE, 0b1000000}},

		{0xC3, {InstrType::JP, Condition::NONE, Mode::A16, Register::NONE, Register::NONE, 0}},

		{0xF3, {InstrType::DI, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},

		// Default "wrong" opcode
		{0x100, { InstrType::NONE, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0 }},
	};

	/**
	* Get the right instruction associated with the opcode.
	* @param opcode Opcode to translate to an instruction
	*/
	const Instruction* fromOpcode(u8 opcode) {

		if (auto search = INSTRUCTIONS_FROM_OPCODES.find(opcode); search != INSTRUCTIONS_FROM_OPCODES.end()) {
			return &(search->second);
		}

		return &(INSTRUCTIONS_FROM_OPCODES.at(0x100));
	}


	// Lookup for the names
	static const std::array<const char*, 47> NAME_LOOKUP = {
		"NONE",
		"NOP",
		"LD",
		"INC",
		"DEC",
		"RLCA",
		"ADD",
		"RRCA",
		"STOP",
		"RLA",
		"JR",
		"RRA",
		"DAA",
		"CPL",
		"SCF",
		"CCF",
		"HALT",
		"ADC",
		"SUB",
		"SBC",
		"AND",
		"XOR",
		"OR",
		"CP",
		"RET",
		"POP",
		"JP",
		"CALL",
		"PUSH",
		"RST",
		"PREFIX",
		"RETI",
		// JPHL?
		"LDH",
		"DI",
		"EI",
		"CB_ERR",
		"CB_RLC",
		"CB_RRC",
		"CB_RL",
		"CB_RR",
		"CB_SLA",
		"CB_SRA",
		"CB_SWAP",
		"CB_SRL",
		"CB_BIT",
		"CB_RES",
		"CB_SET"
	};

	const char* instructionName(const Instruction instr) {
		return NAME_LOOKUP[int(instr._type)];
	}
}
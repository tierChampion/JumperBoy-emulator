#include <instructions.h>
#include <unordered_map>

namespace jmpr {

	// Opcodes: https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
	static const std::unordered_map<u16, Instruction> INSTRUCTIONS_FROM_OPCODES = {
		// HEX		TYPE	CONDITION	ADDR_MODE	REG1	REG1	LENGTH	CYC	EXT_CYC	FLAGS
		{0x00, {InstrType::NOP, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 1, 4, 0, 0}},
		{0x01, {InstrType::LD, Condition::NONE, Mode::REG_D16, Register::BC, Register::NONE, 3, 12, 0, 0}},
		// TODO..

		// Default "wrong" opcode
		{0x100, { InstrType::NONE, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0, 0, 0, 0 }},
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
}
#include <cpu/instructions.h>

#include <cpu/cpu.h>

#include <unordered_map>
#include <array>

namespace jmpr
{

	bool is16Bits(Register reg)
	{
		return reg >= Register::AF;
	}

	/**
	 * Get the right instruction associated with the opcode.
	 * @param opcode Opcode to translate to an instruction
	 */
	const Instruction *fromOpcode(u8 opcode)
	{
		// Opcodes: https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
		static const std::unordered_map<u16, Instruction> INSTRUCTIONS_FROM_OPCODES = {
			// HEX		TYPE	CONDITION	ADDR_MODE	REG1	REG2	FLAGS
			// 0
			{0x00, {InstrType::NOP, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
			{0x01, {InstrType::LD, Condition::NONE, Mode::REG_D16, Register::BC, Register::NONE, 0}},
			{0x02, {InstrType::LD, Condition::NONE, Mode::REGADD_REG, Register::BC, Register::A, 0}},
			{0x03, {InstrType::INC, Condition::NONE, Mode::REG, Register::BC, Register::NONE, 0}},
			{0x04, {InstrType::INC, Condition::NONE, Mode::REG, Register::B, Register::NONE, 0b11100000}},
			{0x05, {InstrType::DEC, Condition::NONE, Mode::REG, Register::B, Register::NONE, 0b11100000}},
			{0x06, {InstrType::LD, Condition::NONE, Mode::REG_D8, Register::B, Register::NONE, 0}},
			{0x07, {InstrType::RLCA, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0b11110000}},
			{0x08, {InstrType::LD, Condition::NONE, Mode::A16_REG, Register::NONE, Register::SP, 0}},
			{0x09, {InstrType::ADD, Condition::NONE, Mode::REG_REG, Register::HL, Register::BC, 0b01110000}},
			{0x0A, {InstrType::LD, Condition::NONE, Mode::REG_REGADD, Register::A, Register::BC, 0}},
			{0x0B, {InstrType::DEC, Condition::NONE, Mode::REG, Register::BC, Register::NONE, 0}},
			{0x0C, {InstrType::INC, Condition::NONE, Mode::REG, Register::C, Register::NONE, 0b11100000}},
			{0x0D, {InstrType::DEC, Condition::NONE, Mode::REG, Register::C, Register::NONE, 0b11100000}},
			{0x0E, {InstrType::LD, Condition::NONE, Mode::REG_D8, Register::C, Register::NONE, 0}},
			{0x0F, {InstrType::RRCA, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0b11110000}},
			// 1
			{0x10, {InstrType::STOP, Condition::NONE, Mode::D8, Register::NONE, Register::NONE, 0}},
			{0x11, {InstrType::LD, Condition::NONE, Mode::REG_D16, Register::DE, Register::NONE, 0}},
			{0x12, {InstrType::LD, Condition::NONE, Mode::REGADD_REG, Register::DE, Register::A, 0}},
			{0x13, {InstrType::INC, Condition::NONE, Mode::REG, Register::DE, Register::NONE, 0}},
			{0x14, {InstrType::INC, Condition::NONE, Mode::REG, Register::D, Register::NONE, 0b11100000}},
			{0x15, {InstrType::DEC, Condition::NONE, Mode::REG, Register::D, Register::NONE, 0b11100000}},
			{0x16, {InstrType::LD, Condition::NONE, Mode::REG_D8, Register::D, Register::NONE, 0}},
			{0x17, {InstrType::RLA, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0b11110000}},
			{0x18, {InstrType::JR, Condition::NONE, Mode::D8, Register::NONE, Register::NONE, 0}},
			{0x19, {InstrType::ADD, Condition::NONE, Mode::REG_REG, Register::HL, Register::DE, 0b01110000}},
			{0x1A, {InstrType::LD, Condition::NONE, Mode::REG_REGADD, Register::A, Register::DE, 0}},
			{0x1B, {InstrType::DEC, Condition::NONE, Mode::REG, Register::DE, Register::NONE, 0}},
			{0x1C, {InstrType::INC, Condition::NONE, Mode::REG, Register::E, Register::NONE, 0b11100000}},
			{0x1D, {InstrType::DEC, Condition::NONE, Mode::REG, Register::E, Register::NONE, 0b11100000}},
			{0x1E, {InstrType::LD, Condition::NONE, Mode::REG_D8, Register::E, Register::NONE, 0}},
			{0x1F, {InstrType::RRA, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0b11110000}},
			// 2
			{0x20, {InstrType::JR, Condition::NZ, Mode::D8, Register::NONE, Register::NONE, 0}},
			{0x21, {InstrType::LD, Condition::NONE, Mode::REG_D16, Register::HL, Register::NONE, 0}},
			{0x22, {InstrType::LD, Condition::NONE, Mode::HLINC_REG, Register::HL, Register::A, 0}},
			{0x23, {InstrType::INC, Condition::NONE, Mode::REG, Register::HL, Register::NONE, 0}},
			{0x24, {InstrType::INC, Condition::NONE, Mode::REG, Register::H, Register::NONE, 0b11100000}},
			{0x25, {InstrType::DEC, Condition::NONE, Mode::REG, Register::H, Register::NONE, 0b11100000}},
			{0x26, {InstrType::LD, Condition::NONE, Mode::REG_D8, Register::H, Register::NONE, 0}},
			{0x27, {InstrType::DAA, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0b10110000}},
			{0x28, {InstrType::JR, Condition::Z, Mode::D8, Register::NONE, Register::NONE, 0}},
			{0x29, {InstrType::ADD, Condition::NONE, Mode::REG_REG, Register::HL, Register::HL, 0b01110000}},
			{0x2A, {InstrType::LD, Condition::NONE, Mode::REG_HLINC, Register::A, Register::HL, 0}},
			{0x2B, {InstrType::DEC, Condition::NONE, Mode::REG, Register::HL, Register::NONE, 0}},
			{0x2C, {InstrType::INC, Condition::NONE, Mode::REG, Register::L, Register::NONE, 0b11100000}},
			{0x2D, {InstrType::DEC, Condition::NONE, Mode::REG, Register::L, Register::NONE, 0b11100000}},
			{0x2E, {InstrType::LD, Condition::NONE, Mode::REG_D8, Register::L, Register::NONE, 0}},
			{0x2F, {InstrType::CPL, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0b01100000}},
			// 3
			{0x30, {InstrType::JR, Condition::NC, Mode::D8, Register::NONE, Register::NONE, 0}},
			{0x31, {InstrType::LD, Condition::NONE, Mode::REG_D16, Register::SP, Register::NONE, 0}},
			{0x32, {InstrType::LD, Condition::NONE, Mode::HLDEC_REG, Register::HL, Register::A, 0}},
			{0x33, {InstrType::INC, Condition::NONE, Mode::REG, Register::SP, Register::NONE, 0}},
			{0x34, {InstrType::INC, Condition::NONE, Mode::REGADD, Register::HL, Register::NONE, 0b11100000}},
			{0x35, {InstrType::DEC, Condition::NONE, Mode::REGADD, Register::HL, Register::NONE, 0b11100000}},
			{0x36, {InstrType::LD, Condition::NONE, Mode::REGADD_D8, Register::HL, Register::NONE, 0}},
			{0x37, {InstrType::SCF, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0b01110000}},
			{0x38, {InstrType::JR, Condition::C, Mode::D8, Register::NONE, Register::NONE, 0}},
			{0x39, {InstrType::ADD, Condition::NONE, Mode::REG_REG, Register::HL, Register::SP, 0b01110000}},
			{0x3A, {InstrType::LD, Condition::NONE, Mode::REG_HLDEC, Register::A, Register::HL, 0}},
			{0x3B, {InstrType::DEC, Condition::NONE, Mode::REG, Register::SP, Register::NONE, 0}},
			{0x3C, {InstrType::INC, Condition::NONE, Mode::REG, Register::A, Register::NONE, 0b11100000}},
			{0x3D, {InstrType::DEC, Condition::NONE, Mode::REG, Register::A, Register::NONE, 0b11100000}},
			{0x3E, {InstrType::LD, Condition::NONE, Mode::REG_D8, Register::A, Register::NONE, 0}},
			{0x3F, {InstrType::CCF, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0b01110000}},
			// 4
			{0x40, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::B, Register::B, 0}},
			{0x41, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::B, Register::C, 0}},
			{0x42, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::B, Register::D, 0}},
			{0x43, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::B, Register::E, 0}},
			{0x44, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::B, Register::H, 0}},
			{0x45, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::B, Register::L, 0}},
			{0x46, {InstrType::LD, Condition::NONE, Mode::REG_REGADD, Register::B, Register::HL, 0}},
			{0x47, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::B, Register::A, 0}},
			{0x48, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::C, Register::B, 0}},
			{0x49, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::C, Register::C, 0}},
			{0x4A, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::C, Register::D, 0}},
			{0x4B, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::C, Register::E, 0}},
			{0x4C, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::C, Register::H, 0}},
			{0x4D, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::C, Register::L, 0}},
			{0x4E, {InstrType::LD, Condition::NONE, Mode::REG_REGADD, Register::C, Register::HL, 0}},
			{0x4F, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::C, Register::A, 0}},
			// 5
			{0x50, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::D, Register::B, 0}},
			{0x51, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::D, Register::C, 0}},
			{0x52, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::D, Register::D, 0}},
			{0x53, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::D, Register::E, 0}},
			{0x54, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::D, Register::H, 0}},
			{0x55, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::D, Register::L, 0}},
			{0x56, {InstrType::LD, Condition::NONE, Mode::REG_REGADD, Register::D, Register::HL, 0}},
			{0x57, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::D, Register::A, 0}},
			{0x58, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::E, Register::B, 0}},
			{0x59, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::E, Register::C, 0}},
			{0x5A, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::E, Register::D, 0}},
			{0x5B, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::E, Register::E, 0}},
			{0x5C, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::E, Register::H, 0}},
			{0x5D, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::E, Register::L, 0}},
			{0x5E, {InstrType::LD, Condition::NONE, Mode::REG_REGADD, Register::E, Register::HL, 0}},
			{0x5F, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::E, Register::A, 0}},
			// 6
			{0x60, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::H, Register::B, 0}},
			{0x61, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::H, Register::C, 0}},
			{0x62, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::H, Register::D, 0}},
			{0x63, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::H, Register::E, 0}},
			{0x64, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::H, Register::H, 0}},
			{0x65, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::H, Register::L, 0}},
			{0x66, {InstrType::LD, Condition::NONE, Mode::REG_REGADD, Register::H, Register::HL, 0}},
			{0x67, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::H, Register::A, 0}},
			{0x68, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::L, Register::B, 0}},
			{0x69, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::L, Register::C, 0}},
			{0x6A, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::L, Register::D, 0}},
			{0x6B, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::L, Register::E, 0}},
			{0x6C, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::L, Register::H, 0}},
			{0x6D, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::L, Register::L, 0}},
			{0x6E, {InstrType::LD, Condition::NONE, Mode::REG_REGADD, Register::L, Register::HL, 0}},
			{0x6F, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::L, Register::A, 0}},
			// 7
			{0x70, {InstrType::LD, Condition::NONE, Mode::REGADD_REG, Register::HL, Register::B, 0}},
			{0x71, {InstrType::LD, Condition::NONE, Mode::REGADD_REG, Register::HL, Register::C, 0}},
			{0x72, {InstrType::LD, Condition::NONE, Mode::REGADD_REG, Register::HL, Register::D, 0}},
			{0x73, {InstrType::LD, Condition::NONE, Mode::REGADD_REG, Register::HL, Register::E, 0}},
			{0x74, {InstrType::LD, Condition::NONE, Mode::REGADD_REG, Register::HL, Register::H, 0}},
			{0x75, {InstrType::LD, Condition::NONE, Mode::REGADD_REG, Register::HL, Register::L, 0}},
			{0x76, {InstrType::HALT, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
			{0x77, {InstrType::LD, Condition::NONE, Mode::REGADD_REG, Register::HL, Register::A, 0}},
			{0x78, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::A, Register::B, 0}},
			{0x79, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::A, Register::C, 0}},
			{0x7A, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::A, Register::D, 0}},
			{0x7B, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::A, Register::E, 0}},
			{0x7C, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::A, Register::H, 0}},
			{0x7D, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::A, Register::L, 0}},
			{0x7E, {InstrType::LD, Condition::NONE, Mode::REG_REGADD, Register::A, Register::HL, 0}},
			{0x7F, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::A, Register::A, 0}},
			// 8
			{0x80, {InstrType::ADD, Condition::NONE, Mode::REG_REG, Register::A, Register::B, 0b11110000}},
			{0x81, {InstrType::ADD, Condition::NONE, Mode::REG_REG, Register::A, Register::C, 0b11110000}},
			{0x82, {InstrType::ADD, Condition::NONE, Mode::REG_REG, Register::A, Register::D, 0b11110000}},
			{0x83, {InstrType::ADD, Condition::NONE, Mode::REG_REG, Register::A, Register::E, 0b11110000}},
			{0x84, {InstrType::ADD, Condition::NONE, Mode::REG_REG, Register::A, Register::H, 0b11110000}},
			{0x85, {InstrType::ADD, Condition::NONE, Mode::REG_REG, Register::A, Register::L, 0b11110000}},
			{0x86, {InstrType::ADD, Condition::NONE, Mode::REG_REGADD, Register::A, Register::HL, 0b11110000}},
			{0x87, {InstrType::ADD, Condition::NONE, Mode::REG_REG, Register::A, Register::A, 0b11110000}},
			{0x88, {InstrType::ADC, Condition::NONE, Mode::REG_REG, Register::A, Register::B, 0b11110000}},
			{0x89, {InstrType::ADC, Condition::NONE, Mode::REG_REG, Register::A, Register::C, 0b11110000}},
			{0x8A, {InstrType::ADC, Condition::NONE, Mode::REG_REG, Register::A, Register::D, 0b11110000}},
			{0x8B, {InstrType::ADC, Condition::NONE, Mode::REG_REG, Register::A, Register::E, 0b11110000}},
			{0x8C, {InstrType::ADC, Condition::NONE, Mode::REG_REG, Register::A, Register::H, 0b11110000}},
			{0x8D, {InstrType::ADC, Condition::NONE, Mode::REG_REG, Register::A, Register::L, 0b11110000}},
			{0x8E, {InstrType::ADC, Condition::NONE, Mode::REG_REGADD, Register::A, Register::HL, 0b11110000}},
			{0x8F, {InstrType::ADC, Condition::NONE, Mode::REG_REG, Register::A, Register::A, 0b11110000}},
			// 9
			{0x90, {InstrType::SUB, Condition::NONE, Mode::REG_REG, Register::A, Register::B, 0b11110000}},
			{0x91, {InstrType::SUB, Condition::NONE, Mode::REG_REG, Register::A, Register::C, 0b11110000}},
			{0x92, {InstrType::SUB, Condition::NONE, Mode::REG_REG, Register::A, Register::D, 0b11110000}},
			{0x93, {InstrType::SUB, Condition::NONE, Mode::REG_REG, Register::A, Register::E, 0b11110000}},
			{0x94, {InstrType::SUB, Condition::NONE, Mode::REG_REG, Register::A, Register::H, 0b11110000}},
			{0x95, {InstrType::SUB, Condition::NONE, Mode::REG_REG, Register::A, Register::L, 0b11110000}},
			{0x96, {InstrType::SUB, Condition::NONE, Mode::REG_REGADD, Register::A, Register::HL, 0b11110000}},
			{0x97, {InstrType::SUB, Condition::NONE, Mode::REG_REG, Register::A, Register::A, 0b11110000}},
			{0x98, {InstrType::SBC, Condition::NONE, Mode::REG_REG, Register::A, Register::B, 0b11110000}},
			{0x99, {InstrType::SBC, Condition::NONE, Mode::REG_REG, Register::A, Register::C, 0b11110000}},
			{0x9A, {InstrType::SBC, Condition::NONE, Mode::REG_REG, Register::A, Register::D, 0b11110000}},
			{0x9B, {InstrType::SBC, Condition::NONE, Mode::REG_REG, Register::A, Register::E, 0b11110000}},
			{0x9C, {InstrType::SBC, Condition::NONE, Mode::REG_REG, Register::A, Register::H, 0b11110000}},
			{0x9D, {InstrType::SBC, Condition::NONE, Mode::REG_REG, Register::A, Register::L, 0b11110000}},
			{0x9E, {InstrType::SBC, Condition::NONE, Mode::REG_REGADD, Register::A, Register::HL, 0b11110000}},
			{0x9F, {InstrType::SBC, Condition::NONE, Mode::REG_REG, Register::A, Register::A, 0b11110000}},
			// A
			{0xA0, {InstrType::AND, Condition::NONE, Mode::REG, Register::B, Register::NONE, 0b11110000}},
			{0xA1, {InstrType::AND, Condition::NONE, Mode::REG, Register::C, Register::NONE, 0b11110000}},
			{0xA2, {InstrType::AND, Condition::NONE, Mode::REG, Register::D, Register::NONE, 0b11110000}},
			{0xA3, {InstrType::AND, Condition::NONE, Mode::REG, Register::E, Register::NONE, 0b11110000}},
			{0xA4, {InstrType::AND, Condition::NONE, Mode::REG, Register::H, Register::NONE, 0b11110000}},
			{0xA5, {InstrType::AND, Condition::NONE, Mode::REG, Register::L, Register::NONE, 0b11110000}},
			{0xA6, {InstrType::AND, Condition::NONE, Mode::REGADD, Register::HL, Register::NONE, 0b11110000}},
			{0xA7, {InstrType::AND, Condition::NONE, Mode::REG, Register::A, Register::NONE, 0b11110000}},
			{0xA8, {InstrType::XOR, Condition::NONE, Mode::REG, Register::B, Register::NONE, 0b11110000}},
			{0xA9, {InstrType::XOR, Condition::NONE, Mode::REG, Register::C, Register::NONE, 0b11110000}},
			{0xAA, {InstrType::XOR, Condition::NONE, Mode::REG, Register::D, Register::NONE, 0b11110000}},
			{0xAB, {InstrType::XOR, Condition::NONE, Mode::REG, Register::E, Register::NONE, 0b11110000}},
			{0xAC, {InstrType::XOR, Condition::NONE, Mode::REG, Register::H, Register::NONE, 0b11110000}},
			{0xAD, {InstrType::XOR, Condition::NONE, Mode::REG, Register::L, Register::NONE, 0b11110000}},
			{0xAE, {InstrType::XOR, Condition::NONE, Mode::REGADD, Register::HL, Register::NONE, 0b11110000}},
			{0xAF, {InstrType::XOR, Condition::NONE, Mode::REG, Register::A, Register::NONE, 0b1000000}},
			// B
			{0xB0, {InstrType::OR, Condition::NONE, Mode::REG, Register::B, Register::NONE, 0b11110000}},
			{0xB1, {InstrType::OR, Condition::NONE, Mode::REG, Register::C, Register::NONE, 0b11110000}},
			{0xB2, {InstrType::OR, Condition::NONE, Mode::REG, Register::D, Register::NONE, 0b11110000}},
			{0xB3, {InstrType::OR, Condition::NONE, Mode::REG, Register::E, Register::NONE, 0b11110000}},
			{0xB4, {InstrType::OR, Condition::NONE, Mode::REG, Register::H, Register::NONE, 0b11110000}},
			{0xB5, {InstrType::OR, Condition::NONE, Mode::REG, Register::L, Register::NONE, 0b11110000}},
			{0xB6, {InstrType::OR, Condition::NONE, Mode::REGADD, Register::HL, Register::NONE, 0b11110000}},
			{0xB7, {InstrType::OR, Condition::NONE, Mode::REG, Register::A, Register::NONE, 0b11110000}},
			{0xB8, {InstrType::CP, Condition::NONE, Mode::REG, Register::B, Register::NONE, 0b11110000}},
			{0xB9, {InstrType::CP, Condition::NONE, Mode::REG, Register::C, Register::NONE, 0b11110000}},
			{0xBA, {InstrType::CP, Condition::NONE, Mode::REG, Register::D, Register::NONE, 0b11110000}},
			{0xBB, {InstrType::CP, Condition::NONE, Mode::REG, Register::E, Register::NONE, 0b11110000}},
			{0xBC, {InstrType::CP, Condition::NONE, Mode::REG, Register::H, Register::NONE, 0b11110000}},
			{0xBD, {InstrType::CP, Condition::NONE, Mode::REG, Register::L, Register::NONE, 0b11110000}},
			{0xBE, {InstrType::CP, Condition::NONE, Mode::REGADD, Register::HL, Register::NONE, 0b11110000}},
			{0xBF, {InstrType::CP, Condition::NONE, Mode::REG, Register::A, Register::NONE, 0b1000000}},
			// C
			{0xC0, {InstrType::RET, Condition::NZ, Mode::NONE, Register::NONE, Register::NONE, 0}},
			{0xC1, {InstrType::POP, Condition::NONE, Mode::REG, Register::BC, Register::NONE, 0}},
			{0xC2, {InstrType::JP, Condition::NZ, Mode::D16, Register::NONE, Register::NONE, 0}},
			{0xC3, {InstrType::JP, Condition::NONE, Mode::D16, Register::NONE, Register::NONE, 0}},
			{0xC4, {InstrType::CALL, Condition::NZ, Mode::D16, Register::NONE, Register::NONE, 0}},
			{0xC5, {InstrType::PUSH, Condition::NONE, Mode::REG, Register::BC, Register::NONE, 0}},
			{0xC6, {InstrType::ADD, Condition::NONE, Mode::REG_D8, Register::A, Register::NONE, 0b11110000}},
			{0xC7, {InstrType::RST, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
			{0xC8, {InstrType::RET, Condition::Z, Mode::NONE, Register::NONE, Register::NONE, 0}},
			{0xC9, {InstrType::RET, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
			{0xCA, {InstrType::JP, Condition::Z, Mode::D16, Register::NONE, Register::NONE, 0}},
			{0xCB, {InstrType::PREFIX, Condition::NONE, Mode::D8, Register::NONE, Register::NONE, 0}},
			{0xCC, {InstrType::CALL, Condition::Z, Mode::D16, Register::NONE, Register::NONE, 0}},
			{0xCD, {InstrType::CALL, Condition::NONE, Mode::D16, Register::NONE, Register::NONE, 0}},
			{0xCE, {InstrType::ADC, Condition::NONE, Mode::REG_D8, Register::A, Register::NONE, 0b11110000}},
			{0xCF, {InstrType::RST, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
			// D
			{0xD0, {InstrType::RET, Condition::NC, Mode::NONE, Register::NONE, Register::NONE, 0}},
			{0xD1, {InstrType::POP, Condition::NONE, Mode::REG, Register::DE, Register::NONE, 0}},
			{0xD2, {InstrType::JP, Condition::NC, Mode::D16, Register::NONE, Register::NONE, 0}},
			{0xD4, {InstrType::CALL, Condition::NC, Mode::D16, Register::NONE, Register::NONE, 0}},
			{0xD5, {InstrType::PUSH, Condition::NONE, Mode::REG, Register::DE, Register::NONE, 0}},
			{0xD6, {InstrType::SUB, Condition::NONE, Mode::REG_D8, Register::A, Register::NONE, 0b11110000}},
			{0xD7, {InstrType::RST, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
			{0xD8, {InstrType::RET, Condition::C, Mode::NONE, Register::NONE, Register::NONE, 0}},
			{0xD9, {InstrType::RETI, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
			{0xDA, {InstrType::JP, Condition::C, Mode::D16, Register::NONE, Register::NONE, 0}},
			{0xDC, {InstrType::CALL, Condition::C, Mode::D16, Register::NONE, Register::NONE, 0}},
			{0xDE, {InstrType::SBC, Condition::NONE, Mode::REG_D8, Register::A, Register::NONE, 0b11110000}},
			{0xDF, {InstrType::RST, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
			// E
			{0xE0, {InstrType::LDH, Condition::NONE, Mode::A8_REG, Register::NONE, Register::A, 0}},
			{0xE1, {InstrType::POP, Condition::NONE, Mode::REG, Register::HL, Register::NONE, 0}},
			{0xE2, {InstrType::LD, Condition::NONE, Mode::REGADD_REG, Register::C, Register::A, 0}},
			{0xE5, {InstrType::PUSH, Condition::NONE, Mode::REG, Register::HL, Register::NONE, 0}},
			{0xE6, {InstrType::AND, Condition::NONE, Mode::D8, Register::NONE, Register::NONE, 0b11110000}},
			{0xE7, {InstrType::RST, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
			{0xE8, {InstrType::ADD, Condition::NONE, Mode::REG_D8, Register::SP, Register::NONE, 0b11110000}},
			{0xE9, {InstrType::JP, Condition::NONE, Mode::REG, Register::HL, Register::NONE, 0}},
			{0xEA, {InstrType::LD, Condition::NONE, Mode::A16_REG, Register::NONE, Register::A, 0}},
			{0xEE, {InstrType::XOR, Condition::NONE, Mode::D8, Register::NONE, Register::NONE, 0b11110000}},
			{0xEF, {InstrType::RST, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
			// F
			{0xF0, {InstrType::LDH, Condition::NONE, Mode::REG_A8, Register::A, Register::NONE, 0}},
			{0xF1, {InstrType::POP, Condition::NONE, Mode::REG, Register::AF, Register::NONE, 0}},
			{0xF2, {InstrType::LD, Condition::NONE, Mode::REG_REGADD, Register::A, Register::C, 0}},
			{0xF3, {InstrType::DI, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
			{0xF5, {InstrType::PUSH, Condition::NONE, Mode::REG, Register::AF, Register::NONE, 0}},
			{0xF6, {InstrType::OR, Condition::NONE, Mode::D8, Register::NONE, Register::NONE, 0b11110000}},
			{0xF7, {InstrType::RST, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
			{0xF8, {InstrType::LD, Condition::NONE, Mode::HL_SPR8, Register::HL, Register::SP, 0b11110000}},
			{0xF9, {InstrType::LD, Condition::NONE, Mode::REG_REG, Register::SP, Register::HL, 0}},
			{0xFA, {InstrType::LD, Condition::NONE, Mode::REG_A16, Register::A, Register::NONE, 0}},
			{0xFB, {InstrType::EI, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
			{0xFE, {InstrType::CP, Condition::NONE, Mode::D8, Register::NONE, Register::NONE, 0b11110000}},
			{0xFF, {InstrType::RST, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
			// Default "wrong" opcode
			{0x100, {InstrType::NONE, Condition::NONE, Mode::NONE, Register::NONE, Register::NONE, 0}},
		};

		if (auto search = INSTRUCTIONS_FROM_OPCODES.find(opcode);
			search != INSTRUCTIONS_FROM_OPCODES.end())
		{
			return &(search->second);
		}

		return &(INSTRUCTIONS_FROM_OPCODES.at(0x100));
	}

	const char *instructionName(const Instruction instr)
	{
		// Lookup for the names
		static const std::array<const char *, 47> NAME_LOOKUP = {
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
			"CB_SET",
		};

		return NAME_LOOKUP[int(instr._type)];
	}

	void CPU::displayCurrentInstruction() const
	{
		static const std::array<const char *, 5> COND_LOOKUP = {
			"",
			"Z",
			"NZ",
			"C",
			"NC",
		};

		static const std::array<const char *, 0xF> REG_LOOKUP = {
			"NONE",
			"A",
			"F",
			"B",
			"C",
			"D",
			"E",
			"H",
			"L",
			"AF",
			"BC",
			"DE",
			"HL",
			"SP",
			"PC",
		};

		printf("[%02X (%02X, %02X)] %s", _curr_opcode, loByte(_curr_fetch), hiByte(_curr_fetch),
			   instructionName(*_curr_instr));

		if (_curr_instr->_cond != Condition::NONE)
		{
			printf(" <%s>", COND_LOOKUP[(u8)_curr_instr->_cond]);
		}

		switch (_curr_instr->_mode)
		{

		case Mode::NONE:
			break;

		case Mode::REG_D16:

			printf(" %s %04X", REG_LOOKUP[(u8)_curr_instr->_reg1], _curr_fetch);
			break;

		case Mode::REGADD_REG:

			printf(" $%04X %s", _mem_dest, REG_LOOKUP[(u8)_curr_instr->_reg2]);
			break;

		case Mode::REG:

			printf(" %s", REG_LOOKUP[(u8)_curr_instr->_reg1]);
			break;

		case Mode::REG_D8:

			printf(" %s %02X", REG_LOOKUP[(u8)_curr_instr->_reg1], _curr_fetch & 0xFF);
			break;

		case Mode::A16_REG:

			printf(" $%04X %s", _mem_dest, REG_LOOKUP[(u8)_curr_instr->_reg2]);
			break;

		case Mode::REG_REG:

			printf(" %s %s", REG_LOOKUP[(u8)_curr_instr->_reg1], REG_LOOKUP[(u8)_curr_instr->_reg2]);
			break;

		case Mode::REG_REGADD:

			printf(" %s $%04X", REG_LOOKUP[(u8)_curr_instr->_reg1], readRegister(_curr_instr->_reg2));
			break;

		case Mode::HLINC_REG:

			printf(" $(%04X++) %s", _mem_dest, REG_LOOKUP[(u8)_curr_instr->_reg2]);
			break;

		case Mode::REG_HLINC:

			printf(" %s $(%04X++)", REG_LOOKUP[(u8)_curr_instr->_reg1], readRegister(Register::HL) - 1);
			break;

		case Mode::HLDEC_REG:

			printf(" $(%04X--) %s", _mem_dest, REG_LOOKUP[(u8)_curr_instr->_reg2]);
			break;

		case Mode::REGADD:

			printf(" $%04X", _mem_dest);
			break;

		case Mode::REGADD_D8:

			printf(" $%04X %02X", _mem_dest, _curr_fetch & 0xFF);
			break;

		case Mode::REG_HLDEC:

			printf(" %s $(%04X--)", REG_LOOKUP[(u8)_curr_instr->_reg1], readRegister(Register::HL) + 1);
			break;

		case Mode::D16:

			printf(" %04X", _curr_fetch);
			break;

		case Mode::D8:

			printf(" %02X", _curr_fetch & 0xFF);
			break;

		case Mode::A8_REG:

			printf(" $%02X %s", _mem_dest, REG_LOOKUP[(u8)_curr_instr->_reg2]);
			break;

		case Mode::REG_A8:

			printf(" %s $%02X", REG_LOOKUP[(u8)_curr_instr->_reg1], _curr_fetch & 0xFF);
			break;

		case Mode::HL_SPR8:

			printf(" HL $(%04X + %02X)", _SP, _curr_fetch & 0xFF);
			break;

		case Mode::REG_A16:

			printf(" %s $%04X", REG_LOOKUP[(u8)_curr_instr->_reg1], _curr_fetch);
			break;
		}

		printf("\n");
	}
}
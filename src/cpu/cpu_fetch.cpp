#include <cpu/cpu.h>

#include <gb.h>

namespace jmpr {

	/**
	* Read the data requested by the current instruction.
	*/
	void CPU::fetchData() {

		_mem_dest = 0;
		_dest_is_mem = false;

		switch (_curr_instr->_mode) {

		case Mode::NONE: break;


		case Mode::REG_D16: {
			// 2 cycles long

			u8 lo = _bus->read(_PC);
			GameBoy::getInstance()->cycle(1);

			u8 hi = _bus->read(_PC + 1);
			GameBoy::getInstance()->cycle(1);

			_curr_fetch = merge16(hi, lo);
			_PC += 2;
			break;
		}


		case Mode::REGADD_REG: {
			// 0 cycles long

			_curr_fetch = readRegister(_curr_instr->_reg2);

			u16 address = readRegister(_curr_instr->_reg1);

			// Special case for some instruction.
			if (_curr_instr->_reg1 == Register::C) {
				address = merge16(0xFF, address);
			}

			_mem_dest = address;
			_dest_is_mem = true;
			break;
		}


		case Mode::REG:

			_curr_fetch = readRegister(_curr_instr->_reg1);
			break;


		case Mode::REG_D8:

			_curr_fetch = _bus->read(_PC);
			GameBoy::getInstance()->cycle(1);

			_PC++;
			break;


		case Mode::A16_REG: {

			u8 lo = _bus->read(_PC);
			GameBoy::getInstance()->cycle(1);
			u8 hi = _bus->read(_PC + 1);
			GameBoy::getInstance()->cycle(1);

			_PC += 2;

			_curr_fetch = readRegister(_curr_instr->_reg2);

			_mem_dest = merge16(hi, lo);
			_dest_is_mem = true;

			break;
		}


		case Mode::REG_REG:

			_curr_fetch = readRegister(_curr_instr->_reg2);
			break;


		case Mode::REG_REGADD: {

			u16 address = readRegister(_curr_instr->_reg2);

			// Special case for the LDH instruction.
			if (_curr_instr->_reg2 == Register::C) {
				address = merge16(0xFF, address);
			}

			_curr_fetch = _bus->read(address);
			GameBoy::getInstance()->cycle(1);

			break;
		}

		case Mode::HLINC_REG:

			_mem_dest = readRegister(Register::HL);
			_dest_is_mem = true;
			writeRegister(Register::HL, readRegister(Register::HL) + 1);

			_curr_fetch = readRegister(_curr_instr->_reg2);

			break;


		case Mode::REG_HLINC:

			_curr_fetch = _bus->read(readRegister(Register::HL));
			GameBoy::getInstance()->cycle(1);

			writeRegister(Register::HL, readRegister(Register::HL) + 1);

			break;


		case Mode::HLDEC_REG:

			_mem_dest = readRegister(Register::HL);
			_dest_is_mem = true;
			writeRegister(Register::HL, readRegister(Register::HL) - 1);

			_curr_fetch = readRegister(_curr_instr->_reg2);

			break;


		case Mode::REGADD:

			_mem_dest = readRegister(_curr_instr->_reg1);
			_dest_is_mem = true;

			_curr_fetch = _bus->read(_mem_dest);
			GameBoy::getInstance()->cycle(1);
			break;


		case Mode::REGADD_D8:

			_curr_fetch = _bus->read(_PC);
			GameBoy::getInstance()->cycle(1);

			_PC++;

			_mem_dest = readRegister(_curr_instr->_reg1);
			_dest_is_mem = true;

			break;


		case Mode::REG_HLDEC:

			_curr_fetch = _bus->read(readRegister(Register::HL));
			GameBoy::getInstance()->cycle(1);

			writeRegister(Register::HL, readRegister(Register::HL) - 1);

			break;


		case Mode::D16: {

			u8 lo = _bus->read(_PC);
			GameBoy::getInstance()->cycle(1);
			u8 hi = _bus->read(_PC + 1);
			GameBoy::getInstance()->cycle(1);

			_curr_fetch = merge16(hi, lo);

			_PC += 2;
			break;
		}


		case Mode::D8:

			_curr_fetch = _bus->read(_PC);
			GameBoy::getInstance()->cycle(1);

			_PC++;

			break;


		case Mode::A8_REG: {

			_curr_fetch = readRegister(_curr_instr->_reg2);

			u16 address = _bus->read(_PC);
			GameBoy::getInstance()->cycle(1);
			_PC++;

			// Required for LDH instruction.
			address = merge16(0xFF, address);

			_mem_dest = address;
			_dest_is_mem = true;
			break;
		}

		case Mode::REG_A8: {

			u16 address = _bus->read(_PC);
			GameBoy::getInstance()->cycle(1);
			_PC++;

			// Required for LDH instruction.
			address = merge16(0xFF, address);

			_curr_fetch = _bus->read(address);
			GameBoy::getInstance()->cycle(1);

			break;
		}


		case Mode::HL_SPR8:

			// todo check if signed works
			_curr_fetch = _SP + (s8)_bus->read(_PC);
			GameBoy::getInstance()->cycle(1);

			_PC++;
			break;


		case Mode::REG_A16: {

			u8 lo = _bus->read(_PC);
			GameBoy::getInstance()->cycle(1);
			u8 hi = _bus->read(_PC + 1);
			GameBoy::getInstance()->cycle(1);

			_PC += 2;

			// Required for LDH instruction.
			u16 address = merge16(hi, lo);

			_curr_fetch = _bus->read(address);
			GameBoy::getInstance()->cycle(1);

			break;
		}


		default: {
			std::cerr << "Error: Addressing mode not implemented yet: " <<
				std::hex << int(_curr_instr->_mode) << std::endl;
		}
		}
	}
}
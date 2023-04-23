#include <cpu.h>

#include <gb.h>

namespace jmpr {

	// fetch second reg
	// set dest to first reg
	// 1 gameboy cycles when bus read

	/**
	* Read the data requested by the current instruction.
	*/
	void CPU::fetchData() {

		_mem_dest = 0;
		_dest_is_mem = false;

		switch (_current_instr->_mode) {

		case Mode::NONE: break;


		case Mode::REG:

			_current_fetch = readRegister(_current_instr->_reg1);
			break;


		case Mode::REG_REG:

			_current_fetch = readRegister(_current_instr->_reg2);
			break;


		case Mode::REGADD:

			_mem_dest = readRegister(_current_instr->_reg1);
			_dest_is_mem = true;

			_current_fetch = _bus->read(_mem_dest);
			GameBoy::cycle(1);
			break;


		case Mode::REG_REGADD: {

			u16 address = readRegister(_current_instr->_reg2);

			// Special case for the LDH instruction.
			if (_current_instr->_reg2 == Register::C) {
				address = merge(0xFF, address);
			}

			_current_fetch = _bus->read(address);
			GameBoy::cycle(1);

			break;
		}


		case Mode::REGADD_REG: {

			_current_fetch = readRegister(_current_instr->_reg1);

			u16 address = readRegister(_current_instr->_reg2);

			// Special case for the LDH instruction.
			if (_current_instr->_reg2 == Register::C) {
				address = merge(0xFF, address);
			}

			_mem_dest = address;
			_dest_is_mem = true;
			break;
		}


		case Mode::REG_A8: {

			u16 address = _bus->read(_PC);
			GameBoy::cycle(1);
			_PC++;

			// Required for LDH instruction.
			address = merge(0xFF, address);

			_current_fetch = _bus->read(address);
			GameBoy::cycle(1);

			break;
		}


		case Mode::A8_REG: {

			_current_fetch = readRegister(_current_instr->_reg1);

			u16 address = _bus->read(_PC);
			GameBoy::cycle(1);
			_PC++;

			// Required for LDH instruction.
			address = merge(0xFF, address);

			_mem_dest = _bus->read(address);
			GameBoy::cycle(1);
			_dest_is_mem = true;
			break;
		}


		default: {
			noImpl();
		}

		}
	}
}
#include <cpu.h>

#include <bus.h>

namespace jmpr {

	CPU::CPU() {

		_PC = 0x100;
		_current_instr = nullptr;
	}

	/**
	* Read data from the requested register.
	* @param reg Register to read
	*/
	u16 CPU::readRegister(Register reg) const {

		u16 ret = 0;

		switch (reg) {
		case Register::A: ret = _registers._A; break;
		case Register::F: ret = _registers._F; break;
		case Register::AF: ret = merge(_registers._A, _registers._F); break;
		case Register::B: ret = _registers._B; break;
		case Register::C: ret = _registers._C; break;
		case Register::BC: ret = merge(_registers._B, _registers._C); break;
		case Register::D: ret = _registers._D; break;
		case Register::E: ret = _registers._E; break;
		case Register::DE: ret = merge(_registers._D, _registers._E); break;
		case Register::H: ret = _registers._H; break;
		case Register::L: ret = _registers._L; break;
		case Register::HL: ret = merge(_registers._H, _registers._L); break;
		case Register::SP: ret = _SP; break;
		case Register::PC: ret = _PC; break;
		}

		return ret;
	}

	void CPU::fetchOpcode() {
		_current_opcode = _bus->read(_PC++);
		_current_instr = fromOpcode(_current_opcode);
	}

	void CPU::fetchData() {

		// Skip instructions without types
		if (_current_instr->_type == InstrType::NONE) return;


	}

	void CPU::execute() {
		// TODO...
	}

	bool CPU::cycle() {
		if (!_halted) {
			fetchOpcode();
			fetchData();
			execute();
		}

		return true;
	}
}
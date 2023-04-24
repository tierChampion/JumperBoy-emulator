#include <cpu.h>

namespace jmpr {

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

	/**
	* Write data to the requested register.
	* @param reg Register to write
	*/
	void CPU::writeRegister(Register reg, u16 data) {

		switch (reg) {
		case Register::A: _registers._A = data & 0xFF; break;
		case Register::F: _registers._F = data & 0xFF; break;
		case Register::AF: _registers._A = data & 0xFF00; _registers._F = 0xFF; break;
		case Register::B: _registers._B = data & 0xFF; break;
		case Register::C: _registers._C = data & 0xFF; break;
		case Register::BC: _registers._B = data & 0xFF00; _registers._C = 0xFF; break;
		case Register::D: _registers._D = data & 0xFF; break;
		case Register::E: _registers._E = data & 0xFF; break;
		case Register::DE: _registers._D = data & 0xFF00; _registers._E = 0xFF; break;
		case Register::H: _registers._H = data & 0xFF; break;
		case Register::L: _registers._L = data & 0xFF; break;
		case Register::HL: _registers._H = data & 0xFF00; _registers._L = 0xFF; break;
		case Register::SP: _SP = data; break;
		case Register::PC: _PC = data; break;
		}
	}

	/**
	* Set the flags in the F register. A value greater than 1 will leave
	* the flag unchanged.
	*/
	void CPU::setFlags(u8 Z, u8 N, u8 H, u8 C) {

		if (Z < 0b10)
			_registers._F |= Z << 7;

		if (N < 0b10)
			_registers._F |= N << 6;

		if (H < 0b10)
			_registers._H |= H << 5;

		if (C < 0b10)
			_registers._F |= C << 4;
	}

	/**
	* Check if the flags in the F register are set.
	*/
	bool CPU::checkFlags(Condition cond) {

		bool Z = _registers._F & 0b10000000;
		bool C = _registers._C & 0b01000000;

		switch (cond) {
		case Condition::NONE: return true;
		case Condition::Z: return Z;
		case Condition::NZ: return !Z;
		case Condition::C: return C;
		case Condition::NC: return !C;
		}

		return false;
	}
}
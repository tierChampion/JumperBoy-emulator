#include <cpu.h>

#include <bus.h>

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
		case Register::A: _registers._A = loByte(data); break;
		case Register::F: _registers._F = loByte(data); break;
		case Register::AF: _registers._A = hiByte(data); _registers._F = loByte(data); break;
		case Register::B: _registers._B = loByte(data); break;
		case Register::C: _registers._C = loByte(data); break;
		case Register::BC: _registers._B = hiByte(data); _registers._C = loByte(data); break;
		case Register::D: _registers._D = loByte(data); break;
		case Register::E: _registers._E = loByte(data); break;
		case Register::DE: _registers._D = hiByte(data); _registers._E = loByte(data); break;
		case Register::H: _registers._H = loByte(data); break;
		case Register::L: _registers._L = loByte(data); break;
		case Register::HL: _registers._H = hiByte(data); _registers._L = loByte(data); break;
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
	* Get the value of the desired flag.
	* @param flag 0 for Z, 1 for N, 2 for H and 3 for C
	*/
	u8 CPU::readFlag(u8 flag) {
		return bit(_registers._F, 7 - flag);
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

	/**
	* Push a byte to the stack.
	*/
	void CPU::pushStack8(u8 data) {
		_SP--;
		_bus->write(_SP, data);
	}

	/**
	* Push two bytes to the stack.
	*/
	void CPU::pushStack16(u16 data) {

		pushStack8(hiByte(data));
		pushStack8(loByte(data));
	}

	/**
	* Pop a byte from the stack.
	*/
	u8 CPU::popStack8() {
		u8 popped = _bus->read(_SP);
		_SP++;
		return popped;
	}

	/**
	* Pop two bytes from the stack.
	*/
	u16 CPU::popStack16() {

		u8 lo = popStack8();
		u8 hi = popStack8();

		return merge(hi, lo);
	}
}
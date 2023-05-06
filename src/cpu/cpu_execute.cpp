#include <cpu.h>

#include <gb.h>

namespace jmpr {

	/**
	* Execute the current instruction.
	*/
	void CPU::execute() {

		ProcessFunction process = _PROCESSES.at(_curr_instr->_type);

		(this->*process)();
	}

	/**
	* Error operation. Stops the program.
	*/
	void CPU::XXX() {

		std::cerr << "Error: Unknown instruction trying to execute." << std::endl;
		exit(-3);
	}

	/**
	* Perform no operation.
	*/
	void CPU::NOP() {}

	/**
	* Load a value into a register.
	*/
	void CPU::LD() {

		if (_dest_is_mem) {

			// 16 bit registers need 2 writes
			if (is16Bits(_curr_instr->_reg2)) {

				_bus->write16(_mem_dest, _curr_fetch);
				GameBoy::cycle(1);
			}
			else {
				_bus->write(_mem_dest, _curr_fetch);
			}
		}
		else {

			if (_curr_instr->_mode == Mode::HL_SPR8) {

				// todo: check if it works

				u8 half_carry = ((readRegister(_curr_instr->_reg2) & 0xF) >
					(_curr_fetch & 0xF));

				u8 full_carry = ((readRegister(_curr_instr->_reg2) & 0xFF) >
					(_curr_fetch & 0xFF));

				setFlags(0, 0, half_carry, full_carry);
			}

			writeRegister(_curr_instr->_reg1, _curr_fetch);
		}
	}

	/**
	* Increment a register.
	*/
	void CPU::INC() {

		if (_dest_is_mem) {

			_bus->write(_mem_dest, _curr_fetch + 1);
			GameBoy::cycle(1);
		}
		else {
			writeRegister(_curr_instr->_reg1, readRegister(_curr_instr->_reg1) + 1);
		}

		bool is16 = is16Bits(_curr_instr->_reg1);

		if (is16) {
			GameBoy::cycle(1);
		}

		if (!is16 || _dest_is_mem) {
			setFlags(readRegister(_curr_instr->_reg1) == 0, 0,
				(readRegister(_curr_instr->_reg1) & 0xF) == 0, -1);
		}
	}

	/**
	* Decrement a register.
	*/
	void CPU::DEC() {

		if (_dest_is_mem) {

			_bus->write(_mem_dest, _curr_fetch - 1);
			GameBoy::cycle(1);
		}
		else {
			writeRegister(_curr_instr->_reg1, readRegister(_curr_instr->_reg1) - 1);
		}

		bool is16 = is16Bits(_curr_instr->_reg1);

		if (is16) {
			GameBoy::cycle(1);
		}

		if (!is16 || _dest_is_mem) {
			setFlags(readRegister(_curr_instr->_reg1) == 0, 1,
				(readRegister(_curr_instr->_reg1) & 0xF) == 0xF, -1);
		}
	}

	/**
	* Rotate the bits to the left.
	*/
	void CPU::RLCA() {

		u8 c = bit(_registers._A, 7);
		_registers._A = (_registers._A << 1) | c;

		setFlags(0, 0, 0, c);
	}

	/**
	* Addition between two values.
	*/
	void CPU::ADD() {

		u32 val = readRegister(_curr_instr->_reg1) + _curr_fetch;

		bool is16 = is16Bits(_curr_instr->_reg1);

		if (is16) {
			GameBoy::cycle(1);
		}

		bool toStackPointer = (_curr_instr->_reg1 == Register::SP);

		if (toStackPointer) {
			val = readRegister(_curr_instr->_reg1) + s8(_curr_fetch);
		}

		// 8 bit flags
		u8 z = (val & 0xFF) == 0;
		u8 h = (readRegister(_curr_instr->_reg1) & 0xF) + (_curr_fetch & 0xF) >= 0x10;
		u8 c = (readRegister(_curr_instr->_reg1) & 0xFF) + (_curr_fetch & 0xFF) >= 0x100;

		// regular 16 bit flags
		if (is16 && !toStackPointer) {
			z = -1;
			h = (readRegister(_curr_instr->_reg1) & 0xFFF) + (_curr_fetch & 0xFFF) >= 0x1000;
			c = (readRegister(_curr_instr->_reg1) & 0xFFFF) +
				(_curr_fetch & 0xFFFF) >= 0x10000;
		}
		// special case
		else if (toStackPointer) {
			z = 0;
		}

		writeRegister(_curr_instr->_reg1, val & 0xFFFF);
		setFlags(z, 0, h, c);
	}

	/**
	* Rotate the bits right.
	*/
	void CPU::RRCA() {

		u8 c = bit(_registers._A, 0);
		_registers._A = (_registers._A >> 1) | (c << 7);

		setFlags(0, 0, 0, c);
	}

	/**
	* Stop the cpu.
	*/
	void CPU::STOP() { _stopped = true; }

	/**
	* Rotate the bits to the left. Add the carry at the LSB instead of the rotated bit.
	*/
	void CPU::RLA() {

		u8 c = carryFlag();

		u8 rotated_bit = bit(_registers._A, 7);

		_registers._A = (_registers._A << 1) | c;

		setFlags(0, 0, 0, rotated_bit);
	}

	/**
	* Jump to a relative location.
	*/
	void CPU::JR() {

		if (checkFlags(_curr_instr->_cond)) {
			// check if it works
			_PC += (s8)(_curr_fetch & 0xFF);
			GameBoy::cycle(1);
		}
	}

	/**
	* Rotate the bits to the right. Add the carry at the MSB instead of the rotated bit.
	*/
	void CPU::RRA() {

		u8 c = carryFlag();

		u8 rotated_bit = bit(_registers._A, 0);

		_registers._A = (_registers._A >> 1) | (c << 7);

		setFlags(0, 0, 0, rotated_bit);
	}

	/**
	* Decimal addition adjust. Fix potential errors with BCD addition.
	*/
	void CPU::DAA() {

		u8 correction = 0;
		u8 full_carry = 0;

		// Fix BCD decimal if there is a half carry or the value is not legal
		if ((!negationFlag() && loNibble(_registers._A) > 0x9) || halfCarryFlag()) {
			correction = 0x6;
		}

		if ((!negationFlag() && _registers._A > 0x99) || carryFlag()) {
			correction |= 0x60;
			full_carry = 1;
		}

		_registers._A += negationFlag() ? -correction : correction;

		setFlags(_registers._A == 0, -1, 0, full_carry);
	}

	/**
	* Complement the accumulator.
	*/
	void CPU::CPL() {

		_registers._A = ~_registers._A;
		setFlags(-1, 1, 1, -1);
	}

	/**
	* Set the carry flag.
	*/
	void CPU::SCF() {

		setFlags(-1, 0, 0, 1);
	}

	/**
	* Complement the carry flag.
	*/
	void CPU::CCF() {

		bool C = checkFlags(_curr_instr->_cond);

		setFlags(-1, 0, 0, !(bool)carryFlag());
	}

	/**
	* Halt the CPU. Enter a low power stopped state.
	*/
	void CPU::HALT() {
		_halted = true;

		// halt bug
		_inter_handler.checkHaltBug(_PC);
	}

	/**
	* Addition with carry.
	*/
	void CPU::ADC() {

		u16 add = (carryFlag() + _curr_fetch);

		u8 res = (_registers._A + add) & 0xFF;

		u8 half_carry = ((res & 0xF) < (_registers._A & 0xF)) ||
			((res & 0xF) == (_registers._A & 0xF) && carryFlag());

		u8 full_carry = (res < _registers._A) ||
			(res == _registers._A && carryFlag());

		_registers._A = res;

		setFlags(_registers._A == 0, 0, half_carry, full_carry);
	}

	/**
	* Substraction without carry.
	*/
	void CPU::SUB() {

		u8 half_carry = (_curr_fetch & 0xF) > (_registers._A & 0xF);

		u8 full_carry = (_curr_fetch > _registers._A);

		_registers._A -= _curr_fetch;

		setFlags(_registers._A == 0, 1, half_carry, full_carry);
	}

	/**
	* Substraction with carry.
	*/
	void CPU::SBC() {

		u16 sub = (carryFlag() + _curr_fetch);

		u8 res = (_registers._A - sub) & 0xFF;

		u8 half_carry = ((_curr_fetch & 0xF) > (_registers._A & 0xF)) ||
			((_curr_fetch & 0xF) == (_registers._A & 0xF) && carryFlag());

		u8 full_carry = (_curr_fetch > _registers._A) ||
			(_curr_fetch == _registers._A && carryFlag());

		_registers._A = res;

		setFlags(_registers._A == 0, 1, half_carry, full_carry);
	}

	/**
	* AND logical operation. Result stored in the accumulator.
	*/
	void CPU::AND() {

		_registers._A &= (_curr_fetch & 0xFF);
		setFlags(_registers._A == 0, 0, 1, 0);
	}

	/**
	* XOR logical operation. Result stored in the accumulator.
	*/
	void CPU::XOR() {

		_registers._A ^= (_curr_fetch & 0xFF);
		setFlags(_registers._A == 0, 0, 0, 0);
	}

	/**
	* OR logical operation. Result stored in the accumulator.
	*/
	void CPU::OR() {

		_registers._A |= (_curr_fetch & 0xFF);
		setFlags(_registers._A == 0, 0, 0, 0);
	}

	/**
	* Compare two values. Check if the substraction is 0.
	*/
	void CPU::CP() {

		u8 result = _registers._A - (_curr_fetch & 0xFF);
		u8 half_res = (_registers._A & 0xF) - (_curr_fetch & 0xF);

		setFlags(result == 0, 1, half_res > (_registers._A & 0xF), result > _registers._A);
	}

	/**
	* Return from a function. Jump to the location at the top of the stack
	*/
	void CPU::RET() {

		if (checkFlags(_curr_instr->_cond)) {

			u8 lo = popStack8();
			GameBoy::cycle(1);
			u8 hi = popStack8();
			GameBoy::cycle(1);

			_PC = merge(hi, lo);
		}
	}

	/**
	* Pop from the stack.
	*/
	void CPU::POP() {

		u8 lo = popStack8();
		GameBoy::cycle(1);
		u8 hi = popStack8();
		GameBoy::cycle(1);

		writeRegister(_curr_instr->_reg1, merge(hi, lo));
	}

	/**
	* Jump to a location.
	*/
	void CPU::JP() {

		if (checkFlags(_curr_instr->_cond)) {
			_PC = _curr_fetch;
			GameBoy::cycle(1);
		}
	}

	/**
	* Jump to the beginning of a function and push the PC to the stack
	*/
	void CPU::CALL() {

		if (checkFlags(_curr_instr->_cond)) {

			pushStack8(hiByte(_PC));
			GameBoy::cycle(1);
			pushStack8(loByte(_PC));
			GameBoy::cycle(1);

			_PC = _curr_fetch;
		}
	}

	/**
	* Push to the stack.
	*/
	void CPU::PUSH() {

		pushStack8(hiByte(_curr_fetch));
		GameBoy::cycle(1);
		pushStack8(loByte(_curr_fetch));
		GameBoy::cycle(1);
	}

	/**
	* Jump to the boot rom and reinitialize the cpu.
	*/
	void CPU::RST() {

		pushStack8(hiByte(_PC));
		GameBoy::cycle(1);
		pushStack8(hiByte(_PC));
		GameBoy::cycle(1);

		// todo: to see
		u8 addr = ((_curr_opcode & 0x30)) | (_curr_opcode & 0x8) << 3;

		_PC = addr;
	}

	/**
	* Launch a CB binary operation.
	*/
	void CPU::PREFIX() {

		// CB Instruction to execute
		_curr_opcode = _curr_fetch & 0xFF;

		Register reg;

		// Register affected
		switch (_curr_opcode & 0x7) {
		case 0: reg = Register::B; break;
		case 1: reg = Register::C; break;
		case 2: reg = Register::D; break;
		case 3: reg = Register::E; break;
		case 4: reg = Register::H; break;
		case 5: reg = Register::L; break;
		case 6: {
			reg = Register::HL;
			_mem_dest = readRegister(reg);
			_dest_is_mem = true;

			GameBoy::cycle(1);
			break;
		}
		default: reg = Register::A; break;
		}

		// Bit checked / affected
		_curr_fetch = 2 * ((_curr_opcode & 0x40) >> 8) + ((_curr_opcode & 0x8) >> 3);

		// RLC
		if (_curr_opcode < 0x08)
			CB_RLC(reg);
		else if (_curr_opcode < 0x10)
			CB_RRC(reg);
		else if (_curr_opcode < 0x18)
			CB_RL(reg);
		else if (_curr_opcode < 0x20)
			CB_RR(reg);
		else if (_curr_opcode < 0x28)
			CB_SLA(reg);
		else if (_curr_opcode < 0x30)
			CB_SRA(reg);
		else if (_curr_opcode < 0x38)
			CB_SWAP(reg);
		else if (_curr_opcode < 0x40)
			CB_SRL(reg);
		else if (_curr_opcode < 0x80)
			CB_BIT(reg);
		else if (_curr_opcode < 0xC0)
			CB_RES(reg);
		else
			CB_SET(reg);

	}

	/**
	* Return from a function and enable interrupts.
	*/
	void CPU::RETI() {

		u8 lo = popStack8();
		GameBoy::cycle(1);
		u8 hi = popStack8();
		GameBoy::cycle(1);

		_PC = merge(hi, lo);

		_inter_handler.enableInterrupts();
	}


	/**
	* Load to the high ram.
	*/
	void CPU::LDH() {

		u16 test = merge(0xFF, readRegister(Register::C));

		if (_dest_is_mem) {

			_bus->write(_mem_dest, _curr_fetch);
		}
		else {

			writeRegister(_curr_instr->_reg1, _curr_fetch);
		}

		GameBoy::cycle(1);
	}

	/**
	* Disable the interrupts.
	*/
	void CPU::DI() {
		_inter_handler.enableInterrupts();
	}

	/**
	* Enable the interrupts.
	*/
	void CPU::EI() {
		_inter_handler.enableInterrupts();
	}

	/**
	* Rotate the bits to the left.
	*/
	void CPU::CB_RLC(Register reg) {

		u8 rotated = 0;
		u8 isZ = 0;

		if (_dest_is_mem) {

			u8 data = _bus->read(_mem_dest);
			GameBoy::cycle(1);

			rotated = bit(data, 7);
			isZ = data == 0;

			_bus->write(_mem_dest, (data << 1) | rotated);
			GameBoy::cycle(1);
		}
		else {

			rotated = bit(readRegister(reg), 7);
			isZ = readRegister(reg) == 0;

			writeRegister(reg, (readRegister(reg) << 1) | rotated);
		}

		setFlags(isZ, 0, 0, rotated);
	}

	/**
	* Rotate the bits to the right.
	*/
	void CPU::CB_RRC(Register reg) {

		u8 rotated = 0;
		u8 isZ = 0;

		if (_dest_is_mem) {

			u8 data = _bus->read(_mem_dest);
			GameBoy::cycle(1);

			rotated = bit(data, 0);
			isZ = data == 0;

			_bus->write(_mem_dest, (data >> 1) | (rotated << 7));
			GameBoy::cycle(1);
		}
		else {

			rotated = bit(readRegister(reg), 0);
			isZ = readRegister(reg) == 0;

			writeRegister(reg, (readRegister(reg) >> 1) | (rotated << 7));
		}

		setFlags(isZ, 0, 0, rotated);

	}

	/**
	* Rotate the bits to the left. Set the LSB to the carry.
	*/
	void CPU::CB_RL(Register reg) {

		u8 rotated = 0;
		u8 isZ = 0;

		if (_dest_is_mem) {

			u8 data = _bus->read(_mem_dest);
			GameBoy::cycle(1);

			rotated = bit(data, 7);
			isZ = data == 0;

			_bus->write(_mem_dest, (data << 1) | carryFlag());
			GameBoy::cycle(1);
		}
		else {

			rotated = bit(readRegister(reg), 7);
			isZ = readRegister(reg) == 0;

			writeRegister(reg, (readRegister(reg) << 1) | carryFlag());
		}

		setFlags(isZ, 0, 0, rotated);

	}

	/**
	* Rotate the bits right. Set the MSB to the carry.
	*/
	void CPU::CB_RR(Register reg) {

		u8 rotated = 0;
		u8 isZ = 0;

		if (_dest_is_mem) {

			u8 data = _bus->read(_mem_dest);
			GameBoy::cycle(1);

			rotated = bit(data, 0);
			isZ = data == 0;

			_bus->write(_mem_dest, (data >> 1) | (carryFlag() << 7));
			GameBoy::cycle(1);
		}
		else {

			rotated = bit(readRegister(reg), 0);
			isZ = readRegister(reg) == 0;

			writeRegister(reg, (readRegister(reg) >> 1) | (carryFlag() << 7));
		}

		setFlags(isZ, 0, 0, rotated);
	}


	/**
	* Left shift a register by 1. Set the LSB at 0.
	*/
	void CPU::CB_SLA(Register reg) {

		u8 shifted = 0;
		u8 isZ = 0;

		if (_dest_is_mem) {

			u8 data = _bus->read(_mem_dest);
			GameBoy::cycle(1);

			shifted = bit(data, 7);
			isZ = (data & 0x7F) == 0;

			_bus->write(_mem_dest, data << 1);
			GameBoy::cycle(1);
		}
		else {

			shifted = bit(readRegister(reg), 7);
			isZ = (readRegister(reg) & 0x7F) == 0;

			writeRegister(reg, readRegister(reg) << 1);
		}

		setFlags(isZ, 0, 0, shifted);
	}


	/**
	* Right shift a register by 1. Leave the MSB at its old value.
	*/
	void CPU::CB_SRA(Register reg) {

		u8 shifted = 0;
		u8 isZ = 0;

		if (_dest_is_mem) {

			u8 data = _bus->read(_mem_dest);
			GameBoy::cycle(1);

			shifted = bit(data, 0);
			isZ = data < 0x02;

			_bus->write(_mem_dest, (data >> 1) & (data & (1 << 7)));
			GameBoy::cycle(1);
		}
		else {

			shifted = bit(readRegister(reg), 0);
			isZ = readRegister(reg) < 0x02;

			writeRegister(reg, readRegister(reg) >> 1 & (readRegister(reg) & (1 << 7)));
		}

		setFlags(isZ, 0, 0, shifted);
	}


	/**
	* Swap the hi nibble and the lo nibble of a register.
	*/
	void CPU::CB_SWAP(Register reg) {

		u8 hi = 0;
		u8 lo = 0;

		if (_dest_is_mem) {

			u8 data = _bus->read(_mem_dest);
			GameBoy::cycle(1);

			hi = hiNibble(data);
			lo = loNibble(data);

			_bus->write(_mem_dest, merge(lo, hi));
			GameBoy::cycle(1);
		}
		else {
			hi = hiNibble(readRegister(reg));
			lo = loNibble(readRegister(reg));

			writeRegister(reg, merge(lo, hi));
		}

		setFlags(hi == 0 && lo == 0, 0, 0, 0);
	}


	/**
	* Right shift a register by 1. Set the MSB at 0.
	*/
	void CPU::CB_SRL(Register reg) {

		u8 shifted = 0;
		u8 isZ = 0;

		if (_dest_is_mem) {

			u8 data = _bus->read(_mem_dest);
			GameBoy::cycle(1);

			shifted = bit(data, 0);
			isZ = data < 0x02;

			_bus->write(_mem_dest, data >> 1);
			GameBoy::cycle(1);
		}
		else {

			shifted = bit(readRegister(reg), 0);
			isZ = readRegister(reg) < 0x02;

			writeRegister(reg, readRegister(reg) >> 1);
		}

		setFlags(isZ, 0, 0, shifted);
	}


	/**
	* Check if a bit is 0 in a register.
	*/
	void CPU::CB_BIT(Register reg) {

		u8 isZ = 0;

		if (_dest_is_mem) {

			isZ = bit(_bus->read(_mem_dest), _curr_fetch);
			GameBoy::cycle(1);
		}
		else {
			isZ = bit(readRegister(reg), _curr_fetch);
		}
		setFlags(isZ == 0, 0, 1, -1);
	}


	/**
	* Reset a bit in a register.
	*/
	void CPU::CB_RES(Register reg) {

		if (_dest_is_mem) {

			u8 data = _bus->read(_mem_dest);
			GameBoy::cycle(1);
			_bus->write(_mem_dest, data & ~(1 << _curr_fetch));
			GameBoy::cycle(1);
		}
		else {

			writeRegister(reg, readRegister(reg) & ~(1 << _curr_fetch));
		}
	}


	/**
	* Set a bit in a register.
	*/
	void CPU::CB_SET(Register reg) {

		if (_dest_is_mem) {

			u8 data = _bus->read(_mem_dest);
			GameBoy::cycle(1);
			_bus->write(_mem_dest, data | (1 << _curr_fetch));
			GameBoy::cycle(1);
		}
		else {
			writeRegister(reg, readRegister(reg) | (1 << _curr_fetch));
		}
	}


	// Function Mapping
	const std::unordered_map<InstrType, CPU::ProcessFunction> CPU::_PROCESSES = {

		{InstrType::NONE, &CPU::XXX},
		{InstrType::NOP, &CPU::NOP},
		{InstrType::LD, &CPU::LD},
		{InstrType::INC, &CPU::INC},
		{InstrType::DEC, &CPU::DEC},
		{InstrType::RLCA, &CPU::RLCA},
		{InstrType::ADD, &CPU::ADD},
		{InstrType::RRCA, &CPU::RRCA},
		{InstrType::STOP, &CPU::XXX},
		{InstrType::RLA, &CPU::RLA},
		{InstrType::JR, &CPU::JR},
		{InstrType::RRA, &CPU::RRA},
		{InstrType::DAA, &CPU::DAA},
		{InstrType::CPL, &CPU::CPL},
		{InstrType::SCF, &CPU::SCF},
		{InstrType::CCF, &CPU::CCF},
		{InstrType::HALT, &CPU::XXX},
		{InstrType::ADC, &CPU::ADC},
		{InstrType::SUB, &CPU::SUB},
		{InstrType::SBC, &CPU::SBC},
		{InstrType::AND, &CPU::AND},
		{InstrType::XOR, &CPU::XOR},
		{InstrType::OR, &CPU::OR},
		{InstrType::CP, &CPU::CP},
		{InstrType::RET, &CPU::RET},
		{InstrType::POP, &CPU::POP},
		{InstrType::JP, &CPU::JP},
		{InstrType::CALL, &CPU::CALL},
		{InstrType::PUSH, &CPU::PUSH},
		{InstrType::RST, &CPU::RST},
		{InstrType::PREFIX, &CPU::PREFIX},
		{InstrType::RETI, &CPU::RETI},
		{InstrType::LDH, &CPU::LDH},
		{InstrType::DI, &CPU::DI},
		{InstrType::EI, &CPU::EI},
	};
}
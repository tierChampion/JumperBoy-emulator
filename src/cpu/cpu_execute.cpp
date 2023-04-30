#include <cpu.h>

#include <gb.h>

namespace jmpr {

	void CPU::execute() {

		ProcessFunction process = _PROCESSES.at(_current_instr->_type);

		(this->*process)();
	}

	void CPU::XXX() {

		std::cerr << "Error: Unknown instruction trying to execute." << std::endl;
		exit(-3);
	}

	void CPU::NOP() {}

	void CPU::LD() {

		if (_dest_is_mem) {

			// 16 bit registers need 2 writes
			if (is16Bits(_current_instr->_reg2)) {

				_bus->write16(_mem_dest, _current_fetch);
				GameBoy::cycle(1);
			}
			else {
				_bus->write(_mem_dest, _current_fetch);
			}
		}
		else {

			if (_current_instr->_mode == Mode::HL_SPR8) {

				u8 half_carry = ((readRegister(_current_instr->_reg2) & 0xF) >
					(_current_fetch & 0xF));

				u8 full_carry = ((readRegister(_current_instr->_reg2) & 0xFF) >
					(_current_fetch & 0xFF));

				setFlags(0, 0, half_carry, full_carry);
			}

			writeRegister(_current_instr->_reg1, _current_fetch);
		}
	}

	void CPU::INC() {

		if (_dest_is_mem) {

			_bus->write(_mem_dest, _current_fetch + 1);
			GameBoy::cycle(1);
		}
		else {
			writeRegister(_current_instr->_reg1, readRegister(_current_instr->_reg1) + 1);
		}

		bool is16 = is16Bits(_current_instr->_reg1);

		if (is16) {
			GameBoy::cycle(1);
		}

		if (!is16 || _dest_is_mem) {
			setFlags(readRegister(_current_instr->_reg1) == 0, 0,
				(readRegister(_current_instr->_reg1) & 0xF) == 0, -1);
		}
	}

	void CPU::DEC() {

		if (_dest_is_mem) {

			_bus->write(_mem_dest, _current_fetch - 1);
			GameBoy::cycle(1);
		}
		else {
			writeRegister(_current_instr->_reg1, readRegister(_current_instr->_reg1) - 1);
		}

		bool is16 = is16Bits(_current_instr->_reg1);

		if (is16) {
			GameBoy::cycle(1);
		}

		if (!is16 || _dest_is_mem) {
			setFlags(readRegister(_current_instr->_reg1) == 0, 1,
				(readRegister(_current_instr->_reg1) & 0xF) == 0xF, -1);
		}
	}

	void CPU::RLCA() {

		u8 c = bit(_registers._A, 7);
		_registers._A = (_registers._A << 1) | c;

		setFlags(0, 0, 0, c);
	}

	void CPU::ADD() {

		u32 val = readRegister(_current_instr->_reg1) + _current_fetch;

		bool is16 = is16Bits(_current_instr->_reg1);

		if (is16) {
			GameBoy::cycle(1);
		}

		bool toStackPointer = _current_instr->_reg1 == Register::SP;

		if (toStackPointer) {
			val = readRegister(_current_instr->_reg1) + s8(_current_fetch);
		}

		// 8 bit flags
		u8 z = (val & 0xFF) == 0;
		u8 h = (readRegister(_current_instr->_reg1) & 0xF) + (_current_fetch & 0xF) >= 0x10;
		u8 c = (readRegister(_current_instr->_reg1) & 0xFF) + (_current_fetch & 0xFF) >= 0x100;

		// regular 16 bit flags
		if (is16 && !toStackPointer) {
			z = -1;
			h = (readRegister(_current_instr->_reg1) & 0xFFF) + (_current_fetch & 0xFFF) >= 0x1000;
			c = (readRegister(_current_instr->_reg1) & 0xFFFF) +
				(_current_fetch & 0xFFFF) < readRegister(_current_instr->_reg1);
		}
		// special case
		else if (toStackPointer) {
			z = 0;
		}

		writeRegister(_current_instr->_reg1, val & 0xFFFF);
		setFlags(z, 0, h, c);
	}

	void CPU::RRCA() {

		u8 c = bit(_registers._A, 0);
		_registers._A = (_registers._A >> 1) | (c << 7);

		setFlags(0, 0, 0, c);
	}

	void CPU::STOP() { noImpl(); }

	void CPU::RLA() {

		u8 c = readFlag(3);

		u8 rotated_bit = bit(_registers._A, 7);

		_registers._A = (_registers._A << 1) | c;

		setFlags(0, 0, 0, rotated_bit);
	}

	void CPU::JR() {

		if (checkFlags(_current_instr->_cond)) {
			// check if it works
			_PC += (s8)_current_fetch;
			GameBoy::cycle(1);
		}
	}

	void CPU::RRA() {

		u8 c = readFlag(3);

		u8 rotated_bit = bit(_registers._A, 0);

		_registers._A = (_registers._A >> 1) | (c << 7);

		setFlags(0, 0, 0, rotated_bit);
	}

	void CPU::DAA() {

		s8 correction = 0;
		u8 full_carry = 0;

		// Fix BCD decimal if there is a half carry or the value is not legal
		if ((readFlag(1) && (_registers._A & 0xF) > 0x9) || readFlag(2)) {
			correction = 0x6;
		}

		if ((!readFlag(1) && (_registers._A & 0xF0) > 0x90) || readFlag(3)) {
			correction |= 0x60;
			full_carry = 1;
		}

		if (readFlag(1)) {
			correction *= -1;
		}

		_registers._A += correction;

		setFlags(_registers._A == 0, -1, 0, full_carry);
	}

	void CPU::CPL() {

		_registers._A = ~_registers._A;
		setFlags(-1, 1, 1, -1);
	}

	void CPU::SCF() {

		setFlags(-1, 0, 0, 1);
	}

	void CPU::CCF() {

		bool C = checkFlags(_current_instr->_cond);

		setFlags(-1, 0, 0, ~(bool)readFlag(3));
	}

	void CPU::HALT() { noImpl(); }

	void CPU::ADC() {

		_registers._A += (readFlag(3) + _current_fetch) & 0xFF;

		u8 half_carry = (_registers._A & 0xF) <
			((_current_fetch + readFlag(3)) & 0xF);

		u8 full_carry = (_registers._A & 0xFF) <
			((_current_fetch + readFlag(3)) & 0xFF);

		setFlags(_registers._A == 0, 0, half_carry, full_carry);
	}

	void CPU::SUB() {

		_registers._A -= _current_fetch;

		u8 half_carry = (_registers._A & 0xF) >
			(_current_fetch & 0xF);

		u8 full_carry = (_registers._A & 0xFF) >
			(_current_fetch & 0xFF);

		setFlags(_registers._A == 0, 1, half_carry, full_carry);
	}

	void CPU::SBC() {

		_registers._A -= (readFlag(3) + _current_fetch) & 0xFF;

		u8 half_carry = (_registers._A & 0xF) >
			((_current_fetch + readFlag(3)) & 0xF);

		u8 full_carry = (_registers._A & 0xFF) >
			((_current_fetch + readFlag(3)) & 0xFF);

		setFlags(_registers._A == 0, 1, half_carry, full_carry);
	}

	void CPU::AND() {

		_registers._A &= (_current_fetch & 0xFF);
		setFlags(_registers._A == 0, 0, 1, 0);
	}

	void CPU::XOR() {

		_registers._A ^= (_current_fetch & 0xFF);
		setFlags(_registers._A == 0, 0, 0, 0);
	}

	void CPU::OR() {

		_registers._A |= (_current_fetch & 0xFF);
		setFlags(_registers._A == 0, 0, 0, 0);
	}

	void CPU::CP() {

		u8 result = _registers._A - (_current_fetch & 0xFF);
		u8 half_res = (_registers._A & 0xF) - (_current_fetch & 0xF);

		setFlags(_registers._A == 0, 1, half_res > (_registers._A & 0xF), result > _registers._A);
	}

	void CPU::RET() {

		if (checkFlags(_current_instr->_cond)) {

			u8 lo = popStack8();
			GameBoy::cycle(1);
			u8 hi = popStack8();
			GameBoy::cycle(1);

			printf("Popped %04X from the stack.\n", merge(hi, lo));

			_PC = merge(hi, lo);
		}
	}

	void CPU::POP() {

		u8 lo = popStack8();
		GameBoy::cycle(1);
		u8 hi = popStack8();
		GameBoy::cycle(1);

		writeRegister(_current_instr->_reg1, merge(hi, lo));
	}

	void CPU::JP() {

		if (checkFlags(_current_instr->_cond)) {
			_PC = _current_fetch;
			GameBoy::cycle(1);
		}
	}

	void CPU::CALL() {

		if (checkFlags(_current_instr->_cond)) {

			pushStack8(hiByte(_PC));
			GameBoy::cycle(1);
			pushStack8(loByte(_PC));
			GameBoy::cycle(1);

			printf("Pushed %04X to the stack.\n", _PC);

			_PC = _current_fetch;

			printf("Jumped to %04X.\n", _PC);
		}
	}

	void CPU::PUSH() {

		pushStack8(hiByte(_current_fetch));
		GameBoy::cycle(1);
		pushStack8(loByte(_current_fetch));
		GameBoy::cycle(1);
	}

	void CPU::RST() {

		pushStack8(hiByte(_PC));
		GameBoy::cycle(1);
		pushStack8(hiByte(_PC));
		GameBoy::cycle(1);

		// to see
		u8 addr = ((_current_opcode & 0x30)) | (_current_opcode & 0x8) << 3;

		_PC = addr;
	}

	void CPU::PREFIX() {

		// CB Instruction to execute
		_current_opcode = _current_fetch;

		Register reg;

		// Register affected
		switch (_current_opcode & 0x7) {
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
		case 7: reg = Register::A; break;
		}

		// Bit checked / affected
		_current_fetch = 2 * ((_current_opcode & 0x40) >> 8) + ((_current_opcode & 0x8) >> 3);

		// RLC
		if (_current_opcode < 0x08) {
			CB_RLC();
		}
		else if (_current_opcode < 0x10) {
			CB_RRC();
		}
		else if (_current_opcode < 0x18) {
			CB_RL();
		}
		else if (_current_opcode < 0x20) {
			CB_RR();
		}
		else if (_current_opcode < 0x28) {
			CB_SLA();
		}
		else if (_current_opcode < 0x30) {
			CB_SRA();
		}
		else if (_current_opcode < 0x38) {
			CB_SWAP();
		}
		else if (_current_opcode < 0x40) {
			CB_SRL();
		}
		else if (_current_opcode < 0x80) {
			CB_BIT();
		}
		else if (_current_opcode < 0xC0) {
			CB_RES();
		}
		else {
			CB_SET();
		}
	}

	void CPU::RETI() {

		u8 lo = popStack8();
		GameBoy::cycle(1);
		u8 hi = popStack8();
		GameBoy::cycle(1);

		_PC = merge(hi, lo);

		_IME = true;
	}


	void CPU::LDH() {

		if (_dest_is_mem) {

			_bus->write(_mem_dest, _current_fetch);
		}
		else {

			writeRegister(_current_instr->_reg1, _current_fetch);
		}

		GameBoy::cycle(1);
	}

	void CPU::DI() {
		_IME = false;
	}

	void CPU::EI() {
		_IME = true;
	}

	void CPU::CB_RLC(Register reg) { noImpl(); }
	void CPU::CB_RRC(Register reg) { noImpl(); }
	void CPU::CB_RL(Register reg) { noImpl(); }
	void CPU::CB_RR(Register reg) { noImpl(); }


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


	void CPU::CB_SWAP(Register reg) {

		u8 hi = 0;
		u8 lo = 0;

		if (_dest_is_mem) {

			u8 data = _bus->read(_mem_dest);
			GameBoy::cycle(1);

			hi = hiByte(data);
			lo = loByte(data);

			_bus->write(_mem_dest, merge(lo, hi));
			GameBoy::cycle(1);
		}
		else {
			hi = hiByte(readRegister(reg));
			lo = loByte(readRegister(reg));

			writeRegister(reg, merge(lo, hi));
		}

		setFlags(hi == 0 && lo == 0, 0, 0, 0);
	}


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


	void CPU::CB_BIT(Register reg) {

		u8 isZ = 0;

		if (_dest_is_mem) {

			isZ = bit(_bus->read(_mem_dest), _current_fetch);
			GameBoy::cycle(1);
		}
		else {
			isZ = bit(readRegister(reg), _current_fetch);
		}
		setFlags(isZ == 0, 0, 1, -1);
	}


	void CPU::CB_RES(Register reg) {

		if (_dest_is_mem) {

			u8 data = _bus->read(_mem_dest);
			GameBoy::cycle(1);
			_bus->write(_mem_dest, data & ~(1 << _current_fetch));
			GameBoy::cycle(1);
		}
		else {

			writeRegister(reg, readRegister(reg) & ~(1 << _current_fetch));
		}
	}


	void CPU::CB_SET(Register reg) {

		if (_dest_is_mem) {

			u8 data = _bus->read(_mem_dest);
			GameBoy::cycle(1);
			_bus->write(_mem_dest, data | (1 << _current_fetch));
			GameBoy::cycle(1);
		}
		else {
			writeRegister(reg, readRegister(reg) | (1 << _current_fetch));
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
		//	{InstrType::STOP, &CPU::XXX},
		{InstrType::RLA, &CPU::RLA},
		{InstrType::JR, &CPU::JR},
		{InstrType::RRA, &CPU::RRA},
		{InstrType::DAA, &CPU::DAA},
		{InstrType::CPL, &CPU::CPL},
		{InstrType::SCF, &CPU::SCF},
		{InstrType::CCF, &CPU::CCF},
		//	{InstrType::HALT, &CPU::XXX},
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
		//	{InstrType::PREFIX, &CPU::XXX},
		{InstrType::RETI, &CPU::RETI},
		{InstrType::LDH, &CPU::LDH},
		{InstrType::DI, &CPU::DI},
		{InstrType::EI, &CPU::EI},
	};
}
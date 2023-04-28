#include <cpu.h>

#include <gb.h>

namespace jmpr {

	void CPU::execute() {

		ProcessFunction process = _PROCESSES.at(_current_instr->_type);

		(this->*process)();
	}

	void CPU::XXX() {

		std::cerr << "Error: Unknown instruction trying to execute." << std::endl;
		//	exit(-3);
	}

	void CPU::NOP() {}

	void CPU::LD() {

		if (_dest_is_mem) {

			// 16 bit registers need 2 writes
			if (is16Bits(_current_instr->_reg2)) {

				GameBoy::cycle(1);
				_bus->write16(_mem_dest, _current_fetch);
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

		noImpl(); // todo
	}

	void CPU::DEC() {

		noImpl(); // todo
	}

	void CPU::RLCA() { noImpl(); }
	void CPU::ADD() { noImpl(); } // special case for sp, r8
	void CPU::RRCA() { noImpl(); }
	void CPU::STOP() { noImpl(); }
	void CPU::RLA() { noImpl(); }

	void CPU::JR() {

		if (checkFlags(_current_instr->_cond)) {
			// check if it works
			_PC += (s8)_current_fetch;
			GameBoy::cycle(1);
		}
	}

	void CPU::RRA() { noImpl(); }
	void CPU::DAA() { noImpl(); }
	void CPU::CPL() { noImpl(); }
	void CPU::SCF() { noImpl(); }
	void CPU::CCF() { noImpl(); }
	void CPU::HALT() { noImpl(); }
	void CPU::ADC() { noImpl(); }
	void CPU::SUB() { noImpl(); }
	void CPU::SBC() { noImpl(); }
	void CPU::AND() { noImpl(); }

	void CPU::XOR() {

		_registers._A ^= (_current_fetch & 0xFF);
		setFlags(_registers._A == 0, 0, 0, 0);

	}

	void CPU::OR() { noImpl(); }
	void CPU::CP() { noImpl(); }

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

	void CPU::PREFIX() { noImpl(); }

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

	void CPU::CB_ERR() { noImpl(); }
	void CPU::CB_RLC() { noImpl(); }
	void CPU::CB_RRC() { noImpl(); }
	void CPU::CB_RL() { noImpl(); }
	void CPU::CB_RR() { noImpl(); }
	void CPU::CB_SLA() { noImpl(); }
	void CPU::CB_SRA() { noImpl(); }
	void CPU::CB_SWAP() { noImpl(); }
	void CPU::CB_SRL() { noImpl(); }
	void CPU::CB_BIT() { noImpl(); }
	void CPU::CB_RES() { noImpl(); }
	void CPU::CB_SET() { noImpl(); }


	// Function Mapping
	const std::unordered_map<InstrType, CPU::ProcessFunction> CPU::_PROCESSES = {

		{InstrType::NONE, &CPU::XXX},
		{InstrType::NOP, &CPU::NOP},
		{InstrType::LD, &CPU::LD},
		{InstrType::INC, &CPU::INC},
		{InstrType::DEC, &CPU::DEC},
		//	{InstrType::RLCA, &CPU::XXX},
		//	{InstrType::ADD, &CPU::XXX},
		//	{InstrType::RRCA, &CPU::XXX},
		//	{InstrType::STOP, &CPU::XXX},
		//	{InstrType::RLA, &CPU::XXX},
		{InstrType::JR, &CPU::JR},
		//	{InstrType::RRA, &CPU::XXX},
		//	{InstrType::DAA, &CPU::XXX},
		//	{InstrType::CPL, &CPU::XXX},
		//	{InstrType::SCF, &CPU::XXX},
		//	{InstrType::CCF, &CPU::XXX},
		//	{InstrType::HALT, &CPU::XXX},
		//	{InstrType::ADC, &CPU::XXX},
		//	{InstrType::SUB, &CPU::XXX},
		//	{InstrType::SBC, &CPU::XXX},
		//	{InstrType::AND, &CPU::XXX},
		{InstrType::XOR, &CPU::XOR},
		//	{InstrType::OR, &CPU::XXX},
		//	{InstrType::CP, &CPU::XXX},
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
		//	{InstrType::CB_ERR, &CPU::XXX},
		//	{InstrType::CB_RLC, &CPU::XXX},
		//	{InstrType::CB_RRC, &CPU::XXX},
		//	{InstrType::CB_RL, &CPU::XXX},
		//	{InstrType::CB_RR, &CPU::XXX},
		//	{InstrType::CB_SLA, &CPU::XXX},
		//	{InstrType::CB_SRA, &CPU::XXX},
		//	{InstrType::CB_SWAP, &CPU::XXX},
		//	{InstrType::CB_SRL, &CPU::XXX},
		//	{InstrType::CB_BIT, &CPU::XXX},
		//	{InstrType::CB_RES, &CPU::XXX},
		//	{InstrType::CB_SET, &CPU::XXX},
	};
}
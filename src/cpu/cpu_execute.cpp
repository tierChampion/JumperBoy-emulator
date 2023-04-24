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

	void CPU::LD() { noImpl(); }
	void INC() { noImpl(); }
	void DEC() { noImpl(); }
	void RLCA() { noImpl(); }
	void ADD() { noImpl(); }
	void RRCA() { noImpl(); }
	void STOP() { noImpl(); }
	void RLA() { noImpl(); }
	void JR() { noImpl(); }
	void RRA() { noImpl(); }
	void DAA() { noImpl(); }
	void CPL() { noImpl(); }
	void SCF() { noImpl(); }
	void CCF() { noImpl(); }
	void HALT() { noImpl(); }
	void ADC() { noImpl(); }
	void SUB() { noImpl(); }
	void SBC() { noImpl(); }
	void AND() { noImpl(); }

	void CPU::XOR() {

		_registers._A ^= (_current_fetch & 0xFF);
		setFlags(_registers._A == 0, 0, 0, 0);

	}

	void OR() { noImpl(); }
	void CP() { noImpl(); }
	void RET() { noImpl(); }
	void POP() { noImpl(); }

	void CPU::JP() {

		if (checkFlags(_current_instr->_cond)) {
			_PC = _current_fetch;
			GameBoy::cycle(1);
		}
	}

	void CALL() { noImpl(); }
	void PUSH() { noImpl(); }
	void RST() { noImpl(); }
	void PREFIX() { noImpl(); }
	void RETI() { noImpl(); }
	void LDH() { noImpl(); }

	void CPU::DI() {
		_IME = false;
	}

	void EI() { noImpl(); }
	void CB_ERR() { noImpl(); }
	void CB_RLC() { noImpl(); }
	void CB_RRC() { noImpl(); }
	void CB_RL() { noImpl(); }
	void CB_RR() { noImpl(); }
	void CB_SLA() { noImpl(); }
	void CB_SRA() { noImpl(); }
	void CB_SWAP() { noImpl(); }
	void CB_SRL() { noImpl(); }
	void CB_BIT() { noImpl(); }
	void CB_RES() { noImpl(); }
	void CB_SET() { noImpl(); }


	// Function Mapping
	const std::unordered_map<InstrType, CPU::ProcessFunction> CPU::_PROCESSES = {
		{InstrType::NONE, &CPU::XXX},
		{InstrType::NOP, &CPU::NOP},
		{InstrType::LD, &CPU::LD},

		{InstrType::XOR, &CPU::XOR},

		{InstrType::JP, &CPU::JP},
		{InstrType::DI, &CPU::DI},
	};
}
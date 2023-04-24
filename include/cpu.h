#pragma once

#include <common.h>
#include <instructions.h>

#include <unordered_map>


namespace jmpr {

	class GameBoy;
	class Bus;

	struct CpuRegisters {
		u8 _A; // Accumulator
		u8 _F; // Flags: Zero Flag, Substraction Flag, Half Carry Flag, Carry Flag
		u8 _B;
		u8 _C;
		u8 _D;
		u8 _E;
		u8 _H;
		u8 _L;
	};

	class CPU {

		CpuRegisters _registers;

		u16 _SP; // Stack pointer
		u16 _PC; // Program counter

		Bus* _bus;

		// Current Instruction
		u8 _current_opcode;
		const Instruction* _current_instr;
		// Instruction information
		u16 _current_fetch;
		u16 _mem_dest;
		bool _dest_is_mem;

		// Status
		bool _IME;
		bool _IME_scheduled;
		bool _halted;
		bool _stepping;

	public:

		CPU();

		void connectBus(Bus* bus) { _bus = bus; }

		u16 readRegister(Register reg) const;
		void writeRegister(Register reg, u16 data);
		void setFlags(u8 Z, u8 N, u8 H, u8 C);
		bool checkFlags(Condition cond);

		void fetchOpcode();
		void fetchData();
		void execute();
		bool cycle();

	private:

		// INSTRUCTION FUNCTIONS
		void XXX();
		void NOP();
		void LD();
		void INC();
		void DEC();
		void RLCA();
		void ADD();
		void RRCA();
		void STOP();
		void RLA();
		void JR();
		void RRA();
		void DAA();
		void CPL();
		void SCF();
		void CCF();
		void HALT();
		void ADC();
		void SUB();
		void SBC();
		void AND();
		void XOR();
		void OR();
		void CP();
		void RET();
		void POP();
		void JP();
		void CALL();
		void PUSH();
		void RST();
		void PREFIX();
		void RETI();
		void LDH();
		void DI();
		void EI();
		void CB_ERR();
		void CB_RLC();
		void CB_RRC();
		void CB_RL();
		void CB_RR();
		void CB_SLA();
		void CB_SRA();
		void CB_SWAP();
		void CB_SRL();
		void CB_BIT();
		void CB_RES();
		void CB_SET();

		// INSTRUCTION FUNCTIONS TABLE
		typedef void(CPU::*ProcessFunction)();
		static const std::unordered_map<InstrType, ProcessFunction> _PROCESSES;
	};
}

#pragma once

#include <common.h>

#include <cpu/instructions.h>
#include <cpu/interrupt.h>

#include <unordered_map>


namespace jmpr {

	class GameBoy;
	class Bus;
	class RAM;
	class InterruptHandler;

	struct CpuRegisters {
		u8 _A; // Accumulator

		// Flags: Zero Flag (result is 0), Substraction Flag (if the operation removes),
		// Half-Carry Flag (if there is a carry in the before last 4 bits) and 
		// Carry Flag (if there is a carry at the end)
		u8 _F;
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
		u8 _curr_opcode;
		const Instruction* _curr_instr;

		// Instruction information
		u16 _curr_fetch;
		u16 _mem_dest;
		bool _dest_is_mem;

		// Interrupts
		InterruptHandler _it_handler;

		// Status
		bool _halted;
		bool _stopped;

	public:

		CPU();

		void connectBus(Bus* bus) { _bus = bus; }

		InterruptHandler* getInterruptHandler() { return &_it_handler; }

		u16 readRegister(Register reg) const;
		void writeRegister(Register reg, u16 data);

		void setFlags(u8 Z, u8 N, u8 H, u8 C);
		u8 zeroFlag() const;
		u8 negationFlag() const;
		u8 halfCarryFlag() const;
		u8 carryFlag() const;

		bool checkFlags(Condition cond) const;

		void fetchOpcode();
		void fetchData();
		void execute();
		bool cycle();
		bool reached(u16 address);
		u8 getOpcode() { return _curr_opcode; }

		u8 readInterruptEnabledRegister();
		void writeInterruptEnabledRegister(u8 data);
		u8 readInterruptFlagRegister();
		void writeInterruptFlagRegister(u8 data);
		void executeInterrupt(bool enabled, InterruptType type, u16 location);

		void displayCurrentInstruction() const;

		// Debugging test
		void testDAA();
		void testADC();
		void testSBC();
		void testSRA();

	private:


		// STACK OPERATIONS
		void pushStack8(u8 data);
		void pushStack16(u16 data);
		u8 popStack8();
		u16 popStack16();

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
		// BINARY INSTRUCTIONS
		void CB_RLC(Register reg);
		void CB_RRC(Register reg);
		void CB_RL(Register reg);
		void CB_RR(Register reg);
		void CB_SLA(Register reg);
		void CB_SRA(Register reg);
		void CB_SWAP(Register reg);
		void CB_SRL(Register reg);
		void CB_BIT(Register reg);
		void CB_RES(Register reg);
		void CB_SET(Register reg);

		// INSTRUCTION FUNCTIONS TABLE
		typedef void(CPU::*ProcessFunction)();
		static const std::unordered_map<InstrType, ProcessFunction> _PROCESSES;
	};
}

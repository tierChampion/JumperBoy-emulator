#pragma once

#include <common.h>
#include <instructions.h>


namespace jmpr {

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

		u16 _current_fetch;
		u16 _mem_dest;
		u8 _current_opcode;
		const Instruction* _current_instr;

		bool _halted;
		bool _stepping;


	public:

		CPU();

		void connectBus(Bus* bus) { _bus = bus; }

		u16 readRegister(Register reg) const;

		void fetchOpcode();
		void fetchData();
		void execute();
		bool cycle();
	};
}

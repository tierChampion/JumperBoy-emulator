#include <cpu/cpu.h>

#include <gb.h>
#include <bus.h>
#include <ram.h>

#include <SDL/SDL.h>

namespace jmpr {

	//#define LOGGING

	CPU::CPU() {

		// Initial DMG register values. See TCAGBD.pdf for more info.
		_registers._A = 0x01;
		_registers._F = 0xB0;
		_registers._B = 0x00;
		_registers._C = 0x13;
		_registers._D = 0x00;
		_registers._E = 0xD8;
		_registers._H = 0x01;
		_registers._L = 0x4D;
		_SP = 0xFFFE;
		_PC = 0x100;

		_curr_fetch = 0x0000;
		_mem_dest = 0x0000;
		_dest_is_mem = false;
		_curr_opcode = 0x00;
		_curr_instr = nullptr;

		_bus = nullptr;

		_inter_handler = InterruptHandler(this);

		_halted = false;
		_stopped = false;
	}

	/**
	* Get the opcode of the instruction to execute.
	*/
	void CPU::fetchOpcode() {

		_curr_opcode = _bus->read(_PC);
		GameBoy::cycle(1);

		// Halt bug, fails to increment PC
		if (!_inter_handler.haltBugged(_PC)) {
			_PC++;
		}

		_curr_instr = fromOpcode(_curr_opcode);
	}

	/**
	* Execute a single CPU instruction. Might take up multiple M-cycles.
	*/
	bool CPU::cycle() {

		_inter_handler.checkInterrupts();

		if (!_halted && !_stopped) {

			u16 programCounter = _PC;

			fetchOpcode();

			fetchData();

#ifdef LOGGING
			printf("%04X: ", programCounter);
			displayCurrentInstruction();
#endif

			execute();
		}
		else if (!_stopped) {

			GameBoy::cycle(1);
		}

		return true;
	}

	bool CPU::reached(u16 address) {
		return _PC == address;
	}
}
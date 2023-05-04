#include <cpu.h>

#include <bus.h>
#include <ram.h>

namespace jmpr {

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
		_stepping = false; // todo?
	}

	/**
	* Get the opcode of the instruction to execute.
	*/
	void CPU::fetchOpcode() {

		_curr_opcode = _bus->read(_PC);

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

		// todo verify that it goes here
		_inter_handler.checkInterrupts();

		if (!_halted) {

			u16 programCounter = _PC;

			fetchOpcode();

			printf("%04X: %s (%02X %02X %02X)",
				programCounter, instructionName(*_curr_instr), int(_curr_opcode), int(_bus->read(programCounter + 1)),
				int(_bus->read(programCounter + 2)));

			fetchData();
			execute();

			printf(" => AF: %02X%02X, BC: %02X%02X, DE: %02X%02X, HL: %02X%02X\n", _registers._A, _registers._F, _registers._B,
				_registers._C, _registers._D, _registers._E, _registers._H, _registers._L);
		}

		return true;
	}
}
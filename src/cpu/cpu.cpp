#include <cpu/cpu.h>

#include <gb.h>
#include <bus.h>
#include <ram.h>

#include <SDL2/SDL.h>

namespace jmpr
{
	// #define LOGGING

	CPU::CPU()
	{

		_bus = nullptr;
		_it_handler = InterruptHandler(this);

		// reboot();
	}

	/**
	 * Reboot the CPU and resets the registers to their initial values.
	 */
	void CPU::reboot()
	{

		// Initial DMG register values. See TCAGBD.pdf for more info.
		if (!GameBoy::getInstance()->isCGB())
		{
			_registers._A = 0x11;
			_registers._F = 0xB0;
			_registers._B = 0x00;
			_registers._C = 0x00;
			_registers._D = 0x00;
			_registers._E = 0x08;
			_registers._H = 0x00;
			_registers._L = 0x00;
		}
		else
		{
			_registers._A = 0x11;
			_registers._F = 0xB0;
			_registers._B = 0x00;
			_registers._C = 0x00;
			_registers._D = 0xFF;
			_registers._E = 0x56;
			_registers._H = 0x00;
			_registers._L = 0x0D;
		}
		_SP = 0xFFFE;
		_PC = 0x100;

		// Fetching states
		_curr_fetch = 0x0000;
		_mem_dest = 0x0000;
		_dest_is_mem = false;
		_curr_opcode = 0x00;
		_curr_instr = nullptr;

		// Cycling state
		_halted = false;
		_stopped = false;

		_it_handler.reboot();
	}

	/**
	 * Get the opcode of the instruction to execute.
	 */
	void CPU::fetchOpcode()
	{

		_curr_opcode = _bus->read(_PC);
		GameBoy::getInstance()->cycle(1);

		// Halt bug, fails to increment PC
		if (!_it_handler.haltBugged(_PC))
		{
			_PC++;
		}

		_curr_instr = fromOpcode(_curr_opcode);
	}

	/**
	 * Execute a single CPU instruction. Might take up multiple M-cycles.
	 */
	bool CPU::cycle()
	{
		_it_handler.checkInterrupts();

		if (!_halted && !_vdma->inProcess() && !_stopped)
		{

			u16 programCounter = _PC;

			fetchOpcode();

			fetchData();

#ifdef LOGGING
			printf("%04X: ", programCounter);
			displayCurrentInstruction();
#endif

			execute();
		}
		else if (!_stopped)
		{
			GameBoy::getInstance()->cycle(1);
		}

		return true;
	}

	bool CPU::reached(u16 address)
	{
		return _PC == address;
	}
}
#include <cpu.h>


namespace jmpr {

	void CPU::execute() {

		ProcessFunction process = _PROCESSES.at(_current_instr->_type);

		(this->*process)();
	}

	/**
	* Error Process
	*/
	void CPU::XXX() {

		std::cerr << "Error: Unknown instruction trying to execute." << std::endl;
		exit(-3);
	}

	/**
	* No Operation
	*/
	void CPU::NOP() {}


	const std::unordered_map<InstrType, CPU::ProcessFunction> CPU::_PROCESSES = {
		{InstrType::NONE, &CPU::XXX},
		{InstrType::NOP, &CPU::NOP},
	};
}
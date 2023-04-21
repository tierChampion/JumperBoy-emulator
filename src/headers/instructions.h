#pragma once

#include <headers/common.h>

// Opcodes: https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html

namespace jmpr {

	struct InstrType {};

	struct Mode {};

	struct Register {};

	struct Condition {};

	struct Instruction {

		InstrType _type;
		Condition _cond;

		Mode _mode;
		Register _reg1;
		Register _reg2;

		u8 _length;
		u8 _cycles;

		u8 _affected_flags;

	};

}
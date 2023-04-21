#pragma once

#include <cinttypes>
#include <iostream>

namespace jmpr {

	// Unsigned types
	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

	// Signed types
	using s8 = int8_t;
	using s16 = int16_t;

	void delay(u32 milliseconds);

	inline u16 merge(u8 high, u8 low) {
		return high << 8 | low;
	}

	inline void noImpl() {
		std::cout << "NOT YET IMPLEMENTED" << std::endl;
	}
}
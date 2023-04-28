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

	inline u16 merge(u8 high, u8 low) {
		return (high << 8) | low;
	}

	inline u8 bit(u8 byte, u8 pos) {
		return (byte & (1 << pos)) >> pos;
	}

	inline u8 hiByte(u16 bytes) {
		return (bytes >> 8) & 0xFF;
	}

	inline u8 loByte(u16 bytes) {
		return bytes & 0xFF;
	}

	inline void notImplemented(const char* file, int line) {
		std::cout << file << " ( line " << std::dec << line << " ): " << "NOT YET IMPLEMENTED" << std::endl;
	}

#define noImpl() notImplemented(__FILE__, __LINE__)
}
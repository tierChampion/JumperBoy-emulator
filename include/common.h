#pragma once

#include <cinttypes>
#include <iostream>
#include <memory>

#ifdef __linux__
#include <unistd.h>
#endif
namespace jmpr {

	// Unsigned types
	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

	// Signed types
	using s8 = int8_t;
	using s16 = int16_t;

	inline u16 merge16(u8 high, u8 low) {
		return (high << 8) | low;
	}

	inline u8 merge8(u8 high, u8 low) {
		return ((high & 0xF) << 4) | (low & 0xF);
	}

	inline u8 bit(u8 byte, u8 pos) {
		return (byte & (1 << pos)) >> pos;
	}

	inline u8 set(u8 byte, u8 pos) {
		return byte | (1 << pos);
	}

	inline u8 reset(u8 byte, u8 pos) {
		return byte & ~(1 << pos);
	}

	inline u8 setBit(u8 byte, u8 pos, u8 val) {
		return byte ^ ((-val ^ byte) & (1 << pos));
	}

	inline u8 hiByte(u16 bytes) {
		return (bytes >> 8) & 0xFF;
	}

	inline u8 loByte(u16 bytes) {
		return bytes & 0xFF;
	}

	inline u8 hiNibble(u8 byte) {
		return (byte >> 4) & 0xF;
	}

	inline u8 loNibble(u8 byte) {
		return byte & 0xF;
	}

	inline bool between(u16 val, u16 lo, u16 hi) {
		return val >= lo && val <= hi;
	}

	inline void notImplemented(const char* file, int line) {
		std::cout << file << " ( line " << std::dec << line << " ): " << "NOT YET IMPLEMENTED" << std::endl;
	}

#define noImpl() notImplemented(__FILE__, __LINE__)

#ifdef _WIN32
#define simpleSleep() _sleep(1)
#elif defined(__linux__)
#define simpleSleep() usleep(1)
#endif
}
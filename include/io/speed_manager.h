#pragma once

#include <common.h>

namespace jmpr
{
	class SpeedManager
	{
		u8 _key_0;
		u8 _key_1;

	public:
		SpeedManager();
		void reboot();
		bool isDoubleSpeed();
		u8 getKey1();
		void requestSpeedSwitch();
		void toggleSpeed();

	private:
		bool isSwitchRequested();
	};
}
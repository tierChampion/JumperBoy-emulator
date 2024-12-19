#include <ppu/dma.h>

#include <bus.h>
#include <ppu/vmem.h>

#include <algorithm>

namespace jmpr
{

	ObjectDMA::ObjectDMA(OAM *oam)
	{
		_bus = nullptr;
		_oam = oam;

		reboot();
	}

	void ObjectDMA::reboot()
	{
		_dma = 0xFF;
		_process_timer = 0x00;
	}

	bool ObjectDMA::inProcess()
	{
		return _process_timer > 0;
	}

	void ObjectDMA::requestDMA(u8 source)
	{
		_dma = source;		   // MSB of the address to copie to OAM.
		_process_timer = 0xA0; // process is 160 cpu cycles long
	}

	void ObjectDMA::processDMA()
	{
		if (inProcess())
		{
			u8 index = 0xA0 - _process_timer;
			u16 readAddress = (_dma * 0x100) + index;

			_oam->write(0xFE00 + index, _bus->read(readAddress), true);

			_process_timer--;
		}
	}

	u8 ObjectDMA::readDMA() const
	{
		return _dma;
	}

	VideoDMA::VideoDMA(VRAM *vram)
	{
		_bus = nullptr;
		_vram = vram;
		reboot();
	}

	void VideoDMA::reboot()
	{
		_mode = 0;
		_current_length = 0;
		_total_length = 0;
		_elapsed_length = 0;
		_source = 0x9FF0;
		_destination = 0xDFF0;
	}

	bool VideoDMA::inProcess() { return _current_length > 0; }

	void VideoDMA::write(u8 address, u8 data)
	{
		switch (address)
		{
		case 0x51:
			_source = (_source & 0x00FF) | (data << 8);
			break;
		case 0x52:
			_source = (_source & 0xFF00) | (data);
			break;
		case 0x53:
			_destination = (_source & 0x00FF) | (data << 8);
			break;
		case 0x54:
			_destination = (_source & 0xFF00) | (data);
			break;
		case 0x55:
			requestDMA(data);
			break;
		default:
			break;
		}
	}

	void VideoDMA::requestDMA(u8 source)
	{
		// std::cout << "VDMA REQUEST: " << int(bit(source, 7)) << std::endl;
		if (_mode == 1 && _total_length > 0 && bit(source, 7) == 0)
		{
			_total_length = 0;
			_current_length = 0;
		}
		else
		{
			_elapsed_length = 0;
			_mode = bit(source, 7);
			_total_length = ((source & 0x7F) + 1) * 0x10;
			if (_mode == 0)
			{
				_current_length = _total_length;
			}
			else
			{
				continueHBlankDMA();
			}
		}
	}

	void VideoDMA::processDMA()
	{
		if (inProcess())
		{
			// do the transfer (2 bytes)
			u16 sourceAddress = (_source & 0xFFF0) + _elapsed_length;
			u16 destinationAddress = ((_destination & 0x1FF0) | 0x8000) + _elapsed_length;

			u8 i = 0;
			while (_current_length > 0 && i < 2)
			{
				_vram->write(destinationAddress, _bus->read(sourceAddress));

				_current_length--;
				_total_length--;
				_elapsed_length++;
				sourceAddress++;
				destinationAddress++;
				i++;
			}

			if (_total_length == 0)
			{
				_source = 0xFFFF;
				_destination = 0xFFFF;
			}
		}
	}

	void VideoDMA::continueHBlankDMA()
	{
		if (_mode == 1)
		{
			_current_length = std::min((u16)0x0010, _total_length);
		}
	}

	u8 VideoDMA::readDMA() const
	{
		return (_total_length / 0x10) - 1;
	}
}
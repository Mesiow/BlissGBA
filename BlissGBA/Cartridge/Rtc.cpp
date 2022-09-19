#include "Rtc.h"

RtcDevice::RtcDevice(MemoryBus* mbus)
	:mbus(mbus)
{
	gpio.data_register = 0;
	gpio.direction_register = 0;
	gpio.control_register = 0;

	rtc_regs = { 0 };
}

u8 RtcDevice::read(GpioAddress address)
{
	if ((gpio.control_register & 0x1) == 0)
		return 0x0;

	switch (address) {
		case GpioAddress::Data: {
			printf("!!RTC Data read\n");
			//if bit is "in (value == 0)" in the direction
			//register, then we can read from the data register
			u8 rtc_data_value = 0;
			for (u8 bit = 0; bit < 4; bit++) {
				if (testBit(gpio.direction_register & 0xF, bit) == IN) {
					rtc_data_value |= (gpio.data_register & (1 << bit));
				}
			}
			return rtc_data_value & 0xF;

		}break;
		case GpioAddress::Direction: return gpio.direction_register & 0xF;
		case GpioAddress::Control: return gpio.control_register & 0x1;
	}
}

void RtcDevice::write(GpioAddress address, u8 value)
{
	switch (address) {
		case GpioAddress::Data: {
		
			u8 prev_data_register = gpio.data_register & 0xF;
			for (u8 bit = 0; bit < 4; bit++) {
				if (testBit(gpio.direction_register & 0xF, bit) == OUT) {
					//write to data register sck, sio, cs
					u8 vbit = (value >> bit) & 0x1;
					gpio.data_register |= (vbit << bit);
				}	
			}

			bool sck_is_low = ((prev_data_register & 0x1) == 0);
			bool cs_is_high = (((prev_data_register >> 2) & 0x1) == 1);

			//if sck was high and cs just rised, begin a new command
			if (!sck_is_low && ((gpio.data_register >> 2) & 0x1) == 0x1) {
				if (transfer_in_progress == false)
					transfer_in_progress = true;
			}

			if (transfer_in_progress) {
				//Sck rising
				if (sck_is_low && ((gpio.data_register & 0x1) == 1)) {
					//sample sio bit
					u8 sio = ((gpio.data_register >> 1) & 0x1);
					if (sio == 1)
						command_byte |= (1 << sample_bit);
					else
						command_byte &= ~(1 << sample_bit);

					sample_bit++;
					if (sample_bit >= 8) {
						sample_bit = 0;
						handleCommand(command_byte);
					}

					//Cs goes low, end command
					if (cs_is_high && (((gpio.data_register >> 2) & 0x1) == 0)) {
						transfer_in_progress = false;
					}
				}
			}


		}break;
		case GpioAddress::Direction: gpio.direction_register = value & 0xF; break;
		case GpioAddress::Control: gpio.control_register = value & 0x1; break;
	}
}

void RtcDevice::handleCommand(u8 command_byte)
{
	//check if 0b0110 is in bit positions 4 - 7,
	//if it's not, reverse it
	u8 final_command_byte = command_byte;
	if (((command_byte >> 4) & 0xF) != 0x6) {
		u8 lower = command_byte & 0xF;
		u8 upper = (command_byte >> 4) & 0xF;
		final_command_byte = (lower << 4) | upper;
	}

	printf("Command byte: 0x%02X ", final_command_byte);
	//Read or write command?
	if ((final_command_byte & 0x1) == 0x0) {
		//write command
		//which register are we writing to? (bits 1 - 3)
		switch ((final_command_byte >> 1) & 0x7) {
			case 0: rtc_regs.reset = 0; break; //reset
			case 1: break; //control
			case 2: break; //date/time
			case 3: break; //time
			case 6: requestInterrupt(mbus, GAMEPAK_INT); break; //gamepak irq
		}
	}
	else {
		//read command
	}
}

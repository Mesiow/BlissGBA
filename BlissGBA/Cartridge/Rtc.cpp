#include "Rtc.h"

RtcDevice::RtcDevice(MemoryBus* mbus)
	:mbus(mbus)
{
	gpio.data_register = 0;
	gpio.direction_register = 0;
	gpio.control_register = 1;

	rtc_regs = { 0 };

	time(&curr_time);
	local_time = localtime(&curr_time);
	reset();
}

void RtcDevice::reset()
{
	rtc_regs.date_time[(u8)DateTimeByte::Day] = toBcd(local_time->tm_mday);
	rtc_regs.date_time[(u8)DateTimeByte::Month] = toBcd(local_time->tm_mon + 1);
	rtc_regs.date_time[(u8)DateTimeByte::Year] = toBcd(local_time->tm_year - 100);
	rtc_regs.date_time[(u8)DateTimeByte::DayOfWeek] = toBcd(local_time->tm_wday);
}

u8 RtcDevice::read(GpioAddress address)
{
	if ((gpio.control_register & 0x1) == 0)
		return 0x0;
	
	switch (address) {
		case GpioAddress::Data: {
			//if bit is "in (value == 0)" in the direction
			//register, then we can read from the data register
			u8 rtc_data_value = 0;
			for (u8 bit = 0; bit < 3; bit++) {
				if (testBit(gpio.direction_register & 0xF, bit) == IN) {
					rtc_data_value |= (1 << ((gpio.data_register >> bit) & 0x1));
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
	u8 prev_data_register = gpio.data_register & 0xF;
	switch (address) {
		case GpioAddress::Data: {		
			for (u8 bit = 0; bit <= 2; bit++) {
				if (((gpio.direction_register >> bit) & 0x1) == OUT) {
					((value >> bit) & 0x1) ? (gpio.data_register |= (1 << bit)) :
						(gpio.data_register &= ~(1 << bit));
				}
			}

		}break;
		case GpioAddress::Direction: gpio.direction_register = value & 0xF; break;
		case GpioAddress::Control: gpio.control_register = value & 0x1; break;
	}

	bool sck_is_low = ((prev_data_register & 0x1) == 0);
	bool cs_is_high = (((prev_data_register >> 2) & 0x1) == 1);

	//if sck is high and cs rises, begin a new command
	if (!transfer_in_progress) {
		if ((gpio.data_register & 0x1) && ((gpio.data_register >> 2) & 0x1) == 0x1) {
			if (transfer_in_progress == false) {
				transfer_in_progress = true;
			}
		}
	}

	if (transfer_in_progress) {
		//Sck rising
		if (sck_is_low && ((gpio.data_register & 0x1) == 1)) {
			if (read_active) //read command from rom trying to read sio
				handleReadCommand();
			else if (write_active)
				handleWriteCommand();
			else
				handleCommandSetup();
		}
		//Cs goes low, end command
		if (cs_is_high && (((gpio.data_register >> 2) & 0x1) == 0)) {
			transfer_in_progress = false;
			read_active = false;
			write_active = false;
		}
	}
}

void RtcDevice::handleCommandBegin(u8 command_byte)
{
	//check if 0b0110 is in bit positions 4 - 7,
	//if it's not, reverse it (bitswap)
	u8 final_command_byte = command_byte;
	if (((command_byte >> 4) & 0xF) != 0x6) {
		u8 temp_cb = ((command_byte & 0x80) >> 7)
			| ((command_byte & 0x40) >> 5)
			| ((command_byte & 0x20) >> 3)
			| ((command_byte & 0x10) >> 1)
			| ((command_byte & 0x08) << 1)
			| ((command_byte & 0x04) << 3)
			| ((command_byte & 0x02) << 5)
			| ((command_byte & 0x01) << 7);
	
		final_command_byte = temp_cb;
	}

	//Read or write command?
	if ((final_command_byte & 0x1) == 0x0) {
		//write command
		//which register is the rom writing to? (bits 1 - 3)
		switch ((final_command_byte >> 1) & 0x7) {
			case 0: rtc_regs.reset = 0; break; //reset
			case 1: register_length_bytes = 1; break; //control
			case 2: break; //date/time (ignore writes)
			case 3: break; //time (ignore writes)
			case 6: requestInterrupt(mbus, GAMEPAK_INT); break; //gamepak irq
		}
	}
	else {
		read_active = true;
		current_n_byte = 0;
		//read command
		//which register are we reading from? (bits 1 - 3)
		switch ((final_command_byte >> 1) & 0x7) {
			case 0: rtc_regs.reset = 0; break; //reset
			case 1: register_length_bytes = 1; break; //control
			case 2: register_length_bytes = 7; break; //date/time
			case 3: register_length_bytes = 3; break; //time
			case 6: requestInterrupt(mbus, GAMEPAK_INT); break; //gamepak irq
		}
	}
}

void RtcDevice::handleCommandSetup()
{
	//sample sio bit from rom program
	u8 sio = ((gpio.data_register >> 1) & 0x1);
	(sio == 1) ? (command_byte |= (1 << sample_bit)) : (command_byte &= ~(1 << sample_bit));

	sample_bit++;
	if (sample_bit > 7) {
		sample_bit = 0;

		handleCommandBegin(command_byte);
		command_byte = 0; //reset command byte for next command
	}
}

void RtcDevice::handleReadCommand()
{
	//current command in progress and register is 
	//ready to read after receiving the command byte
	if (transfer_in_progress && read_active) {
		switch (register_length_bytes) {
			case 1: {
				u8 bit = (rtc_regs.control >> sample_reg_bit) & 0x1;
				(bit == 1) ? (gpio.data_register |= (1 << 1))
					: (gpio.data_register &= ~(1 << 1));
			}
			break; //control
			case 3:
			
			break; //time
			case 7: {
				//set sio in data register
				setDateTime(DateTimeByte::Hour, toBcd(local_time->tm_hour));
				setDateTime(DateTimeByte::Min, toBcd(local_time->tm_min));
				setDateTime(DateTimeByte::Sec, toBcd(local_time->tm_sec));

				u8 sampled_bit = 0;
				switch (current_n_byte) {
					//Date
					case 0: sampled_bit = ((getDateTime(DateTimeByte::Year) >> sample_reg_bit) & 0x1); break;
					case 1: sampled_bit = ((getDateTime(DateTimeByte::Month) >> sample_reg_bit) & 0x1); break;
					case 2: sampled_bit = ((getDateTime(DateTimeByte::Day) >> sample_reg_bit) & 0x1); break;
					case 3: sampled_bit = ((getDateTime(DateTimeByte::DayOfWeek) >> sample_reg_bit) & 0x1); break;

					//Time
					case 4: sampled_bit = ((getDateTime(DateTimeByte::Hour) >> sample_reg_bit) & 0x1); break;
					case 5: sampled_bit = ((getDateTime(DateTimeByte::Min) >> sample_reg_bit) & 0x1); break;
					case 6: sampled_bit = ((getDateTime(DateTimeByte::Sec) >> sample_reg_bit) & 0x1); break;
				}

				(sampled_bit == 1) ? (gpio.data_register |= (1 << 1))
					: (gpio.data_register &= ~(1 << 1));
			}
			break; //date/time
		}
		sample_reg_bit++;
		if (sample_reg_bit > 7) {
			//byte done sampling, move onto next byte of date/time
			current_n_byte++;
			sample_reg_bit = 0;
		}
	}
}

void RtcDevice::handleWriteCommand()
{

}

void RtcDevice::setDateTime(DateTimeByte byte, u8 value)
{
	rtc_regs.date_time[(u8)byte] = value;
}

u8 RtcDevice::getDateTime(DateTimeByte byte)
{
	return rtc_regs.date_time[(u8)byte];
}

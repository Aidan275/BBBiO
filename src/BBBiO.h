#ifndef BBBIO_H
#define BBBIO_H

#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>
#include <string>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/utsname.h>

namespace NUBE {
	class BBBiO {
		public:
			typedef struct {
				std::string module;
				unsigned char sysfs;
				unsigned char index;
				unsigned char muxMode;
				std::string path;
				std::string name;
			} gpioPwm;

			typedef struct {
				std::string mode0;
				std::string mode1;
				std::string mode2;
				std::string mode3;
				std::string mode4;
				std::string mode5;
				std::string mode6;
				std::string mode7;
			} gpioModes;

			typedef struct {
				std::string name;
				std::string key;
				std::string led;
				unsigned int gpioBank;
				unsigned char gpioPin;
				unsigned char bankId;
				unsigned char eeprom;
				std::string mux;
				unsigned int muxRegOffset;
				unsigned char ain;
				unsigned short int scale;
				gpioPwm pwm;
				gpioModes modes;
			} gpioPin;

			static const unsigned char OUTPUT = 0;
			static const unsigned char INPUT = 1;
			static const unsigned char INPUT_PULLUP = 2;

			static const unsigned char LOW = 0;
			static const unsigned char HIGH = 1;

			static const unsigned char PULLDOWN = 0;
			static const unsigned char PULLUP = 1;
			static const unsigned char DISABLED = 2;

			static const unsigned char SLOW = 0;
			static const unsigned char FAST = 1;

			BBBiO();
			~BBBiO();

			unsigned char digitalRead (std::string pinNumber);
			void digitalWrite (std::string pinNumber, std::string value);
			const gpioPin getPin (std::string pinNumber);
			void pinMode (const gpioPin pin, const unsigned char direction, unsigned char mux = 7, unsigned char pullup = DISABLED, unsigned char slew = FAST);
			void pinMode (std::string pinNumber, std::string pinDirection, unsigned char pinMux = 7, std::string pinPullup = "DISABLED", std::string pinSlew = "FAST");

		private: 
			// mmap memory registers
			static const unsigned int MMAP_OFFSET = 0x44C00000;
			static const unsigned int MMAP_SIZE = 0x481AEFFF - MMAP_OFFSET;

			// gpio memory whole 4-byte register
			static const unsigned int GPIO_REGISTER_SIZE = 4;

			// gpio memory registers
			static const unsigned int GPIO_BASE = 0x44e10800;
			static const unsigned int GPIO0 = 0x44E07000;
			static const unsigned int GPIO1 = 0x4804C000;
			static const unsigned int GPIO2 = 0x481AC000;
			static const unsigned int GPIO3 = 0x481AE000;

			static const unsigned int GPIO_OE = 0x134;
			static const unsigned int GPIO_DATAOUT = 0x13C;
			static const unsigned int GPIO_DATAIN = 0x138;

			// clock module registers
			static const unsigned int MODULEMODE_ENABLE = 0x02;
			static const unsigned int IDLEST_MASK = (0x03 << 16);

			static const unsigned int CM_WKUP = 0x44E00400;
			static const unsigned int CM_PER = 0x44E00000;

			static const unsigned int CM_WKUP_GPIO0_CLKCTRL = (CM_WKUP + 0x8);

			static const unsigned int CM_PER_GPIO1_CLKCTRL = (CM_PER + 0xAC);
			static const unsigned int CM_PER_GPIO2_CLKCTRL = (CM_PER + 0xB0);
			static const unsigned int CM_PER_GPIO3_CLKCTRL = (CM_PER + 0xB4);

			static const unsigned int CM_PER_EPWMSS0_CLKCTRL = (CM_PER + 0xD4);
			static const unsigned int CM_PER_EPWMSS1_CLKCTRL = (CM_PER + 0xCC);
			static const unsigned int CM_PER_EPWMSS2_CLKCTRL = (CM_PER + 0xD8);

			static const unsigned int ADC_TSC = 0x44E0D000;
			static const unsigned int ADC_SYSCONFIG = (ADC_TSC + 0x10);
			static const unsigned int ADC_SOFTRESET = 0x01;
			static const unsigned int ADC_STEPCONFIG_WRITE_PROTECT_OFF = (0x01 << 2);
			static const unsigned int ADC_STEPENABLE = (ADC_TSC + 0x54);
			static const unsigned int TSC_ADC_SS_ENABLE = 0x01;
			static const unsigned int ADC_CTRL = (ADC_TSC + 0x40);
			static const unsigned int CM_WKUP_ADC_TSC_CLKCTRL = (CM_WKUP + 0xBC);
			static const unsigned int ADC_FIFO0DATA = (ADC_TSC + 0x100);
			static const unsigned int ADC_FIFO_MASK = 0xFFF;

			// ADC STEPCONFIG
			static const unsigned int ADCSTEPDELAY = ((15 & 0xFF) << 24);

			static const unsigned int ADCSTEPCONFIG1 = (ADC_TSC + 0x64);
			static const unsigned int ADCSTEPCONFIG1_VALUE = (0X00 << 19) | (0x01 << 2);
			static const unsigned int ADCSTEPDELAY1 = (ADC_TSC + 0x68);

			static const unsigned int ADCSTEPCONFIG2 = (ADC_TSC + 0x6C);
			static const unsigned int ADCSTEPCONFIG2_VALUE = (0X01 << 19) | (0x01 << 2);
			static const unsigned int ADCSTEPDELAY2 = (ADC_TSC + 0x70);

			static const unsigned int ADCSTEPCONFIG3 = (ADC_TSC + 0x74);
			static const unsigned int ADCSTEPCONFIG3_VALUE = (0X02 << 19) | (0x01 << 2);
			static const unsigned int ADCSTEPDELAY3 = (ADC_TSC + 0x78);

			static const unsigned int ADCSTEPCONFIG4 = (ADC_TSC + 0x7C);
			static const unsigned int ADCSTEPCONFIG4_VALUE = (0X03 << 19) | (0x01 << 2);
			static const unsigned int ADCSTEPDELAY4 = (ADC_TSC + 0x80);

			static const unsigned int ADCSTEPCONFIG5 = (ADC_TSC + 0x84);
			static const unsigned int ADCSTEPCONFIG5_VALUE = (0X04 << 19) | (0x01 << 2);
			static const unsigned int ADCSTEPDELAY5 = (ADC_TSC + 0x88);

			static const unsigned int ADCSTEPCONFIG6 = (ADC_TSC + 0x8C);
			static const unsigned int ADCSTEPCONFIG6_VALUE = (0X05 << 19) | (0x01 << 2);
			static const unsigned int ADCSTEPDELAY6 = (ADC_TSC + 0x90);

			static const unsigned int ADCSTEPCONFIG7 = (ADC_TSC + 0x94);
			static const unsigned int ADCSTEPCONFIG7_VALUE = (0X06 << 19) | (0x01 << 2);
			static const unsigned int ADCSTEPDELAY7 = (ADC_TSC + 0x98);

			static const unsigned int ADCSTEPCONFIG8 = (ADC_TSC + 0x9C);
			static const unsigned int ADCSTEPCONFIG8_VALUE = (0X07 << 19) | (0x01 << 2);
			static const unsigned int ADCSTEPDELAY8 = (ADC_TSC + 0xA0);

			static const std::map <std::string, const gpioPin> PIN_INDEX;

			static volatile unsigned long int *mapAddress;

			void enableClockModules();
			const unsigned char getPinValue(std::string pinValue);
			static std::map <std::string, const gpioPin> initializePins();
			void mmap();
	};
}

#endif

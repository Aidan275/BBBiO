#include "BBBiO.h"

namespace NUBE {
	const unsigned char BBBiO::OUTPUT;
	const unsigned char BBBiO::INPUT;
	const unsigned char BBBiO::INPUT_PULLUP;

	const unsigned char BBBiO::PULLDOWN;
	const unsigned char BBBiO::PULLUP;
	const unsigned char BBBiO::DISABLED;

	const unsigned char BBBiO::SLOW;
	const unsigned char BBBiO::FAST;

	const unsigned char BBBiO::LOW;
	const unsigned char BBBiO::HIGH;

	const std::map<std::string, const BBBiO::gpioPin> BBBiO::PIN_INDEX = BBBiO::initializePins();

	volatile unsigned long int *BBBiO::mapAddress = nullptr;

	BBBiO::BBBiO() {
		mmap();
		enableClockModules();
	}

	BBBiO::~BBBiO() {
		if(munmap((void *) mapAddress, MMAP_SIZE) < 0) {
			perror("munmap (gpio) failed");
			exit(EXIT_FAILURE);
		}
	}

	unsigned char BBBiO::digitalRead(std::string pinNumber) {
		gpioPin pin = getPin(pinNumber);
		unsigned char value = (mapAddress[(pin.gpioBank - MMAP_OFFSET + GPIO_DATAIN) / GPIO_REGISTER_SIZE] & (1 << pin.bankId)) >> pin.bankId;
		return value;
	}

	void BBBiO::digitalWrite(std::string pinNumber, std::string pinValue) {
		gpioPin pin = getPin(pinNumber);
		const char value = getPinValue(pinValue);

		switch(value) {
			case LOW:
				mapAddress[(pin.gpioBank - MMAP_OFFSET + GPIO_DATAOUT) / GPIO_REGISTER_SIZE] &= ~ (1 << pin.bankId);
				break;
			case HIGH:
				mapAddress[(pin.gpioBank - MMAP_OFFSET + GPIO_DATAOUT) / GPIO_REGISTER_SIZE] |= (1 << pin.bankId);
				break;
			default:
				std::cout << "Undefined pinValue: " << value << "!" << std::endl;
				exit(EXIT_FAILURE);
		}
	}

	const BBBiO::gpioPin BBBiO::getPin(std::string pinNumber) {
		if(PIN_INDEX.count(pinNumber) > 0) {
			return PIN_INDEX.at(pinNumber);
		} else {
			std::cout << "Undefined pin: " << pinNumber << "!" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	const unsigned char BBBiO::getPinValue(std::string pinValue) {
		if(pinValue == "LOW") {
			return LOW;
		} else if(pinValue == "HIGH") {
			return HIGH;
		} else {
			std::cout << "Undefined pin value: " << pinValue << "!" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	void BBBiO::enableClockModules() {
		if(mapAddress[(CM_WKUP_GPIO0_CLKCTRL - MMAP_OFFSET) / GPIO_REGISTER_SIZE] & IDLEST_MASK) {
			mapAddress[(CM_WKUP_GPIO0_CLKCTRL - MMAP_OFFSET) / GPIO_REGISTER_SIZE] |= MODULEMODE_ENABLE;
			while(mapAddress[(CM_WKUP_GPIO0_CLKCTRL - MMAP_OFFSET) / GPIO_REGISTER_SIZE] & IDLEST_MASK);
		}
		if(mapAddress[(CM_PER_GPIO1_CLKCTRL - MMAP_OFFSET) / GPIO_REGISTER_SIZE] & IDLEST_MASK) {
			mapAddress[(CM_PER_GPIO1_CLKCTRL - MMAP_OFFSET) / GPIO_REGISTER_SIZE] |= MODULEMODE_ENABLE;
			while(mapAddress[(CM_PER_GPIO1_CLKCTRL - MMAP_OFFSET) / GPIO_REGISTER_SIZE] & IDLEST_MASK);
		}
		if(mapAddress[(CM_PER_GPIO2_CLKCTRL - MMAP_OFFSET) / GPIO_REGISTER_SIZE] & IDLEST_MASK) {
			mapAddress[(CM_PER_GPIO2_CLKCTRL - MMAP_OFFSET) / GPIO_REGISTER_SIZE] |= MODULEMODE_ENABLE;
			while(mapAddress[(CM_PER_GPIO2_CLKCTRL - MMAP_OFFSET) / GPIO_REGISTER_SIZE] & IDLEST_MASK);
		}
		if(mapAddress[(CM_PER_GPIO3_CLKCTRL - MMAP_OFFSET) / GPIO_REGISTER_SIZE] & IDLEST_MASK) {
			mapAddress[(CM_PER_GPIO3_CLKCTRL - MMAP_OFFSET) / GPIO_REGISTER_SIZE] |= MODULEMODE_ENABLE;
			while(mapAddress[(CM_PER_GPIO3_CLKCTRL - MMAP_OFFSET) / GPIO_REGISTER_SIZE] & IDLEST_MASK);
		}
	}

	void BBBiO::mmap() {
		int fd = 0;

		if((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
			perror("Unable to open '/dev/mem', make sure you have sudo privileges");
			exit(EXIT_FAILURE);
		}

		mapAddress = (volatile unsigned long int *)mmap(nullptr, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, fd, MMAP_OFFSET);

		if(close(fd) < 0){
			perror("Unable to close /dev/mem file descriptor");
			exit(EXIT_FAILURE);
		}

		if(mapAddress == MAP_FAILED) {
			perror("Unable to map /dev/mem");
			exit(EXIT_FAILURE);
		}
	}

	std::map<std::string, const BBBiO::gpioPin> BBBiO::initializePins() {
		std::map<std::string, const gpioPin> pins;

		pins.insert(
			std::pair <std::string, const gpioPin>(
				"USR0", {
					"GPIO1_21",
					"USR0",
					"usr0",
					GPIO1,
					53,
					21,
					0,
					"gpmc_a5",
					0x054,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_a5",
						"gmii2_txd0",
						"rgmii2_td0",
						"rmii2_txd0",
						"gpmc_a21",
						"pr1_mii1_rxd3",
						"eqep1b_in",
						"gpio1_21"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"USR1", {
					"GPIO1_22",
					"USR1",
					"usr1",
					GPIO1,
					54,
					22,
					0,
					"gpmc_a6",
					0x058,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_a6",
						"gmii2_txclk",
						"rgmii2_tclk",
						"mmc2_dat4",
						"gpmc_a22",
						"pr1_mii1_rxd2",
						"eqep1_index",
						"gpio1_22"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"USR2", {
					"GPIO1_23",
					"USR2",
					"usr2",
					GPIO1,
					55,
					23,
					0,
					"gpmc_a7",
					0x05C,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_a7",
						"gmii2_rxclk",
						"rgmii2_rclk",
						"mmc2_dat5",
						"gpmc_a23",
						"pr1_mii1_rxd1",
						"eqep1_strobe",
						"gpio1_23"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"USR3", {
					"GPIO1_24",
					"USR3",
					"usr3",
					GPIO1,
					56,
					24,
					0,
					"gpmc_a8",
					0x060,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_a8",
						"gmii2_rxd3",
						"rgmii2_rd3",
						"mmc2_dat6",
						"gpmc_a24",
						"pr1_mii1_rxd0",
						"mcasp0_aclkx",
						"gpio1_24"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_3", {
					"GPIO1_6",
					"P8_3",
					"",
					GPIO1,
					38,
					6,
					26,
					"gpmc_ad6",
					0x018,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ad6",
						"mmc1_dat6",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio1_6"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_4", {
					"GPIO1_7",
					"P8_4",
					"",
					GPIO1,
					39,
					7,
					27,
					"gpmc_ad7",
					0x01C,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ad7",
						"mmc1_dat7",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio1_7"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_5", {
					"GPIO1_2",
					"P8_5",
					"",
					GPIO1,
					34,
					2,
					22,
					"gpmc_ad2",
					0x008,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ad2",
						"mmc1_dat2",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio1_2"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_6", {
					"GPIO1_3",
					"P8_6",
					"",
					GPIO1,
					35,
					3,
					23,
					"gpmc_ad3",
					0x00C,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ad3",
						"mmc1_dat3",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio1_3"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_7", {
					"TIMER4",
					"P8_7",
					"",
					GPIO2,
					66,
					2,
					41,
					"gpmc_advn_ale",
					0x090,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_advn_ale",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"mmc1_sdcd"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_8", {
					"TIMER7",
					"P8_8",
					"",
					GPIO2,
					67,
					3,
					44,
					"gpmc_oen_ren",
					0x094,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_oen_ren",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio2_3"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_9", {
					"TIMER5",
					"P8_9",
					"",
					GPIO2,
					69,
					5,
					42,
					"gpmc_ben0_cle",
					0x09C,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ben0_cle",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio2_5"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_10", {
					"TIMER6",
					"P8_10",
					"",
					GPIO2,
					68,
					4,
					43,
					"gpmc_wen",
					0x098,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_wen",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio2_4"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_11", {
					"GPIO1_13",
					"P8_11",
					"",
					GPIO1,
					45,
					13,
					29,
					"gpmc_ad13",
					0x034,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ad13",
						"lcd_data18",
						"mmc1_dat5",
						"mmc2_dat1",
						"eqep2B_in",
						"pr1_mii0_txd",
						"pr1_pru0_pru_r30_15",
						"gpio1_13"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_12", {
					"GPIO1_12",
					"P8_12",
					"",
					GPIO1,
					44,
					12,
					28,
					"gpmc_ad12",
					0x030,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ad12",
						"lcd_data19",
						"mmc1_dat4",
						"mmc2_dat0",
						"eqep2a_in",
						"pr1_mii0_txd2",
						"pr1_pru0_pru_r30_14",
						"gpio1_12"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_13", {
					"EHRPWM2B",
					"P8_13",
					"",
					GPIO0,
					23,
					23,
					15,
					"gpmc_ad9",
					0x024,
					{},
					{},
					(gpioPwm) {
						"ehrpwm2",
						6,
						1,
						4,
						"ehrpwm.2:1",
						"EHRPWM2B"
					},
					(gpioModes) {
						"gpmc_ad9",
						"lcd_data22",
						"mmc1_dat1",
						"mmc2_dat5",
						"ehrpwm2B",
						"pr1_mii0_col",
						"N/A",
						"gpio0_23"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_14", {
					"GPIO0_26",
					"P8_14",
					"",
					GPIO0,
					26,
					26,
					16,
					"gpmc_ad10",
					0x028,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ad10",
						"lcd_data21",
						"mmc1_dat2",
						"mmc2_dat6",
						"ehrpwm2_tripzone_input",
						"pr1_mii0_txen",
						"N/A",
						"gpio0_26"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_15", {
					"GPIO1_15",
					"P8_15",
					"",
					GPIO1,
					47,
					15,
					31,
					"gpmc_ad15",
					0x03C,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ad15",
						"lcd_data16",
						"mmc1_dat7",
						"mmc2_dat3",
						"eqep2_strobe",
						"pr1_ecap0_ecap_capin_apwm_o",
						"pr1_pru0_pru_r31_15",
						"gpio1_15"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_16", {
					"GPIO1_14",
					"P8_16",
					"",
					GPIO1,
					46,
					14,
					30,
					"gpmc_ad14",
					0x038,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ad14",
						"lcd_data17",
						"mmc1_dat6",
						"mmc2_dat2",
						"eqep2_index",
						"pr1_mii0_txd0",
						"pr1_pru0_pru_r31_14",
						"gpio1_14"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_17", {
					"GPIO0_27",
					"P8_17",
					"",
					GPIO0,
					27,
					27,
					17,
					"gpmc_ad11",
					0x02C,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ad11",
						"lcd_data20",
						"mmc1_dat3",
						"mmc2_dat7",
						"ehrpwm0_synco",
						"pr1_mii0_txd3",
						"N/A",
						"gpio0_27"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_18", {
					"GPIO2_1",
					"P8_18",
					"",
					GPIO2,
					65,
					1,
					40,
					"gpmc_clk",
					0x08C,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_clk",
						"lcd_memory_clk_mux",
						"N/A",
						"mmc2_clk",
						"N/A",
						"N/A",
						"mcasp0_fsr",
						"gpio2_1"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_19", {
					"EHRPWM2A",
					"P8_19",
					"",
					GPIO0,
					22,
					22,
					14,
					"gpmc_ad8",
					0x020,
					{},
					{},
					(gpioPwm) {
						"ehrpwm2",
						5,
						0,
						4,
						"ehrpwm.2:0",
						"EHRPWM2A"
					},
					(gpioModes) {
						"gpmc_ad8",
						"lcd_data23",
						"mmc1_dat0",
						"mmc2_dat4",
						"ehrpwm2A",
						"pr1_mii_mt0_clk",
						"N/A",
						"gpio0_22"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_20", {
					"GPIO1_31",
					"P8_20",
					"",
					GPIO1,
					63,
					31,
					39,
					"gpmc_csn2",
					0x084,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_csn2",
						"gpmc_be1n",
						"mmc1_cmd",
						"pr1_edio_data_in7",
						"pr1_edio_data_out7",
						"pr1_pru1_pru_r30_13",
						"pr1_pru1_pru_r31_13",
						"gpio1_31"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_21", {
					"GPIO1_30",
					"P8_21",
					"",
					GPIO1,
					62,
					30,
					38,
					"gpmc_csn1",
					0x080,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_csn1",
						"gpmc_clk",
						"mmc1_clk",
						"pr1_edio_data_in6",
						"pr1_edio_data_out6",
						"pr1_pru1_pru_r30_12",
						"pr1_pru1_pru_r31_12",
						"gpio1_30"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_22", {
					"GPIO1_5",
					"P8_22",
					"",
					GPIO1,
					37,
					5,
					25,
					"gpmc_ad5",
					0x014,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ad5",
						"mmc1_dat5",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio1_5"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_23", {
					"GPIO1_4",
					"P8_23",
					"",
					GPIO1,
					36,
					4,
					24,
					"gpmc_ad4",
					0x010,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ad4",
						"mmc1_dat4",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio1_4"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_24", {
					"GPIO1_1",
					"P8_24",
					"",
					GPIO1,
					33,
					1,
					21,
					"gpmc_ad1",
					0x004,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ad1",
						"mmc1_dat1",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio1_1"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_25", {
					"GPIO1_0",
					"P8_25",
					"",
					GPIO1,
					32,
					0,
					20,
					"gpmc_ad0",
					0x000,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ad0",
						"mmc1_dat0",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio1_0"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_26", {
					"GPIO1_29",
					"P8_26",
					"",
					GPIO1,
					61,
					29,
					37,
					"gpmc_csn0",
					0x07C,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_csn0",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio1_29"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_27", {
					"GPIO2_22",
					"P8_27",
					"",
					GPIO2,
					86,
					22,
					57,
					"lcd_vsync",
					0x0e0,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_vsync",
						"gpmc_a8",
						"N/A",
						"pr1_edio_data_in2",
						"pr1_edio_data_out2",
						"pr1_pru1_pru_r30_8",
						"pr1_pru1_pru_r31_8",
						"gpio2_22"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_28", {
					"GPIO2_24",
					"P8_28",
					"",
					GPIO2,
					88,
					24,
					59,
					"lcd_pclk",
					0x0e8,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_pclk",
						"gpmc_a10",
						"pr1_mii0_crs",
						"pr1_edio_data_in4",
						"pr1_edio_data_out4",
						"pr1_pru1_pru_r30_10",
						"pr1_pru1_pru_r31_10",
						"gpio2_24"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_29", {
					"GPIO2_23",
					"P8_29",
					"",
					GPIO2,
					87,
					23,
					58,
					"lcd_hsync",
					0x0e4,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_hsync",
						"gpmc_a9",
						"N/A",
						"pr1_edio_data_in3",
						"pr1_edio_data_out3",
						"pr1_pru1_pru_r30_9",
						"pr1_pru1_pru_r31_9",
						"gpio2_23"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_30", {
					"GPIO2_25",
					"P8_30",
					"",
					GPIO2,
					89,
					25,
					60,
					"lcd_ac_bias_en",
					0x0EC,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_ac_bias_en",
						"gpmc_a11",
						"pr1_mii1_crs",
						"pr1_edio_data_in5",
						"pr1_edio_data_out5",
						"pr1_pru1_pru_r30_11",
						"pr1_pru1_pru_r31_11",
						"gpio2_25"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_31", {
					"UART5_CTSN",
					"P8_31",
					"",
					GPIO0,
					10,
					10,
					7,
					"lcd_data14",
					0x0d8,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_data14",
						"gpmc_a18",
						"N/A",
						"mcasp0_axr1",
						"N/A",
						"N/A",
						"N/A",
						"gpio0_10"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_32", {
					"UART5_RTSN",
					"P8_32",
					"",
					GPIO0,
					11,
					11,
					8,
					"lcd_data15",
					0x0DC,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_data15",
						"gpmc_a19",
						"N/A",
						"mcasp0_ahclkx",
						"mcasp0_axr3",
						"N/A",
						"N/A",
						"gpio0_11"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_33", {
					"UART4_RTSN",
					"P8_33",
					"",
					GPIO0,
					9,
					9,
					6,
					"lcd_data13",
					0x0d4,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_data13",
						"gpmc_a17",
						"N/A",
						"mcasp0_fsr",
						"mcasp0_axr3",
						"N/A",
						"N/A",
						"gpio0_9"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_34", {
					"UART3_RTSN",
					"P8_34",
					"",
					GPIO2,
					81,
					17,
					56,
					"lcd_data11",
					0x0CC,
					{},
					{},
					(gpioPwm) {
						"ehrpwm1",
						4,
						1,
						2,
						"ehrpwm.1:1",
						"EHRPWM1B"
					},
					(gpioModes) {
						"lcd_data11",
						"gpmc_a15",
						"N/A",
						"mcasp0_ahclkr",
						"mcasp0_axr2",
						"N/A",
						"N/A",
						"gpio2_17"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_35", {
					"UART4_CTSN",
					"P8_35",
					"",
					GPIO0,
					8,
					8,
					5,
					"lcd_data12",
					0x0d0,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_data12",
						"gpmc_a16",
						"N/A",
						"mcasp0_aclkr",
						"mcasp0_axr2",
						"N/A",
						"N/A",
						"gpio0_8"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_36", {
					"UART3_CTSN",
					"P8_36",
					"",
					GPIO2,
					80,
					16,
					55,
					"lcd_data10",
					0x0c8,
					{},
					{},
					(gpioPwm) {
						"ehrpwm1",
						3,
						0,
						2,
						"ehrpwm.1:0",
						"EHRPWM1A"
					},
					(gpioModes) {
						"lcd_data10",
						"gpmc_a14",
						"ehrpwm1A",
						"mcasp0_axr0",
						"mcasp0_axr0",
						"pr1_mii0_rxd1",
						"uart3_ctsn",
						"gpio2_16"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_37", {
					"UART5_TXD",
					"P8_37",
					"",
					GPIO2,
					78,
					14,
					53,
					"lcd_data8",
					0x0c0,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_data8",
						"gpmc_a12",
						"N/A",
						"mcasp0_aclkx",
						"N/A",
						"N/A",
						"uart2_ctsn",
						"gpio2_14"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_38", {
					"UART5_RXD",
					"P8_38",
					"",
					GPIO2,
					79,
					15,
					54,
					"lcd_data9",
					0x0c4,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_data9",
						"gpmc_a13",
						"N/A",
						"mcasp0_fsx",
						"N/A",
						"N/A",
						"uart2_rtsn",
						"gpio2_15"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_39", {
					"GPIO2_12",
					"P8_39",
					"",
					GPIO2,
					76,
					12,
					51,
					"lcd_data6",
					0x0b8,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_data6",
						"gpmc_a6",
						"pr1_edio_data_in6",
						"eqep2_index",
						"pr1_edio_data_out6",
						"pr1_pru1_pru_r30_6",
						"pr1_pru1_pru_r31_6",
						"gpio2_12"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_40", {
					"GPIO2_13",
					"P8_40",
					"",
					GPIO2,
					77,
					13,
					52,
					"lcd_data7",
					0x0BC,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_data7",
						"gpmc_a7",
						"pr1_edio_data_in7",
						"eqep2_strobe",
						"pr1_pru1_pru_r30_7",
						"pr1_pru_pru1_r30_7",
						"pr1_pru1_pru_r31_7",
						"gpio2_13"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_41", {
					"GPIO2_10",
					"P8_41",
					"",
					GPIO2,
					74,
					10,
					49,
					"lcd_data4",
					0x0b0,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_data4",
						"gpmc_a4",
						"pr1_mii0_txd1",
						"eQEP2A_in",
						"N/A",
						"pr1_pru1_pru_r30_4",
						"pr1_pru1_pru_r31_4",
						"gpio2_10"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_42", {
					"GPIO2_11",
					"P8_42",
					"",
					GPIO2,
					75,
					11,
					50,
					"lcd_data5",
					0x0b4,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_data5",
						"gpmc_a5",
						"pr1_mii0_txd0",
						"eqep2b_in",
						"N/A",
						"pr1_pru1_pru_r30_5",
						"pr1_pru1_pru_r31_5",
						"gpio2_11"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_43", {
					"GPIO2_8",
					"P8_43",
					"",
					GPIO2,
					72,
					8,
					47,
					"lcd_data2",
					0x0a8,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_data2",
						"gpmc_a2",
						"pr1_mii0_txd3",
						"ehrpwm2_tripzone_input",
						"N/A",
						"pr1_pru1_pru_r30_2",
						"pr1_pru1_pru_r31_2",
						"gpio2_8"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_44", {
					"GPIO2_9",
					"P8_44",
					"",
					GPIO2,
					73,
					9,
					48,
					"lcd_data3",
					0x0AC,
					{},
					{},
					{},
					(gpioModes) {
						"lcd_data3",
						"gpmc_a3",
						"pr1_mii0_txd2",
						"ehrpwm0_synco",
						"N/A",
						"pr1_pru1_pru_r30_3",
						"pr1_pru1_pru_r31_3",
						"gpio2_9"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_45", {
					"GPIO2_6",
					"P8_45",
					"",
					GPIO2,
					70,
					6,
					45,
					"lcd_data0",
					0x0a0,
					{},
					{},
					(gpioPwm) {
						"ehrpwm2",
						5,
						0,
						3,
						"ehrpwm.2:0",
						"EHRPWM2A"
					},
					(gpioModes) {
						"lcd_data0",
						"gpmc_a0",
						"pr1_mii_mt0_clk",
						"ehrpwm2A",
						"N/A",
						"pr1_pru1_pru_r30_0",
						"pr1_pru1_pru_r31_0",
						"gpio2_6"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P8_46", {
					"GPIO2_7",
					"P8_46",
					"",
					GPIO2,
					71,
					7,
					46,
					"lcd_data1",
					0x0a4,
					{},
					{},
					(gpioPwm) {
						"ehrpwm2",
						6,
						1,
						3,
						"ehrpwm.2:1",
						"EHRPWM2B"
					},
					(gpioModes) {
						"lcd_data1",
						"gpmc_a1",
						"pr1_mii0_txen",
						"ehrpwm2B",
						"N/A",
						"pr1_pru1_pru_r30_1",
						"pr1_pru1_pru_r31_1",
						"gpio2_7"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_11", {
					"UART4_RXD",
					"P9_11",
					"",
					GPIO0,
					30,
					30,
					18,
					"gpmc_wait0",
					0x070,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_wait0",
						"mii2_crs",
						"N/A",
						"rmii2_crs_dv",
						"mmc1_sdcd",
						"N/A",
						"N/A",
						"gpio0_30"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_12", {
					"GPIO1_28",
					"P9_12",
					"",
					GPIO1,
					60,
					28,
					36,
					"gpmc_ben1",
					0x078,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_ben1",
						"mii2_col",
						"N/A",
						"mmc2_dat3",
						"N/A",
						"N/A",
						"mcasp0_aclkr",
						"gpio1_28"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_13", {
					"UART4_TXD",
					"P9_13",
					"",
					GPIO0,
					31,
					31,
					19,
					"gpmc_wpn",
					0x074,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_wpn",
						"mii2_rxerr",
						"N/A",
						"rmii2_rxerr",
						"mmc2_sdcd",
						"N/A",
						"N/A",
						"gpio0_31"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_14", {
					"EHRPWM1A",
					"P9_14",
					"",
					GPIO1,
					50,
					18,
					34,
					"gpmc_a2",
					0x048,
					{},
					{},
					(gpioPwm) {
						"ehrpwm1",
						3,
						0,
						6,
						"ehrpwm.1:0",
						"EHRPWM1A"
					},
					(gpioModes) {
						"gpmc_a2",
						"gmii2_txd3",
						"rgmii2_td3",
						"mmc2_dat1",
						"gpmc_a18",
						"pr1_mii1_txd2",
						"ehrpwm1A",
						"gpio1_18"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_15", {
					"GPIO1_16",
					"P9_15",
					"",
					GPIO1,
					48,
					16,
					32,
					"mii1_rxd3",
					0x040,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_a0",
						"gmii2_txen",
						"rgmii2_tctl",
						"rmii2_txen",
						"gpmc_a16",
						"pr1_mii_mt1_clk",
						"ehrpwm1_tripzone_input",
						"gpio1_16"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_16", {
					"EHRPWM1B",
					"P9_16",
					"",
					GPIO1,
					51,
					19,
					35,
					"gpmc_a3",
					0x04C,
					{},
					{},
					(gpioPwm) {
						"ehrpwm1",
						4,
						1,
						6,
						"ehrpwm.1:1",
						"EHRPWM1B"
					},
					(gpioModes) {
						"gpmc_a3",
						"gmii2_txd2",
						"rgmii2_td2",
						"mmc2_dat2",
						"gpmc_a19",
						"pr1_mii1_txd1",
						"ehrpwm1B",
						"gpio1_19"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_17", {
					"I2C1_SCL",
					"P9_17",
					"",
					GPIO0,
					5,
					5,
					3,
					"spi0_cs0",
					0x15C,
					{},
					{},
					{},
					(gpioModes) {
						"spi0_cs0",
						"mmc2_sdwp",
						"i2c1_scl",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio0_5"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_18", {
					"I2C1_SDA",
					"P9_18",
					"",
					GPIO0,
					4,
					4,
					2,
					"spi0_d1",
					0x158,
					{},
					{},
					{},
					(gpioModes) {
						"spi0_d1",
						"mmc1_sdwp",
						"i2c1_sda",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio0_4"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_19", {
					"I2C2_SCL",
					"P9_19",
					"",
					GPIO0,
					13,
					13,
					9,
					"uart1_rtsn",
					0x17C,
					{},
					{},
					{},
					(gpioModes) {
						"uart1_rtsn",
						"N/A",
						"d_can0_rx",
						"i2c2_scl",
						"spi1_cs1",
						"N/A",
						"N/A",
						"gpio0_13"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_20", {
					"I2C2_SDA",
					"P9_20",
					"",
					GPIO0,
					12,
					12,
					10,
					"uart1_ctsn",
					0x178,
					{},
					{},
					{},
					(gpioModes) {
						"uart1_ctsn",
						"N/A",
						"d_can0_tx",
						"i2c2_sda",
						"spi1_cs0",
						"N/A",
						"N/A",
						"gpio0_12"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_21", {
					"UART2_TXD",
					"P9_21",
					"",
					GPIO0,
					3,
					3,
					1,
					"spi0_d0",
					0x154,
					{},
					{},
					(gpioPwm) {
						"ehrpwm0",
						1,
						1,
						3,
						"ehrpwm.0:1",
						"EHRPWM0B"
					},
					(gpioModes) {
						"spi0_d0",
						"uart2_txd",
						"i2c2_scl",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio0_3"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_22", {
					"UART2_RXD",
					"P9_22",
					"",
					GPIO0,
					2,
					2,
					0,
					"spi0_sclk",
					0x150,
					{},
					{},
					(gpioPwm) {
						"ehrpwm0",
						0,
						0,
						3,
						"ehrpwm.0:0",
						"EHRPWM0A"
					},
					(gpioModes) {
						"spi0_sclk",
						"uart2_rxd",
						"i2c2_sda",
						"N/A",
						"N/A",
						"N/A",
						"N/A",
						"gpio0_2"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_23", {
					"GPIO1_17",
					"P9_23",
					"",
					GPIO1,
					49,
					17,
					33,
					"gpmc_a1",
					0x044,
					{},
					{},
					{},
					(gpioModes) {
						"gpmc_a1",
						"gmii2_rxdv",
						"rgmii2_rctl",
						"mmc2_dat0",
						"gpmc_a17",
						"pr1_mii1_txd3",
						"ehrpwm0_synco",
						"gpio1_17"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_24", {
					"UART1_TXD",
					"P9_24",
					"",
					GPIO0,
					15,
					15,
					12,
					"uart1_txd",
					0x184,
					{},
					{},
					{},
					(gpioModes) {
						"uart1_txd",
						"mmc2_sdwp",
						"d_can1_rx",
						"i2c1_scl",
						"N/A",
						"pr1_uart0_txd_mux1",
						"N/A",
						"gpio0_15"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_25", {
					"GPIO3_21",
					"P9_25",
					"",
					GPIO3,
					117,
					21,
					66,
					"mcasp0_ahclkx",
					0x1AC,
					{},
					{},
					{},
					(gpioModes) {
						"mcasp0_ahclkx",
						"N/A",
						"mcasp0_axr3",
						"mcasp1_axr1",
						"N/A",
						"N/A",
						"N/A",
						"gpio3_21"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_26", {
					"UART1_RXD",
					"P9_26",
					"",
					GPIO0,
					14,
					14,
					11,
					"uart1_rxd",
					0x180,
					{},
					{},
					{},
					(gpioModes) {
						"uart1_rxd",
						"mmc1_sdwp",
						"d_can1_tx",
						"i2c1_sda",
						"N/A",
						"pr1_uart0_rxd_mux1",
						"N/A",
						"gpio0_14"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_27", {
					"GPIO3_19",
					"P9_27",
					"",
					GPIO3,
					115,
					19,
					64,
					"mcasp0_fsr",
					0x1a4,
					{},
					{},
					{},
					(gpioModes) {
						"mcasp0_fsr",
						"N/A",
						"mcasp0_axr3",
						"mcasp1_fsx",
						"N/A",
						"pr1_pru0_pru_r30_5",
						"N/A",
						"gpio3_19"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_28", {
					"SPI1_CS0",
					"P9_28",
					"",
					GPIO3,
					113,
					17,
					63,
					"mcasp0_ahclkr",
					0x19C,
					{},
					{},
					(gpioPwm) {
						"ecap2",
						7,
						2,
						4,
						"ecap.2",
						"ECAPPWM2"
					},
					(gpioModes) {
						"mcasp0_ahclkr",
						"N/A",
						"mcasp0_axr2",
						"spi1_cs0",
						"eCAP2_in_PWM2_out",
						"N/A",
						"N/A",
						"gpio3_17"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_29", {
					"SPI1_D0",
					"P9_29",
					"",
					GPIO3,
					111,
					15,
					61,
					"mcasp0_fsx",
					0x194,
					{},
					{},
					(gpioPwm) {
						"ehrpwm0",
						1,
						1,
						1,
						"ehrpwm.0:1",
						"EHRPWM0B"
					},
					(gpioModes) {
						"mcasp0_fsx",
						"ehrpwm0B",
						"N/A",
						"spi1_d0",
						"mmc1_sdcd",
						"N/A",
						"N/A",
						"gpio3_15"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_30", {
					"SPI1_D1",
					"P9_30",
					"",
					GPIO3,
					112,
					16,
					62,
					"mcasp0_axr0",
					0x198,
					{},
					{},
					{},
					(gpioModes) {
						"mcasp0_axr0",
						"N/A",
						"N/A",
						"spi1_d1",
						"mmc2_sdcd",
						"N/A",
						"N/A",
						"gpio3_16"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_31", {
					"SPI1_SCLK",
					"P9_31",
					"",
					GPIO3,
					110,
					14,
					65,
					"mcasp0_aclkx",
					0x190,
					{},
					{},
					(gpioPwm) {
						"ehrpwm0",
						0,
						0,
						1,
						"ehrpwm.0:0",
						"EHRPWM0A"
					},
					(gpioModes) {
						"mcasp0_aclkx",
						"ehrpwm0A",
						"N/A",
						"spi1_sclk",
						"mmc0_sdcd",
						"N/A",
						"N/A",
						"gpio3_14"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_33", {
					"AIN4",
					"P9_33",
					"",
					0,
					4,
					4,
					71,
					"",
					0,
					4,
					4096,
					{},
					{}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_35", {
					"AIN6",
					"P9_35",
					"",
					0,
					6,
					6,
					73,
					"",
					0,
					6,
					4096,
					{},
					{}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_36", {
					"AIN5",
					"P9_36",
					"",
					0,
					5,
					5,
					72,
					"",
					0,
					5,
					4096,
					{},
					{}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_37", {
					"AIN2",
					"P9_37",
					"",
					0,
					2,
					2,
					69,
					"",
					0,
					2,
					4096,
					{},
					{}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_38", {
					"AIN3",
					"P9_38",
					"",
					0,
					3,
					3,
					70,
					"",
					0,
					3,
					4096,
					{},
					{}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_39", {
					"AIN0",
					"P9_39",
					"",
					0,
					0,
					0,
					67,
					"",
					0,
					0,
					4096,
					{},
					{}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_40", {
					"AIN1",
					"P9_40",
					"",
					0,
					1,
					1,
					68,
					"",
					0,
					1,
					4096,
					{},
					{}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_41", {
					"CLKOUT2",
					"P9_41",
					"",
					GPIO0,
					20,
					20,
					13,
					"xdma_event_intr1",
					0x1b4,
					{},
					{},
					{},
					(gpioModes) {
						"xdma_event_intr1",
						"N/A",
						"N/A",
						"clkout2",
						"N/A",
						"N/A",
						"N/A",
						"gpio0_20"
					}
				}
			)
		);
		pins.insert(
			std::pair <std::string, const gpioPin>(
				"P9_42", {
					"GPIO0_7",
					"P9_42",
					"",
					GPIO0,
					7,
					7,
					4,
					"ecap0_in_pwm0_out",
					0x164,
					{},
					{},
					(gpioPwm) {
						"ecap0",
						2,
						0,
						0,
						"ecap.0",
						"ECAPPWM0"
					},
					(gpioModes) {
						"eCAP0_in_PWM0_out",
						"uart3_txd",
						"spi1_cs1",
						"pr1_ecap0_ecap_capin_apwm_o",
						"spi1_sclk",
						"mmc0_sdwp",
						"xdma_event_intr2",
						"gpio0_7"
					}
				}
			)
		);
		return pins;
	}
}

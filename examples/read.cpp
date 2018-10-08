#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include "BBBiO.h"

int main() {
	NUBE::BBBiO myExample;

	NUBE::BBBiO::gpioPin DI1 = myExample.getPin("P9_13");
	NUBE::BBBiO::gpioPin DI2 = myExample.getPin("P9_15");
	NUBE::BBBiO::gpioPin DI3 = myExample.getPin("P9_17");
	NUBE::BBBiO::gpioPin DI4 = myExample.getPin("P9_19");
	NUBE::BBBiO::gpioPin DI5 = myExample.getPin("P9_23");
	NUBE::BBBiO::gpioPin DI6 = myExample.getPin("P9_25");
	NUBE::BBBiO::gpioPin DI7 = myExample.getPin("P9_27");

	for (int i = 0; i < 1000; i++) {
		std::cout << "DI1: " << (int)myExample.digitalRead(DI1) << std::endl;
		std::cout << "DI2: " << (int)myExample.digitalRead(DI2) << std::endl;
		std::cout << "DI3: " << (int)myExample.digitalRead(DI3) << std::endl;
		std::cout << "DI4: " << (int)myExample.digitalRead(DI4) << std::endl;
		std::cout << "DI5: " << (int)myExample.digitalRead(DI5) << std::endl;
		std::cout << "DI6: " << (int)myExample.digitalRead(DI6) << std::endl;
		std::cout << "DI7: " << (int)myExample.digitalRead(DI7) << std::endl;
		usleep(100000);
	}

	myExample.~BBBiO();

	exit (EXIT_SUCCESS);
}
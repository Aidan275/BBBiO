#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include "BBBiO.h"

int main() {
  NUBE::BBBiO myExample;

  NUBE::BBBiO::gpioPin P8_7 = myExample.getPin ("P8_7");
  NUBE::BBBiO::gpioPin P9_11 = myExample.getPin ("P9_11");
  NUBE::BBBiO::gpioPin P8_13 = myExample.getPin ("P8_13");

  const unsigned char high = myExample.HIGH;

  myExample.digitalWrite (P8_7, high);
  myExample.digitalWrite (P9_11, high);
  myExample.digitalWrite (P8_13, high);

  myExample.~BBBiO ();

  exit (EXIT_SUCCESS);
}
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include "BBBiO.h"

int main() {
  NUBE::BBBiO myExample;

  NUBE::BBBiO::gpioPin P8_7 = myExample.getPin ("P8_7");
  NUBE::BBBiO::gpioPin P9_11 = myExample.getPin ("P9_11");

  const unsigned char low = myExample.LOW;

  myExample.digitalWrite (P8_7, low);
  myExample.digitalWrite (P9_11, low);

  myExample.~BBBiO ();

  exit (EXIT_SUCCESS);
}
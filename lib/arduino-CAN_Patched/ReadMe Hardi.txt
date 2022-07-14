Espressif has added an aditional prescaler bit to be able to generate lower CAN baud rates
Unfortunately this bit is is part of the IER which was set to 0xFF in the original
CAN library from https://github.com/sandeepmistry/arduino-CAN.

This caused a wrong baud rate if an ESP32 with chip revision >=2 is used ;-(

mister0wl has tried to fix this (See: https://github.com/sandeepmistry/arduino-CAN/issues/62)
in his clone https://github.com/mister0wl/arduino-CAN

Unfortunately he overlooked that the register was written at the end of the procedure when the interrupts are enabled.

I have corrected this in ESP32SJA1000.cpp

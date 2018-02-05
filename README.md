# STM32F103C8T6

I've used SPI2 of the microcontroller to communicate with the LCD (SPI is mapped to JTAG pins so I couldn't use it).

LCD to board pinout :

GND   ----> GND  
LIGHT ----> GND  
VCC   ----> 3.3 V on board  
CLK   ----> PB2  
DIN   ----> PB4  
DC    ----> PA9  
CE    ----> PB1  
RST   ----> PA8  

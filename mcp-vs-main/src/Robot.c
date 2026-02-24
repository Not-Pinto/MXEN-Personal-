//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Robot.h"


int main(void)
{
    adc_init();
    _delay_ms(20);
    DDRK = 0x00;
    PORTK |= (1<<PK0); //Pull up 
    DDRA = 0xFF;
    PORTA = 0b00000000;
    static uint16_t xJoystick = 0;
    static uint16_t yJoystick = 0;
    static uint8_t level = 0;
    static uint8_t leds = 0b00000000;
    static uint8_t xJoystickChannel = 0;
    static uint8_t yJoystickChannel = 1;

  while(1)
  {
    leds = 0;
    if(PINK & (1<<PK0))
    {
      xJoystick = adc_read(xJoystickChannel);
      level = (xJoystick * 8) / 1024;
    }
    else
    {
      yJoystick = adc_read(yJoystickChannel);
      level = (yJoystick * 8) / 1024;
    }

    for(int i = 0; i<level; i ++)
    {
      leds |= (1<<i);
    }
    PORTA = leds;
  }
  return(1);
}
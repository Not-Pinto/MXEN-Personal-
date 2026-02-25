//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Robot.h"


int main(void)
{
    adc_init();
    _delay_ms(20);
    DDRA = 0xFF;
    PORTA = 0b00000000;
    static uint16_t record = 0;
    static uint8_t lightChannel = 0;
    static uint8_t level = 0;
    static uint8_t leds = 0b00000000;
  while(1)
  {
    leds = 0;
    record = adc_read(lightChannel);
    level = (((record) * 10) / 1024)-1;
    for(int i = 0; i<level; i ++)
    {
      leds |= (1<<i);
    }
    PORTA = leds;
  }
   // for(int i = 0; i<level; i ++)
   // {
  //    leds |= (1<<i);
  //  }
   // PORTA = leds;
  
  return(1);
}



/*
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
    static uint8_t xJoystickChannel = 0;
    static uint8_t yJoystickChannel = 1;
    static uint8_t level = 0;
    static uint8_t leds = 0b00000000;
  while(1)
  {
    leds = 0;
    if(PINK & (1<<PK0))
    {
      xJoystick = adc_read(xJoystickChannel);
      level = (xJoystick * 9) / 1024;
    }
    else
    {
      yJoystick = adc_read(yJoystickChannel);
      level = (yJoystick * 9) / 1024;
    }

    for(int i = 0; i<level; i ++)
    {
      leds |= (1<<i);
    }
    PORTA = leds;
  }
  return(1);
}
*/
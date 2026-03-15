//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Robot.h"
#define debounceDelay 200

volatile uint32_t rawtime = 0;
volatile bool start = false;

int main(void)
{
  serial0_init();
  milliseconds_init();

  char send[40];
  uint16_t firstpoint = 0; 
  uint16_t seconds = 0;

  DDRD &= ~(1<<DDD0);   
  PORTD |= (1<<PORTD0);

  DDRE &= ~(1<<DDE4);   
  PORTE |= (1<<PORTE4); 

  TCCR1A = 0x00;
  TCCR1B = 0x00;
  TCCR1C = 0x00;

  cli();

  EICRA |= (1<<ISC01);
  EICRA &= ~(1<<ISC00);
  EIMSK |= (1<<INT0);

  EICRB |= (1<<ISC41);
  EICRB &= ~(1<<ISC40);
  EIMSK |= (1<<INT4);

  TCCR1B |= (1<<WGM12); 
  TCCR1B |= (1<<CS11);
  TCCR1B |= (1<<CS10);
  OCR1A = 249;

  sei(); 

  while(1)
  {
    if (rawtime == 0)
    {
      sprintf(send, "\nTime: 0.0");
    }
    else
    {
      seconds = rawtime / 1000;
      firstpoint = (rawtime % 1000)/100;
      sprintf(send, "\nTime: %u.%u\n", seconds, firstpoint);
    }
    serial0_print_string(send);
  }
}

ISR(TIMER1_COMPA_vect)
{
  rawtime += 1;
}

ISR(INT4_vect) // start/stop
{
  uint32_t currentTime = milliseconds_now();
  static uint32_t previousTime = 0;
  if (currentTime - previousTime > debounceDelay)
  {
    previousTime = currentTime;
    if (start == false)
    {
      start = true;
      TIMSK1 = (1<<OCIE1A);
    }
    else
    {
      start = false;
      TIMSK1 &= ~(1<<OCIE1A);
    }
  }
}

ISR(INT0_vect) // reset 
{
  uint32_t currentTime = milliseconds_now();
  static uint32_t previousTime = 0;
  if (currentTime - previousTime > debounceDelay)
  {
    previousTime = currentTime;
    TIMSK1 &= ~(1<<OCIE1A);
    start = false;
    rawtime = 0;
  }
}
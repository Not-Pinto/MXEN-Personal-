//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Robot.h"
#define debounceDelay 
volatile uint8_t edges = 0;

int main(void)
{   
    milliseconds_init();
    serial0_init();

    DDRA = 0xFF; 
    PORTA = 0b00000000;
    DDRE &= ~(1<<PE4); 
    PORTE |= (1<<PE4); 

    char send[50];
    uint32_t secs = 0;
    uint32_t new = 0;
    uint32_t previousTime = 0;

    cli();
    EICRB &= ~(1<<ISC40);
    EICRB |= (1<<ISC41);
    EIMSK |= (1<<INT4); 
    sei();

  while(1)
  {
    new = milliseconds_now();
    if ((new - previousTime) >= 1000)
    {
        sprintf(send, "Edges recoded in sec %lu: %u\n", secs, edges);
        serial0_print_string(send);
        edges = 0;
        secs += 1;
        previousTime = new;
    }
  }
  return(1);
}

ISR(INT4_vect)
{
    uint32_t currentTime = milliseconds_now();
    static uint32_t previousTime = 0;
    if (currentTime - previousTime > debounceDelay)
    {
        previousTime = currentTime;
        edges += 1;
    }
}


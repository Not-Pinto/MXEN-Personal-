/*
#include "Robot.h"

volatile uint8_t flag = 0;

int main(void)
{
    serial0_init();
    adc_init();
    _delay_ms(20);

    DDRA = 0xFF; 
    PORTA = 0b00000000;
    DDRE &= ~(1<<PE4);  
    PORTE |= (1<<PE4);

    static uint16_t record = 0;
    static uint8_t lightChannel = 0;
    static float voltage = 0;
    char send[20];

    cli();
    EICRB &= ~(1<<ISC40);
    EICRB |= (1<<ISC41);
    EIMSK |= (1<<INT4); 
    sei();

  while(1)
  {
    record = adc_read(lightChannel);
    voltage = (record * 5.0) / 1024;
    if (flag)
    {
        sprintf(send, "\nVoltage: %.2f\n", voltage);
        serial0_send_string(send);
        flag = 0;
    }
  }
  return(1);
}

ISR(INT4_vect)
{
    flag = 1;
}
*/










/*
#include "Robot.h"
#define debounceDelay 50

volatile uint8_t flag = 0;

int main(void)
{   
    milliseconds_init();

    serial0_init();
    adc_init();
    _delay_ms(20);

    DDRA = 0xFF; 
    PORTA = 0b00000000;
    DDRE &= ~(1<<PE4);  
    PORTE |= (1<<PE4);

    static uint16_t record = 0;
    static uint8_t lightChannel = 0;
    static float voltage = 0;
    char send[20];

    cli();
    EICRB &= ~(1<<ISC40);
    EICRB |= (1<<ISC41);
    EIMSK |= (1<<INT4); 
    sei();

  while(1)
  {
    record = adc_read(lightChannel);
    voltage = (record * 5.0) / 1024;
    if (flag)
    {
        sprintf(send, "\nVoltage: %.2f\n", voltage);
        serial0_send_string(send);
        flag = 0;
    }
  }
  return(1);
}

ISR(INT4_vect)
{
    uint32_t currentTime = milliseconds_get();
    static uint32_t previousTime = 0;
    if (currentTime - previousTime > debounceDelay)
    {
        previousTime = currentTime;
        flag = 1;
    }
}
*/










/*
#include "Robot.h"
#define interval 1000

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
    new = milliseconds_get();
    if ((new - previousTime) >= 1000)
    {
        sprintf(send, "Edges recoded in sec %lu: %u\n", secs, edges);
        serial0_send_string(send);
        edges = 0;
        secs += 1;
        previousTime = new;
    }
  }
  return(1);
}

ISR(INT4_vect)
{
    edges += 1; 
}
*/





/*
#include "Robot.h"

volatile uint8_t flag = 0;

int main(void)
{
    serial0_init();
    adc_init();
    _delay_ms(20);

    DDRA = 0xFF; 
    PORTA = 0b00000001;
    DDRE &= ~(1<<PE4);  
    PORTE |= (1<<PE4);

    static uint16_t record = 0;
    static uint8_t lightChannel = 0;
    static uint16_t voltage = 0;
    char send[20];

    cli();
    EICRB &= ~(1<<ISC40);
    EICRB |= (1<<ISC41);
    EIMSK |= (1<<INT4); 
    sei();

  while(1)
  {
    record = adc_read(lightChannel);
    voltage = (record * 5.0);
    if (flag)
    {
    sprintf(send, "\nmilliVoltage: %hu\n", voltage);
    serial0_print_string(send);
    flag = 0;
    }
  }
  return(1);
}

ISR(INT4_vect)
{
    flag = 1;
}
*/






/*
#include "Robot.h"
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
    EICRB &= ~(1<<ISC41);
    EICRB |= (1<<ISC40);
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
    edges += 1; 
}
*/







/*
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
*/



// for Lab 4 onwards
/*
#include "Robot.h"

int main(void)
  {
    DDRA |= (1<<PA0);
    PORTA &= ~(1<<PA0);

    while(1)
    {
      PORTA |= (1<<PA0);
      ms_delay_(500);
      PORTA &= ~(1<<PA0); 
      ms_delay_(500); 
    }
  }
*/



/*
#include "Robot.h"

int main(void)
  {
    uint8_t RangeChannel = 1;
    uint16_t record;

    DDRA |= (1<<PA0);
    PORTA &= ~(1<<PA0);
    DDRF = 0x00;

    while(1)
    {
      record = adc_read(RangeChannel);
      if (record > 510)
        {
          PORTA |= (1<<PA0);
        }
      else
        {
          PORTA &= ~(1<<PA0);
        }
    }
  }
*/





/*
#include "Robot.h"

int main(void)
  {
    uint8_t RangeChannel = 1;
    uint16_t record;
    char send[10];
    serial0_init();
    DDRF = 0x00;
while(1)
    {
      record = adc_read(RangeChannel);
      sprintf(send, "ADC = %u \n", record);
      serial0_print_string(send);
      _delay_ms(100);
    }
  }
*/
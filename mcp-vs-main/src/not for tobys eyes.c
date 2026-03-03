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
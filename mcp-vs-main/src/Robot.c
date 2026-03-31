//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Robot.h"


int main(void){
 
serial2_init();
milliseconds_init();
adc_init();

uint32_t current_ms;
uint32_t last_send_ms = 0;

uint8_t x_joy_value8;
uint8_t y_joy_value8;

uint8_t recievedData[2];

uint16_t left_light_16;
uint16_t right_light_16;
uint16_t range_16;

uint8_t left_light_8;
uint8_t right_light_8;
uint8_t range_8;

  DDRB |= (1 << PB5);
  DDRB |= (1<< PB6);

  DDRF = 0;

  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1B |= (1 << WGM13);

  TCCR1A |= (1 << COM1A1);
  TCCR1A |= (1 << COM1B1);

  TCCR1B |= (1 << CS11);
  ICR1 = 20000;

uint16_t x_servo_value;
uint16_t y_servo_value;

while(1){

  current_ms = milliseconds_now();

  left_light_16 = adc_read(0);
  right_light_16 = adc_read(1);
  range_16 = adc_read(2);

  left_light_8 = ((left_light_16 / 1024.0) * 256);
  right_light_8 = ((right_light_16 / 1024.0) * 256);
  range_8 = ((range_16 / 1024.0) * 256);

  if( (current_ms-last_send_ms) >= 100) 
	{
		serial2_write_bytes(3, left_light_8, right_light_8, range_8); 
		last_send_ms = current_ms;
	}

  if(serial2_available()) 
	{
		serial2_get_data(recievedData, 2);
    x_joy_value8 = recievedData[0];
    y_joy_value8 = recievedData[1];
	}

  x_servo_value = 980 + (x_joy_value8 * 4);
  y_servo_value = 980 + (y_joy_value8 * 4);

  OCR1A = x_servo_value;
  OCR1B = y_servo_value;
}
} 
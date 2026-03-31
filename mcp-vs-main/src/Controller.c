//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Controller.h"



int main(void){

serial2_init();
milliseconds_init();
adc_init();
lcd_init();

DDRK = 0;

uint32_t current_ms;
uint32_t last_send_ms = 0;

uint16_t x_joy_value16;
uint16_t y_joy_value16;

uint8_t x_joy_value8 = 127;
uint8_t y_joy_value8 = 127;

uint8_t recievedData[3];

uint8_t left_light_8 = 0;
uint8_t right_light_8 = 0;
uint8_t range_8 = 0;

uint8_t diff;
char direction[10];
char move[10];

char lcd_string1[16];
char lcd_string2[16];

while(1)
{
	current_ms = milliseconds_now();

  x_joy_value16 = adc_read(0);  
  y_joy_value16 = adc_read(1);

  x_joy_value8 = (x_joy_value16 / 1024.0) * 256;
  y_joy_value8 = (y_joy_value16 / 1024.0) * 256;

	if( (current_ms-last_send_ms) >= 100) 
	{
		serial2_write_bytes(2, x_joy_value8, y_joy_value8); 
		last_send_ms = current_ms;
	}

  if(serial2_available()) 
	{
		serial2_get_data(recievedData,3);
    left_light_8 = recievedData[0];
    right_light_8 = recievedData[1];
    range_8 = recievedData [2];
	}

if (left_light_8 > right_light_8){
  diff = left_light_8 - right_light_8;
  if (diff > 20){
    sprintf(direction, "LEFT");
  }
  else{
    sprintf(direction, "CENTRE");
  }
}
else{
  diff = right_light_8 - left_light_8;
  if (diff > 20){
    sprintf(direction, "RIGHT");
  }
  else{
    sprintf(direction, "CENTRE");
  }
}

if (range_8 < 100) {
  sprintf(move, "STOP"); 
}
else{
  sprintf(move, "FORWARD");
}

sprintf(lcd_string1 , "TOWARD: %s" , direction);
sprintf(lcd_string2, "MOVE: %s" , move);
lcd_home(); 
lcd_clrscr();
lcd_puts(lcd_string2);
lcd_goto(0x40);
lcd_puts(lcd_string1);
}
}
//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Robot.h"

int main(void)
  {
    uint8_t RangeChannel = 0;
    uint16_t record;
    uint16_t cent;
    char send[40];
    serial0_init();
    adc_init();
while(1)
    {
      record = adc_read(RangeChannel);
      cent = (4518/(record-28));
      sprintf(send, "Centermeters = %u \n", cent);
      serial0_print_string(send);
      _delay_ms(1000);
    }
  }

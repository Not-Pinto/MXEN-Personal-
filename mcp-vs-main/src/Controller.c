/*
test to see if i can get simple one way wireless comunication


#include "Controller.h"


int main(void)
{
    serial2_init();      
    adc_init();          
    milliseconds_init();

    uint32_t last_send_ms = 0;
    uint16_t joy_x;
    uint8_t joy_x_8bit;

    while (1)
    {
        uint32_t current_ms = milliseconds_now();

        if ((current_ms - last_send_ms) >= 100)
        {
            joy_x = adc_read(0);

            joy_x_8bit = joy_x >> 2;

            serial2_write_byte(joy_x_8bit);

            last_send_ms = current_ms;
        }
    }
}
*/




// for outcome 3
/*
#include "Controller.h"

int main(void)
{
    serial2_init();      
    adc_init();          
    milliseconds_init();

    uint32_t last_send_ms = 0;
    uint16_t joy_x;
    uint8_t joy_x_8bit;

    

    while (1)
    {
        uint32_t current_ms = milliseconds_now();

        if ((current_ms - last_send_ms) >= 100)
        {
            joy_x = adc_read(0);

            joy_x_8bit = (joy_x * 253)/1023;

            serial2_write_byte(joy_x_8bit);

            last_send_ms = current_ms;
        }
    }
}

*/


// remainder of the lab
/*

#include "Controller.h"

int main(void)
{
    serial2_init();
    adc_init();
    milliseconds_init();

    uint32_t last_send_ms = 0;
    uint16_t left_y;
    uint16_t right_y;
    uint8_t left_y_8;
    uint8_t right_y_8;

    while (1)
    {
        uint32_t current_ms = milliseconds_now();

        if ((current_ms - last_send_ms) >= 100)
        {
            left_y = adc_read(15);
            right_y = adc_read(0);

            left_y_8 = (left_y * 253) / 1023;
            right_y_8 = (right_y * 253) / 1023;

            serial2_write_bytes(2, left_y_8, right_y_8);

            last_send_ms = current_ms;
        }
    }
}

*/
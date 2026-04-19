/*
test to see if i can get simple one way wireless comunication

#include "Robot.h"


int main(void)
{
    serial2_init();
    serial0_init();

    uint8_t received;

    while (1)
    {
        if (serial2_available())
        {
            received = serial2_read_byte();
            char buffer[50];
            sprintf(buffer, "Joystick X: %u\n", received);
            serial0_print_string(buffer);
        }
    }
}

*/



// for outcome 3
/*
#include "Robot.h"


int main(void)
{
    serial2_init();
    serial0_init();

    uint8_t received = 120;

    DDRB |= (1 << PB5);
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= (1 << WGM13);
    TCCR1A |= (1 << COM1A1);
    TCCR1B |= (1 << CS11);
    ICR1 = 5000;

    while (1)
    {
        if (serial2_available())
        {
            received = serial2_read_byte();
        }

        if (received < 116){
          OCR1A = ((uint32_t)5000 * (115 - received))/115;
        }
        else if (received > 139){
          OCR1A = ((uint32_t)5000 * (received - 139))/115;
        }
        else{
          OCR1A = 0;
        }
    }
}

*/



// remainder of the lab

/*
#include "Robot.h"

int main(void)
{
    serial2_init();
    serial0_init();

    uint8_t received_data[2] = {120, 120};
    uint8_t left_y = 120;
    uint8_t right_y = 120;

    DDRB |= (1 << PB5);   
    DDRB |= (1 << PB6);   

    DDRA |= (1 << PA0);   
    DDRA |= (1 << PA1);   
    DDRA |= (1 << PA2);   
    DDRA |= (1 << PA3);   

    PORTA &= ~(1 << PA0);
    PORTA &= ~(1 << PA1);
    PORTA &= ~(1 << PA2);
    PORTA &= ~(1 << PA3);

    
    TCCR1A = 0;
    TCCR1B = 0;

    TCCR1B |= (1 << WGM13);              
    TCCR1A |= (1 << COM1A1);             
    TCCR1A |= (1 << COM1B1);             
    TCCR1B |= (1 << CS11);               

    ICR1 = 5000;                         
    OCR1A = 0;
    OCR1B = 0;

    while (1)
    {
        if (serial2_available())
        {
            serial2_get_data(received_data, 2);
            left_y = received_data[0];
            right_y = received_data[1];
        }

        if (left_y < 116)
        {
            PORTA &= ~(1 << PA0);
            PORTA |=  (1 << PA1);
            OCR1A = ((uint32_t)ICR1 * (115 - left_y)) / 115;
        }
        else if (left_y > 138)
        {
            PORTA |=  (1 << PA0);
            PORTA &= ~(1 << PA1);
            OCR1A = ((uint32_t)ICR1 * (left_y - 138)) / 115;
        }
        else
        {
            PORTA &= ~(1 << PA0);
            PORTA &= ~(1 << PA1);
            OCR1A = 0;
        }

        if (right_y < 116)
        {
            PORTA &= ~(1 << PA2);
            PORTA |=  (1 << PA3);

            OCR1B = ((uint32_t)ICR1 * (115 - right_y)) / 115;
        }
        else if (right_y > 138)
        {

            PORTA |=  (1 << PA2);
            PORTA &= ~(1 << PA3);
            OCR1B = ((uint32_t)ICR1 * (right_y - 138)) / 115;
        }
        else
        {
            PORTA &= ~(1 << PA2);
            PORTA &= ~(1 << PA3);
            OCR1B = 0;
        }
    }
}
*/
#include <avr/io.h>
#include <avr/interrupt.h>

#define TRIGGER_ECHO_PIN PD0 

volatile uint16_t timer_count = 0;
volatile uint8_t sensor_state = 0; 
// 0 not doing anything
// 1 sending pulse
// 2 waiting for return pulse
// 3 waiting for second return pulse:

void init_interrupts() {
    // Configure INT0 for rising edge detection initially

    EICRA |= (1 << ISC00) | (1 << ISC01);
    EIMSK |= (1 << INT0);
}


void init_timer1() {
    // Set up timer one, TCCR1B |= (1 << CS11);, and TCCR1B &= ~(1 << CS11);
    // will be used to togal the timer on and off elsewhere in the code

    TCCR1A = 0;           
    TCCR1B = 0;           
    TCCR1C = 0;           
    TCNT1 = 0; 
}


void trigger_sensor() {
    // Start sending pulse to the sensor and set up a 5 us timer

    if (sensor_state != 0) {
        return;
    }
    sensor_state = 1;              
    DDRD |= (1 << TRIGGER_ECHO_PIN);   
    PORTD |= (1 << TRIGGER_ECHO_PIN);  

    TCNT1 = 0;
    OCR1A = 10;  
    TIMSK1 |= (1 << OCIE1A);   
    TCCR1B = (1 << CS11);             
}


ISR(TIMER1_COMPA_vect) {   
    // End pulse after 5 us, set up timeout timer

    if (sensor_state != 1) {
        return;
    }
    TCCR1B = 0;
    TIMSK1 &= ~(1 << OCIE1A); 
    PORTD &= ~(1 << TRIGGER_ECHO_PIN);
    DDRD &= ~(1 << TRIGGER_ECHO_PIN);
    
    sensor_state = 2;
    
    TCNT1 = 0;
    OCR1B = 40000; 
    TIMSK1 |= (1 << OCIE1B);
    TCCR1B = (1 << CS11);
}


ISR(TIMER1_COMPB_vect) {
    // time out error, if waiting for a response from sensor for as least 20ms return error

    if (sensor_state != 2 && sensor_state != 3)  {
        return;
    }
    TCCR1B = 0;
    TIMSK1 &= ~(1 << OCIE1B);
    sensor_state = 0;
    return; 
}


ISR(INT0_vect) {
    // read the time for the sound to be detected by the sensor

    if (sensor_state == 2) {
        TCNT1 = 0;
        sensor_state = 3;
        EICRA &= ~(1 << ISC00);
        EICRA |= (1 << ISC01);  
    } else if (sensor_state == 3) {
        timer_count = TCNT1;
        TIMSK1 &= ~(1 << OCIE1B);
        TCCR1B = 0;
        EICRA |= (1 << ISC00);
        EICRA |= (1 << ISC01);
        sensor_state = 0; 
    }
    else {
        return; 
    }
}


uint16_t calculate_distance() {
    // convert the the time into distance 
    // assuming in room tempreture and the speed of sound is 343.6 m/s

    return (timer_count / 2) / 58;  
}


int main() {
    // Configure the shared pin as input initially
    DDRD &= ~(1 << TRIGGER_ECHO_PIN);
    PORTD &= ~(1 << TRIGGER_ECHO_PIN); // Ensure the pin is low

    // Initialize peripherals
    init_interrupts();
    init_timer1();
    sei(); // Enable global interrupts

    uint16_t distance;
    uint16_t distance_dated = 0; // a variable used to corectly update the distance variable 
                                 // every loop in the while(1)

    while (1) {

        if (distance_dated == 0){
            distance = calculate_distance();
            distance_dated = 1;
        }
        else if (sensor_state == 0) {
            distance_dated = 0;
            trigger_sensor();
        }
        
        // Use the distance value (e.g., send it over UART or display it)

    }
}

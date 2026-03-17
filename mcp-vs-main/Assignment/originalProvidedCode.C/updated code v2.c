#include <avr/io.h>
#include <avr/interrupt.h>

#define TRIGGER_ECHO_PIN PD0 // First change is to swap PD2 to PD0, as the program use
                             // Int0 and PD2 is Int 2 where as PD0 is Int 0 [1 p. 2]
                             // alternativly INT0_vect could be changed to INT2_vect
                             // and ISC00 to ISC20 and ISC01 to ISC21 [2 p. 110]), but changing 
                             // the TRIGGER_ECHO_PIN is less work and keeps more ot the 
                             // original code


volatile uint16_t timer_count = 0;
volatile uint8_t sensor_state = 0; // 0 not doing anything, 1 sending pulse, 2 waiting hold off,
                                   // 3 sending out, 4 waiting for return, 5 delay before measurment 


void init_interrupts() {
    // Configure INT0 for rising edge detection initially

    EICRA |= (1 << ISC00) | (1 << ISC01); // Confirmed from Data sheet [2 p. 110] that
    EIMSK |= (1 << INT0);                 // this is rising edge, no changes needed
}


void init_timer1() {
    // Configure Timer1 in normal mode

    TCCR1A = 0;           // The timer is set up with normal mode and Prescaler = 8
    TCCR1B = 0;           // for this WGM0, WGM1, WGM2, WGM3 must all be 0, to set to 
    TCCR1C = 0;           // normal mode and for Prescaler = 8 CS10, CS12 are set to 0 and
                          // CS11 is set to 1, compare output mode, input capture Noise
                          // cancleler input capture edge select and force outpare compaer
                          // will not be used so all there bits are set to 0, this results 
                          // in TCCR1A all being 0, TTCCR1B has the CS11 bit set to one and
                          // TTCCR1C register is all set to 0, thus no changes are needed
                          // From datasheet [2 p. 130, 154, 157, 156, 158]

    TCNT1 = 0; // Confirmed in the data sheet [2 p. 115) thst TNCTn is the where the timer
               // value is stored, aslo confirmed that setting it 0 is a valid operation
               // no changes needed
}




void trigger_sensor() {
    if (sensor_state != 0) {
        return -1;
    }

    sensor_state = 1;              

    DDRD |= (1 << TRIGGER_ECHO_PIN);   
    PORTD |= (1 << TRIGGER_ECHO_PIN);  

    sensor_state = 1;
    TCNT1 = 0;
    OCR1A = 10;  
    TIMSK1 |= (1 << OCIE1A);   
    TCCR1B = (1 << CS11);             
}


ISR(TIMER1_COMPA_vect) {   // timere for how long to send pulse to sensor 
    if (sensor_state != 1) {
        return -1;
    }
    TCCR1B = 0;
    TIMSK1 &= ~(1 << OCIE1A); 
    PORTD &= ~(1 << TRIGGER_ECHO_PIN);
    DDRD &= ~(1 << TRIGGER_ECHO_PIN);
    sensor_state = 2;
    
    TCNT1 = 0;
    OCR1B = 40000; // 20ms, if no response from the Sensor by then smth has goe wrong
    TIMSK1 |= (1 << OCIE1B);
    TCCR1B = (1 << CS11);
}


ISR(TIMER1_COMPB_vect) {
    if (sensor_state != 2 && sensor_state != 3)  {
        return -1;
    }
    sensor_state = 0;
    return -1; // missing data form sensor
}

ISR(INT0_vect) {
    if (sensor_state == 2) {
        sensor_state = 3;

        EICRA &= ~(1 << ISC00);
        EICRA |= (1 << ISC01);  

    } else if (sensor_state == 3) {
        // Falling edge detected, stop Timer1
        timer_count = TCNT1;

        TIMSK1 &= ~(1 << OCIE1B);

        sensor_state = 0;
        
        EICRA |= (1 << ISC00);
        EICRA |= (1 << ISC01);
    }

    else {
        return -1; // unexpected state 
    }
}

uint16_t calculate_distance() {
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

    while (1) {

        if (sensor_state == 0) {
            distance = calculate_distance();

            // Use the distance value (e.g., send it over UART or display it)
        }
    }
}

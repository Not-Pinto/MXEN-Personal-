#include <avr/io.h>
#include <avr/interrupt.h>

#define TRIGGER_ECHO_PIN PD0 // First change is to swap PD2 to PD0, as the program use
                             // Int0 and PD2 is Int 2 where as PD0 is Int 0 [1 p. 2]
                             // alternativly INT0_vect could be changed to INT2_vect
                             // and ISC00 to ISC20 and ISC01 to ISC21 [2 p. 110]), but changing 
                             // the TRIGGER_ECHO_PIN is less work and keeps more ot the 
                             // original code


volatile uint16_t timer_count = 0;
volatile uint8_t echo_received = 0;

volatile uint8_t sensor_read_complete = 1; // Change 2 addded a flag is the sensor is curently 
                                           // reading or not 

void init_interrupts() {
    // Configure INT0 for rising edge detection initially

    EICRA |= (1 << ISC00) | (1 << ISC01); // Confirmed from Data sheet [2 p. 110] that
    EIMSK |= (1 << INT0);                 // this is rising edge, no changes needed
}


void init_timer1() {
    // Configure Timer1 in normal mode

    TCCR1A = 0;           // The timer is set up with normal mode and Prescaler = 8
    TCCR1B = (1 << CS11); // for this WGM0, WGM1, WGM2, WGM3 must all be 0, to set to 
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
    // Set the pin as output to send the trigger pulse

    sensor_read_complete = 0; // change 3 set the sensor_read_flag to 0 whilst it is reading

    DDRD |= (1 << TRIGGER_ECHO_PIN); // Port D is an  output
    PORTD |= (1 << TRIGGER_ECHO_PIN); // Start the pulse

    // Checking length of time of each tick, PRE = 8, clock speed = 16,000,000
    // the time of every click = PRE * (1/Hz) = 8 * (1/16,000,000) = 0.0000005 s
    // 0.0000005 = 0.5 µs, thus confirming every tick = 0.5 µs

    TCNT1 = 0; // Reset Timer1
    TCCR1B |= (1 << CS11); // Start Timer1 with prescaler = 8

    while (TCNT1 < 10); // Change 4, original condition was TCNT1 < 20 which would be 10 µs
                        // however the ping data sheet [3 p. 2] says a typical input of 5 µs
                        // thus the conditon was ultered TCNT1 < 10 (10 * 0.5 = 5)
                        // however this is still blocking
                        
    PORTD &= ~(1 << TRIGGER_ECHO_PIN); // End the pulse
    TCCR1B &= ~(1 << CS11); // Stop Timer1
    TCNT1 = 0; // Reset Timer1

    // Set the pin as input to listen for the echo
    DDRD &= ~(1 << TRIGGER_ECHO_PIN);
}


ISR(INT0_vect) {
    if (echo_received == 0) {
        // Rising edge detected, start Timer1
        TCNT1 = 0;
        TCCR1B |= (1 << CS11); // Start Timer1 with prescaler = 8
        echo_received = 1;
        // Change INT0 to detect falling edge
        EICRA &= ~(1 << ISC00);
        EICRA |= (1 << ISC01);  // could be slight error here as the interupt could be called again by noise before the pin is turned to falling edge
    } else {
        // Falling edge detected, stop Timer1
        TCCR1B &= ~(1 << CS11); // Stop Timer1
        timer_count = TCNT1;
        echo_received = 0;

        sensor_read_complete = 1; // cchange 5 mark the sensor as competed its reading
        
        // Change INT0 to detect rising edge
        EICRA |= (1 << ISC00);
        EICRA |= (1 << ISC01);
    }
}

uint16_t calculate_distance() {

    // 0.5 µs  per tick has been confirmed true above
    // speed of sound at room tempreture is 343.6 m/s [4] which is 0.03436 cm/μs
    // 0.03436/2 = 0.01718 cm/μs, 1/0.01718 = 58.2 μs/cm, thus (timer_count/2) / 58
    // is corrected, but 58 is not cm/μs as in the original comments it is μs/cm
    // thus the code is left unchanged, but just note the original comments were 
    // incoreect 
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

        if (echo_received == 0) {
            distance = calculate_distance();

            if (sensor_read_complete == 1) // change 6 make sure the previous read has 
                {                          // finished before triggering another one
                trigger_sensor();
                }
            // Use the distance value (e.g., send it over UART or display it)
        }
    }
}







// [1] Arduino, "Arduino Mega 2560 Rev3 Full Pinout," [Online]. Available: https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf. [Accessed: 17-Mar-2026].
// [2] Microchip Technology Inc., "ATmega640/1280/1281/2560/2561 Datasheet," [Online]. Available: https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/Product
// Documents/DataSheets/ATmega640-1280-1281-2560-2561-Datasheet-DS40002211A.pdf. [Accessed: 17-Mar-2026].
// [3] Parallax Inc., "PING))) Ultrasonic Distance Sensor Product Guide," [Online]. Available: https://www.mouser.com/datasheet/2/321/28015-PING-Sensor-Product-Guide-v2.0-461050.pdf. [Accessed: 17-Mar-2026].
// [4] NDT Resource Center, "Temperature and Speed of Sound," [Online]. Available: https://www.nde-ed.org/Physics/Sound/tempandspeed.xhtml. [Accessed: 17-Mar-2026].
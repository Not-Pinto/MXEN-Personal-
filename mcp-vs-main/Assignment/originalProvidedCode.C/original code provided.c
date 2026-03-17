#include <avr/io.h>
#include <avr/interrupt.h>

#define TRIGGER_ECHO_PIN PD2 // Shared Trigger and Echo pin connected to PD2 (INT0)

volatile uint16_t timer_count = 0;
volatile uint8_t echo_received = 0;

void init_interrupts() {
    // Configure INT0 for rising edge detection initially
    EICRA |= (1 << ISC00) | (1 << ISC01); // ISC01 = 1, ISC00 = 1 -> Rising edge
    EIMSK |= (1 << INT0);  // Enable INT0
}

void init_timer1() {
    // Configure Timer1 in normal mode
    TCCR1A = 0; // Normal mode
    TCCR1B = (1 << CS11); // Prescaler = 8
    TCNT1 = 0; // (is the the timer value) Reset timer count
}

void trigger_sensor() {
    // Set the pin as output to send the trigger pulse
    DDRD |= (1 << TRIGGER_ECHO_PIN); // Port D is an  output
    PORTD |= (1 << TRIGGER_ECHO_PIN); // Start the pulse

    // Use Timer1 to generate a 10 Âµs pulse.  // chack the time
    TCNT1 = 0; // Reset Timer1
    TCCR1B |= (1 << CS11); // Start Timer1 with prescaler = 8
    while (TCNT1 < 20); // Wait for 10 Âµs (20 ticks at 0.5 Âµs per tick)

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
        // Change INT0 to detect rising edge
        EICRA |= (1 << ISC00);
        EICRA |= (1 << ISC01);
    }
}

uint16_t calculate_distance() {
    // Calculate distance in cm
    // Timer1 clock = 16MHz / 8 = 2MHz -> 0.5us per tick
    // Distance = (timer_count * 0.5us) / 58 (speed of sound in cm/us)
    return (timer_count / 2) / 58;  // probably best to add a tempreture variable as this could vary with tempreture
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
        trigger_sensor();
        _delay_ms(60); // Wait for the echo to complete
        if (echo_received == 0) {
            distance = calculate_distance();
            // Use the distance value (e.g., send it over UART or display it)
        }
    }
}
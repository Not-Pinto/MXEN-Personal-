#include <avr/io.h>
#include <avr/interrupt.h> 
#include "serial.h" // allow for serial comunication, needed to diplay any errors

#define TRIGGER_ECHO_PIN PD0 // Shared Trigger and Echo pin connected to PD0 (INT0)

volatile uint8_t distance_updated = 0; // a flag if it is time to update the distance variable in main
volatile uint16_t timer_count = 0; // the amount of 0.5 us ticks recived during the echo pulse width
volatile uint8_t error_state = 0; // a flag for diffrent errors
// 0 no error 
// 1 error, tried to trigger sensor whilst the sensor was still in use
// 2 error, tried to end a pulse that wasnt sent 
// 3 error, time out error, missing respose from sensor
// 4 error, time out error called during wrong comunication state
// 5 error, interupt 0 called during wrong comunications state
// 6 error, reult out of range


volatile uint8_t sensor_state = 0; // the current point in reading the sensor 
// 0 not doing anything
// 1 sending pulse
// 2 waiting for return pulse start of echo pulse
// 3 waiting for second return pulse end of echo pulse

void init_interrupts() {
    // Configure INT0 for rising edge detection initially

    EICRA |= (1 << ISC00) | (1 << ISC01); // rising edge selected 
    EIMSK |= (1 << INT0); // turn on INT0 
}


void init_timer1() {
    // to set up timer 1 to be in normal mode TCCR1B |= (1 << CS11); shoudld be set, 
    // with everything else low, TTCCR1A/C register should also be set to low, 
    // scince the timer should only be running when it is needed all three registers
    // should be set low intitially and TCCR1B |= (1 << CS11);, and TCCR1B = 0; will
    // be used to togal the timer on off respectivley.

    TCCR1A = 0;           
    TCCR1B = 0;           
    TCCR1C = 0;           
    TCNT1 = 0; 
    OCR1B = 40000; // set compare B for 20 ms (at 0.5 us per tick, 40000 * 0.5 = 20 ms)
    OCR1A = 10; // set compare A for 5 us (10 ticks at 0.5 us per tick = 5 us)
}

void reset_sensor () {
    // if an error occurs then this finction will be called to reset any changes
    // to pin states or timers that have been set, so the next run starts fresh

    // turn off timer 1, disable timer 1 compare A and B interupts and reset it to 0
    TCCR1B = 0;
    TIMSK1 &= ~(1 << OCIE1A);
    TIMSK1 &= ~(1 << OCIE1B);
    TCNT1 = 0;

    // set up the trigger pin to input with no pull up 
    DDRD &= ~(1 << TRIGGER_ECHO_PIN);
    PORTD &= ~(1 << TRIGGER_ECHO_PIN);

    // reset interupt 0 to rising edge
    EICRA |= (1 << ISC00) | (1 << ISC01);
    
    timer_count = 0; // reset the echo pulse recorded length back to 0
    distance_updated = 0; // ensure main doesnt try to update distance
    sensor_state = 0; // set sensor to idle
}


void trigger_sensor() {
    // Start sending pulse to the sensor and set up a 5 us timer

    if (sensor_state != 0) { // confirm trigger_sensor has not been called 
        error_state = 1;     // whilst still getting value from a previous call
        reset_sensor();
        return;
    }

    timer_count = 0; // reset the return pulse counter from last reading

    distance_updated = 0; // the distance flag set 0 as distance will not 
                          // be updated whilst sensor protocol is running

    sensor_state = 1;  // sets the sensor state to 1 to indicate it is sending
                       // a pulse and begining the comunication protocol

    // set up comunication pin PD0 as an output and set to high (outputing)
    DDRD |= (1 << TRIGGER_ECHO_PIN);
    PORTD |= (1 << TRIGGER_ECHO_PIN);

    // set up for TIMER1 compare A interupt to trigger after 5 us and end the pulse
    // 5 is the typical length of time for the trigger pulse in the sensor protocol
    TCNT1 = 0; // reset the timer to 0
    TIMSK1 |= (1 << OCIE1A); // turn on interupt compare A interupt (5 us)
    TCCR1B = (1 << CS11); // turn on the timer             
}


ISR(TIMER1_COMPA_vect) {   
    // End pulse after 5 us as per protocol for the sensor, and set up time out
    // interupt and interupt0 to listen for return pulse (rising edge)

    // turn off timer 1, stop it from triggering at OCIE1A and reset it to 0
    TCCR1B = 0;
    TIMSK1 &= ~(1 << OCIE1A);
    TCNT1 = 0;

    // confirm that the timer1 compare A was triggered at the correct step in 
    // the comunication protocol, and set error flag if not.
    if (sensor_state != 1) {
        error_state = 2; // tried to end pulse when pulse was not being sent
        reset_sensor();
        return;
    }

    sensor_state = 2; // update curent step in the protocol to waiting for return pulse

    // end trigger pulse and set pin for input to recive echo
    PORTD &= ~(1 << TRIGGER_ECHO_PIN); 
    DDRD &= ~(1 << TRIGGER_ECHO_PIN);
    
    // set OCR1B so timer 1 compare B goes off after 20 ms
    TCNT1 = 0;
    TIMSK1 |= (1 << OCIE1B); // turn on timer 1 compare B interupt (20 ms)
    TCCR1B = (1 << CS11); // start the timer
}


ISR(TIMER1_COMPB_vect) {
    // time out timer, will trigger if there are no return signals from the sensor 
    // within 20 ms, from the data sheet, the worst case the entire comunication protocol 
    // should be finished within 19.65 ms, including the steps already completed, thus 20 ms 
    // is used for a bit of leeway

    // if the start of echo pulse was never recived or if the end of the echo pulse has not 
    // been recived within 20 ms of the trigger pulse being turned off
    if (sensor_state == 2 || sensor_state == 3) {
        error_state = 3; 
        reset_sensor();
    }
    else {
        error_state = 4; // time out error called during wrong step in the protocol
        reset_sensor();
    }
}



ISR(INT0_vect) {
    // either, on first call, reset a timer 1 to 0 and set up falling edge, on second call
    // record timer value as timer_count reset iterupt 0 to rising edge and reset sensor 
    // state to 0 for next call, and distance updated flag is set to 1 so main knows a new 
    // timer_count has been recorded and the local distance variable can be updated

    // rising edge detected, start of the return pulse
    if (sensor_state == 2) { 
        TCNT1 = 0; // set the timer back to 0, to start recording the time of the return pulse

        sensor_state = 3; // update the sensor_state flag to reflect the start of the pulse 
                          // has been detected and await for the falling edge

        // set up falling edge to detect the end of the return pulse
        EICRA &= ~(1 << ISC00);
        EICRA |= (1 << ISC01); 
     }

    // falling edge detected, end of the return pulse
    else if (sensor_state == 3) {
        timer_count = TCNT1; // record the time since the pulse started as timer_count

        // turn off timer 1, turn off the timer 1 compare B interupt and set the timer back to 0
        TCCR1B = 0;
        TIMSK1 &= ~(1 << OCIE1B);
        TCNT1 = 0;

        // set interupt 0 back to rising edge for next run of the comunication protocol 
        EICRA |= (1 << ISC00) | (1 << ISC01); // rising edge selected 

        sensor_state = 0; // set sensor state to idle for next run
        distance_updated = 1; // let the main function know that a new recording is ready 
                              // to be converted into a useable distance
    }

    // if the interupt 0 was activated whilst not waiting for the start or end of the return pulse 
    // from the sensor, a error falg will be set and timer and interupt will be reset for another run
    else {
        error_state = 5; // error to show that interupt was called at the wrong time
        reset_sensor();
    }
}


uint16_t calculate_distance() {
    // Calculate distance in cm
    // Timer1 clock = 16MHz / 8 = 2MHz -> 0.5us per tick
    // speed of sound at room tempreture is 343.6 m/s which is 0.03436 cm/μs
    // 0.03436/2 = 0.01718 cm/μs, 1/0.01718 = 58.2 μs/cm, thus (timer_count/2) / 58

    // if outside of reliable range, will flag an error to error stat and return 0,
    // that shoudld provide enough for the rest of the code to figure out what to do 
    // in that scenario 

    uint16_t value; // where the calculated distance will be temporarly stored

    value = (timer_count / 2) / 58; // convert timer count into distance
    if (value >= 2 && value <= 300) { // if in reliable range
        return value; // return the distance
    }
    else { // if outside of reliable range
        error_state = 6; // flag error, out of reliable range
        return 0; // return 0, since 0 itself is out of range then the only
                  //  way 0 could be a possible output is if the sensor is out 
                  // of range, the user can set up what to do in main if the
                  // the sensor is out of range (if calculate_distance returns 0)
    }
}


void error_handler() {
    // checks what error has been flagged and prints it to serial monitor 
    // resets error_state to 0 so the program can be run again, as some errors
    // such as out of reliable range would be expected and should not end the program

    if (error_state == 1) { // if this error occurs
        serial0_print_string("Error: tried to trigger sensor whilst already in use"); // print this message to  serial monitor 
    }
    else if (error_state == 2) {
        serial0_print_string("Error: tried to end a pulse that has not been started");
    }
    else if (error_state == 3) {
        serial0_print_string("Error: time out error, missing a response from the sensor");
    }
    else if (error_state == 4) {
        serial0_print_string("Time out error called during the wrong step in the comunication protocal");
    }
    else if (error_state == 5) {
        serial0_print_string("Interupt 0 called during wrong state of comunication");
    }
    else if (error_state == 6) {
        serial0_print_string("Out of reliable range (2 cm to 3 m)");
    }
    error_state = 0; // reset for next attempt  
}


int main() {
    // the main loop of the code that contains the setup and main loop

    // Initialize peripherals
    init_interrupts();
    init_timer1();
    serial0_init(); // needed for error handling

    // set pin D0 as input and low
    DDRD &= ~(1 << TRIGGER_ECHO_PIN);
    PORTD &= ~(1 << TRIGGER_ECHO_PIN); 

    uint16_t distance = 0; // a local variable where the recorded distance (cm) will be recorded

    sei(); // Enable global interrupts

    while (1) {
        if (error_state != 0) { // if an error occured in the last run
            error_handler(); // run error handler to inform the user 
            }
        else if (distance_updated == 1) { // if ready to calculate distance, note 
                                          // distance updated will only be flaged as 1 
                                          // if the previous comunication with the sensor 
                                          // was a success, ie an echo pulse was received, amd
                                          // both the start and end of the echo pulse were recorded

            distance = calculate_distance(); // sets local variable to the calculated distance
                                             // or 0 if the distance was out of the reilible 
                                             // range
            distance_updated = 0; // the distance has now been updated to reflect the current
                                  // reading, will be set back to when a new reading is ready 
            }
        else if (sensor_state == 0) { // if the sensor is idle then trigger is to get a reading
            trigger_sensor();
            }

        // do stuff with the distance here
        // if an error is raised occurs it will be sent to the serial monitor 
        // at the start of the next loop, if any error besides an out of range 
        // error the distance will not have been updated and will loop through 
        // with the old distance once again, if out of range the distance will be 0 
        // (0 is not in range thus the only time distance would be 0 is if it was out 
        // of range), the user should implement some sort of check to see if the distance 
        // is out of range as it would mean that the object is either closer than 2 cm, or 
        // further than 3 m
        }
}
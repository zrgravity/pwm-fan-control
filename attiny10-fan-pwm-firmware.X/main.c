/*
 * File:   main.c
 * Author: Zeke Gustafson
 *
 * Created on October 19, 2021, 12:44 PM
 */


#include <avr/io.h>
#include <inttypes.h>

int main(void) {
    // Setup system clock - 8MHz
    CCP = 0xD8; // Unlock registers
    CLKMSR = 0; // Internal 8MHz
    CLKPSR = 0; // Prescaler set to 1
    
    // Setup Pins
    // PB0 Timer OC0A Output
    DDRB |= _BV(PB0);
    // PB1 ADC input
    DDRB &= ~(_BV(PB1));
    // PB2 Output
    DDRB |= _BV(PB2);
    // PB3 ~RESET
    
    // Setup ADC
    PRR &= ~(_BV(PRADC)); // Clear PRADC
    ADCSRA |= _BV(ADEN); // Set ADEN
    ADMUX = 1; // Set channel PB1
    DIDR0 |= _BV(ADC1D); // Set PB1 digital input disable
    ADCSRA |= (_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0)); // Set clock prescaler to 128 (8MHz/128 = 62.5kHz)
    
    // Setup PWM output
    
    // Setup status led
    
    while (1) {
        // Read ADC
        ADCSRA |= _BV(ADSC); // Start conversion
        while (ADCSRA & _BV(ADSC)) // Wait for conversion
            ;
        uint8_t adc_result = ADCL;
        
        // Set PWM duty cycle
    }
}

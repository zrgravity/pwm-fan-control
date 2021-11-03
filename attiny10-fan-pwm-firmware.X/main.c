/*
 * File:   main.c
 * Author: Zeke Gustafson
 *
 * Created on October 19, 2021, 12:44 PM
 */


#include <avr/io.h>
#include <inttypes.h>
#include "light_ws2812/light_ws2812_AVR/ws2812_config.h"
#include "light_ws2812/light_ws2812_AVR/Light_WS2812/light_ws2812.h"

struct cRGB led[1];

int main(void) {
    // Setup system clock - 8MHz
    CCP = 0xD8; // Unlock registers
    CLKMSR = 0; // Internal 8MHz
    CCP = 0xD8; // Unlock registers
    CLKPSR = 0; // Prescaler set to 1
    
    // Setup Pins
    // PB0 OC0A for PWM out
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
    
    // Setup Fast PWM output
    // 8MHz, Prescale = 1; TOP at 799 -> 10kHz output
    GTCCR |= _BV(TSM); // Stop timer
    ICR0 = 799;
    TCCR0B &= ~(_BV(CS02) | _BV(CS01)); // Prescale = 1; CS02 = 0, CS01 = 0; CS00 = 1
    TCCR0B |= _BV(CS00);
    // Setup OC0A - Fast PWM Mode, TOP at ICR0
    TCCR0B |= _BV(WGM03); // WGM03 = 1;
    TCCR0B |= _BV(WGM02); // WGM02 = 1;
    TCCR0A |= _BV(WGM01); // WGM01 = 1;
    TCCR0A &= ~_BV(WGM00); // WGM00 = 0;
    // Clear OC0A on match, high at BOTTOM
    TCCR0A |= _BV(COM0A1); // COM0A1 = 1;
    TCCR0A &= ~_BV(COM0A0); // COM0A0 = 0;
    // PB0 Timer OC0A Output
    DDRB |= _BV(PB0);
    
    OCR0A = 400; // Reset duty cycle to 50% * (799+1) = 400
    GTCCR = 0; // Restart timer
    
    // Blink status led
    led[0].r = 0;
    led[0].g = 255;
    led[0].b = 0;
    ws2812_setleds(led, 1);
    
    while (1) {
        // Read ADC
        ADCSRA |= _BV(ADSC); // Start conversion
        while (ADCSRA & _BV(ADSC)) // Wait for conversion
            ;
        uint8_t adc_result = ADCL;
        
        // Set PWM duty cycle
        OCR0A = (uint16_t)((uint32_t)adc_result * 47) / 15;
    }
}

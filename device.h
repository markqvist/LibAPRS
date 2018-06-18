#include "constants.h"

#ifndef DEVICE_CONFIGURATION
#define DEVICE_CONFIGURATION

// CPU settings
#ifndef TARGET_CPU
    #define TARGET_CPU m328p
#endif

#ifndef F_CPU
    #define F_CPU 16000000
#endif

#ifndef FREQUENCY_CORRECTION
    #define FREQUENCY_CORRECTION 0
#endif

// Sampling & timer setup
#define CONFIG_AFSK_DAC_SAMPLERATE 9600

// Port settings
#if TARGET_CPU == m328p
    #define DAC_PORT PORTD
    #define DAC_DDR  DDRD
    #define LED_PORT PORTB
    #define LED_DDR  DDRB
    #define ADC_PORT PORTC
    #define ADC_DDR  DDRC
#endif

#endif
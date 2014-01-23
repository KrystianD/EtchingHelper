#ifndef __HARDWARE_H__
#define __HARDWARE_H__

// IO defines
#define A1 PORTC,4
#define B1 PORTC,5
#define C1 PORTB,5
#define D1 PORTC,0
#define E1 PORTC,1
#define F1 PORTC,2
#define G1 PORTC,3
#define A2 PORTD,3
#define B2 PORTD,4
#define C2 PORTD,5
#define D2 PORTB,1
#define E2 PORTB,4
#define F2 PORTD,2
#define G2 PORTD,6

#define BTN1 PORTD,7
#define BTN2 PORTB,2

#define LED_GREEN PORTB,3
#define LED_RED PORTB,7

#define TX PORTD,1
#define RX PORTD,0

#define SWITCH PORTB,6

// Settings
#define BTN_LEFT  1
#define BTN_RIGHT 0

#define TEMP_START 70
#define TEMP_MAX 90
#define TEMP_MIN 10

#define NO_TEMP_CHANGE_TIMEOUT 15
#define MAX_RELAY_ON_TIME 20
#define RELAY_DELAY_TIME 10

// Utils
#define LED_RED_ON IO_HIGH(LED_RED)
#define LED_RED_OFF IO_LOW(LED_RED)

#define LED_GREEN_ON IO_HIGH(LED_GREEN)
#define LED_GREEN_OFF IO_LOW(LED_GREEN)

#define SWITCH_ON IO_LOW(SWITCH)
#define SWITCH_OFF IO_HIGH(SWITCH)

#endif

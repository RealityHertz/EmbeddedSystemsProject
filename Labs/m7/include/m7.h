//Header file for m7_app.c
/*Include Statements*/
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

/*Define M4 Header*/
#ifndef M7_H
#define M7_H

/*Defines*/
#define USART_MAIN USART2
#define TIMER_PERIOD 9999 //Default

/*Extern Variables From Other Files*/
extern uint8_t key[];
extern uint8_t key2[];
extern uint32_t timer2Period;
extern uint16_t direction;
extern bool state;

//LCD Menu status
enum status {
    SELECTING_LED,
	SET_PERIOD,
	ENCRYPTING_TEXT
};

/*Functions*/
void clock_setup(void);
void usart_setup(void);
void timer2_setup(void);
void timer7_setup(void);
void rotate_key(void);
void rotate_key_by(uint8_t num);
void reset_key(void);
void periodLED(uint16_t ledPeriodMS);
uint8_t encypher(uint8_t data);
uint32_t ascii_to_integer(uint8_t text[4]);

#endif
/*
* File        - m4.h
* Author      - Jaxson Billings
* Data        - 03-28-2023
* Description - Header file for m4.c
*/


/*Include Statements*/
#include "esos.h"
#include "user_app.h"
#include <stdio.h>
#include <string.h>

#ifdef __linux
#include "esos_pc.h"
#include "esos_pc_stdio.h"
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#else
#include "esos_stm32l4.h"
#include "esos_stm32l4_rs232.h"
#endif

/*Define M4 Header*/
#ifndef M4_H
#define M4_H

/*Defines*/
#define USART_MAIN USART2
#define TIMER_PERIOD 9999 //Default
#define DATABITS = 8
#define ODDBITS = 1
#define THREADSIZE = 5

/*Extern Variables From Other Files*/
extern uint8_t key[];
extern uint8_t key2[];
extern uint32_t timer2Period;
extern uint16_t direction;
extern bool state;

/*Functions*/
void clock_setup(void);
void usart_setup(void);
void timer2_setup(void);
void timer7_setup(void);
void rotate_key(void);
void rotate_key2(void);
void rotate_key_by(uint8_t num);
void reset_key(void);
void periodLED(uint16_t ledPeriodMS);
void __user_init_hw(void);
uint8_t encypher(uint8_t data);
uint32_t ascii_to_integer(uint8_t text[4]);
extern ESOS_SEMAPHORE(mutex);
extern ESOS_SEMAPHORE(barrier);

/*Threads*/
ESOS_USER_TASK(thread1);
ESOS_USER_TASK(thread2);
ESOS_USER_TASK(thread3);
ESOS_USER_TASK(thread4);
ESOS_USER_TASK(thread5);
ESOS_USER_TASK(thread6);
ESOS_USER_TASK(thread7);
ESOS_USER_TASK(thread8);
ESOS_USER_TASK(thread9);
ESOS_USER_TASK(thread10);
ESOS_USER_TASK(thread11);
ESOS_USER_TASK(thread12);

#endif
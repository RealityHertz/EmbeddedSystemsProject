#ifndef STM32L4
   #define STM32L4
#endif

#include "keypad.h"
#include "ptb.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libopencm3/stm32/gpio.h>

#define ROW0_OFFSET 8
#define ROW1_OFFSET 9
#define ROW2_OFFSET 10
#define ROW3_OFFSET 11

#define KEY0_OFFSET 0
#define KEY1_OFFSET 1
#define KEY2_OFFSET 2
#define KEY3_OFFSET 3
#define KEY4_OFFSET 4
#define KEY5_OFFSET 5
#define KEY6_OFFSET 6
#define KEY7_OFFSET 7
#define KEY8_OFFSET 8
#define KEY9_OFFSET 9
#define KEYA_OFFSET 10
#define KEYB_OFFSET 11
#define KEYC_OFFSET 12
#define KEYD_OFFSET 13
#define KEYPOUND_OFFSET 14
#define KEYSTAR_OFFSET 15

/*Basic delay loop*/
static void delay(int32_t loops)
{
    while(loops > 0)
    {
       asm("nop");
       loops--;
    }
}

/*Reads keypad inputs*/
uint16_t read_keypad(void)
{
    uint16_t keys = 0x0000;

    /*Clears col 0*/
    EDUB_KEYPAD_COL0_CLEAR();
    EDUB_KEYPAD_COL1_CLEAR();
    EDUB_KEYPAD_COL2_CLEAR();
    EDUB_KEYPAD_COL3_CLEAR();
    EDUB_KEYPAD_COL0_SET();
    delay(50);

    /*Reads all keys on col 0*/
    keys |= (EDUB_KEY1_GETVAL() >> ROW0_OFFSET) << KEY1_OFFSET;
    keys |= (EDUB_KEY4_GETVAL() >> ROW1_OFFSET) << KEY4_OFFSET;
    keys |= (EDUB_KEY7_GETVAL() >> ROW2_OFFSET) << KEY7_OFFSET;
    keys |= (EDUB_KEYSTAR_GETVAL() >> ROW3_OFFSET) << KEYSTAR_OFFSET;

    EDUB_KEYPAD_COL0_CLEAR();
    EDUB_KEYPAD_COL1_CLEAR();
    EDUB_KEYPAD_COL2_CLEAR();
    EDUB_KEYPAD_COL3_CLEAR();
    EDUB_KEYPAD_COL1_SET();
    delay(50);

    /*Reads all keys on col 1*/
    keys |= (EDUB_KEY2_GETVAL() >> ROW0_OFFSET) << KEY2_OFFSET;
    keys |= (EDUB_KEY5_GETVAL() >> ROW1_OFFSET) << KEY5_OFFSET;
    keys |= (EDUB_KEY8_GETVAL() >> ROW2_OFFSET) << KEY8_OFFSET;
    keys |= (EDUB_KEY0_GETVAL() >> ROW3_OFFSET) << KEY0_OFFSET;

    EDUB_KEYPAD_COL0_CLEAR();
    EDUB_KEYPAD_COL1_CLEAR();
    EDUB_KEYPAD_COL2_CLEAR();
    EDUB_KEYPAD_COL3_CLEAR();
    EDUB_KEYPAD_COL2_SET();
    delay(50);

    /*Reads all keys on col 2*/
    keys |= (EDUB_KEY3_GETVAL() >> ROW0_OFFSET) << KEY3_OFFSET;
    keys |= (EDUB_KEY6_GETVAL() >> ROW1_OFFSET) << KEY6_OFFSET;
    keys |= (EDUB_KEY9_GETVAL() >> ROW2_OFFSET) << KEY9_OFFSET;
    keys |= (EDUB_KEYPOUND_GETVAL() >> ROW3_OFFSET) << KEYPOUND_OFFSET;

    EDUB_KEYPAD_COL0_CLEAR();
    EDUB_KEYPAD_COL1_CLEAR();
    EDUB_KEYPAD_COL2_CLEAR();
    EDUB_KEYPAD_COL3_CLEAR();
    EDUB_KEYPAD_COL3_SET();
    delay(50);

    /*Reads all keys on col 3*/
    keys |= (EDUB_KEYA_GETVAL() >> ROW0_OFFSET) << KEYA_OFFSET;
    keys |= (EDUB_KEYB_GETVAL() >> ROW1_OFFSET) << KEYB_OFFSET;
    keys |= (EDUB_KEYC_GETVAL() >> ROW2_OFFSET) << KEYC_OFFSET;
    keys |= (EDUB_KEYD_GETVAL() >> ROW3_OFFSET) << KEYD_OFFSET;
}
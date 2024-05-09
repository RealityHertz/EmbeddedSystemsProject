#include "m7.h"
#include "ptb.h"
#include "esos.h"
#include <stdbool.h>
#include <stdint.h>

#ifndef KEYPAD_H
#define KEYPAD_H

#define NUM_KEYPAD_KEYS 16

uint16_t read_keypad(void);
uint16_t read_key(void);

#endif
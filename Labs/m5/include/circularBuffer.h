
#ifndef circularBuffer_H
#define ciruclarBuffer_H

#include <inttypes.h>
#include <stdio.h>

#define BUFFER_SIZE 256  // size of the circular buffer

// Define the buffer structure
typedef struct {
    int buffer[BUFFER_SIZE];
    int head;
    int tail;
    int full;
} CircularBuffer;

// Initializes a buffer when called
void circular_buffer_init(CircularBuffer *cb) {
    cb->head = 0;
    cb->tail = 0;
    cb->full = 0;
}

// Returns true if the buffer is empty
int circular_buffer_is_empty(CircularBuffer *cb) {
    return !cb->full && cb->head == cb->tail;
}

// Returns true if the buffer is full
int circular_buffer_is_full(CircularBuffer *cb) {
    return cb->full;
}

// Returns the buffer size
int circular_buffer_size(CircularBuffer *cb) {
    if (cb->full) {
        return BUFFER_SIZE;
    } else {
        return (cb->tail - cb->head + BUFFER_SIZE) % BUFFER_SIZE;
    }
}

// Writes a value into the buffer and updates locations
void circular_buffer_write(CircularBuffer *cb, int value) {
    cb->buffer[cb->tail] = value;
    if (circular_buffer_is_full(cb)) {
        cb->head = (cb->head + 1) % BUFFER_SIZE;
    }
    cb->tail = (cb->tail + 1) % BUFFER_SIZE;
    cb->full = cb->tail == cb->head;
}

// Reads from the buffer
int circular_buffer_read(CircularBuffer *cb) {
    int value = cb->buffer[cb->head];
    cb->full = 0;
    cb->head = (cb->head + 1) % BUFFER_SIZE;
    return value;
}

int circular_buffer_read_array(CircularBuffer *cb, uint8_t size){
    uint8_t i;
    for(i = 0; i < size; i++){
        circular_buffer_read(cb);
    }
}

#endif

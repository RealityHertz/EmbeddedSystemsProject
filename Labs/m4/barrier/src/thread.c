
#include "m4.h"
#include "esos.h"
#include "user_app.h"
#include <stdio.h>
#include <string.h>

#ifdef __linux
#include "esos_pc.h"
#include "esos_pc_stdio.h"
// INCLUDE these so our printf and other PC hacks work
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#else
#include "esos_stm32l4.h"
#include "esos_stm32l4_rs232.h"
#endif


uint8_t tCount = 0;
uint8_t i = 0;
uint8_t n = 5;
uint8_t size[3] = { 0x0D,0x0A,0 };
ESOS_SEMAPHORE(mutex);
ESOS_SEMAPHORE(barrier);

ESOS_CHILD_TASK(mutex_wait, uint32_t n) {

    ESOS_TASK_BEGIN();
    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("Task: ");
    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(n);
    ESOS_TASK_WAIT_ON_SEND_STRING(" Wait Mutex");
    ESOS_TASK_WAIT_ON_SEND_STRING(size);
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    ESOS_TASK_END();
}

ESOS_CHILD_TASK(barrier_wait, uint32_t n) {

    ESOS_TASK_BEGIN();
    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("Task: ");
    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(n);
    ESOS_TASK_WAIT_ON_SEND_STRING(" Wait Barrier");
    ESOS_TASK_WAIT_ON_SEND_STRING(size);
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    ESOS_TASK_END();
}

ESOS_CHILD_TASK(mutex_signal, uint32_t n) {

    ESOS_TASK_BEGIN();
    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("Task: ");
    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(n);
    ESOS_TASK_WAIT_ON_SEND_STRING(" Signal Mutex");
    ESOS_TASK_WAIT_ON_SEND_STRING(size);
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    ESOS_TASK_END();
}

ESOS_CHILD_TASK(barrier_signal, uint32_t n) {

    ESOS_TASK_BEGIN();
    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("Task: ");
    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(n);
    ESOS_TASK_WAIT_ON_SEND_STRING(" Signal Barrier");
    ESOS_TASK_WAIT_ON_SEND_STRING(size);
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    ESOS_TASK_END();
}

ESOS_USER_TASK(thread1) {
    static uint32_t localVar;
    static ESOS_TASK_HANDLE child_mutex_wait, child_barrier_wait, child_mutex_signal, child_barrier_signal;

    ESOS_TASK_BEGIN();
    localVar = i;
    i++;
    while (TRUE)
    {
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_wait, mutex_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        tCount = tCount + 1;
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_signal, mutex_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        if (tCount == 5) {
            ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
            ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();


        }
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_wait, barrier_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();


    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(thread2) {
    static uint32_t localVar;
    static ESOS_TASK_HANDLE child_mutex_wait, child_barrier_wait, child_mutex_signal, child_barrier_signal;

    ESOS_TASK_BEGIN();
    localVar = i;
    i++;
    while (TRUE)
    {
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_wait, mutex_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        tCount = tCount + 1;
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_signal, mutex_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        if (tCount == 5) {
            ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
            ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        }
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_wait, barrier_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(thread3) {
    static uint32_t localVar;
    static ESOS_TASK_HANDLE child_mutex_wait, child_barrier_wait, child_mutex_signal, child_barrier_signal;

    ESOS_TASK_BEGIN();
    localVar = i;
    i++;
    while (TRUE)
    {
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_wait, mutex_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        tCount = tCount + 1;
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_signal, mutex_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        if (tCount == 5) {
            ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
            ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        }
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_wait, barrier_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(thread4) {
    static uint32_t localVar;
    static ESOS_TASK_HANDLE child_mutex_wait, child_barrier_wait, child_mutex_signal, child_barrier_signal;

    ESOS_TASK_BEGIN();
    localVar = i;
    i++;
    while (TRUE)
    {
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_wait, mutex_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        tCount = tCount + 1;
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_signal, mutex_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        if (tCount == 5) {
            ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
            ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        }
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_wait, barrier_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(thread5) {
    static uint32_t localVar;
    static ESOS_TASK_HANDLE child_mutex_wait, child_barrier_wait, child_mutex_signal, child_barrier_signal;

    ESOS_TASK_BEGIN();
    localVar = i;
    i++;
    while (TRUE)
    {
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_wait, mutex_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        tCount = tCount + 1;
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_signal, mutex_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        if (tCount == 5) {
            ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
            ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        }
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_wait, barrier_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(thread6) {
    static uint32_t localVar;
    static ESOS_TASK_HANDLE child_mutex_wait, child_barrier_wait, child_mutex_signal, child_barrier_signal;

    ESOS_TASK_BEGIN();
    localVar = i;
    i++;
    while (TRUE)
    {
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_wait, mutex_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        tCount = tCount + 1;
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_signal, mutex_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        if (tCount == 5) {
            ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
            ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        }
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_wait, barrier_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(thread7) {
    static uint32_t localVar;
    static ESOS_TASK_HANDLE child_mutex_wait, child_barrier_wait, child_mutex_signal, child_barrier_signal;

    ESOS_TASK_BEGIN();
    localVar = i;
    i++;
    while (TRUE)
    {
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_wait, mutex_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        tCount = tCount + 1;
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_signal, mutex_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        if (tCount == 5) {
            ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
            ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

        }
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_wait, barrier_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(thread8) {
    static uint32_t localVar;
    static ESOS_TASK_HANDLE child_mutex_wait, child_barrier_wait, child_mutex_signal, child_barrier_signal;

    ESOS_TASK_BEGIN();
    localVar = i;
    i++;
    while (TRUE)
    {
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_wait, mutex_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        tCount = tCount + 1;
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_signal, mutex_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        if (tCount == 5) {
            ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
            ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

        }
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_wait, barrier_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(thread9) {
    static uint32_t localVar;
    static ESOS_TASK_HANDLE child_mutex_wait, child_barrier_wait, child_mutex_signal, child_barrier_signal;

    ESOS_TASK_BEGIN();
    localVar = i;
    i++;
    while (TRUE)
    {
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_wait, mutex_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        tCount = tCount + 1;
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_signal, mutex_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        if (tCount == 5) {
            ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
            ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

        }
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_wait, barrier_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(thread10) {
    static uint32_t localVar;
    static ESOS_TASK_HANDLE child_mutex_wait, child_barrier_wait, child_mutex_signal, child_barrier_signal;

    ESOS_TASK_BEGIN();
    localVar = i;
    i++;
    while (TRUE)
    {
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_wait, mutex_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        tCount = tCount + 1;
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_signal, mutex_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        if (tCount == 5) {
            ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
            ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

        }
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_wait, barrier_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(thread11) {
    static uint32_t localVar;
    static ESOS_TASK_HANDLE child_mutex_wait, child_barrier_wait, child_mutex_signal, child_barrier_signal;

    ESOS_TASK_BEGIN();
    localVar = i;
    i++;
    while (TRUE)
    {
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_wait, mutex_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        tCount = tCount + 1;
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_signal, mutex_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        if (tCount == 5) {
            ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
            ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

        }
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_wait, barrier_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(thread12) {
    static uint32_t localVar;
    static ESOS_TASK_HANDLE child_mutex_wait, child_barrier_wait, child_mutex_signal, child_barrier_signal;

    ESOS_TASK_BEGIN();
    localVar = i;
    i++;
    while (TRUE)
    {
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_wait, mutex_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        tCount = tCount + 1;
        ESOS_ALLOCATE_CHILD_TASK(child_mutex_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_mutex_signal, mutex_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        if (tCount == 5) {
            ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
            ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

        }
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_wait);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_wait, barrier_wait, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);
        ESOS_ALLOCATE_CHILD_TASK(child_barrier_signal);
        ESOS_TASK_SPAWN_AND_WAIT(child_barrier_signal, barrier_signal, localVar);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

    }
    ESOS_TASK_END();
}

void delayOPs(uint32_t ops) {
    for (int i = 0; i < ops; i++) {
        __asm("nop");
    }
}
//Includes go here
#include "esos.h"
#include "esos_stm32l4_edub_4bit_lcd44780wo.h"
#include "esos_lcd44780wo.h"
#include <ctype.h>
#include <string.h>

#ifdef __linux
#include    "esos_pc.h"
#include    "esos_pc_stdio.h"
// INCLUDE these so our printf and other PC hacks work
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#else
#include    "esos_stm32l4.h"
#include    "esos_stm32l4_rs232.h"
#endif

void __user_init_hw(void);

/*Task: lcdtest
* Description: Tests the operation of the edubase LCD,
* the tested operations are listed below.
* Operations: clearScreen, writeChar, writeString, DisplayVisible
* CursorBlink, CurosrDisplay, setCursor
*/
ESOS_USER_TASK(lcdtest)
{
    char input[] = {'H','E','L','L','O'};
    uint8_t LCDPos = 0;

    ESOS_TASK_BEGIN();
    while(TRUE)
    {
        esos_lcd44780_clearScreen();
        esos_lcd44780_writeChar(0, LCDPos, 'A');
        ESOS_TASK_WAIT_TICKS(1000);
        esos_lcd44780_clearScreen();
        esos_lcd44780_writeString(0, LCDPos, input);
        ESOS_TASK_WAIT_TICKS(1000);
        esos_lcd44780_clearScreen();
        esos_lcd44780_setDisplayVisible(FALSE);
        ESOS_TASK_WAIT_TICKS(1000);
        esos_lcd44780_setDisplayVisible(TRUE);
        ESOS_TASK_WAIT_TICKS(1000);
        esos_lcd44780_setCursorDisplay(FALSE);
        ESOS_TASK_WAIT_TICKS(1000);
        esos_lcd44780_setCursorDisplay(TRUE);
        ESOS_TASK_WAIT_TICKS(1000);
        esos_lcd44780_setCursorBlink(FALSE);
        ESOS_TASK_WAIT_TICKS(1000);
        esos_lcd44780_setCursorBlink(TRUE);
        ESOS_TASK_WAIT_TICKS(1000);
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

/*Function: __user_init_hw
* Description: Configures and initialized the LCD
*/
void __user_init_hw(void)
{
    esos_lcd44780_configDisplay();
    __esos_lcd44780_init();
}

/*Function: user_init
* Calls the __user_init_hw function and then registers
* the lcdtest task
*/
void user_init(void)
{
    __user_init_hw();
    esos_RegisterTask(lcdtest);
}
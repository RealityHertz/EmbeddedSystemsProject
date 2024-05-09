/*
 * "Copyright (c) 2019 J. W. Bruce ("AUTHOR(S)")"
 * All rights reserved.
 * (J. W. Bruce, jwbruce_AT_tntech.edu, Tennessee Tech University)
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the authors appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Please maintain this header in its entirety when copying/modifying
 * these files.
 *
 *
 */

// #define ESOS_USE_IRQS

// INCLUDEs go here  (First include the main esos.h file)
//      After that, the user can include what they need
#include "esos.h"
#include "m7.h"
#include "ptb.h"
#include "circularBuffer.h"
#include "user_app.h"
#include <ctype.h>
#include <string.h>
#include "esos_stm32l4_edub_sui.h"
#include "esos_sui.h"
#include    "esos_stm32l4.h"

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


// DEFINEs go here

/*
 * PROTOTYPEs go here
 *
 */
void __user_init_hw( void );
void __initLCDMenu(uint8_t LEDNum);


// GLOBALs go here
//  Generally, the user-created semaphores will be defined/allocated here
uint8_t key[] = "BOLDFEARLESSCONFIDENT";
uint8_t key2[] = "BOLDFEARLESSCONFIDENT";
bool state = false;

// timer globals
uint32_t timer2Period = 1000;
uint32_t edub_LD0_period = 1000;
uint32_t edub_LD1_period = 1000;
uint32_t edub_LD2_period = 1000;
uint32_t edub_LD3_period = 1000;
uint32_t LEDPeriods[5] = {1000,1000,1000,1000,1000}; 
uint32_t sum[5][4];
uint8_t i;
uint8_t startup = 0;
uint32_t restore;

//SUI Defines
ESOS_SUI_SWITCH_HANDLE SW2;
ESOS_SUI_SWITCH_HANDLE SW3;
ESOS_SUI_SWITCH_HANDLE SW4;
ESOS_SUI_SWITCH_HANDLE SW5;
ESOS_SUI_SWITCH_HANDLE B1;
ESOS_SUI_LED_HANDLE LED0;
ESOS_SUI_LED_HANDLE LED1;
ESOS_SUI_LED_HANDLE LED2;
ESOS_SUI_LED_HANDLE LED3;
ESOS_SUI_LED_HANDLE LD2;

//Menu and LED defines
uint8_t* SelectMenuLineOne = "         #### ms";
uint8_t* SelectMenuLineTwo = " press D to edit";
#define  LD2_OPTION   0
#define  LED0_OPTION  1
#define  LED1_OPTION  2
#define  LED2_OPTION  3
#define  LED3_OPTION  4
#define  NUM_LEDS     5
uint8_t* LEDNames[NUM_LEDS] = {"LD2 ", "LED0", "LED1", "LED2", "LED3"};
uint8_t  LCDCurrentState = LD2_OPTION;  // initialize to "lowest" LED
enum status status_menu = SELECTING_LED;

uint8_t InCharBuff[16] = "                ";
uint8_t OutCharBuff[16] = "                ";

//Places for period editing
#define THOUSANDS 4
#define HUNDREDS  3
#define TENS      2
#define ONES      1
#define PERIOD_DIGITS 5


#ifdef _linux
/*
 * Simulate the timer ISR found on a MCU
 *   The PC doesn't have a timer ISR, so this task will periodically
 *   call the timer services callback instead.
 *   USED ONLY FOR DEVELOPMENT AND TESTING ON PC.
 *   Real MCU hardware doesn't need this task
*/
ESOS_USER_TASK( __simulated_isr ) {
  ESOS_TASK_BEGIN();
  while (TRUE) {
    // call the ESOS timer services callback just like a real H/W ISR would
    __esos_tmrSvcsExecute();
    ESOS_TASK_WAIT_TICKS( 1 );

  } // endof while(TRUE)
  ESOS_TASK_END();
} // end child_task
#endif

//Main 
ESOS_USER_TASK(receivToSender)
{
  //All needed variables for the various functions
  static uint8_t toUpper;
  static uint8_t encrypt;
  static uint8_t decrypt;
  static uint8_t chooseLED;
  static uint8_t input;
  static uint8_t digit;

  ESOS_TASK_BEGIN();
    while (TRUE)
    {

  if (startup == 0)
  {
    for (i = 0; i < 5; i++)
    {
      sum[i][0] = '1';
      sum[i][1] = '0';
      sum[i][2] = '0';
      sum[i][3] = '0';
    }
  }
  startup = 1;
      //Waits for INCOMM and receives data
      ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
      ESOS_TASK_WAIT_ON_GET_UINT8(input);
      ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

      if(!input == 0)
      { 

        if(!esos_sui_isSWPressed(B1))
        {
          //Input 'S' is used to select an LED
          if(input == 'S')
          {
            //Receives LED selection from serial
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(chooseLED);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
         
            chooseLED = chooseLED%48;
            if(chooseLED == 0)
            {
              //Edits LED0 period from inputted serial period
              while (i<4)
              {
                ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
                ESOS_TASK_WAIT_ON_GET_UINT8(digit);
                ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
                sum[chooseLED][i] = digit; 
                i++;

              }
              edub_LD0_period = ((sum[chooseLED][0]%48)*1000) + ((sum[chooseLED][1]%48)*100) + ((sum[chooseLED][2]%48)*10) + ((sum[chooseLED][3]%48));
            }
            else if(chooseLED == 1)
            {
              //Edits LED1 period from inputted serial period
              while (i<4)
              {
                ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
                ESOS_TASK_WAIT_ON_GET_UINT8(digit);
                ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
                sum[chooseLED][i] = digit; 
                i++;

              }
              edub_LD1_period = ((sum[chooseLED][0]%48)*1000) + ((sum[chooseLED][1]%48)*100) + ((sum[chooseLED][2]%48)*10) + ((sum[chooseLED][3]%48));
            }
            else if(chooseLED == 2)
            {
              //Edits LED2 period from inputted serial period
              while (i<4)
              {
                ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
                ESOS_TASK_WAIT_ON_GET_UINT8(digit);
                ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
                sum[chooseLED][i] = digit; 
                i++;

              }
              edub_LD2_period = ((sum[chooseLED][0]%48)*1000) + ((sum[chooseLED][1]%48)*100) + ((sum[chooseLED][2]%48)*10) + ((sum[chooseLED][3]%48));
            }
            else if(chooseLED == 3)
            {
              //Edits LED3 period from inputted serial period
              while (i<4)
              {
                ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
                ESOS_TASK_WAIT_ON_GET_UINT8(digit);
                ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
                sum[chooseLED][i] = digit;  
                i++;

              }
              edub_LD3_period = ((sum[chooseLED][0]%48)*1000) + ((sum[chooseLED][1]%48)*100) + ((sum[chooseLED][2]%48)*10) + ((sum[chooseLED][3]%48));
            }
            else if(chooseLED == 4)
            {
              //Edits timer2 period from inputted serial period
              while (i<4)
              {
                ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
                ESOS_TASK_WAIT_ON_GET_UINT8(digit);
                ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
                sum[chooseLED][i] = digit; 
                i++;

              }
              timer2Period = ((sum[chooseLED][0]%48)*1000) + ((sum[chooseLED][1]%48)*100) + ((sum[chooseLED][2]%48)*10) + ((sum[chooseLED][3]%48));
            }
            else{
              continue;
            }
          }
          //Input 'L' is used to display selected LED's current period
          else if(input == 'L')
          {
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(chooseLED);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
 
            chooseLED = chooseLED%48; 
            if (chooseLED == 0)
            {
              //Displays LED0 period to serial 
              for (i = 0; i < 4; i++)
              {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_UINT8(sum[chooseLED][i]);
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
              }
              
            }
            else if (chooseLED == 1)
            {
              //Displays LED1 period to serial 
              for (i = 0; i < 4; i++)
              {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_UINT8(sum[chooseLED][i]);
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM(); 
              }
            }
            else if (chooseLED == 2)
            {
              //Displays LED2 period to serial            
              for (i = 0; i < 4; i++)
              {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_UINT8(sum[chooseLED][i]);
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM(); 
              }
            }
            else if (chooseLED == 3)
            {
              //Displays LED3 period to serial 
              for (i = 0; i < 4; i++)
              {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_UINT8(sum[chooseLED][i]);
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM(); 
              }
            }
            else if (chooseLED == 4)
            {
              //Displays timer2 period to serial 
              for (i = 0; i < 4; i++)
              {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_UINT8(sum[chooseLED][i]);
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM(); 
              }
            }
            else{
              continue;
            }
          }
          //While SW2 is pressed, LD3's period is halved
          else if(esos_sui_isSWPressed(SW2))
          {
            restore = edub_LD3_period;
            while(esos_sui_isSWPressed(SW2))
            {
              edub_LD3_period = edub_LD3_period/2;
            }
            edub_LD3_period = restore;
          }
          else
          {
            continue;
          }
          input = 0;
          i = 0;


        }
      //If switch 2 on the PTB is pressed, all nonalphanumeric characters will be removed from
      //the receive buffer
        else if (esos_sui_isSWPressed(SW2)){

          if(isalnum(input)){
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_UINT8(input);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM(); 
          }
          else{
            continue;   
          }

          input = 0;
        }
      //If switch 3 on the PTB is pressed, all incoming characters will be converted to uppercase
        else if(esos_sui_isSWPressed(SW3)){

          toUpper = toupper(input);
          ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
          ESOS_TASK_WAIT_ON_SEND_UINT8(toUpper);
          ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();  
          
          input = 0;
        }
      //If switch 4 on the PTB is pressed, incoming data will be encrypted
        else if(esos_sui_isSWPressed(SW4)){
          encrypt = encypher(input);

          ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
          ESOS_TASK_WAIT_ON_SEND_UINT8(encrypt);
          ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM(); 

          input = 0;
        }
      //If switch 5 on the PTB is pressed, incoming data will be encrypted
        else if(esos_sui_isSWPressed(SW5)){
          decrypt = decypher(input);

          ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
          ESOS_TASK_WAIT_ON_SEND_UINT8(decrypt);
          ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM(); 
          input = 0;
        }

        else if(B1_PUSHED() && !EDUB_SW2_PUSHED() && !EDUB_SW3_PUSHED() && !EDUB_SW4_PUSHED() && !EDUB_SW5_PUSHED()){
          ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
          ESOS_TASK_WAIT_ON_SEND_UINT8(input);
          ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM(); 
          input = 0;
        }
        ESOS_TASK_YIELD();
      }
      else{
        ESOS_TASK_YIELD();
      }
    
    }
  ESOS_TASK_END();
}

ESOS_USER_TASK(keypad)
{
  //Keypad defines
    uint16_t KeypadRead;
    static uint8_t userBuffer[4] = {'_','_','_','_'};
    static uint8_t bufferpoint = 3;
    uint8_t base_2n;
    uint32_t period = 0;
    uint32_t periodDigits[PERIOD_DIGITS] = {'0','0','0','0'};
    static ESOS_TASK_HANDLE edit_led;

    ESOS_TASK_BEGIN();
    while(TRUE)
    {
        //Reads input from keypad
        KeypadRead = read_keypad();
        //This If statement will work for selecting an LED to edit the period of
        if (status_menu == SELECTING_LED)
        { 
            //Checks if Button A on the keypad is pressed 
            if(KeypadRead & KEYAMASK)
            {
                LCDCurrentState++;
                //Sets a border so that you cannot move past the last LED
                if(LCDCurrentState > LED3_OPTION){
                    LCDCurrentState = LED3_OPTION;
                }

                ESOS_TASK_WAIT_TICKS(250);
                ESOS_TASK_WAIT_UNTIL((KeypadRead & KEYAMASK) == 0);

                esos_lcd44780_writeString(0,0,SelectMenuLineOne);
              //If the LCD is not on the last option it will add the arrow to move up
                if(LCDCurrentState < LED3_OPTION){
                    esos_lcd44780_writeChar(0,0,'^');
                }

                esos_lcd44780_writeString(1,0,SelectMenuLineTwo);
                esos_lcd44780_writeChar(1,0,'v');
                //Write LED name to display
                esos_lcd44780_writeString(0, 1, LEDNames[LCDCurrentState]);

                uint32_t period = LEDPeriods[LCDCurrentState];

                uint32_t periodDigits[NUM_LEDS];
                    
                //Stores period as an array
                periodDigits[THOUSANDS] = period / 1000;

                periodDigits[HUNDREDS] = (period / 100) - (periodDigits[THOUSANDS] * 10);

                periodDigits[TENS] = (period / 10) - (periodDigits[THOUSANDS] * 100) - (periodDigits[HUNDREDS] * 10);

                periodDigits[ONES] = period - (periodDigits[THOUSANDS] * 1000) 
                                                - (periodDigits[HUNDREDS] * 100)
                                                - (periodDigits[TENS] * 10);

                //Write period to display
                esos_lcd44780_writeChar(0, 9,  '0' + periodDigits[THOUSANDS]);
                esos_lcd44780_writeChar(0, 10, '0' + periodDigits[HUNDREDS]);
                esos_lcd44780_writeChar(0, 11, '0' + periodDigits[TENS]);
                esos_lcd44780_writeChar(0, 12, '0' + periodDigits[ONES]);                
            }
            //Checks if Button B is pressed on the keypad
            else if(KeypadRead & KEYBMASK)
            {
                LCDCurrentState--;
                //Keeps the choices within boundaries 
                if(LCDCurrentState > LED3_OPTION){ 
                    LCDCurrentState = LD2_OPTION;
                }
                
                ESOS_TASK_WAIT_TICKS(250);
                ESOS_TASK_WAIT_UNTIL((KeypadRead & KEYBMASK) == 0);
                //Writes menu line one to the LCD
                esos_lcd44780_writeString(0,0,SelectMenuLineOne);
                //Adds a down arrow to the menu if not on the edge of the boundary
                if(LCDCurrentState > LD2_OPTION){
                    esos_lcd44780_writeChar(1,0,'v');
                }
                //Writes menu line 2 and up arrow to the LCD
                esos_lcd44780_writeString(1,0,SelectMenuLineTwo);
                esos_lcd44780_writeChar(0,0,'^');
                //Write LED name to display
                esos_lcd44780_writeString(0, 1, LEDNames[LCDCurrentState]);

                uint32_t period = LEDPeriods[LCDCurrentState];

                uint32_t periodDigits[NUM_LEDS];
                    
                //Stores period as an array
                periodDigits[THOUSANDS] = period / 1000;

                periodDigits[HUNDREDS] = (period / 100) - (periodDigits[THOUSANDS] * 10);

                periodDigits[TENS] = (period / 10) - (periodDigits[THOUSANDS] * 100) - (periodDigits[HUNDREDS] * 10);

                periodDigits[ONES] = period - (periodDigits[THOUSANDS] * 1000) 
                                                - (periodDigits[HUNDREDS] * 100)
                                                - (periodDigits[TENS] * 10);

                //Write period to display
                esos_lcd44780_writeChar(0, 9,  '0' + periodDigits[THOUSANDS]);
                esos_lcd44780_writeChar(0, 10, '0' + periodDigits[HUNDREDS]);
                esos_lcd44780_writeChar(0, 11, '0' + periodDigits[TENS]);
                esos_lcd44780_writeChar(0, 12, '0' + periodDigits[ONES]);  
            }
        }
        //This If statement is to edit a LED period after selecting
        else if (status_menu == SET_PERIOD)
        {
            ESOS_TASK_WAIT_UNTIL(!read_keypad());
            while (!(read_keypad() & KEYSTARMASK))
            {
              //Clears LCD screen and writes the period edit screen
                esos_lcd44780_clearScreen();
                esos_lcd44780_writeString(0, 1, LEDNames[LCDCurrentState]);
                esos_lcd44780_writeString(0,9, userBuffer);
                esos_lcd44780_writeString(0,13," ms");
                esos_lcd44780_writeString(1,1,"Confirm with *");
            
                ESOS_TASK_YIELD();
            }
            //Replaces all '_' with '0' on the period edit screen
            for (int i = 0; i < 4; i++)
            {
                if(userBuffer[i] == '_')
                    userBuffer[i] = '0';
            }
            //Flashes the selected LED at the edited period
            esos_sui_flashLED(LCDCurrentState, atoi(userBuffer));
            LEDPeriods[LCDCurrentState] = atoi(userBuffer);
            userBuffer[0] = userBuffer[1] = userBuffer[2] = userBuffer[3] = '_';
            __initLCDMenu(LCDCurrentState);
            status_menu = SELECTING_LED;
        }
        //This If is used when encrypting text and displays to the LCD
        else if (status_menu = ENCRYPTING_TEXT)
        {
            esos_lcd44780_clearScreen();
            //While either switch is pressed, incoming text will display to the LCD
            while (esos_sui_isSWPressed(SW4) || esos_sui_isSWPressed(SW5))
            {
                ESOS_TASK_WAIT_TICKS(250);
                esos_lcd44780_writeString(0, 0, InCharBuff);
                esos_lcd44780_writeString(1, 0, OutCharBuff);
                ESOS_TASK_YIELD();
            }

            __initLCDMenu(LCDCurrentState);
            //Replaces all positions of buffers with ' '
            for(int i = 0; i < 16; i++)
            {
                InCharBuff[i] = ' ';
                OutCharBuff[i] = ' ';
            }
            //Switches to Selecting LED menu
            status_menu = SELECTING_LED;
        }
        //Switches to Set Period menu
        if(KeypadRead & KEYDMASK){
            status_menu = SET_PERIOD;
        }
        //Switches to encyption menu
        if(esos_sui_isSWPressed(SW4) || esos_sui_isSWPressed(SW5)){
            status_menu = ENCRYPTING_TEXT;
        }

        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}
/************************************************************************
 * User supplied functions
 ************************************************************************
 */
void __user_init_hw(void) {

esos_lcd44780_configDisplay();
__esos_lcd44780_init();
esos_lcd44780_clearScreen();

//ESOS SUI tag assignments
LED0 = esos_sui_registerLED(EDUB_LD0_PORT, EDUB_LD0_PIN);
LED1 = esos_sui_registerLED(EDUB_LD1_PORT, EDUB_LD1_PIN);
LED2 = esos_sui_registerLED(EDUB_LD2_PORT, EDUB_LD2_PIN);
LED3 = esos_sui_registerLED(EDUB_LD3_PORT, EDUB_LD3_PIN);
LD2 = esos_sui_registerLED(LD2_PORT, LD2_PIN);
B1 = esos_sui_registerSwitch(B1_PORT, B1_PIN);
SW2 = esos_sui_registerSwitch(EDUB_SW2_PORT, EDUB_SW2_PIN);
SW3 = esos_sui_registerSwitch(EDUB_SW3_PORT, EDUB_SW3_PIN);
SW4 = esos_sui_registerSwitch(EDUB_SW4_PORT, EDUB_SW4_PIN);
SW5 = esos_sui_registerSwitch(EDUB_SW5_PORT, EDUB_SW5_PIN);

//ESOS SUI flashing LED 
esos_sui_flashLED(LD2, timer2Period);
esos_sui_flashLED(LED0, edub_LD0_period);
esos_sui_flashLED(LED1, edub_LD1_period);
esos_sui_flashLED(LED2, edub_LD2_period);
esos_sui_flashLED(LED3, edub_LD3_period);

//Setups for the keypad
EDUB_KEYPAD_COL0_SETUP();
EDUB_KEYPAD_COL1_SETUP();
EDUB_KEYPAD_COL2_SETUP();
EDUB_KEYPAD_COL3_SETUP();
}


/******************************************************************************
 * Function:        void user_init(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        user_init is a centralized initialization routine where
 *                  the user can setup their application.   It is called
 *                  automagically by ES_OS during the operating system
 *                  initialization.
 *
 * Note:            The user should set up any state machines and init
 *                  all application variables.  They can also turn on
 *                  any needed peripherals here.
 *
 *                  The user SHALL NOT mess with the interrupt hardware
 *                  directly!!!  The ES_OS must be aware of the interrupts
 *                  and provides osXXXXXXX functions for the user to use.
 *                  Using these ES_OS-provided functions, the user may
 *                  (and probably should) initialize, register, and enable
 *                  interrupts in this routine.
 *
 *                  Furthermore, the user should register AT LEAST one
 *                  user application task here (via esos_RegisterTask) or
 *                  the ES_OS scheduler will have nothing to schedule
 *                  to run when this function returns.
 *
 *****************************************************************************/
void user_init(void) {

  // setup our software structures here

  // call our function to setup the hardware we like it.
  __user_init_hw();

  // Drive the UART directly.....  to print the HELLO_MSG
  __esos_unsafe_PutString( HELLO_MSG );

  /*
   * Now, let's get down and dirty with ESOS and our user tasks
   *
   *   Once tasks are registered, they will start executing in
   *   the ESOS scheduler.
   */

  esos_RegisterTask(receivToSender);
  esos_RegisterTask(keypad);
  __esos_InitSUI();
} // end user_init()

//Init LCD Menu for m7
void __initLCDMenu(uint8_t LEDNum)
{
  /*Write default screen to LCD*/
  esos_lcd44780_writeString(0,0,SelectMenuLineOne);
  esos_lcd44780_writeChar(0,0,'^');
  esos_lcd44780_writeString(1,0,SelectMenuLineTwo);

//Write LED name to display
  esos_lcd44780_writeString(0, 1, LEDNames[LEDNum]);

  uint32_t period = LEDPeriods[LEDNum];

  uint32_t periodDigits[NUM_LEDS];
  
  //Stores period as an array
  periodDigits[THOUSANDS] = period / 1000;

  periodDigits[HUNDREDS] = (period / 100) - (periodDigits[THOUSANDS] * 10);

  periodDigits[TENS] = (period / 10) - (periodDigits[THOUSANDS] * 100) - (periodDigits[HUNDREDS] * 10);

  periodDigits[ONES] = period - (periodDigits[THOUSANDS] * 1000) 
                                - (periodDigits[HUNDREDS] * 100)
                                - (periodDigits[TENS] * 10);

//Write period to display
  esos_lcd44780_writeChar(0, 9,  '0' + periodDigits[THOUSANDS]);
  esos_lcd44780_writeChar(0, 10, '0' + periodDigits[HUNDREDS]);
  esos_lcd44780_writeChar(0, 11, '0' + periodDigits[TENS]);
  esos_lcd44780_writeChar(0, 12, '0' + periodDigits[ONES]);

}

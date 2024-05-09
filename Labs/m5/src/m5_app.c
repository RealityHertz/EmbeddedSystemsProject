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
#include "m5.h"
#include "ptb.h"
#include "circularBuffer.h"
#include "user_app.h"
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


// DEFINEs go here

/*
 * PROTOTYPEs go here
 *
 */
void __user_init_hw( void );


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
uint32_t sum[5][4];
uint8_t i;
uint8_t startup = 0;


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

//Flashes the PTB LED0 
ESOS_USER_TASK(flash_EDU_LD0)
{
    ESOS_TASK_BEGIN();
    while (TRUE)
    {
        gpio_toggle(EDUB_LD0_PORT, EDUB_LD0_PIN);
        ESOS_TASK_WAIT_TICKS(edub_LD0_period / 2);
    }
    ESOS_TASK_END();
}//end flash_EDU_LD0

//Flashes the PTB LED1
ESOS_USER_TASK(flash_EDU_LD1)
{
    ESOS_TASK_BEGIN();
    while (TRUE)
    {
        gpio_toggle(EDUB_LD1_PORT, EDUB_LD1_PIN);
        ESOS_TASK_WAIT_TICKS(edub_LD1_period / 2);
    }
    ESOS_TASK_END();
}//end flash_EDU_LD1

//Flashes the PTB LED2
ESOS_USER_TASK(flash_EDU_LD2)
{
    ESOS_TASK_BEGIN();
    while (TRUE)
    {
        gpio_toggle(EDUB_LD2_PORT, EDUB_LD2_PIN);
        ESOS_TASK_WAIT_TICKS(edub_LD2_period / 2);
    } 
    ESOS_TASK_END();
}//end flash_EDU_LD2

//Flashes the PTB LED3
ESOS_USER_TASK(flash_EDU_LD3)
{
    ESOS_TASK_BEGIN();
    while (TRUE)
    {
        EDUB_LD3_TOGGLE();
        //gpio_toggle(EDUB_LD3_PORT, EDUB_LD3_PIN);
        ESOS_TASK_WAIT_TICKS(edub_LD3_period / 2);
    } 
    ESOS_TASK_END();
}//end flash_EDU_LD3

// Flashes the onboard LED to act as a "heartbeat"
ESOS_USER_TASK(flash_LED) { //AKA LED 4
  ESOS_TASK_BEGIN();
  while (TRUE) 
  {
    gpio_toggle(LD2_PORT,LD2_PIN);
    ESOS_TASK_WAIT_TICKS(timer2Period/2)
  }
  ESOS_TASK_END();
} // end flash_LED()


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

      ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
      ESOS_TASK_WAIT_ON_GET_UINT8(input);
      ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

      if(!input == 0)
      { 
        if(!B1_PUSHED())
        {
          if(input == 'S')
          {

            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(chooseLED);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
         
            chooseLED = chooseLED%48;
            if(chooseLED == 0)
            {
              
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
          else if(input == 'L')
          {
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(chooseLED);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
 
            chooseLED = chooseLED%48; 
            if (chooseLED == 0)
            {
              for (i = 0; i < 4; i++)
              {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_UINT8(sum[chooseLED][i]);
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
              }
              
            }
            else if (chooseLED == 1)
            {
              for (i = 0; i < 4; i++)
              {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_UINT8(sum[chooseLED][i]);
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM(); 
              }
            }
            else if (chooseLED == 2)
            {
              for (i = 0; i < 4; i++)
              {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_UINT8(sum[chooseLED][i]);
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM(); 
              }
            }
            else if (chooseLED == 3)
            {
              for (i = 0; i < 4; i++)
              {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_UINT8(sum[chooseLED][i]);
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM(); 
              }
            }
            else if (chooseLED == 4)
            {
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
          else
          {
            continue;
          }
          input = 0;
          i = 0;


        }
      //If switch 2 on the PTB is pressed, all nonalphanumeric characters will be removed from
      //the receive buffer
        else if (EDUB_SW2_PUSHED()){

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
        else if(EDUB_SW3_PUSHED()){

          toUpper = toupper(input);
          ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
          ESOS_TASK_WAIT_ON_SEND_UINT8(toUpper);
          ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();  
          
          input = 0;
        }
      //If switch 4 on the PTB is pressed, incoming data will be encrypted
        else if(EDUB_SW4_PUSHED()){
          encrypt = encypher(input);
          ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
          ESOS_TASK_WAIT_ON_SEND_UINT8(encrypt);
          ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM(); 
          input = 0;
        }
      //If switch 4 on the PTB is pressed, incoming data will be encrypted
        else if(EDUB_SW5_PUSHED()){
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


/************************************************************************
 * User supplied functions
 ************************************************************************
 */
void __user_init_hw(void) {
  /* GPIO Ports Clock Enable */
  rcc_periph_clock_enable(GPIOA);
  rcc_periph_clock_enable(GPIOC);
  rcc_periph_clock_enable(GPIOB);

  /*Configure GPIO pin : PC13 */
  gpio_mode_setup(B1_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, B1_PIN);

  /*Configure GPIO pin : LED2_Pin */
  gpio_mode_setup(LD2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LD2_PIN);
  EDUB_LD0_SETUP();
  EDUB_LD1_SETUP();
  EDUB_LD2_SETUP();
  EDUB_LD3_SETUP();
  EDUB_SW2_ENABLE();
  EDUB_SW3_ENABLE();
  EDUB_SW4_ENABLE();
  EDUB_SW5_ENABLE();
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
  esos_RegisterTask(flash_EDU_LD0);
  esos_RegisterTask(flash_EDU_LD1);
  esos_RegisterTask(flash_EDU_LD2);
  esos_RegisterTask(flash_EDU_LD3);
  esos_RegisterTask(flash_LED);
} // end user_init()

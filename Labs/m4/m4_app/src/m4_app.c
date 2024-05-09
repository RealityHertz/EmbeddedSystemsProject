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
#include "m4.h"
#include "circularBuffer.h"
#include "user_app.h"

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
//static uint8_t psz_CRNL[3]= {0x0D, 0x0A, 0};
CircularBuffer bufferReceiver;
CircularBuffer bufferSender;
uint8_t key[] = "BOLDFEARLESSCONFIDENT";
uint8_t key2[] = "BOLDFEARLESSCONFIDENT";
bool state = false;

// timer globals
uint32_t timer2Period = 1000;


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

// Flashes the onboard LED to act as a "heartbeat"
ESOS_USER_TASK(flash_LED) {
  ESOS_TASK_BEGIN();
  while (TRUE) 
  {
    gpio_toggle(LD2_PORT,LD2_PIN);
    ESOS_TASK_WAIT_TICKS(timer2Period/2)
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end flash_LED()

// Uses the USART to receive from a serial connection
ESOS_USER_TASK(usart_receive){
  static uint8_t recvUSART;

  ESOS_TASK_BEGIN();
  while(TRUE)
  {
    ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();            //Waits for incomm to be available
    ESOS_TASK_WAIT_ON_GET_UINT8(recvUSART);           //Receives data and assigns it to recvUSART var
    circular_buffer_write(&bufferReceiver, recvUSART);//Puts recvUSART data into the receive buffer
    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();             //Signals that incomm is available
    ESOS_TASK_YIELD();
  }
  ESOS_TASK_END();
}

// Uses the USART to send to a serial connection
ESOS_USER_TASK(usart_sender){
  static uint8_t sendUSART;

  ESOS_TASK_BEGIN();
  while(TRUE)
  {
    if(!circular_buffer_is_empty(&bufferSender)){
      ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();          //Waits until outcomm is available
      sendUSART = circular_buffer_read(&bufferSender); //Takes data out of send buffer
      ESOS_TASK_WAIT_ON_SEND_UINT8(sendUSART);         //Waits until outcomm is clear to send
      ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();           //Signals that outcomm is avaliable
      ESOS_TASK_YIELD();
    }
    else
      ESOS_TASK_YIELD();
  }
  ESOS_TASK_END();
}

// Moves incoming data between buffers and uses the circular buffer to encrypt or decrypt 
// depending on the state of the onboard button - Button1
ESOS_USER_TASK(receivToSender)
{
  static uint8_t r2s;
  static uint8_t s2r;
  static uint8_t encrypt;
  static uint8_t decrypt;
  ESOS_TASK_BEGIN();
    while (TRUE)
    {
      if(!circular_buffer_is_empty(&bufferReceiver))
      { 
        //Button1 is pushed which will decrypt what is in the receive buffer and move
        //to the send buffer to display
        if(B1_PUSHED()){
          s2r = circular_buffer_read(&bufferReceiver);
          decrypt = decypher(s2r);
          circular_buffer_write(&bufferSender, decrypt);
        }
        //Button1 is not pushed which will encrypt what is in the receive buffer and move
        //tto the send buffer to display
        else if(!B1_PUSHED()){
          r2s = circular_buffer_read(&bufferReceiver);
          encrypt = encypher(r2s);
          circular_buffer_write(&bufferSender, encrypt);
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

  /*Configure GPIO pin : PC13 */
  gpio_mode_setup(B1_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, B1_PIN);

  /*Configure GPIO pin : LED2_Pin */
  gpio_mode_setup(LD2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LD2_PIN);

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
  circular_buffer_init(&bufferReceiver);
  circular_buffer_init(&bufferSender);
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
  esos_RegisterTask(flash_LED);
  esos_RegisterTask(usart_receive);
  esos_RegisterTask(usart_sender);
  esos_RegisterTask(receivToSender);

} // end user_init()

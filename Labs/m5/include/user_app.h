#ifndef USER_APP_H
#define USER_APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

void Error_Handler(void);

//#ifdef __NUCLEO_BOARD
        //NUCLEO BOARD DEFINES
    #define LD2_PORT        GPIOA
    #define LD2_PIN         GPIO5
    #define LD2_SETUP()     gpio_mode_setup(LD2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LD2_PIN)
    #define LD2_TOGGLE()    gpio_toggle(LD2_PORT,LD2_PIN) 
    #define LD2_SET()       gpio_set(LD2_PORT, LD2_PIN)
    #define LD2_CLEAR()     gpio_clear(LD2_PORT, LD2_PIN)
    #define LD2_GETVAL()    gpio_get(LD2_PORT, LD2_PIN)
    #define B1_PORT         GPIOC
    #define B1_PIN          GPIO13
    #define B1_EXTI         EXTI13
    #define B1_NVIC         13
    #define B1_EN()         gpio_mode_setup(B1_PORT,GPIO_MODE_INPUT,GPIO_PUPD_NONE,B1_PIN)
    #define B1_PUSHED()     (gpio_get(B1_PORT,B1_PIN))
//#endif

#ifdef __cplusplus
}
#endif

#endif
#ifndef PTB_H
#define PTB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

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

        //PTB BOARD DEFINES
    //LEDs Defines
    #define EDUB_LD0_PORT  GPIOA   
    #define EDUB_LD0_PIN   GPIO1
    #define EDUB_LD1_PORT  GPIOA   
    #define EDUB_LD1_PIN   GPIO0
    #define EDUB_LD2_PORT  GPIOC   
    #define EDUB_LD2_PIN   GPIO7
    #define EDUB_LD3_PORT  GPIOC   
    #define EDUB_LD3_PIN   GPIO8
    //EDUB SWITCH PORTS & PINS
    #define EDUB_SW2_PORT   GPIOB   
    #define EDUB_SW2_PIN    GPIO11
    #define EDUB_SW3_PORT   GPIOB  
    #define EDUB_SW3_PIN    GPIO10
    #define EDUB_SW4_PORT   GPIOB  
    #define EDUB_SW4_PIN    GPIO9
    #define EDUB_SW5_PORT   GPIOB   
    #define EDUB_SW5_PIN    GPIO8
    // EDUB Keypad Buttons
    #define EDUB_KEYPAD_ROW0_PORT    GPIOB   
    #define EDUB_KEYPAD_ROW0_PIN     GPIO8
    #define EDUB_KEYPAD_ROW1_PORT    GPIOB  
    #define EDUB_KEYPAD_ROW1_PIN     GPIO9
    #define EDUB_KEYPAD_ROW2_PORT    GPIOB   
    #define EDUB_KEYPAD_ROW2_PIN     GPIO10
    #define EDUB_KEYPAD_ROW3_PORT    GPIOB   
    #define EDUB_KEYPAD_ROW3_PIN     GPIO11
    #define EDUB_KEYPAD_COL0_PORT    GPIOB  
    #define EDUB_KEYPAD_COL0_PIN     GPIO1
    #define EDUB_KEYPAD_COL1_PORT    GPIOB   
    #define EDUB_KEYPAD_COL1_PIN     GPIO2
    #define EDUB_KEYPAD_COL2_PORT    GPIOB 
    #define EDUB_KEYPAD_COL2_PIN     GPIO3
    #define EDUB_KEYPAD_COL3_PORT    GPIOB  
    #define EDUB_KEYPAD_COL3_PIN     GPIO4
    // EDUB LCD DISPLAY DEFINES
    #define EDUB_LCD_RS_PORT    GPIOA        
    #define EDUB_LCD_RS_PIN     GPIO12
    // EDUB PWM DEFINES
    #define EDUB_PWM1_PORT       GPIOC     
    #define EDUB_PWM1_PIN        GPIO6
    // PWM2
    #define EDUB_PWM3_PORT       GPIOA     
    #define EDUB_PWM3_PIN        GPIO11
    #define EDUB_PWM4_PORT       GPIOB    
    #define EDUB_PWM4_PIN        GPIO14
    // EDUB LED0 MACROS
    #define EDUB_LD0_SETUP()        gpio_mode_setup(EDUB_LD0_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, EDUB_LD0_PIN)
    #define EDUB_LD0_TOGGLE()       gpio_toggle(EDUB_LD0_PORT,EDUB_LD0_PIN)
    #define EDUB_LD0_SET()          gpio_set(EDUB_LD0_PORT, EDUB_LD0_PIN)
    #define EDUB_LD0_CLEAR()        gpio_clear(EDUB_LD0_PORT, EDUB_LD0_PIN)
    #define EDUB_LD0_GETVAL()       gpio_get(EDUB_LD0_PORT, EDUB_LD0_PIN)
    // EDUB LED1 MACROS
    #define EDUB_LD1_SETUP()        gpio_mode_setup(EDUB_LD1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, EDUB_LD1_PIN)
    #define EDUB_LD1_TOGGLE()       gpio_toggle(EDUB_LD1_PORT,EDUB_LD1_PIN)
    #define EDUB_LD1_SET()          gpio_set(EDUB_LD1_PORT, EDUB_LD1_PIN)
    #define EDUB_LD1_CLEAR()        gpio_clear(EDUB_LD1_PORT, EDUB_LD1_PIN)
    #define EDUB_LD1_GETVAL()       gpio_get(EDUB_LD1_PORT, EDUB_LD1_PIN)
    // EDUB LED2 MACROS
    #define EDUB_LD2_SETUP()        gpio_mode_setup(EDUB_LD2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, EDUB_LD2_PIN)
    #define EDUB_LD2_TOGGLE()       gpio_toggle(EDUB_LD2_PORT,EDUB_LD2_PIN)
    #define EDUB_LD2_SET()          gpio_set(EDUB_LD2_PORT, EDUB_LD2_PIN)
    #define EDUB_LD2_CLEAR()        gpio_clear(EDUB_LD2_PORT, EDUB_LD2_PIN)
    #define EDUB_LD2_GETVAL()       gpio_get(EDUB_LD2_PORT, EDUB_LD2_PIN)
    // EDUB LD3 MACROS
    #define EDUB_LD3_SETUP()        gpio_mode_setup(EDUB_LD3_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, EDUB_LD3_PIN)
    #define EDUB_LD3_TOGGLE()       gpio_toggle(EDUB_LD3_PORT,EDUB_LD3_PIN)
    #define EDUB_LD3_SET()          gpio_set(EDUB_LD3_PORT, EDUB_LD3_PIN)
    #define EDUB_LD3_CLEAR()        gpio_clear(EDUB_LD3_PORT, EDUB_LD3_PIN)
    #define EDUB_LD3_GETVAL()       gpio_get(EDUB_LD3_PORT, EDUB_LD3_PIN)
    //SW2
    #define EDUB_SW2_ENABLE()      gpio_mode_setup(EDUB_SW2_PORT,GPIO_MODE_INPUT,GPIO_PUPD_NONE,EDUB_SW2_PIN)
    #define EDUB_SW2_PUSHED()      gpio_get(EDUB_SW2_PORT,EDUB_SW2_PIN)
    //SW3
    #define EDUB_SW3_ENABLE()      gpio_mode_setup(EDUB_SW3_PORT,GPIO_MODE_INPUT,GPIO_PUPD_NONE,EDUB_SW3_PIN)
    #define EDUB_SW3_PUSHED()      gpio_get(EDUB_SW3_PORT,EDUB_SW3_PIN)
    //SW4
    #define EDUB_SW4_ENABLE()      gpio_mode_setup(EDUB_SW4_PORT,GPIO_MODE_INPUT,GPIO_PUPD_NONE,EDUB_SW4_PIN)
    #define EDUB_SW4_PUSHED()      gpio_get(EDUB_SW4_PORT,EDUB_SW4_PIN)
    //SW5
    #define EDUB_SW5_ENABLE()      gpio_mode_setup(EDUB_SW5_PORT,GPIO_MODE_INPUT,GPIO_PUPD_NONE,EDUB_SW5_PIN)
    #define EDUB_SW5_PUSHED()      gpio_get(EDUB_SW5_PORT,EDUB_SW5_PIN)
    //Keypad Initialization
    #define EDUB_KEYPAD_ROW0_SETUP()   gpio_mode_setup(EDUB_KEYPAD_ROW0_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_ROW0_PIN)
    #define EDUB_KEYPAD_ROW1_SETUP()   gpio_mode_setup(EDUB_KEYPAD_ROW1_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_ROW1_PIN)
    #define EDUB_KEYPAD_ROW2_SETUP()   gpio_mode_setup(EDUB_KEYPAD_ROW2_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_ROW2_PIN)
    #define EDUB_KEYPAD_ROW3_SETUP()   gpio_mode_setup(EDUB_KEYPAD_ROW3_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_ROW3_PIN)
    #define EDUB_KEYPAD_COL0_SETUP()   gpio_mode_setup(EDUB_KEYPAD_COL0_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_COL0_PIN)
    #define EDUB_KEYPAD_COL1_SETUP()   gpio_mode_setup(EDUB_KEYPAD_COL1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_COL1_PIN)
    #define EDUB_KEYPAD_COL2_SETUP()   gpio_mode_setup(EDUB_KEYPAD_COL2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_COL2_PIN)
    #define EDUB_KEYPAD_COL3_SETUP()   gpio_mode_setup(EDUB_KEYPAD_COL3_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_COL3_PIN)
    //Keypad Functions
    #define EDUB_KEYPAD_COL0_SET()     gpio_set(EDUB_KEYPAD_COL0_PORT, EDUB_KEYPAD_COL0_PIN)
    #define EDUB_KEYPAD_COL0_CLEAR()   gpio_clear(EDUB_KEYPAD_COL0_PORT, EDUB_KEYPAD_COL0_PIN)
    #define EDUB_KEYPAD_COL1_SET()     gpio_set(EDUB_KEYPAD_COL1_PORT, EDUB_KEYPAD_COL1_PIN)
    #define EDUB_KEYPAD_COL1_CLEAR()   gpio_clear(EDUB_KEYPAD_COL1_PORT, EDUB_KEYPAD_COL1_PIN)
    #define EDUB_KEYPAD_COL2_SET()     gpio_set(EDUB_KEYPAD_COL2_PORT, EDUB_KEYPAD_COL2_PIN)
    #define EDUB_KEYPAD_COL2_CLEAR()   gpio_clear(EDUB_KEYPAD_COL2_PORT, EDUB_KEYPAD_COL2_PIN)
    #define EDUB_KEYPAD_COL3_SET()     gpio_set(EDUB_KEYPAD_COL3_PORT, EDUB_KEYPAD_COL3_PIN)
    #define EDUB_KEYPAD_COL3_CLEAR()   gpio_clear(EDUB_KEYPAD_COL3_PORT, EDUB_KEYPAD_COL3_PIN)
    //Key Functions
    #define EDUB_KEY0_GETVAL()         gpio_get(EDUB_KEYPAD_ROW3_PORT, EDUB_KEYPAD_ROW3_PIN)
    #define EDUB_KEY1_GETVAL()         gpio_get(EDUB_KEYPAD_ROW0_PORT, EDUB_KEYPAD_ROW0_PIN)
    #define EDUB_KEY2_GETVAL()         gpio_get(EDUB_KEYPAD_ROW0_PORT, EDUB_KEYPAD_ROW0_PIN)
    #define EDUB_KEY3_GETVAL()         gpio_get(EDUB_KEYPAD_ROW0_PORT, EDUB_KEYPAD_ROW0_PIN)
    #define EDUB_KEY4_GETVAL()         gpio_get(EDUB_KEYPAD_ROW1_PORT, EDUB_KEYPAD_ROW1_PIN)
    #define EDUB_KEY5_GETVAL()         gpio_get(EDUB_KEYPAD_ROW1_PORT, EDUB_KEYPAD_ROW1_PIN)
    #define EDUB_KEY6_GETVAL()         gpio_get(EDUB_KEYPAD_ROW1_PORT, EDUB_KEYPAD_ROW1_PIN)
    #define EDUB_KEY7_GETVAL()         gpio_get(EDUB_KEYPAD_ROW2_PORT, EDUB_KEYPAD_ROW2_PIN)
    #define EDUB_KEY8_GETVAL()         gpio_get(EDUB_KEYPAD_ROW2_PORT, EDUB_KEYPAD_ROW2_PIN)
    #define EDUB_KEY9_GETVAL()         gpio_get(EDUB_KEYPAD_ROW2_PORT, EDUB_KEYPAD_ROW2_PIN)
    #define EDUB_KEYA_GETVAL()        gpio_get(EDUB_KEYPAD_ROW0_PORT, EDUB_KEYPAD_ROW0_PIN)
    #define EDUB_KEYB_GETVAL()        gpio_get(EDUB_KEYPAD_ROW1_PORT, EDUB_KEYPAD_ROW1_PIN)
    #define EDUB_KEYC_GETVAL()        gpio_get(EDUB_KEYPAD_ROW2_PORT, EDUB_KEYPAD_ROW2_PIN)
    #define EDUB_KEYD_GETVAL()        gpio_get(EDUB_KEYPAD_ROW3_PORT, EDUB_KEYPAD_ROW3_PIN)
    #define EDUB_KEYSTAR_GETVAL()        gpio_get(EDUB_KEYPAD_ROW3_PORT, EDUB_KEYPAD_ROW3_PIN)
    #define EDUB_KEYPOUND_GETVAL()        gpio_get(EDUB_KEYPAD_ROW3_PORT, EDUB_KEYPAD_ROW3_PIN)

//#endif

#ifdef __cplusplus
}
#endif

#endif
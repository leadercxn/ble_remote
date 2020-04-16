#ifndef CUSTOM_BOARD_H
#define CUSTOM_BOARD_H

/**
 * @brief 该头文件供SDK (例如:board.c)使用,使用该头文件的时候，需要定义好相关宏(BOARD_CUSTOM)，来指定包括的头文件
 * 
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

// LEDs definitions for CUSTOM_BOARD
#define LEDS_NUMBER    4

//#define LED_START      17

#define LED_1          17
#define LED_2          18
#define LED_3          19
#define LED_4          20
//#define LED_STOP       20

#define LEDS_ACTIVE_STATE 0

#define LEDS_INV_MASK  LEDS_MASK

#define LEDS_LIST { LED_1, LED_2, LED_3, LED_4 }

#define BSP_LED_0      LED_1
#define BSP_LED_1      LED_2
#define BSP_LED_2      LED_3
#define BSP_LED_3      LED_4

#define BUTTONS_NUMBER 4

//#define BUTTON_START   13

#define BUTTON_1       10
#define BUTTON_2       25
#define BUTTON_3       9
#define BUTTON_4       28
//#define BUTTON_STOP    16
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4 }

#define BSP_BUTTON_0   BUTTON_1
#define BSP_BUTTON_1   BUTTON_2
#define BSP_BUTTON_2   BUTTON_3
#define BSP_BUTTON_3   BUTTON_4



#ifdef __cplusplus
}
#endif











#endif
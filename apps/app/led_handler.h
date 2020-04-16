#ifndef _LED_HANDLER_H
#define _LED_HANDLER_H

typedef struct                      //用户设置的led相关参数
{                                   
  uint16_t led_pin;                 //传参的led事件
  uint8_t  active_level;            //有效电平
  bool     is_blink;                //是否闪烁
  uint16_t led_on_time;             //灯亮时间长度          led_on_time*period
  uint16_t led_off_time;            //灯灭时间长度      led_off_time*period
  uint16_t count_blink;             //闪烁的次数，0为无限次
} app_led_para_t;

/**
 * @brief 初始化led io
 */
uint8_t led_handler_init(uint16_t *pin_group , uint16_t total_pins , uint16_t led_timer_period );

/**
 * @brief led开启
 * 
 * @param [out] led_list_index 返回该led在led_list的序号
 * @return 设置结果
 */
uint8_t led_on_control(app_led_para_t app_led_para , uint8_t *led_list_index );

/**
 * @brief led关闭
 */
void led_off_control(uint8_t led_list_index );

#endif



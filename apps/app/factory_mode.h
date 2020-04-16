#ifndef __FACTORY_MODE_H
#define __FACTORY_MODE_H



/**
 *@brief  读取 flash 中的配置数据进行配置
 */
bool config_from_flash(void);

/**
 * @brief 出厂模式
 */
void factory_mode(void);

#endif


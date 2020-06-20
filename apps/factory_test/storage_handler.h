/* Copyright (c) 2019 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#ifndef __STORAGE_HANDLER_H
#define __STORAGE_HANDLER_H

#define STORAGE_START_ADDR     0x2E000
#define STORAGE_END_ADDR       0x30000

/**
 * @brief fstorage初始化
 */
void fstorage_init(void);

/**
 *@brief  fstorage 写
 *        注意： write_addr-> 写flash的地址应为4整数倍
 *              p_data-> 数据起始地址应为4整数倍   len->数据长度为4字节倍数
 */
void fstorage_write(uint32_t write_addr, void const * p_data, uint32_t len);


/**
 *@brief  fstorage 读
 */
void fstorage_read(uint32_t read_addr, void * p_data, uint32_t len);

/**
 * @brief 擦出页
 */
void fstorage_erase(uint32_t page, uint32_t len);


#endif
















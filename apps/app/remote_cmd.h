/* Copyright (c) 2020 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#ifndef REMOTE_CMD_H__
#define REMOTE_CMD_H__

void remote_cmd_init(void);

void remote_cmd_data_get(uint8_t cmd, uint16_t count, uint8_t * p_cmd_data, uint16_t * p_size);

void remote_cmd_key_set(uint8_t key[16]);

#endif

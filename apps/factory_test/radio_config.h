#ifndef __RADIO_CONFIG_H
#define __RADIO_CONFIG_H

typedef struct
{
    void (*on_event_end_handler)(void);
} ble_radio_event_handler_t;

typedef struct
{
    int8_t  power;
    uint8_t mode;
    uint8_t channel;
} ble_radio_config_t;


void ble_radio_disable(void);

void ble_radio_init(ble_radio_event_handler_t * p_handler);

void ble_radio_config_set(ble_radio_config_t config);

void ble_radio_packet_set(uint8_t * p_packet);

void ble_radio_tx_enable(void);

bool ble_radio_crc_status_get(void);

int8_t ble_radio_rssi_get(void);

void ble_radio_rx_enable(void);








#endif





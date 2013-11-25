#ifndef __BLE_CHARA
#define __BLE_cHARA

#include "ble_ms.h"

uint32_t accepted_char_add(ble_ms_t * p_ms, const ble_ms_init_t * p_ms_init);
uint32_t declined_char_add(ble_ms_t * p_ms, const ble_ms_init_t * p_ms_init);
uint32_t pending_char_add(ble_ms_t * p_ms, const ble_ms_init_t * p_ms_init);
uint32_t bump_char_add(ble_ms_t * p_ms, const ble_ms_init_t * p_ms_init);
uint32_t temp_char_add(ble_ms_t * p_ms, const ble_ms_init_t * p_ms_init);


#endif
#ifndef __BLE_H
#define __BLE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "ble_eval_board_pins.h"
#include "app_scheduler.h"
#include "ble_stack_handler.h"
#include "app_timer.h"
#include "app_gpiote.h"
#include "app_button.h"
#include "ble_debug_assert_handler.h"
#include "ble_ms.h"
#include "ble.h"
#include "main.h"

//ble_ms_t*  start_ble();
//int64_t* get_pending_mugs();
void power_manage(void);
int is_ready();
void debug_ble_ids();
void ble_update_temp(uint8_t* val);
void ble_update_bump(uint8_t* val);
void set_replies();

#endif // __BLE_H

/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup ble_sdk_srv_ms LEDButton Service
 * @{
 * @ingroup ble_sdk_srv
 * @brief LEDButton Service module.
 *
 * @details This module implements the LEDButton Service with the Battery Level characteristic.
 *          During initialization it adds the LEDButton Service and Battery Level characteristic
 *          to the BLE stack datalbse. Optionally it can also add a Report Reference descriptor
 *          to the Battery Level characteristic (used when including the LEDButton Service in
 *          the HID service).
 *
 *          If specified, the module will support notification of the Battery Level characteristic
 *          through the ble_ms_battery_level_update() function.
 *          If an event handler is supplied by the application, the LEDButton Service will
 *          generate LEDButton Service events to the application.
 *
 * @note The application must propagate BLE stack events to the LEDButton Service module by calling
 *       ble_ms_on_ble_evt() from the from the @ref ble_stack_handler callback.
 */

#ifndef BLE_MS_H__
#define BLE_MS_H__


#include <stdbool.h>
#include "slip_ble.h"
#define MS_UUID_BASE {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}
#define MS_UUID_SERVICE 0x1523
#define MS_UUID_ACC_CHAR 0x1524
#define MS_UUID_DECLINED_CHAR 0x1525
#define MS_UUID_PENDING_CHAR 0x1526

#define MAX_LEN 15

// Forward declaration of the ble_ms_t type. 
typedef struct ble_ms_s ble_ms_t;

/**@brief Meeting Service event handler type. */
typedef void (*ble_ms_pending_write_handler_t) (ble_ms_t * p_ms, uint8_t data[], uint16_t length);

/**@brief Meeting Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_ms_pending_write_handler_t   pending_write_handler;                   
} ble_ms_init_t;


/**@brief Meeting Service structure. This contains various status information for the service. */
typedef struct ble_ms_s
{         
    uint16_t                     service_handle;                   
    ble_gatts_char_handles_t     accepted_char_handles;
    ble_gatts_char_handles_t     declined_char_handles;     
    ble_gatts_char_handles_t     pending_char_handles;          
    uint8_t                      uuid_type;
    uint16_t                     conn_handle;  
    uint16_t                     pending_ids[MAX_LEN]; 
    uint16_t                     accepted_ids[MAX_LEN]; 
    uint16_t                     declined_ids[MAX_LEN]; 
    bool                         is_notifying;
    ble_ms_pending_write_handler_t  pending_write_handler;
} ble_ms_t;


/**@brief Initialize the Meeting Service.
 *
 * @param[out]  p_ms       
 * @param[in]   p_ms_init  
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_ms_init(ble_ms_t * p_ms, const ble_ms_init_t * p_ms_init);

/**@brief Meeting Service BLE stack event handler.
 *
 * @details Handles all events from the BLE stack of interest to the Meeting Service.
 *
 * @param[in]   p_ms       Meeting Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_ms_on_ble_evt(ble_ms_t * p_ms, ble_evt_t * p_ble_evt);

//TODO add brief
uint8_t id_decode(uint8_t* encoded_ids, uint16_t encoded_len, uint16_t * decoded_buffer);
//TODO add brief
uint32_t ble_ms_accepted_ids_update(ble_ms_t * p_ms, uint16_t* ids, uint16_t len);
//TODO add brief
uint32_t ble_ms_declined_ids_update(ble_ms_t * p_ms, uint16_t* ids, uint16_t len);

#endif // BLE_MS_H__

/** @} */

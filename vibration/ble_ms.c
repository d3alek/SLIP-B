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

#include "ble_ms.h"
#include <string.h>


#include "app_util.h"


/**@brief Connect event handler.
 *
 * @param[in]   p_ms      Meeting Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_ms_t * p_ms, ble_evt_t * p_ble_evt)
{
    p_ms->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Disconnect event handler.
 *
 * @param[in]   p_ms       Meeting Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_ms_t * p_ms, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_ms->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Write event handler.
 *
 * @param[in]   p_ms       Meeting Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_ms_t * p_ms, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    
   if ((p_evt_write->handle == p_ms->pending_char_handles.value_handle) &&
       (p_ms->pending_write_handler != NULL))
   {
       p_ms->pending_write_handler(p_ms, p_evt_write->data,p_evt_write->len);
   }
}


void ble_ms_on_ble_evt(ble_ms_t * p_ms, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_ms, p_ble_evt);
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_ms, p_ble_evt);
            break;
            
        case BLE_GATTS_EVT_WRITE:
            on_write(p_ms, p_ble_evt);
            break;
            
        default:
            break;
    }
}

//Adds accepted chacteristic to Meeting Service
static uint32_t accepted_char_add(ble_ms_t * p_ms, const ble_ms_init_t * p_ms_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    
    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_ms->uuid_type;
    ble_uuid.uuid = MS_UUID_ACC_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint16_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint16_t) * MAX_LEN;
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_ms->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_ms->accepted_char_handles);
}

//Adds declined chacteristic to Meeting Service
static uint32_t declined_char_add(ble_ms_t * p_ms, const ble_ms_init_t * p_ms_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    
    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_ms->uuid_type;
    ble_uuid.uuid = MS_UUID_DECLINED_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint16_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint16_t) * MAX_LEN;
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_ms->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_ms->declined_char_handles);
}

//Adds pending chacteristic to Meeting Service
static uint32_t pending_char_add(ble_ms_t * p_ms, const ble_ms_init_t * p_ms_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    

    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_ms->uuid_type;
    ble_uuid.uuid = MS_UUID_PENDING_CHAR;
    
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint16_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint16_t) * MAX_LEN;
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_ms->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_ms->pending_char_handles);
}



uint32_t ble_ms_init(ble_ms_t * p_ms, const ble_ms_init_t * p_ms_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_ms->conn_handle       = BLE_CONN_HANDLE_INVALID;
    p_ms->pending_write_handler = p_ms_init->pending_write_handler;
    
    // Add base UUID to softdevice's internal list. 
    ble_uuid128_t base_uuid = {MS_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_ms->uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    ble_uuid.type = p_ms->uuid_type;
    ble_uuid.uuid = MS_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_ms->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
   
   err_code  = accepted_char_add(p_ms, p_ms_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code  = declined_char_add(p_ms, p_ms_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code  = pending_char_add(p_ms, p_ms_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    return NRF_SUCCESS;
}


//Changes the uint8_t recieves data into a array of uint16_t ids
uint8_t id_decode(uint8_t* encoded_ids, uint16_t encoded_len, uint16_t * decoded_buffer)
{  
    uint8_t num_ids = 0;
    int i;
    

    for(i=0;i< encoded_len; i++){
        decoded_buffer[num_ids]  = (encoded_ids[i] << 16);
        i++;
        decoded_buffer[num_ids++]  &= encoded_ids[i];
    }
    
    return num_ids;

}


//splits each uint16_t id into two uint8_t data items so it can by sent by GATT 
static uint8_t id_encode(uint16_t* ids, uint8_t num_ids, uint8_t * encoded_buffer)
{  
    uint8_t len = 0;
    int i;
    
    // Encode id measurement
    for(i=0;i< num_ids; i++){
         encoded_buffer[len++]  = (ids[i] & 0xFFFF0000) >> 16;
         encoded_buffer[len++]  = ids[i] & 0x0000FFFF;
    }
  
    
    return len;  
}


//updates the accepted chracteristic in GATT and notifies the client
uint32_t ble_ms_accepted_ids_update(ble_ms_t * p_ms, uint16_t* ids, uint16_t len)
{
    uint32_t err_code = NRF_SUCCESS;

    // Save new id set
    memset(p_ms->accepted_ids, 0, sizeof(p_ms->accepted_ids));
    memcpy(p_ms->accepted_ids, ids, sizeof(ids));

    //encoded for GATT
    uint8_t encoded_ids[MAX_LEN * 2]; 
 
    uint8_t encode_len = id_encode(ids,len,encoded_ids);
   
        
    // Update GATT database
    err_code = sd_ble_gatts_value_set(p_ms->accepted_char_handles.value_handle,
                                          0,
                                          &len,
                                          encoded_ids);
    if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
        

    // Send updated value to client
    ble_gatts_hvx_params_t hvx_params;
    uint16_t               hvx_len;
    hvx_len = encode_len;
            
    memset(&hvx_params, 0, sizeof(hvx_params));
            
    hvx_params.handle   = p_ms->accepted_char_handles.value_handle;
    hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.offset   = 0;
    hvx_params.p_len    = &hvx_len;
    hvx_params.p_data   = encoded_ids;
            
    err_code = sd_ble_gatts_hvx(p_ms->conn_handle, &hvx_params);

    if ((err_code == NRF_SUCCESS) && (hvx_len != len))
        {
            err_code = NRF_ERROR_DATA_SIZE;
        }

   // conn=1;

    return err_code;
}


//updates the declined chracteristic in GATT and notifies the client
uint32_t ble_ms_declined_ids_update(ble_ms_t * p_ms, uint16_t* ids, uint16_t len)
{
    uint32_t err_code = NRF_SUCCESS;

    // Save new id set
    memset(p_ms->declined_ids, 0, sizeof(p_ms->declined_ids));
    memcpy(p_ms->declined_ids, ids, sizeof(ids));

    //encoded for GATT
    uint8_t encoded_ids[MAX_LEN * 2]; 
 
    uint8_t encode_len = id_encode(ids,len,encoded_ids);
   
        
    // Update GATT database
    err_code = sd_ble_gatts_value_set(p_ms->declined_char_handles.value_handle,
                                          0,
                                          &len,
                                          encoded_ids);
    if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
        

    // Send updated value to client
    ble_gatts_hvx_params_t hvx_params;
    uint16_t               hvx_len;
    hvx_len = encode_len;
            
    memset(&hvx_params, 0, sizeof(hvx_params));
            
    hvx_params.handle   = p_ms->declined_char_handles.value_handle;
    hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.offset   = 0;
    hvx_params.p_len    = &hvx_len;
    hvx_params.p_data   = encoded_ids;
            
    err_code = sd_ble_gatts_hvx(p_ms->conn_handle, &hvx_params);

    if ((err_code == NRF_SUCCESS) && (hvx_len != len))
        {
            err_code = NRF_ERROR_DATA_SIZE;
        }

    return err_code;
}








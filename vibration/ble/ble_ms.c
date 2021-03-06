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
#include "simple_uart.h"
#include "ble_characteristics.h"
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
       p_ms->pending_write_handler(p_ms, p_evt_write->data);
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


//Initialises the ble meeting service structure
uint32_t ble_ms_init(ble_ms_t * p_ms, const ble_ms_init_t * p_ms_init, MUG_STATUS* mugs)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

      //  p_ms->mug_len = 0; 
        p_ms->ready =0;        
                  
    p_ms->conn_handle       = BLE_CONN_HANDLE_INVALID;
    p_ms->pending_write_handler = p_ms_init->pending_write_handler;
    p_ms->mugs = mugs;
    
    // Add meeting service base UUID to softdevice's internal list. 
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
    
   //initalises the accepted characteristic
   err_code  = accepted_char_add(p_ms, p_ms_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    //initalises the declined characteristic
    err_code  = declined_char_add(p_ms, p_ms_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    //initalises the pending characteristic
    err_code  = pending_char_add(p_ms, p_ms_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    
    //initalises the pending characteristic
    err_code  = bump_char_add(p_ms, p_ms_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    

    return NRF_SUCCESS;
}


//Decodes the uint8_t[] recieved data into a uint64_t ids
uint64_t id_decode(uint8_t* encoded_ids)
{ 

    uint64_t decoded_id = 0;  //decoded unsigned 64bit in to return
    char buf[36];
    uint8_t ids;
    
    int i=0;
    //data is sent as a string, so loop over each char till end
    while(encoded_ids[i] != '\0'){        

     sprintf((char*)buf, "%c",encoded_ids[i]);
     ids = strtol(buf,NULL,16);                 //convert char to hex digit
     decoded_id = (decoded_id * 16) + ids;      //add digit to existing values

      i = i+1;  //inc loop
    }

   
     return decoded_id;
}

//splits each uint64_t id into 16 uint8_t data items chars so it can by sent by GATT 
static uint8_t id_encode(uint64_t* ids, uint8_t num_ids, uint8_t * encoded_buffer)
{  

    uint8_t len =0;  //length of encoded buffer
    int i,j;
    for(i=0;i< num_ids; i++){ //loops over each 64 bit ID 
      for(j=7;j>=0;j--){       //loops over each 8 bit in the encoded buffer
       encoded_buffer[(i*8)+j] = (uint8_t) ((ids[i] & (((uint64_t)0xFF)<< j*8)) >> (j*8));
       len++;
      }
   
     
    }

    return len;  
}


//updates the accepted chracteristic in GATT and notifies the client
uint32_t ble_ms_accepted_ids_update(ble_ms_t * p_ms, uint64_t* ids, uint16_t len)
{
    uint32_t err_code = NRF_SUCCESS;

    //encoded string for GATT
    uint8_t encoded_ids[MAX_LEN * 16]; 
 
    //encode the ids and return length of encoding
    uint8_t encode_len = id_encode(ids,len,encoded_ids);
   
        
    //Update GATT accpeted characteristic in database
    err_code = sd_ble_gatts_value_set(p_ms->accepted_char_handles.value_handle,
                                          0,
                                          &len,
                                          encoded_ids);
    if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
        
    char buf[30];

    // Send updated value notification to andriod app
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

    if ((err_code == NRF_SUCCESS) && (hvx_len != encode_len))
        {
            err_code = NRF_ERROR_DATA_SIZE;
        }

    sprintf((char*)buf, "ERROR : %lx\n", err_code);
    simple_uart_putstring(buf);

    return err_code;
}

//updates the declined chracteristic in GATT and notifies the client
uint32_t ble_ms_declined_ids_update(ble_ms_t * p_ms, uint64_t* ids, uint16_t len)
{
    uint32_t err_code = NRF_SUCCESS;

    //encoded string for GATT
    uint8_t encoded_ids[MAX_LEN * 16]; 
    
    //encode the ids and return the length of encoding
    uint8_t encode_len = id_encode(ids,len,encoded_ids);
   
        
    //Update GATT database with new declied characterisic
    err_code = sd_ble_gatts_value_set(p_ms->declined_char_handles.value_handle,
                                          0,
                                          &len,
                                          encoded_ids);
    if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
        

    // Send updated value notification to android app
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

    if ((err_code == NRF_SUCCESS) && (hvx_len != encode_len))
        {
            err_code = NRF_ERROR_DATA_SIZE;
        }

    return err_code;
}


//updates the declined chracteristic in GATT and notifies the client
uint32_t ble_ms_pending_ids_update(ble_ms_t * p_ms, uint64_t* ids, uint16_t len)
{
    uint32_t err_code = NRF_SUCCESS;
    char buf[30];
    
    //encoded string for GATT
    uint8_t encoded_ids[MAX_LEN * 16]; 
    
    //encode the ids and return the length of encoding
    uint8_t encode_len = id_encode(ids,len,encoded_ids);
   
    sprintf((char*)buf, "ID : %llX len: %d\n",ids[0],encode_len);
    simple_uart_putstring(buf);
        
    //Update GATT database with new declied characterisic
    err_code = sd_ble_gatts_value_set(p_ms->pending_char_handles.value_handle,
                                          0,
                                          &len,
                                          encoded_ids);
    if (err_code != NRF_SUCCESS){
           simple_uart_putstring("Error setting characterisic\n");
           return err_code;
    }
        
    // Send updated value notification to android app
    ble_gatts_hvx_params_t hvx_params;
    uint16_t               hvx_len;
    hvx_len = encode_len;
            
    memset(&hvx_params, 0, sizeof(hvx_params));
            
    hvx_params.handle   = p_ms->pending_char_handles.value_handle;
    hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.offset   = 0;
    hvx_params.p_len    = &hvx_len;
    hvx_params.p_data   = encoded_ids;
            
    err_code = sd_ble_gatts_hvx(p_ms->conn_handle, &hvx_params);

    if ((err_code == NRF_SUCCESS) && (hvx_len != encode_len)){
            err_code = NRF_ERROR_DATA_SIZE;
       
    }

    return err_code;
}

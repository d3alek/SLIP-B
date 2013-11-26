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

/**
 * This file is the main file for the application described in application note
 * nAN-36 Creating Bluetooth® Low Energy Applications Using nRF51822.
 */


#include "slip_ble.h"
#include "simple_uart.h"

static ble_ms_t                         m_ms;                                       /**< Stucture holding BLE data,defined in ble_ms.h*/

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

//checks if id is already present in mugs struct
static bool check_duplicate(ble_ms_t * p_ms, uint64_t id){
  
   int i;

   for(i=0;i<p_ms->mug_len;i++){
      if(p_ms->mugs[i].MUG_ID == id){
         return true;
      }

   }
   return false;

}


/**@brief Function for handling a write to the Pending characteristic of the Meeting service.
 * @detail A pointer to this function is passed to the service in its init structure.
 *           
 *        Called on a write event to the pending charcteristic, which is defined in ble_ms.c
 */

//called on pending write event defined in ble_ms.c
static void pending_write_handler(ble_ms_t * p_ms, uint8_t* data)
{

    uint32_t      err_code;
    uint64_t      mug_id;  //recived Mug ID 
    unsigned char buf[32];
   
    mug_id = id_decode(data); //Decodes the written data 
    sprintf((char*)buf, "BLE WRITE %d %llX\n",p_ms->mug_len,mug_id);
    simple_uart_putstring(buf);
    //Reset list of mug ids that need to be 
    if(mug_id == 0x1111111111111111){

       p_ms->ready = 1;

    }
    //otherwise update pending list with new mug ID
    else{

        //checks if mug id has already been seen
        if(!check_duplicate(p_ms,mug_id)){
           p_ms->mugs[p_ms->mug_len].MUG_ID = mug_id;           //set mug id in struct
           p_ms->mugs[p_ms->mug_len].PIPELINE_STATUS = NONE;    //init pipeline status
           p_ms->mug_len = p_ms->mug_len + 1;    //increment number of mugs in struct
       }
       else{
         sprintf((char*)buf, "DUPLICATE MUG ID %llX\n",mug_id);
         simple_uart_putstring(buf);
       }

     }

     //Update GATTS pending characteristics to zero so that the android app 
     //knows that we have recieved the mug ID 
     uint16_t zlen = sizeof(uint8_t);
     uint8_t zero = 0;
     err_code = sd_ble_gatts_value_set(p_ms->pending_char_handles.value_handle,
                                           0,
                                           &zlen,
                                           &zero);
     APP_ERROR_CHECK(err_code);


     //Send notification to andriod app that the chracteristic has changed
     ble_gatts_hvx_params_t hvx_params;
     uint16_t               hvx_len;
     hvx_len = zlen;
                
     memset(&hvx_params, 0, sizeof(hvx_params));

     hvx_params.handle   = p_ms->pending_char_handles.value_handle;
     hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
     hvx_params.offset   = 0;
     hvx_params.p_len    = &hvx_len;
     hvx_params.p_data   = &zero;

     err_code = sd_ble_gatts_hvx(p_ms->conn_handle, &hvx_params);

  
    

}

/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    on_ble_evt(p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    ble_ms_on_ble_evt(&m_ms, p_ble_evt);
}



/**@brief Function for initializing services that will be used by the application.
 */
void services_init(MUG_STATUS* mugs)
{
    uint32_t err_code;
    ble_ms_init_t init;
    init.pending_write_handler = pending_write_handler;

    err_code = ble_ms_init(&m_ms, &init, mugs);
    APP_ERROR_CHECK(err_code);
}


//==================================================================
//Dummy function for debugging accepted and declined Characteristics
//==================================================================
void set_replies(int c){
  int acc_size = 3;
  int dec_size = 2;
  uint32_t err_code = NRF_SUCCESS;
  
  uint64_t acc_id1 = 0x1234567890123456;
  uint64_t acc_id2 = 0xabcdefabcdefabcd;
  uint64_t acc_id3 = 0xa1b2c3d4e5f6a1b2;
  uint64_t acc_ids[1];
  // acc_ids[0] = acc_id1;
  // acc_ids[1] = acc_id2; 
  // acc_ids[2] = acc_id3;

  char buf[30];

  
  int i;
  if(c < acc_size)
     i = c;
  else
     i = c % acc_size;

  if(i == 0){
    acc_ids[0] = acc_id1;
    simple_uart_putstring("SENDING ACK 1 TO APP\n");
  }
  else if(i==1){
    acc_ids[0] = acc_id2; 
    simple_uart_putstring("SENDING ACK 2 TO APP\n");
  }
  else{
    acc_ids[0] = acc_id3;
    simple_uart_putstring("SENDING ACK 3 TO APP\n");
  }
   
  err_code = ble_ms_pending_ids_update(&m_ms,acc_ids,1);
  if(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING && err_code != 0){

    sprintf((char*)buf, "ERROR sending notification: %lx\n", err_code);
    simple_uart_putstring(buf);
    APP_ERROR_CHECK(err_code);
  }

}


//Returns true if device is connected and all mug ids have been seen
int is_ready(){
  if (is_connected() && m_ms.ready){
      return true;
  }    

  return false;
}


//function for debugging the mug id structure
void debug_ble_ids(){
  int i;
  unsigned char buf[32];
  
  sprintf((char*)buf, "%d MUGS SEEN, CONN %d\n",m_ms.mug_len, is_connected());
  simple_uart_putstring(buf);

}

/**@brief Function for initialising ble
 */
ble_ms_t* start_ble(MUG_STATUS* mug_list, int first_call)
{ char buf[30];
    
  sprintf((char*)buf, "START first call %d\n",first_call);
  simple_uart_putstring(buf);
      
    ble_setup(&m_ms,mug_list,first_call);
   
 
    return &m_ms;

}

static void mugs_reset(){
  int i;
  for(i=0;i<m_ms.mug_len;i++){
     m_ms.mugs[i].MUG_ID = 0;
     m_ms.mugs[i].PIPELINE_STATUS = 0;

  }


  m_ms.mug_len = 0;
   m_ms.ready=0;

}

//Sends accpeted and rejected mug ids to the app
//Then resets muss struct
void RSVP_App(){

  uint32_t err_code = NRF_SUCCESS;
  int i;
  char buf[30];
  int acked_size = 0;
  uint64_t acked_ids[m_ms.mug_len];

  sprintf((char*)buf, "RSVP LEN %d\n",m_ms.mug_len);
  simple_uart_putstring(buf);


  //populates list of acked mugs
  for(i=0;i<m_ms.mug_len;i++){
     sprintf((char*)buf, "MUG %llX STATUS %d\n",m_ms.mugs[i].MUG_ID,m_ms.mugs[i].PIPELINE_STATUS);
     simple_uart_putstring(buf);
     if(m_ms.mugs[i].PIPELINE_STATUS == ON ||m_ms.mugs[i].PIPELINE_STATUS == ACCEPTED ||m_ms.mugs[i].PIPELINE_STATUS == REJECTED||m_ms.mugs[i].PIPELINE_STATUS == INVITED ){
             acked_ids[acked_size] = m_ms.mugs[i].MUG_ID;
             acked_size++;
       }
  }

  if (acked_size == 0 ){
    simple_uart_putstring("NO IDS TO RETURN TO APP\n");
    return;
  }

  uint64_t curr_id[1];

  for(i=0;i<acked_size;i++){
  
     curr_id[0] = acked_ids[i];
     sprintf((char*)buf, "RSVP MUG %llX\n",curr_id[0]);
     simple_uart_putstring(buf);

    
     err_code = ble_ms_pending_ids_update(&m_ms,curr_id,1);
     if(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING){
        APP_ERROR_CHECK(err_code);
     }
  }

  //Reset Mugs ids ready for next set of invitations   
  //mugs_reset();
  simple_uart_putstring("SENT RSVP\n");
 
    
}

// //updates the bump chracteristic in GATT and notifies the client
// void ble_update_bump(uint8_t* val)
// {
//     uint32_t err_code = NRF_SUCCESS;
    
//     uint16_t len = 1;
   
        
//     //Update GATT database with new declied characterisic
//     err_code = sd_ble_gatts_value_set(m_ms.bump_char_handles.value_handle,
//                                           0,
//                                           &len,
//                                           val);
    
        
//     APP_ERROR_CHECK(err_code);

//     // Send updated value notification to android app
//     ble_gatts_hvx_params_t hvx_params;
//     uint16_t               hvx_len;
//     hvx_len = len;
            
//     memset(&hvx_params, 0, sizeof(hvx_params));
            
//     hvx_params.handle   = m_ms.bump_char_handles.value_handle;
//     hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
//     hvx_params.offset   = 0;
//     hvx_params.p_len    = &hvx_len;
//     hvx_params.p_data   = val;
            
//     err_code = sd_ble_gatts_hvx(m_ms.conn_handle, &hvx_params);

//     if ((err_code == NRF_SUCCESS) && (hvx_len != len))
//         {
//             err_code = NRF_ERROR_DATA_SIZE;
//         }

  
//     if(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING && err_code != 0){
//         char buf[30];
//         sprintf((char*)buf, "Bump update ERR %lX\n",err_code);
//         simple_uart_putstring(buf);
//         APP_ERROR_CHECK(err_code);
//      }
//}


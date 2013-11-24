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
 * nAN-36 Creating Bluetooth� Low Energy Applications Using nRF51822.
 */


#include "slip_ble.h"
#include "simple_uart.h"


#define WAKEUP_BUTTON_PIN               EVAL_BOARD_BUTTON_0                          /**< Button used to wake up the application. */

#define DEVICE_NAME                     "Give Me Coffee"                                    /**< Name of device. Will be included in the advertising data. */

#define APP_ADV_INTERVAL                64                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      180                                         /**< The advertising timeout (in units of seconds). */

// YOUR_JOB: Modify these according to requirements.
#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS            2                                           /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL               500 / 1.25            /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               1000 / 1.25         /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                4000 / 10             /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(20000, APP_TIMER_PRESCALER) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (15 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time between each call to sd_ble_gap_conn_param_update after the first (5 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define APP_GPIOTE_MAX_USERS            1                                           /**< Maximum number of users of the GPIOTE handler. */

#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)    /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define SEC_PARAM_TIMEOUT               30                                          /**< Timeout for Pairing Request or Security Request (in seconds). */
#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

static ble_gap_sec_params_t             m_sec_params;                               /**< Security requirements for this application. */
static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
static ble_ms_t                         m_ms;                                       /**< Stucture holding BLE data,defined in ble_ms.h*/

int conn;
#define SCHED_MAX_EVENT_DATA_SIZE       sizeof(app_timer_event_t)                   /**< Maximum size of scheduler events. Note that scheduler BLE stack events do not contain any data, as the events are being pulled from the stack in the event handler. */
#define SCHED_QUEUE_SIZE                10                                          /**< Maximum number of events in the scheduler queue. */



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

/**@brief Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by the application.
 */
static void leds_init(void)
{
    GPIO_LED_CONFIG(ADVERTISING_LED_PIN_NO);
    GPIO_LED_CONFIG(CONNECTED_LED_PIN_NO);

}

bool is_connected(){
  return conn;
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, true);
}


/**@brief Function for the GAP initialization.
 *
 * @details This function shall be used to setup all the necessary GAP (Generic Access Profile)
 *          parameters of the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,(const uint8_t *) DEVICE_NAME, strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    ble_advdata_t scanrsp;
    uint8_t       flags = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

    ble_uuid_t adv_uuids[] = {{MS_UUID_SERVICE, m_ms.uuid_type}};

    //Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags.size              = sizeof(flags);
    advdata.flags.p_data            = &flags;

    memset(&scanrsp, 0, sizeof(scanrsp));
    scanrsp.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    scanrsp.uuids_complete.p_uuids  = adv_uuids;

    err_code = ble_advdata_set(&advdata, &scanrsp);
    APP_ERROR_CHECK(err_code);
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
    sprintf((char*)buf, "BLE WRITE %llX\n",mug_id);
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
         sprintf((char*)buf, "DUPPLICATE MUG ID %llX\n",mug_id);
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


/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(MUG_STATUS* mugs)
{
    uint32_t err_code;
    ble_ms_init_t init;

    init.pending_write_handler = pending_write_handler;

    err_code = ble_ms_init(&m_ms, &init, mugs);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing security parameters.
 */
static void sec_params_init(void)
{
    m_sec_params.timeout      = SEC_PARAM_TIMEOUT;
    m_sec_params.bond         = SEC_PARAM_BOND;
    m_sec_params.mitm         = SEC_PARAM_MITM;
    m_sec_params.io_caps      = SEC_PARAM_IO_CAPABILITIES;
    m_sec_params.oob          = SEC_PARAM_OOB;
    m_sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    m_sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
}


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in]   p_evt   Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    uint32_t             err_code;
    ble_gap_adv_params_t adv_params;

    // Start advertising
    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    adv_params.p_peer_addr = NULL;
    adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    adv_params.interval    = APP_ADV_INTERVAL;
    adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = sd_ble_gap_adv_start(&adv_params);
    APP_ERROR_CHECK(err_code);
    nrf_gpio_pin_set(ADVERTISING_LED_PIN_NO);
}

//==================================================================
//Dummy function for debugging accepted and declined Characteristics
//==================================================================
void set_replies(){
  int acc_size = 3;
  int dec_size = 2;
  uint32_t err_code = NRF_SUCCESS;

  uint64_t acc_id1 = 0x5b83092c6767bb6d;
  uint64_t acc_id2 = 0xc5861596e2118c8d;
  uint64_t acc_id3 = 0x32db4358f348ea3b;
  uint64_t acc_ids[acc_size];
  acc_ids[0] = acc_id1;
  acc_ids[1] = acc_id2;
  acc_ids[2] = acc_id3;

  simple_uart_putstring("ACCPETED BEFORE\n");



  err_code = ble_ms_accepted_ids_update(&m_ms,acc_ids,acc_size);
  if(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING){
        APP_ERROR_CHECK(err_code);
  }

  uint64_t dec_id1 = 0x21de1a2c34a9b06f;
  uint64_t dec_id2 = 0x30b315ab1ce4ea38;
  uint64_t dec_ids[dec_size];
  dec_ids[0] = dec_id1;
  dec_ids[1] = dec_id2;

  simple_uart_putstring("BEFORE\n");

  err_code = ble_ms_declined_ids_update(&m_ms,dec_ids,dec_size);

  if(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING){
        APP_ERROR_CHECK(err_code);
  }

  simple_uart_putstring("SET REPLIES\n");
}

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t                         err_code = NRF_SUCCESS;
    static ble_gap_evt_auth_status_t m_auth_status;
    ble_gap_enc_info_t *             p_enc_info;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            nrf_gpio_pin_set(CONNECTED_LED_PIN_NO);
            nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            conn = 1;
            err_code = app_button_enable();
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            nrf_gpio_pin_clear(CONNECTED_LED_PIN_NO);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            //conn = 0;
            err_code = app_button_disable();
            if (err_code == NRF_SUCCESS)
            {
                advertising_start();
            }
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle,
                                                   BLE_GAP_SEC_STATUS_SUCCESS,
                                                   &m_sec_params);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0);
            break;

        case BLE_GAP_EVT_AUTH_STATUS:
            m_auth_status = p_ble_evt->evt.gap_evt.params.auth_status;
            break;

        case BLE_GAP_EVT_SEC_INFO_REQUEST:
            p_enc_info = &m_auth_status.periph_keys.enc_info;
            if (p_enc_info->div == p_ble_evt->evt.gap_evt.params.sec_info_request.div)
            {
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, p_enc_info, NULL);
            }
            else
            {
                // No keys found for this device
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, NULL, NULL);
            }
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT)
            {
                nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);

                // Go to system-off mode (this function will not return; wakeup will cause a reset)
                GPIO_WAKEUP_BUTTON_CONFIG(WAKEUP_BUTTON_PIN);
                err_code = sd_power_system_off();
            }
            break;


        default:
            break;
    }

    APP_ERROR_CHECK(err_code);
}


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    on_ble_evt(p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    ble_ms_on_ble_evt(&m_ms, p_ble_evt);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    BLE_STACK_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_1000MS_CALIBRATION,
                           BLE_L2CAP_MTU_DEF,
                           ble_evt_dispatch,
                           true);
}


/**@brief Function for the Event Scheduler initialization.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}


/**@brief Function for initializing the GPIOTE handler module.
 */
static void gpiote_init(void)
{
    APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);
}



/**@brief Function for initializing the button handler module.
 */
static void buttons_init(void)
{
    // Note: Array must be static because a pointer to it will be saved in the Button handler
    //       module.
    static app_button_cfg_t buttons[] =
    {
        {WAKEUP_BUTTON_PIN, false, NRF_GPIO_PIN_PULLUP, NULL}
    };

    APP_BUTTON_INIT(buttons, sizeof(buttons) / sizeof(buttons[0]), BUTTON_DETECTION_DELAY, true);
}


//Returns true if device is connected and all mug ids have been seen
int is_ready(){
  if (conn && m_ms.ready){
      return true;
  }
  return false;
}


//function for debugging the mug id structure
void debug_ble_ids(){
  int i;
  unsigned char buf[32];

  sprintf((char*)buf, "%d MUGS SEEN:\n",m_ms.mug_len);

  for(i=0;i< m_ms.mug_len; i++){
     sprintf((char*)buf, "%d\n",m_ms.mugs[i].MUG_ID);
     simple_uart_putstring(buf);
  }

}



/**@brief Function for initialising ble
 */
void start_ble(MUG_STATUS* mug_list)
{


    conn = 0;                //is connected set to false
    leds_init();
    timers_init();
    gpiote_init();
    buttons_init();
    ble_stack_init();
    scheduler_init();
    gap_params_init();
    services_init(mug_list);
    advertising_init();
    conn_params_init();
    sec_params_init();

    // Start advertising
    advertising_start();

}

//updates the temperature chracteristic in GATT and notifies the client
void ble_update_temp(uint8_t* val)
{
    uint32_t err_code = NRF_SUCCESS;

    uint16_t len = 1;


    //Update GATT database with new declied characterisic
    err_code = sd_ble_gatts_value_set(m_ms.temp_char_handles.value_handle,
                                          0,
                                          &len,
                                          val);


    APP_ERROR_CHECK(err_code);

    // Send updated value notification to android app
    ble_gatts_hvx_params_t hvx_params;
    uint16_t               hvx_len;
    hvx_len = len;

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle   = m_ms.temp_char_handles.value_handle;
    hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.offset   = 0;
    hvx_params.p_len    = &hvx_len;
    hvx_params.p_data   = val;

    err_code = sd_ble_gatts_hvx(m_ms.conn_handle, &hvx_params);

    if ((err_code == NRF_SUCCESS) && (hvx_len != len))
        {
            err_code = NRF_ERROR_DATA_SIZE;
        }


    if(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING){
        char buf[30];
        sprintf((char*)buf, "Temp update ERR %lX\n",err_code);
        simple_uart_putstring(buf);
        APP_ERROR_CHECK(err_code);
     }
}




//updates the bump chracteristic in GATT and notifies the client
void ble_update_bump(uint8_t* val)
{
    uint32_t err_code = NRF_SUCCESS;

    uint16_t len = 1;


    //Update GATT database with new declied characterisic
    err_code = sd_ble_gatts_value_set(m_ms.bump_char_handles.value_handle,
                                          0,
                                          &len,
                                          val);


    APP_ERROR_CHECK(err_code);

    // Send updated value notification to android app
    ble_gatts_hvx_params_t hvx_params;
    uint16_t               hvx_len;
    hvx_len = len;

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle   = m_ms.bump_char_handles.value_handle;
    hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.offset   = 0;
    hvx_params.p_len    = &hvx_len;
    hvx_params.p_data   = val;

    err_code = sd_ble_gatts_hvx(m_ms.conn_handle, &hvx_params);

    if ((err_code == NRF_SUCCESS) && (hvx_len != len))
        {
            err_code = NRF_ERROR_DATA_SIZE;
        }


    if(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING){
        char buf[30];
        sprintf((char*)buf, "Bump update ERR %lX\n",err_code);
        simple_uart_putstring(buf);
        APP_ERROR_CHECK(err_code);
     }
}



//Sends accpeted and rejected mug ids to the app
//Then resets muss struct
void RSVP_App(){

  uint32_t err_code = NRF_SUCCESS;
  int i;
  int acc_size =0;
  int dec_size =0;

  //finds the number of accpeted and declined mugs
  for(i=0;i<m_ms.mug_len;i++){
      if(m_ms.mugs[i].PIPELINE_STATUS == REJECTED){
            dec_size++;
      }
      else if(m_ms.mugs[i].PIPELINE_STATUS == ACCEPTED){
            acc_size++;
      }
  }

  uint64_t acc_ids[acc_size];
  uint64_t dec_ids[dec_size];
  int a =0;
  int d =0;

  //populates lists of accepted and declined mugs
  for(i=0;i<m_ms.mug_len;i++){
      if(m_ms.mugs[i].PIPELINE_STATUS == REJECTED){
            dec_ids[d] = m_ms.mugs[i].MUG_ID;
            d++;
      }
      else if(m_ms.mugs[i].PIPELINE_STATUS == ACCEPTED){
            acc_ids[a] = m_ms.mugs[i].MUG_ID;
            a++;
      }
  }

  //Updates GATSS informaton for accpeted ids
  err_code = ble_ms_accepted_ids_update(&m_ms,acc_ids,acc_size);
  if(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING){
        APP_ERROR_CHECK(err_code);
  }

  //Updates GATSS informaton for declined ids
  err_code = ble_ms_declined_ids_update(&m_ms,dec_ids,dec_size);
  if(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING){
        APP_ERROR_CHECK(err_code);
  }

  simple_uart_putstring("SENT RSVP\n");

  //Reset Mugs ids ready for next set of invitations
  m_ms.mug_len = 0;
  m_ms.ready=0;
  memset(m_ms.mugs, 0, sizeof(m_ms.mugs)); //reset all mug ids to zero
  simple_uart_putstring("MUG IDS RESET\n");



}

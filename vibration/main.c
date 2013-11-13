
#include <stdlib.h>
#include <math.h>
#include "nrf_soc.h"
#include "nrf_delay.h"
#include "twi_master.h"
#include "app_timer.h"
#include "libjanek.h"
#include "libalek.h"
#include "math.h"
#include "simple_uart.h"
#include "slip_ble.h"


// Debug helper variables
static volatile bool init_ok, enable_ok, push_ok, pop_ok, tx_success;

/**@brief Error handler function, which is called when an error has occurred. 
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of error.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name. 
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{

    // This call can be used for debug purposes during development of an application.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    //ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover with a reset.
    NVIC_SystemReset();
}

 

int main()
{
    simple_uart_config(0, 23, 0, 22, 0);
    simple_uart_putstring("INIT\n");
    
    start_ble();
    simple_uart_putstring("BLUETOOTH STARTED\n");

    simple_uart_putstring("TWI MASTER INIT\n");
    twi_master_init();
    simple_uart_putstring("DONE TWI MASTER INIT\n");

    simple_uart_putstring("INIT VIBRATION\n");
    init_vibration(); 
    simple_uart_putstring("INIT VIBRATION DONE\n");
    
    NVIC_EnableIRQ(GPIOTE_IRQn);
    __enable_irq();

    simple_uart_putstring("IRQ ENABLED\n");
   
    uint8_t uart_data;
    char* buf[30];
    while (1) {
    if(is_connected()){
      if (simple_uart_get_with_timeout(1, &uart_data)) {
            sprintf((char*)buf, "got %s", uart_data);
            simple_uart_putstring(buf);
            switch (uart_data) {
              case 'v':
               vibration_toggle();
               break;
            }
        }
        vibration_update(); 
    }
        app_sched_execute();
        //power_manage(); // hangs, while loop stops here
        nrf_delay_ms(500);
    }
}

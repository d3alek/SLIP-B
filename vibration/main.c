#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "nrf.h"
#include "nrf_soc.h"
#include "nrf_gpio.h"
#include "nrf51_bitfields.h"
#include "nrf_delay.h"
#include "twi_master.h"
#include "app_timer.h"
#include "libjanek.h"
#include "math.h"
#include "simple_uart.h"

#define GSCALE 2

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
    // ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover with a reset.
    NVIC_SystemReset();
}

uint8_t acceldata[6];

int main()
{
    NRF_CLOCK->LFCLKSRC = 0; // RC Timer

    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    /* Wait for the external oscillator to start up */
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) 
    {
    }

    twi_master_init();

    NVIC_EnableIRQ(GPIOTE_IRQn);
    __enable_irq();

    MMA_init();

    simple_uart_config(0, 23, 0, 22, 0);
    nrf_gpio_cfg_output(15);
    //nrf_gpio_cfg_input(15, NRF_GPIO_PIN_PULLDOWN);
 //   uint16_t x, y, z;
 //   unsigned char buf[32];

//    uint8_t testdata[4];
    
/*
    MMA_get_motion_freefall_test(testdata);

    sprintf((char*)buf, "%d\n%d\n%d\n%d\n", testdata[0], testdata[1], testdata[2], testdata[3]); 
    simple_uart_putstring(buf);
*/

    while (1) {
        nrf_gpio_pin_toggle(15);
        //MMA_getdata(acceldata);
        //x = acceldata[0] << 8 | acceldata[1];
        //y = acceldata[2] << 8 | acceldata[3];
        //z = acceldata[4] << 8 | acceldata[5];

        //sprintf((char*)buf, "X: %d Y: %d Z: %d\n", x, y, z);
        //simple_uart_putstring(buf);
        nrf_delay_ms(500);
    }
}

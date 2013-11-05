
#include <stdlib.h>
#include <math.h>
#include "nrf_soc.h"
#include "nrf_delay.h"
#include "twi_master.h"
#include "app_timer.h"
#include "libjanek.h"
#include "math.h"
#include "simple_uart.h"
#include "slip_ble.h"

#define GSCALE 2
#define DRV1 15

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

static void gpio_init(void)
{
//  NRF_GPIO->PIN_CNF[MY_BUTTON] = (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos)
//                                        | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
//                                        | (NRF_GPIO_PIN_NOPULL << GPIO_PIN_CNF_PULL_Pos)
//                                        | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
//                                        | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);    
//
  NVIC_EnableIRQ(GPIOTE_IRQn);
  NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Set << GPIOTE_INTENSET_PORT_Pos;
}
  
/** GPIOTE interrupt handler.
*/
void GPIOTE_IRQHandler(void)
{
    simple_uart_putstring("INTERRUPT!");
    // Event causing the interrupt must be cleared
    if ((NRF_GPIOTE->EVENTS_PORT != 0))
    {
          NRF_GPIOTE->EVENTS_PORT = 0;
    }
}
 

int main()
{
    simple_uart_config(0, 23, 0, 22, 0);
    simple_uart_putstring("INIT\n");
    start_ble();
    simple_uart_putstring("BLUETOOTH STARTED\n");

//    NRF_CLOCK->LFCLKSRC = 0; // RC Timer

//    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
//    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    /* Wait for the external oscillator to start up */
    /*
    simple_uart_putstring("WAITING FOR EXTERNAL OSCILLATOR\n");
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) 
    {
    }

    simple_uart_putstring("EXTERNAL OSCILLATOR STARTED\n");
    */
    /*simple_uart_putstring("TWI MASTER INIT\n");
    twi_master_init();
    simple_uart_putstring("AFTER TWI MASTER INIT\n");

    */
    NVIC_EnableIRQ(GPIOTE_IRQn);
    __enable_irq();

    simple_uart_putstring("IRQ ENABLED\n");
    
    simple_uart_putstring("GPIO INIT\n");
    
    gpio_init();
    
    simple_uart_putstring("DONE GPIO\n");
    simple_uart_putstring("MMA CONFIG\n");

    MMA_configure_motion_detection();
    simple_uart_putstring("DONE MMA CONFIG\n");

    //MMA_init();

    //simple_uart_putstring("MMA INITIALIZED\n");
    
    nrf_gpio_cfg_output(DRV1);
    //nrf_gpio_cfg_input(15, NRF_GPIO_PIN_PULLDOWN);
 //   uint16_t x, y, z;
    unsigned char buf[32];

//    uint8_t testdata[4];
    
/*
    MMA_get_motion_freefall_test(testdata);

    sprintf((char*)buf, "%d\n%d\n%d\n%d\n", testdata[0], testdata[1], testdata[2], testdata[3]); 
    simple_uart_putstring(buf);
*/

    uint8_t uart_data;
    while (1) {
        if (simple_uart_get_with_timeout(1, &uart_data)) {
            sprintf((char*)buf, "got %s", uart_data);
            simple_uart_putstring(buf);
            switch (uart_data) {
                case 'v':
                    nrf_gpio_pin_toggle(DRV1);
                    break;
            }
        }
        //nrf_gpio_pin_toggle(15);
        app_sched_execute();

        //MMA_getdata(acceldata);
        //x = acceldata[0] << 8 | acceldata[1];
        //y = acceldata[2] << 8 | acceldata[3];
        //z = acceldata[4] << 8 | acceldata[5];

        //sprintf((char*)buf, "X: %d Y: %d Z: %d\n", x, y, z);
        //simple_uart_putstring(buf);
        nrf_delay_ms(500);
    }
}

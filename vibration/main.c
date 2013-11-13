
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
#include "radio_config.h"

static uint8_t packet[PACKET_PAYLOAD_MAXSIZE];  // Packet to transmit.


void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    NVIC_SystemReset();
}


int main()
{
    // // Start 16 MHz crystal oscillator.
    // NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    // NRF_CLOCK->TASKS_HFCLKSTART    = 1;

    // // Wait for the external oscillator to start up.
    // while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {}

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

    // Enable LED
    nrf_gpio_cfg_output(0);

    void send_packet(void)
    {
      // Set payload pointer.
      NRF_RADIO->PACKETPTR = (uint32_t)packet;
      uint8_t i;
      for (i=0; i<4; i++)
      {
        NRF_RADIO->EVENTS_READY = 0U;
        NRF_RADIO->TASKS_TXEN   = 1; // Enable radio and wait for ready.

        while (NRF_RADIO->EVENTS_READY == 0U) {}

        // Start transmission.
        NRF_RADIO->TASKS_START = 1U;
        NRF_RADIO->EVENTS_END  = 0U;

        while(NRF_RADIO->EVENTS_END == 0U) {} // Wait for end of the transmission packet.

        NRF_RADIO->EVENTS_DISABLED = 0U;
        NRF_RADIO->TASKS_DISABLE   = 1U; // Disable the radio.

        while(NRF_RADIO->EVENTS_DISABLED == 0U) {}
      }
    }

    // uint64_t this_device_id = ((uint64_t) NRF_FICR->DEVICEID[1] << 32) | ((uint64_t) NRF_FICR->DEVICEID[0]);
    uint64_t device_id = 0x5b83092c6767bb6d;
    // uint64_t device_id = 0xc5861596e2118c8d;
    uint8_t id_c;
    bool seq_start = true;

    uint8_t uart_data;
    char* buf[30];
    while (1) {
    if ( is_connected() ) {
        nrf_gpio_pin_toggle(0);
        nrf_delay_ms(200);

        simple_uart_putstring("RADIO INIT\n");
        radio_configure();
        simple_uart_putstring("DONE RADIO INIT\n");

        if (seq_start) {
          seq_start = !seq_start;
          // Packet to send over radio
          packet[0] = (uint8_t) 0xcf;

          nrf_gpio_pin_set(0);
          nrf_delay_ms(1000);
        }
        else {
          // Packet to send over radio
          packet[0] = (uint8_t) (device_id >> (id_c*8)) & 0xff;

          nrf_gpio_pin_toggle(0);
          nrf_delay_ms(200);

          if (id_c < 7) {
            id_c++;
          }
          else {
            id_c = 0;
            seq_start = !seq_start;
          }
        }

        // send_packet();
    }
        app_sched_execute();
        //power_manage(); // hangs, while loop stops here
        nrf_delay_ms(500);
    }
}

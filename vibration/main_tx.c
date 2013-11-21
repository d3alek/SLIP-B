
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
#include "app_scheduler.h"

static uint8_t packet[PACKET_PAYLOAD_MAXSIZE];  // Packet to transmit.


void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
	NVIC_SystemReset();
}

void send_packet(void)
{
	// Set payload pointer.
	NRF_RADIO->PACKETPTR = (uint32_t)packet;
	uint8_t i;
	for (i=0; i<1; i++)
	{
		// simple_uart_putstring("TX START\n");
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
		// simple_uart_putstring("TX END\n");
	}
}

int main()
{
	// Start 16 MHz crystal oscillator.
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_HFCLKSTART    = 1;

	// Wait for the external oscillator to start up.
	while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {}

	simple_uart_config(0, 23, 0, 22, 0);
	uint8_t uart_data;
	char* buf[30];

    radio_configure();
    simple_uart_putstring("Radio init\n");

	// start_ble();
	// simple_uart_putstring("Bluetooth init\n");

	twi_master_init();
	simple_uart_putstring("TWI master init\n");

	// init_vibration();
	// simple_uart_putstring("Vibration init\n");

	// Enable LED
	nrf_gpio_cfg_output(0);

	// uint64_t this_device_id = ((uint64_t) NRF_FICR->DEVICEID[1] << 32) | ((uint64_t) NRF_FICR->DEVICEID[0]);
	// uint64_t device_id = 0x5b83092c6767bb6d;
	uint64_t device_id = 0xc5861596e2118c8d;  // Chip without buttons
	// uint64_t device_id = 0x485322d4e0f44c3d;

	uint8_t id_c;
	bool seq_start = true;
	bool sending = false;
	bool receiving = false;

	while (1) {
		if (receiving) {
			simple_uart_putstring("Ready to receive!\n");
			NRF_RADIO->PACKETPTR    = (uint32_t) packet; // Set payload pointer.
			NRF_RADIO->EVENTS_READY = 0U;
			NRF_RADIO->TASKS_RXEN   = 1U; // Enable radio.
			while(NRF_RADIO->EVENTS_READY == 0U) {} // Wait for an event to be ready.
			NRF_RADIO->EVENTS_END  = 0U;
			NRF_RADIO->TASKS_START = 1U; // Start listening and wait for address received event.

			int timeout = 20;
			bool incoming_msg = true;
			while(NRF_RADIO->EVENTS_END == 0U) { // Wait for the end of the packet.
				// if (timeout-- > 0) {
				//     nrf_delay_ms(1);
				// } else {
				//     incoming_msg = false;
				//     break;
				// }
			}

			if (incoming_msg){
				if (NRF_RADIO->CRCSTATUS == 1U) { // Write received data to port 1 on CRC match.
					sprintf((char*)buf, "%x ", packet[0]);
					simple_uart_putstring(buf);
					if (packet[0] == 0xa0) {
						simple_uart_putstring("Rejected!\n");
					} else if (packet[0] == 0xaf) {
						simple_uart_putstring("Accepted!\n");
					} else {
						simple_uart_putstring("Error!\n");
					}
				}
			}
			NRF_RADIO->EVENTS_DISABLED = 0U;
			NRF_RADIO->TASKS_DISABLE   = 1U;  // Disable the radio.
			while(NRF_RADIO->EVENTS_DISABLED == 0U) {}
			receiving = false;
		}
		if (!sending) {
			// Block execution until you get Serial (sends 2 bytes)
			simple_uart_putstring("Ready to send!\n");
			uart_data = simple_uart_get();
			uart_data = simple_uart_get();
			sending = !sending;
			simple_uart_putstring("Sending!\n");
		}


		// if ( is_connected() ) {
			// simple_uart_putstring("Bluetooth connection started\n");
			nrf_gpio_pin_toggle(0);
			// nrf_delay_ms(200);

			if (seq_start) {
				seq_start = false;
				// Packet to send over radio
				packet[0] = (uint8_t) 0xcf;

				nrf_gpio_pin_set(0);
				// nrf_delay_ms(1000);
			} else {
				// Packet to send over radio
				packet[0] = (uint8_t) (device_id >> (id_c*8)) & 0xff;

				nrf_gpio_pin_toggle(0);
				// nrf_delay_ms(200);

				if (id_c < 7) {
					id_c++;
				}
				else {
					id_c = 0;
					sending = false;
					receiving = true;
					seq_start = true;
					simple_uart_putstring("Sent!\n");
				}
			}

		    // sd_softdevice_disable();
			// simple_uart_putstring("Disabled soft device\n");
			radio_configure();
			// simple_uart_putstring("Configured radio\n");

			send_packet();

	        // sd_softdevice_enable(NRF_CLOCK_LFCLKSRC_RC_250_PPM_1000MS_CALIBRATION,app_error_handler);
			// simple_uart_putstring("Enabled soft device\n");
		// }
		// vibration_update();
		app_sched_execute();
		//power_manage(); // hangs, while loop stops here
		nrf_delay_ms(20);
	}
}

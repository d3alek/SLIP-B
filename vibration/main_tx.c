
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

static uint64_t packet[PACKET_PAYLOAD_MAXSIZE];  // Packet to transmit.


void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
	NVIC_SystemReset();
}

// ======= RADIO COMMS =========
void send_packet(uint8_t rep){
	// Set payload pointer.
	NRF_RADIO->PACKETPTR = &packet;
	uint8_t i;
	for (i=0; i<rep; i++)
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

bool receive_packet(uint64_t timeout){
	bool incoming_msg = true;

	NRF_RADIO->PACKETPTR    = &packet; // Set payload pointer.
	NRF_RADIO->EVENTS_READY = 0U;
	NRF_RADIO->TASKS_RXEN   = 1U; // Enable radio.

	while(NRF_RADIO->EVENTS_READY == 0U) {} // Wait for an event to be ready.

	NRF_RADIO->EVENTS_END  = 0U;
	NRF_RADIO->TASKS_START = 1U; // Start listening and wait for address received event.

	while(NRF_RADIO->EVENTS_END == 0U) { // Wait for the end of the packet.
		if (timeout-- > 0) {
		    nrf_delay_ms(1);
		} else {
		    simple_uart_putstring("Timeout!\n");
		    incoming_msg = false;
		    break;
		}
	}

	if (NRF_RADIO->CRCSTATUS == 1U) {} // Write received data to port 1 on CRC match.

	NRF_RADIO->EVENTS_DISABLED = 0U;
	NRF_RADIO->TASKS_DISABLE   = 1U;  // Disable the radio.

	while(NRF_RADIO->EVENTS_DISABLED == 0U) {}

	return incoming_msg;
}

// ======= END - RADIO COMMS =========

int main(){
	// Start 16 MHz crystal oscillator.
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_HFCLKSTART    = 1;

	// Wait for the external oscillator to start up.
	while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {}

	simple_uart_config(0, 23, 0, 22, 0);
	uint8_t uart_data;
	char* buf[32];

    radio_configure();
    simple_uart_putstring("Radio init\n");

	// start_ble();
	// simple_uart_putstring("Bluetooth init\n");

	twi_master_init();
	simple_uart_putstring("TWI master init\n");

	// init_vibration();
	// simple_uart_putstring("Vibration init\n");

	// uint64_t this_device_id = ((uint64_t) NRF_FICR->DEVICEID[1] << 32) | ((uint64_t) NRF_FICR->DEVICEID[0]);
	// sprintf((char*)buf, "ID: %llx\n",  this_device_id);
	// simple_uart_putstring(buf);
	uint64_t device_id = 0xd163bbdd530ec035;  // Chip without buttons
	bool receiving = false;

	while (1) {
		// Block execution until you get Serial (2 bytes including \n)
		simple_uart_putstring("Ready to send!\n");
		uart_data = simple_uart_get();
		uart_data = simple_uart_get();
		simple_uart_putstring("Sending!\n");


		// if ( is_connected() ) {
			// simple_uart_putstring("Bluetooth connection started\n");
			// nrf_delay_ms(200);

				// Discover
				packet[0] = (uint64_t) 0xcfcf;
				packet[1] = device_id;

				send_packet(1);

				if (receive_packet(50)){
					if (packet[0] == 0xaf && packet[1] == device_id) {
						simple_uart_putstring("ACKed\n");
					}
				}

					// Remove this when loop is implemented
					nrf_delay_ms(5000);

				// Get availability
				packet[0] = (uint64_t) 0xabab;
				packet[1] = device_id;

				send_packet(1);

				if (receive_packet(50)){
					if (packet[0] == 0xaa && packet[1] == device_id) {
						simple_uart_putstring("Accepted\n");
					} else if (packet[0] == 0xff && packet[1] == device_id) {
						simple_uart_putstring("Rejected\n");
					}
				}

				// Vibrate
				packet[0] = (uint64_t) 0xdede;
				packet[1] = device_id;

				send_packet(1);

				if (receive_packet(50)){
					if (packet[0] == 0xaf && packet[1] == device_id) {
						simple_uart_putstring("ACKed\n");
					}
				}


		    // sd_softdevice_disable();
			// simple_uart_putstring("Disabled soft device\n");
			// radio_configure();
			// simple_uart_putstring("Configured radio\n");


	        // sd_softdevice_enable(NRF_CLOCK_LFCLKSRC_RC_250_PPM_1000MS_CALIBRATION,app_error_handler);
			// simple_uart_putstring("Enabled soft device\n");
		// }
		// vibration_update();
		app_sched_execute();
		//power_manage(); // hangs, while loop stops here
	}
}

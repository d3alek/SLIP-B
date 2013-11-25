#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "nrf_soc.h"
#include "nrf_delay.h"
#include "nrf_temp.h"
#include "twi_master.h"
#include "app_timer.h"
#include "libjanek.h"
#include "libalek.h"
#include "math.h"
#include "simple_uart.h"
#include "ble/slip_ble.h"
#include "radio_config.h"
#include "app_scheduler.h"

#define MUG_MAX_NUM 10

static uint64_t packet[PACKET_PAYLOAD_MAXSIZE];  // Packet to transmit.
MUG_STATUS MUG_LIST[MUG_MAX_NUM];



void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
	char* buf[32];
	sprintf((char*)buf, "ec: %d\nline: %d\n",  error_code, line_num);
    simple_uart_putstring(buf);

	//NVIC_SystemReset();
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
		    // simple_uart_putstring("Timeout!\n");
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


/*
==============================================
Function: initialize_all(void)

	Initialize oscillator, radio, bluetooth,
	twi and vibration	

==============================================
*/
static void initialize_all()
{
	char buf[30];
	// Start 16 MHz crystal oscillator.
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_HFCLKSTART    = 1;
	
        // oscillator
	while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
		// busy wait until the oscilator is up and running
	}
	
	simple_uart_config(0, 23, 0, 22, 0);
	simple_uart_putstring("INIT\n");
	
	// initiliaze radio
    radio_configure(); 
    simple_uart_putstring("Configured radio\n");

	// initialize bluetooth
    start_ble(MUG_LIST); 
	simple_uart_putstring("BLUETOOTH STARTED\n");
		
	// initialize twi
	twi_master_init();
	simple_uart_putstring("TWI master init\n");

	init_vibration();
	simple_uart_putstring("Vibration init\n");


}


int main(){
	
    initialize_all();
    char buf[30];
	uint8_t uart_data;
	
	// Print Device ID
	uint64_t this_device_id = (((uint64_t) NRF_FICR->DEVICEID[1] << 32) | ((uint64_t) NRF_FICR->DEVICEID[0]));
	sprintf((char*)buf, "ID: %llx\n",  this_device_id);
	simple_uart_putstring(buf);

	uint8_t availability;

	bool radio_executed = false;

	// main application loop
	while (1) {

         //All the mugs that will be invited have been set
		 if (is_connected()) {
			// Deal with radios
		    sd_softdevice_disable();
			radio_configure();
           // simple_uart_putstring("Configured radio\n");
			// simple_uart_putstring("Disabled soft device\n");

			// END - Deal with radios

			bool disovery_complete = false;
			bool all_final_state = true;
			int8_t current_mug = 0;

		 	while (!disovery_complete){
		 		uint64_t device_id = 0;

		 		// vibration_update();
		 		if (receive_packet(20)){
		 		    device_id = packet[1];
		 		    if (this_device_id == device_id){
		 		        if (packet[0] == 0xcfcf){  // Master init sequence
		 		            simple_uart_putstring("Got init!\n");
		 		            packet[0] = 0xaf;  // Send ACK to Master
		 		            send_packet(1);    // ... 3 times ...

		 		            simple_uart_putstring("[a]ccept or [r]eject:\n");
		 		            uart_data = simple_uart_get();
		 		            switch (uart_data) {
		 		              case 'a':
		 		                availability = 0xaa;
		 		                simple_uart_putstring("Accepted\n");
		 		                break;
		 		              case 'r':
		 		                availability = 0xff;
		 		                simple_uart_putstring("Rejected\n");
		 		                break;
		 		            }
		 		            uart_data = simple_uart_get();
		 		        } else if (packet[0] == 0xabab) {  // Master poll availability
		 		            simple_uart_putstring("Sent availability status!\n");
		 		            packet[0] = availability;  // Send ACK to Master
		 		            send_packet(1);
		 		        } else if (packet[0] == 0xdede) {  // Master kettle boiling
		 		            simple_uart_putstring("Got invitation!\n");
		 		            packet[0] = 0xaf;  // Send ACK to Master
		 		            send_packet(1);    // ... 3 times ...
		 		            for (uint8_t i=0; i<100; i++){
		 		                nrf_gpio_pin_toggle(0);
		 		                nrf_delay_ms(200);
		 		            }
		 		        } else {
		 		            sprintf((char*)buf, "Brocken command: %llx\n", packet[0]);
		 		            simple_uart_putstring(buf);
		 		        }
		 		    } else {
		 		        simple_uart_putstring("Not applicable packet\n");
		 		        sprintf((char*)buf, "Target:  %llx\n", packet[1]);
		 		        simple_uart_putstring(buf);
		 		        sprintf((char*)buf, "Command: %llx\n", packet[0]);
		 		        simple_uart_putstring(buf);
		 		    }
		 		}
		 		if (bump_action()) {
		 			disovery_complete = true;
		 			// uint8_t bump_evt = 1;
		 			// ble_update_bump(&bump_evt)
		 		}
			}

		 	// Deal with radios
	         sd_softdevice_enable(NRF_CLOCK_LFCLKSRC_RC_250_PPM_1000MS_CALIBRATION,app_error_handler);
	       	 // simple_uart_putstring("Enabled soft device\n");
			// END - Deal with radios

	       	// uint8_t bump_val = 1;
	       	// ble_update_bump(&bump_val);
	       	// simple_uart_putstring("They see me bumpin', I see 'em hatin'!\n");
	       	disovery_complete = false;

			// RSVP_App();  //sends MUG information back to app via ble, defined in slip_ble.c

		}
		//debug_ble_ids();
		// vibration_update();
		app_sched_execute();
		nrf_delay_ms(500);

	}
}

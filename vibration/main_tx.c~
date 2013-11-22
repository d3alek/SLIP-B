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
#include "slip_ble.h"
#include "radio_config.h"
#include "app_scheduler.h"

#define MUG_MAX_NUM 10

static uint64_t packet[PACKET_PAYLOAD_MAXSIZE];  // Packet to transmit.
MUG_STATUS MUG_LIST[MUG_MAX_NUM];

/*
==============================================
Function: find_temperature(
			uint8_t * temperatureToReach,
			uint8_t * temperatureDifference,
			uint8_t * currentTemperature,
			int8_t * hasReachedTemperature
			)

	Calls the function for finding temperature and
	checking if temperature has rising to a certain
	threshold.

Explanation:
	First the base temperature is found. This is the
	first reading where two temperature readings are
	close to each other (+-2).

	After the base temperature is found, each
	subsequent reading is tested against the temperature
	target. This target is calculated as follows:
		target = base_temperature + temperature_difference

	Once this target has been hit, the passed parameter
	"hasReachedTemperature" is changed from 0 to 1 to
	signal to the caller that the temperature target
	has indeed been hit.

==============================================
 */
void find_temperature(uint8_t * temperatureToReach, uint8_t * temperatureDifference,
		uint8_t * currentTemperature, int8_t * hasReachedTemperature) {
	if (!*hasReachedTemperature) {
		// This function contains workaround for PAN_028 rev2.0A anomalies 28, 29,30 and 31.
		// stops the compiler optimizing anything to do with this variable
		int32_t volatile temperatureTemp;

    	NRF_TEMP->TASKS_START = 1;

		while (NRF_TEMP->EVENTS_DATARDY == 0) {}
    	NRF_TEMP->EVENTS_DATARDY    = 0;

		temperatureTemp = (nrf_temp_read()/4);
    	NRF_TEMP->TASKS_STOP = 1;

		uint8_t temperature = (uint8_t)(temperatureTemp);
		// debugging: print temperature
		char temp_s[80];
		sprintf(temp_s, "Temperature: %d\n", temperature);
 		simple_uart_putstring(temp_s);

		// either we find the base temperature or we check for
		// if the temperature has risen enough
		if (*temperatureToReach != 0) {
			*currentTemperature = temperature;
			// check if we've reached the correct temperature
			if (*currentTemperature >= *temperatureToReach) {
				nrf_gpio_pin_toggle(0); // testing: toggle LED when temperature has been reached
				*hasReachedTemperature = 1;
			}
		} else {
			// get the first reading
			if (*currentTemperature == 0) {
				*currentTemperature = temperature;
			} else {
				// if current and previous reading are different by at most +- 2
				// then we've found our base temperature
				if (*currentTemperature<temperature+2 && *currentTemperature>temperature-2) {
					*temperatureToReach = temperature + *temperatureDifference;
					simple_uart_putstring("Found temperature to reach");
				}
				*currentTemperature = temperature;
			}
		}
	}
}


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

int main(){
	// Start 16 MHz crystal oscillator.
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_HFCLKSTART    = 1;

	// Wait for the external oscillator to start up.
	while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {}

	simple_uart_config(0, 23, 0, 22, 0);
	uint8_t uart_data;
	char* buf[32];

	start_ble();
	simple_uart_putstring("Bluetooth init\n");

	twi_master_init();
	simple_uart_putstring("TWI master init\n");

	// init_vibration();
	// simple_uart_putstring("Vibration init\n");

	// temperature variables
	uint8_t temperatureToReach = 0;
	uint8_t temperatureDifference = 10;
	uint8_t currentTemperature = 0;
	int8_t hasReachedTemperature = 0;
	int8_t useTemperature = 0;
	// initialize temperature
	nrf_temp_init();


	bool radio_executed = false;

	// main application loop
	while (1) {
		// Block execution until you get Serial (2 bytes including \n)
		simple_uart_putstring("Ready to send!\n");
		uart_data = simple_uart_get();
		uart_data = simple_uart_get();
		simple_uart_putstring("Sending!\n");

		MUG_LIST[0].MUG_ID = 0xd163bbdd530ec035;
		MUG_LIST[0].PIPELINE_STATUS = NONE;

		// Check the temperature rise.
		if (useTemperature) {
			find_temperature(&temperatureToReach, &temperatureDifference, &currentTemperature, &hasReachedTemperature);
		}

		if ( !radio_executed ) {
			// Deal with radios
		    sd_softdevice_disable();
			simple_uart_putstring("Disabled soft device\n");
			radio_configure();
			simple_uart_putstring("Configured radio\n");
			// END - Deal with radios

			bool disovery_complete = false;
			bool all_final_state = true;
			int8_t current_mug = 0;

			while (!disovery_complete){
				if (MUG_LIST[current_mug].MUG_ID != 0){
					// sprintf((char*)buf, "Checking : %d %llx\n",  current_mug, MUG_LIST[current_mug].MUG_ID);
					// simple_uart_putstring(buf);

					// Discover
					if (MUG_LIST[current_mug].PIPELINE_STATUS == NONE || MUG_LIST[current_mug].PIPELINE_STATUS == OFF){
						packet[0] = (uint64_t) 0xcfcf;
						packet[1] = MUG_LIST[current_mug].MUG_ID;

						send_packet(1);

						if (receive_packet(50)){
							if (packet[0] == 0xaf && packet[1] == MUG_LIST[current_mug].MUG_ID) {
								simple_uart_putstring("Mug is ON\n");
								MUG_LIST[current_mug].PIPELINE_STATUS = ON;
								all_final_state = false;
							} else {
								sprintf((char*)buf, "ERROR signal: %llx\n",  packet[0]);
								simple_uart_putstring(buf);
							}
						} else {
							sprintf((char*)buf, "ERROR : %llx %llx\n",  packet[0], packet[1]);
							simple_uart_putstring(buf);
							simple_uart_putstring("Mug is OFF\n");
							MUG_LIST[current_mug].PIPELINE_STATUS = OFF;
						}

					}
					// Get availability
					else if (MUG_LIST[current_mug].PIPELINE_STATUS == ON){
						packet[0] = (uint64_t) 0xabab;
						packet[1] = MUG_LIST[current_mug].MUG_ID;

						send_packet(1);

						if (receive_packet(50)){
							if (packet[0] == 0xaa && packet[1] == MUG_LIST[current_mug].MUG_ID) {
								simple_uart_putstring("Mug Accepted\n");
								MUG_LIST[current_mug].PIPELINE_STATUS = ACCEPTED;
								all_final_state = false;
							} else if (packet[0] == 0xff && packet[1] == MUG_LIST[current_mug].MUG_ID) {
								simple_uart_putstring("Mug Rejected\n");
								MUG_LIST[current_mug].PIPELINE_STATUS = REJECTED;
							}
						} else {
							all_final_state = false;
						}
					}
					// Vibrate when water boils
					else if (MUG_LIST[current_mug].PIPELINE_STATUS == ACCEPTED){
				// TODO: && temperature > threshold
						packet[0] = (uint64_t) 0xdede;
						packet[1] = MUG_LIST[current_mug].MUG_ID;

						send_packet(1);

						if (receive_packet(50)){
							if (packet[0] == 0xaf && packet[1] == MUG_LIST[current_mug].MUG_ID) {
								simple_uart_putstring("ACKed\n");
								MUG_LIST[current_mug].PIPELINE_STATUS = INVITED;
							}
						} else {
							all_final_state = false;
						}
					}

					current_mug++;
				} else {
					if (all_final_state) {
						// TODO: check water is boiled as well!
						disovery_complete = true;
						simple_uart_putstring("DISCOVERY DONE!\n");
					} else {
						current_mug = 0;
						all_final_state = true;
					}
				}
			}

			// Deal with radios
	        sd_softdevice_enable(NRF_CLOCK_LFCLKSRC_RC_250_PPM_1000MS_CALIBRATION,app_error_handler);
			simple_uart_putstring("Enabled soft device\n");
			// END - Deal with radios

			// radio_executed = true;
		}
		// vibration_update();
		app_sched_execute();
	}
}

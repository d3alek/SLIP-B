
#include <stdlib.h>
#include <math.h>
#include "nrf_temp.h"
#include "nrf_soc.h"
#include "nrf_delay.h"
//#include "nrf_gpio.h"
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

void send_packet(void)
{
	// Set payload pointer.
	simple_uart_putstring("INIT send packet\n");
	NRF_RADIO->PACKETPTR = (uint32_t)packet;
		uint8_t i;
		for (i=0; i<1; i++)
		{
			simple_uart_putstring("for loop begin\n");
			NRF_RADIO->EVENTS_READY = 0U;
			NRF_RADIO->TASKS_TXEN   = 1; // Enable radio and wait for ready.
			simple_uart_putstring("infinite loop 1\n");
			while (NRF_RADIO->EVENTS_READY == 0U) {}
				
			// Start transmission.
			NRF_RADIO->TASKS_START = 1U;
			NRF_RADIO->EVENTS_END  = 0U;
			simple_uart_putstring("infinite loop 2\n");
			while(NRF_RADIO->EVENTS_END == 0U) {} // Wait for end of the transmission packet.
			
			NRF_RADIO->EVENTS_DISABLED = 0U;
			NRF_RADIO->TASKS_DISABLE   = 1U; // Disable the radio.
			simple_uart_putstring("infinite loop 3\n");
			while(NRF_RADIO->EVENTS_DISABLED == 0U) {}
			simple_uart_putstring("for loop end\n");
		}
}

/*
==============================================
Function: initialize_all(void)

	Initialize oscillator, radio, bluetooth,
	twi and vibration	

==============================================
 */
static void initialize_all()
{
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
	simple_uart_putstring("RADIO INIT\n");
        radio_configure(); 
        simple_uart_putstring("DONE RADIO INIT\n");

	// initialize bluetooth
	start_ble(); 
	simple_uart_putstring("BLUETOOTH STARTED\n");
		
	// initialize twi
	simple_uart_putstring("TWI MASTER INIT\n");
	twi_master_init(); 
	simple_uart_putstring("DONE TWI MASTER INIT\n");
		
	// initialize vibration
	simple_uart_putstring("INIT VIBRATION\n");
	init_vibration();
	simple_uart_putstring("INIT VIBRATION DONE\n");

	// initialize temperature
	nrf_temp_init();
}

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
		uint8_t * currentTemperature, int8_t * hasReachedTemperature) 
{
	// get temperature
	if (!*hasReachedTemperature) {

		// This function contains workaround for PAN_028 rev2.0A anomalies 28, 29,30 and 31.
	    	// stops the compiler optimizing anything to do with this variable
	    	int32_t volatile temperatureTemp;

        	NRF_TEMP->TASKS_START = 1;

        	while (NRF_TEMP->EVENTS_DATARDY == 0)            
		{
       			// Do nothing.
 	      	}
        	NRF_TEMP->EVENTS_DATARDY    = 0;  
     
		temperatureTemp                        = (nrf_temp_read()/4);
        
        	NRF_TEMP->TASKS_STOP        = 1; 

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

int main()
{
	// temperature variables
	uint8_t temperatureToReach = 0;
	uint8_t temperatureDifference = 10;
	uint8_t currentTemperature = 0;
	int8_t hasReachedTemperature = 0;
	int8_t useTemperature = 1;
    
		// Initialize
		initialize_all();

		// Enable LED
		nrf_gpio_cfg_output(0);
		
		// uint64_t this_device_id = ((uint64_t) NRF_FICR->DEVICEID[1] << 32) | ((uint64_t) NRF_FICR->DEVICEID[0]);
		uint64_t device_id = 0x5b83092c6767bb6d;
		// uint64_t device_id = 0xc5861596e2118c8d;
		uint8_t id_c;
		bool seq_start = true;
		
		uint8_t uart_data;
		char* buf[30];

		while (1) {

			// Check the temperature rise.
			if (useTemperature) {
				find_temperature(&temperatureToReach, &temperatureDifference, 
					&currentTemperature, &hasReachedTemperature);	
			}

			
			if ( is_connected() ) {
				simple_uart_putstring("IMCONNECTED !!dd!\n");
				nrf_gpio_pin_toggle(0);
				nrf_delay_ms(200);
					
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
					
					simple_uart_putstring("disable soft devic e\n");
				        sd_softdevice_disable();
					 radio_configure();
					simple_uart_putstring("SEND PACKET\n");
					send_packet();
					simple_uart_putstring("PACKET SENT\n");
				        sd_softdevice_enable(NRF_CLOCK_LFCLKSRC_RC_250_PPM_1000MS_CALIBRATION,app_error_handler);
					simple_uart_putstring("enable soft device\n");
			}
		
			vibration_update();
			app_sched_execute(); 
			//power_manage(); // hangs, while loop stops here
			nrf_delay_ms(500);
		}
}

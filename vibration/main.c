
#include <stdlib.h>
#include <math.h>
#include "nrf_temp.h"
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
static void initialize_all(int8_t useTemperature) 
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
	if (useTemperature) {
		nrf_temp_init();
	}
}

/*
==============================================
Function: main()

	Entry point of the application.

==============================================
 */

int main()
{
	// This function contains workaround for PAN_028 rev2.0A anomalies 28, 29,30 and 31.
    	// stops the compiler optimizing anything to do with this variable
    	int32_t volatile temperatureTemp;
	uint8_t temperatureToReach = 100;
	uint8_t currentTemperature;
	int8_t hasReachedTemperature = 0;
	int8_t useTemperature = 0;
    
	// the following commmented line starts the led range and was included in the sample temperature file
    	//nrf_gpio_range_cfg_output(LED_START, LED_STOP);

		// Initialize
		initialize_all(useTemperature);

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

			if (useTemperature) {
			// get temperature
			if (!hasReachedTemperature) {
		        	NRF_TEMP->TASKS_START = 1; /** Start the temperature measurement. */

        			/* Busy wait while temperature measurement is not finished, you can skip waiting if you enable interrupt for DATARDY event and read the result in the interrupt. */
		        	/*lint -e{845} // A zero has been given as right argument to operator '|'" */
		        	while (NRF_TEMP->EVENTS_DATARDY == 0)            
        			{
            				// Do nothing.
		        	}
		        	NRF_TEMP->EVENTS_DATARDY    = 0;  
        
	       			/**@note Workaround for PAN_028 rev2.0A anomaly 29 - TEMP: Stop task clears the TEMP register. */       
       	 			temperatureTemp                        = (nrf_temp_read()/4);
        
			        /**@note Workaround for PAN_028 rev2.0A anomaly 30 - TEMP: Temp module analog front end does not power down when DATARDY event occurs. */
		        	NRF_TEMP->TASKS_STOP        = 1; /** Stop the temperature measurement. */

				// the follpowing line is from the sample temperature file. It writes the temperature to a given gpio port.
	        		//nrf_gpio_port_write(NRF_GPIO_PORT_SELECT_PORT1, (uint8_t)(temperatureTmp));

				currentTemperature = (uint8_t)(temperatureTemp);
				
				// check if we've reached the correct temperature
				if (currentTemperature >= temperatureToReach) {
					hasReachedTemperature = 1;
				}
			}
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

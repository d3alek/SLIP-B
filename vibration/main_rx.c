#include "radio_config.h"
#include <stdint.h>
#include <stdbool.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "simple_uart.h"


static uint64_t volatile packet[PACKET_PAYLOAD_MAXSIZE];  /**< Received packet buffer. */

void send_packet(uint8_t rep)
{
    // Set payload pointer.
    NRF_RADIO->PACKETPTR = &packet;
    uint8_t i;
    for (i=0; i<rep; i++){
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

bool receive_packet(void){
    bool incoming_msg = false;

    NRF_RADIO->PACKETPTR    = &packet; // Set payload pointer.
    NRF_RADIO->EVENTS_READY = 0U;
    NRF_RADIO->TASKS_RXEN   = 1U; // Enable radio.

    while(NRF_RADIO->EVENTS_READY == 0U) {} // Wait for an event to be ready.

    NRF_RADIO->EVENTS_END  = 0U;
    NRF_RADIO->TASKS_START = 1U; // Start listening and wait for address received event.

    while(NRF_RADIO->EVENTS_END == 0U) {} // Wait for the end of the packet.

    if (NRF_RADIO->CRCSTATUS == 1U) { // Write received data to port 1 on CRC match.
        incoming_msg = true;
    }

    NRF_RADIO->EVENTS_DISABLED = 0U;
    NRF_RADIO->TASKS_DISABLE   = 1U;  // Disable the radio.

    while(NRF_RADIO->EVENTS_DISABLED == 0U) {}

    return incoming_msg;
}

int main(void)
{
    // Start 16 MHz crystal oscillator.
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART    = 1;

    // Wait for the external oscillator to start up.
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {}

    // Enable UART comms
    simple_uart_config(0, 23, 0, 22, 0);
    unsigned char buf[32];
    uint8_t uart_data;

    // Enable LED
    nrf_gpio_cfg_output(0);

    // Set radio configuration parameters.
    radio_configure();

    // Print Device ID
    uint64_t this_device_id = (((uint64_t) NRF_FICR->DEVICEID[1] << 32) | ((uint64_t) NRF_FICR->DEVICEID[0]));
    sprintf((char*)buf, "ID: %llx\n",  this_device_id);
    simple_uart_putstring(buf);

    // Initialise LibAlek
    // init_vibration();

    uint8_t availability;

    while(true){
        uint64_t device_id = 0;

        // vibration_update();
        if (receive_packet()){
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
    }
}

/* Copyright (c) 2013 Janek Mann
*/

#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "twi_master.h"

#include "libjanek.h"

#define MMA_ADDRESS 0x38

static uint8_t mma_init_cmd[] = {0x2a, 0x2d};
static uint8_t mma_init_cmd2[] = {0x2b, 0x03}; // Low Power
static uint8_t mma_reg_x[] = { 0x01 };
static uint8_t mma_reg_who_am_i[] = { 0x0d };

void MMA_init(void) {
		uint8_t data2[2];
		twi_master_transfer(MMA_ADDRESS, mma_init_cmd, 2, TWI_ISSUE_STOP);
        twi_master_transfer(MMA_ADDRESS, mma_init_cmd2, 2, TWI_ISSUE_STOP);
		twi_master_transfer(MMA_ADDRESS, mma_reg_who_am_i, 1, TWI_DONT_ISSUE_STOP);
		twi_master_transfer(MMA_ADDRESS | TWI_READ_BIT, data2, 1, TWI_ISSUE_STOP);
}

void MMA_getdata(uint8_t* data) {
		twi_master_transfer(MMA_ADDRESS, mma_reg_x, 1, TWI_DONT_ISSUE_STOP);
		twi_master_transfer(MMA_ADDRESS | TWI_READ_BIT, data, 6, TWI_ISSUE_STOP);
}		

void MMA_get_motion_freefall_test(uint8_t* data) {
    twi_master_transfer(MMA_ADDRESS, 0x15, 1, TWI_DONT_ISSUE_STOP);
    twi_master_transfer(MMA_ADDRESS | TWI_READ_BIT, data, 1, TWI_ISSUE_STOP);
    
    twi_master_transfer(MMA_ADDRESS, 0x17, 1, TWI_DONT_ISSUE_STOP);
    twi_master_transfer(MMA_ADDRESS | TWI_READ_BIT, data+1, 1, TWI_ISSUE_STOP);

    twi_master_transfer(MMA_ADDRESS, 0x18, 1, TWI_DONT_ISSUE_STOP);
    twi_master_transfer(MMA_ADDRESS | TWI_READ_BIT, data+2, 1, TWI_ISSUE_STOP);

    twi_master_transfer(MMA_ADDRESS, 0x16, 1, TWI_DONT_ISSUE_STOP);
    twi_master_transfer(MMA_ADDRESS | TWI_READ_BIT, data+3, 1, TWI_ISSUE_STOP);
}

static uint8_t mma_standby = {0x2a, 0x18};
static uint8_t mma_motion_set_config = {0x15, 0xd8};
static uint8_t mma_motion_set_thresh = {0x17, 0x30};
static uint8_t mma_motion_set_debounce = {0x18, 0x0A};
static uint8_t mma_motion_enable_interrupt = {0x2D, 0x04};
static uint8_t mma_motion_route_interrupt = {0x2E, 0x04};
//static uint8_t mma_active = {

void MMA_put_active() {
    uint8_t ctrl_reg_data;
    //TODO not tested 
    twi_master_transfer(MMA_ADDRESS, 0x2a, 1, TWI_DONT_ISSUE_STOP);
    twi_master_transfer(MMA_ADDRESS | TWI_READ_BIT, ctrl_reg_data, 1, TWI_ISSUE_STOP);
    ctrl_reg_data |= 0x01;
    uint8_t send = {0x2a, ctrl_reg_data};
    twi_master_transfer(MMA_ADDRESS, send, 2, TWI_ISSUE_STOP);
}
    


void MMA_configure_motion_detection() {
    twi_master_transfer(MMA_ADDRESS, mma_standby, 2, TWI_ISSUE_STOP);
    twi_master_transfer(MMA_ADDRESS, mma_motion_set_config, 2, TWI_ISSUE_STOP);
    twi_master_transfer(MMA_ADDRESS, mma_motion_set_thresh, 2, TWI_ISSUE_STOP);
    twi_master_transfer(MMA_ADDRESS, mma_motion_set_debounce, 2, TWI_ISSUE_STOP);
    twi_master_transfer(MMA_ADDRESS, mma_motion_enable_interrupt, 2, TWI_ISSUE_STOP);
    twi_master_transfer(MMA_ADDRESS, mma_motion_enable_interrupt, 2, TWI_ISSUE_STOP);
    MMA_put_active();
}

#define TMP_ADDRESS (0x49 << 1)

void TMP_getdata(uint8_t* data) {
		twi_master_transfer(TMP_ADDRESS | TWI_READ_BIT, data, 2, TWI_ISSUE_STOP);
}

uint8_t ADS_conf[] = { 0x0c };

void ADS_setconf(uint8_t new_conf) {
		if (new_conf != ADS_conf[0]) {
				ADS_conf[0] = new_conf & 0x0F;
				twi_master_transfer(ADS_ADDRESS, ADS_conf, 1, TWI_ISSUE_STOP);
		}
}

void ADS_getdata(uint8_t* data) {
		twi_master_transfer(ADS_ADDRESS | TWI_READ_BIT, data, 3, TWI_ISSUE_STOP);
}

#define LED_PIN 16

void LED_init(void) {
		nrf_gpio_pin_write(LED_PIN, 0);
		nrf_gpio_cfg_output(LED_PIN);
}

void LED_set(bool state) {
		nrf_gpio_pin_write(LED_PIN, state);
}

bool LED_get(void) {
    return nrf_gpio_pin_read(LED_PIN);
}

#define CHRG_OUT_PIN 25
#define CHRG_IN_PIN  24

void CHRG_init(void) {
    nrf_gpio_cfg_input(CHRG_OUT_PIN, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(CHRG_IN_PIN, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_pin_write(CHRG_OUT_PIN, 1);
}

uint8_t CHRG_getstate(void) {
    uint8_t result = 0;
    result = nrf_gpio_pin_read(CHRG_IN_PIN);
    nrf_gpio_cfg_output(CHRG_OUT_PIN);
    result |= (nrf_gpio_pin_read(CHRG_IN_PIN)<<1);
    nrf_gpio_cfg_input(CHRG_OUT_PIN, NRF_GPIO_PIN_NOPULL);
    return result;
}

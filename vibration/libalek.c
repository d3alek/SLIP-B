
#include <stdlib.h>
#include <math.h>
#include "nrf_soc.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "twi_master.h"
#include "libjanek.h"
#include "simple_uart.h"

#define DRV1 15


#define FRAC_2d1 5000
#define FRAC_2d2 2500
#define FRAC_2d3 1250
#define FRAC_2d4 625 
#define FRAC_2d5 313 
#define FRAC_2d6 156 
#define FRAC_2d7 78 
#define FRAC_2d8 39 
#define FRAC_2d9 20 
#define FRAC_2d10 10 
#define FRAC_2d11 5 
#define FRAC_2d12 2 

#define FULL_SCALE_2G 1
#define FULL_SCALE_4G 2
#define FULL_SCALE_8G 3

static int full_scale = FULL_SCALE_8G;

uint8_t acceldata[6];
uint16_t prevx, prevy, prevz;

char* buf[30];

uint16_t s14_dec(uint8_t hi, uint8_t lo, bool* minus, uint16_t* mantissa) {
    uint16_t data = hi << 8 | lo;
    uint16_t origdata = data;
    if (hi > 0x7F) {
        //simple_uart_putstring("-");
        *minus = true;
        data = (~data + 1);
    }
    
    else {
        *minus = false;
        //simple_uart_putstring("+");
    }

    /*
    uint8_t a, b, c, d, r;

    a = data >> 2 / 1000;
    r = (data >> 2) % 1000;
    b = r / 100;
    r %= 100;
    c = r / 10;
    d = r % 10;

    data = a * 1000 + b * 100 + c * 10 + d;
    */
    
    int int_part;

    hi = data >> 8;
    if (full_scale == FULL_SCALE_2G) {
        int_part = (hi & 0x40) >>6;
        origdata = origdata << 2;
    }
    else if (full_scale == FULL_SCALE_4G) {
        int_part = (hi & 0x60) >>5;
        origdata = origdata <<3;
    }
    else {
        int_part = (hi & 0x70) >>4;
        origdata = origdata <<4;
    }

    int result = 0;
    uint8_t value = origdata >> 8;

    if (value & (1<<7))
        result += FRAC_2d1;
    if (value & (1<<6))
        result += FRAC_2d2;
    if (value & (1<<5))
        result += FRAC_2d3;
    if (value & (1<<4))
        result += FRAC_2d4;
    
    value = (origdata << 4) >> 8;
    if (value & (1<<7))
        result += FRAC_2d5;
    if (value & (1<<6))
        result += FRAC_2d6;
    if (value & (1<<5))
        result += FRAC_2d7;
    if (value & (1<<4))
        result += FRAC_2d8;
    if (value & (1<<3))
        result += FRAC_2d9;
    if (value & (1<<2))
        result += FRAC_2d10;

    if (full_scale != FULL_SCALE_8G) {
        if ((value & (1<<1)) == 1) {
            result += FRAC_2d11;
        }
        if (full_scale == FULL_SCALE_2G) {
            if (value & 1) {
                result += FRAC_2d12;
            }
        }
    }
    *mantissa = result; 
    
    return int_part;
}

// Call before main loop
bool init_vibration() {
    simple_uart_putstring("TWI MASTER INIT\n");
    twi_master_init();
    simple_uart_putstring("DONE TWI MASTER INIT\n");
    simple_uart_putstring("MMA CONFIG\n");
    MMA_init();
    simple_uart_putstring("DONE MMA CONFIG\n");
    nrf_gpio_cfg_output(DRV1);
    prevx = prevy = prevz = -1;
}

void vibration_toggle() {
    nrf_gpio_pin_toggle(DRV1);
}

void vibration_set() {
    nrf_gpio_pin_set(DRV1);
}

void vibration_clear() {
    nrf_gpio_pin_clear(DRV1);
}


// Call at every iteration of the main loop
void vibration_update() {
    uint16_t x, y, z;
    bool xminus, yminus, zminus;
    int xmantissa, ymantissa, zmantissa;
    x = s14_dec(acceldata[0], acceldata[1], &xminus, &xmantissa);
    y = s14_dec(acceldata[2], acceldata[3], &yminus, &ymantissa);
    z = s14_dec(acceldata[4], acceldata[5], &zminus, &zmantissa);
    
    MMA_getdata(acceldata);

    sprintf((char*)buf, "Vibration data: %c%d.%d %c%d.%d %c%d.%d\n", xminus?'-':' ', x, xmantissa, yminus?'-':' ', y, ymantissa, zminus?'-':' ', z, zmantissa);
    simple_uart_putstring(buf);

    if ((x != -1 && abs(prevx - x) >=1)
            || (y != -1 && abs(prevy - y) >=1)
            || (z != -1 && abs(prevz - z) >=1)) {
        simple_uart_putstring("Clearing vibration\n");
        nrf_gpio_pin_clear(DRV1);
    }
    prevx = x; prevy = y; prevz = z;
}

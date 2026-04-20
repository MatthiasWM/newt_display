// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Matthias Melcher

#if defined(DISPLAY_TYPE_ST7796S)

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include <stdio.h>

#include "main.h"


// ----------------------------------------------------------------------------
// Define all pins for the TFT screen output via SPI
const uint tft_pin_cs = 17;
const uint tft_pin_sck = 18;
const uint tft_pin_mosi = 19;
const uint tft_pin_miso = 16;
const uint tft_pin_reset = 14;
const uint tft_pin_dc = 15;     // select data or control
const uint tft_pin_busy = 20;   // input pin: busy signal from ePaper

// TFT related variables
spi_inst_t *tft_spi = spi0;     // use this SPI port
uint16_t tft_line[480];         // store one line of pixels for TFT


// This app currently support the ST7796s with a 480x320 TFT display
// https://www.displayfuture.com/Display/datasheet/controller/ST7796s.pdf


void tft_send_command(uint8_t cmd) {
    gpio_put(tft_pin_dc, 0);
    gpio_put(tft_pin_cs, 0);
    sleep_us(1);
    spi_write_blocking(tft_spi, &cmd, 1);
    sleep_us(1);
    gpio_put(tft_pin_cs, 1);
}

void tft_send_data(const uint8_t data) {
    gpio_put(tft_pin_dc, 1);
    gpio_put(tft_pin_cs, 0);
    sleep_us(1);
    spi_write_blocking(tft_spi, &data, 1);
    sleep_us(1);
    gpio_put(tft_pin_cs, 1);
}

void tft_send_data(const uint8_t *data, size_t size) {
    gpio_put(tft_pin_dc, 1);
    gpio_put(tft_pin_cs, 0);
    sleep_us(1);
    spi_write_blocking(tft_spi, data, size);
    sleep_us(1);
    gpio_put(tft_pin_cs, 1);
}


// Send a command a number of bytes the TFT controller
void tft_send(uint8_t cmd, size_t size=0, const uint8_t *data=nullptr) {
    gpio_put(tft_pin_dc, 0);
    gpio_put(tft_pin_cs, 0);
    sleep_us(1);
    spi_write_blocking(tft_spi, &cmd, 1);
    if (size) {
        sleep_us(1);
        gpio_put(tft_pin_dc, 1);
        sleep_us(1);
        spi_write_blocking(tft_spi, data, size);
    }
    sleep_us(1);
    gpio_put(tft_pin_cs, 1);
}

// Configure a GPIO pin for the TFT diplay port
void tft_pin_config_out(uint pin, bool value) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, value);
}

void tft_pin_config_in(uint pin, int value) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    if (value == 1) {
        gpio_pull_up(pin);
    } else if (value == -1){
        gpio_pull_down(pin);
    }
}

// Initialize SPI and boot the display controller
void tft_init() {
    // define the pins we use directly
    tft_pin_config_out(tft_pin_cs, 1);
    tft_pin_config_out(tft_pin_dc, 1);
    tft_pin_config_out(tft_pin_reset, 1);
    tft_pin_config_in(tft_pin_busy, 0);

    // initialize the SPI port
    spi_init(tft_spi, /*62.5 * */ 1000 * 1000);  // 62.5MHz, can probably be more if needed
    spi_set_format( spi0,           // SPI instance
                    8,              // Number of bits per transfer
                    SPI_CPOL_1,     // Polarity (CPOL)
                    SPI_CPHA_1,     // Phase (CPHA)
                    SPI_MSB_FIRST);
    gpio_set_function(tft_pin_sck, GPIO_FUNC_SPI);
    gpio_set_function(tft_pin_mosi, GPIO_FUNC_SPI);
    gpio_set_function(tft_pin_miso, GPIO_FUNC_SPI);
}

void tft_reset() {
    // reset the TFT controller
    spi.pulse_reset();

    spi.send(0xf0, { 0xc3 } );    // Command Set Control
    spi.send(0xf0, { 0x96 } );    // Command Set Control
    spi.send(0xc5, { 0x1c } );
    spi.send(0x36, { 0xe8 } );    // 
    spi.send(0x3a, { 0x55 } );    // 
    spi.send(0xb0, { 0x80 } );    // 
    spi.send(0xb4, { 0x01 } );    // 
    spi.send(0xb6, { 0x80, 0x02, 0x3b } );
    spi.send(0xb7, { 0xc6 } );    // 
    spi.send(0xf0, { 0x69 } );    // 
    spi.send(0xf0, { 0x3c } );    // 
    spi.send(0x11);
    sleep_ms(150);
    spi.send(0x29);
    sleep_ms(150);
}


void tft_white() {
    spi.send(0x2a, { 0x00, 0x00, 479 >> 8, 479 & 0xff} ); // col range
    spi.send(0x2b, { 0x00, 0x00, 319 >> 8, 319 & 0xff} ); // row range
    spi.start();
    spi.write(0x2c); // memory write
    spi.data_mode();
    for (int i = 0; i < 480*320; i++) {
        spi.write(0xff);
        spi.write(0xff); // white
    }   
    spi.end();
}


void tft_black() {
    spi.send(0x2a, { 0x00, 0x00, 479 >> 8, 479 & 0xff} ); // col range
    spi.send(0x2b, { 0x00, 0x00, 319 >> 8, 319 & 0xff} ); // row range
    spi.start();
    spi.write(0x2c); // memory write
    spi.data_mode();
    for (int i = 0; i < 480*320; i++) {
        spi.write(0x00); 
        spi.write(0x00); // black
    }   
    spi.end();
}


// Send a horizontal line of pixel data to the TFT
// y is the position on the screen
// if rpt is greater 1, the line will be duplicated downward.
void tft_send_line(int y, int rpt) {
    static uint8_t cmd_col[] = {0, 0, 479 >> 8, 479 & 0xff};
    tft_send(0x2a, 4, cmd_col);
    uint8_t cmd_row[] = {(uint8_t)(y>>8), (uint8_t)y, (uint8_t)((y+rpt)>>8), (uint8_t)(y+rpt)};
    tft_send(0x2b, 4, cmd_row);
    static uint8_t cmd12[100] = {0};
    tft_send(0x2c, 480 * 2, (uint8_t *)tft_line);
    for (int i = 0; i < rpt; i++) {
        tft_send(0x3c, 480 * 2, (uint8_t *)tft_line);
    }
}

#if 0
// Leftovers from playing around with the DMA. Please ignore.
PIO pio = pio0;
uint sm = 0;
uint dma_chan = 0;
void init_n2_dma() {
    // Grant high bus priority to the DMA, so it can shove the processors out
    // of the way. This should only be needed if you are pushing things up to
    // >16bits/clk here, i.e. if you need to saturate the bus completely.
    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;

    pio = pio0;
    sm = 0;
    dma_chan = 0;

    uint16_t capture_prog_instr = pio_encode_in(pio_pins, 8);
    struct pio_program capture_prog = {
            .instructions = &capture_prog_instr,
            .length = 1,
            .origin = -1
    };
    uint offset = pio_add_program(pio, &capture_prog);

    // Configure state machine to loop over this `in` instruction forever,
    // with autopush enabled.
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_in_pins(&c, lcd_pin_d0);
    sm_config_set_wrap(&c, offset, offset);
    sm_config_set_clkdiv(&c, 10.0f);
    // Note that we may push at a < 32 bit threshold if pin_count does not
    // divide 32. We are using shift-to-right, so the sample data ends up
    // left-justified in the FIFO in this case, with some zeroes at the LSBs.
    sm_config_set_in_shift(&c, true, true, 8);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    pio_sm_init(pio, sm, offset, &c);
}
#endif

// Create a lookup table for 16 grayscales in to 16 bit RGB.
// This is currently not very useful because the MessagePad LCD output
// is only B&W and simulates grays by switching pixels on and off over time.
#define TFT_GRAY(a) ( (a<<4) | ((a>>1)|((a&0x01)<<15)) | (a<<9) )
uint16_t lcd_convert(uint8_t px) {
    #if 0 // inverse
    static const uint16_t px_lut[16] = {
        TFT_GRAY(0x00), TFT_GRAY(0x01), TFT_GRAY(0x02), TFT_GRAY(0x03),
        TFT_GRAY(0x04), TFT_GRAY(0x05), TFT_GRAY(0x06), TFT_GRAY(0x07),
        TFT_GRAY(0x08), TFT_GRAY(0x09), TFT_GRAY(0x0a), TFT_GRAY(0x0b),
        TFT_GRAY(0x0c), TFT_GRAY(0x0d), TFT_GRAY(0x0e), TFT_GRAY(0x0f),
    };
    #elif 1 // original
    static const uint16_t px_lut[16] = {
        TFT_GRAY(0x0f), TFT_GRAY(0x0e), TFT_GRAY(0x0d), TFT_GRAY(0x0c),
        TFT_GRAY(0x0b), TFT_GRAY(0x0a), TFT_GRAY(0x09), TFT_GRAY(0x08),
        TFT_GRAY(0x07), TFT_GRAY(0x06), TFT_GRAY(0x05), TFT_GRAY(0x04),
        TFT_GRAY(0x03), TFT_GRAY(0x02), TFT_GRAY(0x01), TFT_GRAY(0x00),
    };
    #else // bit swapped
    static const uint16_t px_lut[16] = {
        TFT_GRAY(0x00), TFT_GRAY(0x08), TFT_GRAY(0x04), TFT_GRAY(0x0c),
        TFT_GRAY(0x02), TFT_GRAY(0x0a), TFT_GRAY(0x06), TFT_GRAY(0x0e),
        TFT_GRAY(0x01), TFT_GRAY(0x09), TFT_GRAY(0x05), TFT_GRAY(0x0d),
        TFT_GRAY(0x03), TFT_GRAY(0x0b), TFT_GRAY(0x07), TFT_GRAY(0x0f),
    };
    #endif
    return px_lut[px&0x0f];
}

// Conver the raw LCD data into 16bit RGB data for the TFT.
// y is the source line buffer index.
// Only a single target buffer is used. The image is transfered line by line.
void lcd_line_to_tft(int y) {
    const uint8_t *src = lcd.line(y);
    uint16_t *dst = tft_line;
    for (int i=0; i<lcd.line_bytes(); i++) {
        uint8_t p1 = *src++;
        *dst++ = lcd_convert((p1>>4)&0x08);
        *dst++ = lcd_convert((p1>>3)&0x08);
        *dst++ = lcd_convert((p1>>2)&0x08);
        *dst++ = lcd_convert((p1>>1)&0x08);
        *dst++ = lcd_convert((p1   )&0x08);
        *dst++ = lcd_convert((p1<<1)&0x08);
        *dst++ = lcd_convert((p1<<2)&0x08);
        *dst++ = lcd_convert((p1<<3)&0x08);
    }
}

// Main entry point.
int old_main() {
    stdio_init_all();
    led_init();
    lcd.init();
    tft_init();


    for (;;) {
        led_toggle();

        // wait for the start of the screen data
        lcd.wait_vsync();
        // read all 320 lines and store them in RAM
        for (int i=0; i<lcd.num_lines(); i++) {
            lcd.scan_line(i);
        }
        // convert the screen line by line and send the RGB data to the TFT
        for (int i=0; i<lcd.num_lines(); i++) {
            lcd_line_to_tft(i);
            tft_send_line(i, 1);
        }
    }
}


void EPD_Init() {
    spi.init(20 * 1000 * 1000);
    tft_reset();
}

void EPD_DeepSleep() {
}

void EPD_Update() {
}

void EPD_WhiteScreen_White() {
    tft_white();
}

void EPD_WhiteScreen_Black() {
    tft_black();
}

#endif // DISPLAY_TYPE_ST7796S

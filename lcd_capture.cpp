
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Matthias Melcher


#include "lcd_capture.h"
#include "hardware/gpio.h"

/**
 * Initialize the ports for reading LCD data. This should really be done
 * using a properly set DMA channel.
 */
void CaptureDevice::init() {
    gpio_init(kPinVsync);
    gpio_set_dir(kPinVsync, GPIO_IN);
    gpio_init(kPinHsync);
    gpio_set_dir(kPinHsync, GPIO_IN);
    gpio_init(kPinPclk);
    gpio_set_dir(kPinPclk, GPIO_IN);
    for (int i=0; i<kNumBits; i++) {
        gpio_init(kPinD0 + i);
        gpio_set_dir(kPinD0 + i, GPIO_IN);
    }
}

// Wait for the rising edge of the vertical sync.
void CaptureDevice::wait_vsync() {
    // wait util vsync goes H which is the start of the vertical sync pulse
    while (gpio_get(kPinVsync) == 1) { }
    while (gpio_get(kPinVsync) == 0) { }
}

// Garb one line of pixel data from the LCD connector
void CaptureDevice::scan_line(int y) {
    // wait for the falling ege of the hsync signal
    while (gpio_get(kPinHsync) == 0) { }
    while (gpio_get(kPinHsync) == 1) { }
    // prepare to read 80 bytes
    uint8_t *dst = m_screen_buffer[y];
    for (int i=kLineBytes; i>0; --i) {
        // wait for the falling edge of the pixel clock
        while (gpio_get(kPinPclk) == 0) { }
        while (gpio_get(kPinPclk) == 1) { }
        // read all PIO bits at once, shift them into position, 
        // and write them to the buffer
        *dst++ = (((*(uint32_t*)(SIO_BASE + 0x004)) >> kPinD0) & 0xFF);
    }
}

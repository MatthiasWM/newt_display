
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Matthias Melcher


#ifndef LCD_CAPTURE_H
#define LCD_CAPTURE_H

#include "pico/stdlib.h"

/**
 * This is the code for capturing the raw pixel data from the LCD port of the
 * Apple Newton MessagePad. The LCD provides a stream of black and white pixel
 * data at a rate of about 1.5MHz. The data is organized in lines of 480 pixels.
 *
 * TODO: the Newton pulses pixels to create 16 shades of gray. We need to find
 * out how exactly the pulses are delivered and the average to incoming B&W
 * data over time to get a non-flickering grayscale image.
 *
 * TODO: the current implementation uses the CPU to read the data. This is really
 * inefficient and should be replaced by a DMA channel and the PIO to read the 
 * data and reformat it for easier conversion.
 */
class CaptureDevice {
    static constexpr uint kPinVsync = 2;    // 75Hz
    static constexpr uint kPinHsync = 3;    // 24kHz
    static constexpr uint kPinPclk = 4;     // 1.5Mhz
    static constexpr uint kPinD0 = 6;       // 1.5MHz B&W pixel data
    static constexpr uint kNumBits = 8;     // 8 data pins form kPinD0..kPinD0+kNumBits-1
    static constexpr int kLineBytes = 60;   // 60 bytes times 8 gives us 480 pixels across
    static constexpr int kNumLines = 320;   // we send 320 lines of image data

    // TODO: for now we store the raw bytes here, but we will have to implement 
    // screen buffer management.
    uint8_t m_screen_buffer[kNumLines][kLineBytes];

public:
    CaptureDevice() = default;
    void init();
    void wait_vsync();
    void scan_line(int y);
    uint num_lines() const { return kNumLines; }
    uint line_bytes() const { return kLineBytes; }
    const uint8_t* line(int y) const { return m_screen_buffer[y]; }
};

#endif // LCD_CAPTURE_H


// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Matthias Melcher

#ifndef MAIN_H
#define MAIN_H

#include "lcd_capture.h"
#include "spi_3pin.h"

extern CaptureDevice lcd;
extern SPI_3Pin spi;

extern void led_init();
extern void led_toggle();

#endif // MAIN_H

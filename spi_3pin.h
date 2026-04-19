
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Matthias Melcher


#ifndef SPI_3PIN_H
#define SPI_3PIN_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdint.h>
#include <vector>

class SPI_3Pin {
    static constexpr uint kPinSck = 18;
    static constexpr uint kPinMosi = 19;
    static constexpr uint kPinMiso = 16;
    static constexpr uint kPinCs = 17;
    static constexpr uint kPinReset = 14;
    static constexpr uint kPinDc = 15;
    static constexpr uint kPinBusy = 20;
    spi_inst_t* port { spi0 };

    void pin_config_out(uint pin, bool value);
    void pin_config_in(uint pin, int value);
public:
    SPI_3Pin() = default;
    void init();
    void wait_ready();
    void send_command(uint8_t cmd);
    void send_data(const uint8_t *data, size_t size);
    void send_data(uint8_t data); // { send_data(&data, 1); }
    void recv_data(const uint8_t *data, size_t size);
    void pulse_reset();
    void send(uint8_t cmd, std::vector<uint8_t> data = {});
};

#endif // SPI_3PIN_H

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
    static constexpr uint kPinTx = 19;
    static constexpr uint kPinRx = 16;
    static constexpr uint kPinCs = 17;
    static constexpr uint kPinReset = 14;
    static constexpr uint kPinDc = 15;
    static constexpr uint kPinBusy = 20;
    spi_inst_t* port { spi0 };
    bool m_in_read_mode { false };

    void pin_config_out(uint pin, bool value);
    void pin_config_in(uint pin, int value);
public:
    // Initialization
    SPI_3Pin() = default;
    void init(uint clockrate = 1000 * 1000);

    // Low level SPI operations
    void pulse_reset();
    void wait_ready();
    void start();
    void data_mode();
    void write(const uint8_t *data, size_t size);
    void write(uint8_t data) { write(&data, 1); }
    void start_read_mode();
    void end_read_mode();
    void read(uint8_t *data, size_t size);
    void end();

    // High level SPI operations
    void send(uint8_t cmd);
    void send(uint8_t cmd, const std::vector<uint8_t> data);
    void query(uint8_t cmd, const std::vector<uint8_t> data, std::vector<uint8_t> &response);
};

#endif // SPI_3PIN_H
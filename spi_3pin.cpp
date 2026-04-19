
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Matthias Melcher

#include "spi_3pin.h"
#include "hardware/gpio.h"

/**
 * Configure a GPIO pin as output and set it to the given value.
 */
void SPI_3Pin::pin_config_out(uint pin, bool value) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, value);
}

/**
 * Configure a GPIO pin as input and set the pull-up/down resistors.
 * value = 1 means pull-up, value = -1 means pull-down, value = 0 means no pull.
 */
void SPI_3Pin::pin_config_in(uint pin, int value) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    if (value == 1) {
        gpio_pull_up(pin);
    } else if (value == -1){
        gpio_pull_down(pin);
    }
}

// Initialize SPI and boot the display controller
void SPI_3Pin::init(uint clockrate) {
    // define the pins we use directly
    pin_config_out(kPinCs, 1);
    pin_config_out(kPinDc, 1);
    pin_config_out(kPinReset, 1);
    pin_config_in(kPinBusy, 1); // busy is active low, so we pull it up

    // initialize the SPI port
    spi_init(port, clockrate);
    spi_set_format( port,           // SPI instance
                    8,              // Number of bits per transfer
                    SPI_CPOL_1,     // Polarity (CPOL)
                    SPI_CPHA_1,     // Phase (CPHA)
                    SPI_MSB_FIRST);
    gpio_set_function(kPinSck, GPIO_FUNC_SPI);
    gpio_set_function(kPinTx, GPIO_FUNC_SPI);
    gpio_set_function(kPinRx, GPIO_FUNC_SPI);
}

void SPI_3Pin::pulse_reset() {
    gpio_put(kPinReset, 0);
    sleep_ms(100);
    gpio_put(kPinReset, 1);
    sleep_ms(100);
}

void SPI_3Pin::wait_ready() { 
    while( gpio_get(kPinBusy)==0 ) { }
}  

void SPI_3Pin::start() {
    wait_ready();
    gpio_put(kPinDc, 0);
    gpio_put(kPinCs, 0);
}

void SPI_3Pin::data_mode() {
    gpio_put(kPinDc, 1);
}

void SPI_3Pin::write(const uint8_t *data, size_t size) {
    spi_write_blocking(port, data, size);
}

void SPI_3Pin::start_read_mode() {
    gpio_set_function(kPinTx, GPIO_FUNC_SIO);
    gpio_set_dir(kPinTx, GPIO_IN); // High impedance
    m_in_read_mode = true;
}

void SPI_3Pin::end_read_mode() {
    gpio_set_dir(kPinTx, GPIO_OUT);
    gpio_set_function(kPinTx, GPIO_FUNC_SPI);
    m_in_read_mode = false;
}

void SPI_3Pin::read(uint8_t *data, size_t size) {
    spi_read_blocking(port, 0, data, size);
}

void SPI_3Pin::end() {
    gpio_put(kPinCs, 1);
    gpio_put(kPinDc, 0);
}

void SPI_3Pin::send(uint8_t cmd) {
    start();
    write(&cmd, 1);
    end();
}

void SPI_3Pin::send(uint8_t cmd, const std::vector<uint8_t> data) {
    start();
    write(&cmd, 1);
    if (!data.empty()) {
        data_mode();
        write(data.data(), data.size());
    }
    end();
}

void SPI_3Pin::query(uint8_t cmd, const std::vector<uint8_t> data, std::vector<uint8_t> &response) {
    start();
    write(&cmd, 1);
    if (!data.empty()) {
        data_mode();
        write(data.data(), data.size());
    }
    if (!response.empty()) {
        data_mode();
        start_read_mode();
        wait_ready();
        read(response.data(), response.size());
        end_read_mode();
    }
    end();
}



#if 0
// Send a command a number of bytes the TFT controller
void SPI_3Pin::tft_send(uint8_t cmd, size_t size, const uint8_t *data) {
    gpio_put(kPinDc, 0);
    gpio_put(kPinCs, 0);
    sleep_us(1);
    spi_write_blocking(port, &cmd, 1);
    if (size) {
        sleep_us(1);
        gpio_put(kPinDc, 1);
        sleep_us(1);
        spi_write_blocking(port, data, size);
    }
    sleep_us(1);
    gpio_put(kPinCs, 1);
}
#endif





#if 0

#include "Display_EPD_W21_spi.h"

void SPI_Write(unsigned char value)                                    
{                                                           
    unsigned char i;  
    for(i=0; i<8; i++)   
    {
        EPD_W21_CLK_0;
       if(value & 0x80)
          EPD_W21_MOSI_1;
        else
          EPD_W21_MOSI_0;   
        value = (value << 1); 
        EPD_W21_CLK_1; 
    }
}
//SPI write command
void EPD_W21_WriteCMD(unsigned char command)
{
	EPD_W21_CS_0;
	EPD_W21_DC_0;  // D/C#   0:command  1:data  
	SPI_Write(command);
	EPD_W21_CS_1;
}
//SPI write data
void EPD_W21_WriteDATA(unsigned char datas)
{
	EPD_W21_CS_0;
	EPD_W21_DC_1;  // D/C#   0:command  1:data
	SPI_Write(datas);
	EPD_W21_CS_1;
}
unsigned char EPD_W21_ReadDATA(void)
{
	
	unsigned char i,temp;
   pinMode(23, INPUT);  //SDA IN
    EPD_W21_CS_0;                   
	  EPD_W21_DC_1;		// command write(Must be added)
     for(i=0; i<8; i++)   
    {
    EPD_W21_CLK_0;
		if(EPD_W21_READ==1)    	
	    temp=(temp<<1)|0x01;
		else
      temp=temp<<1;
    EPD_W21_CLK_1; 
    EPD_W21_CLK_0; 
    }
	pinMode(23, OUTPUT);  //SDA OUT
	return(temp);
}



#include "hardware/spi.h"
#include "hardware/gpio.h"

// Define pins
#define SPI_PORT spi0
#define TX_PIN 3
#define RX_PIN 4
#define CS_PIN 5

void safe_spi_read(uint8_t *buffer, size_t len) {
    // 1. Pull CS Low (Start Transfer)
    gpio_put(CS_PIN, 0);

    // 2. Turn off TX (switch to SIO mode)
    gpio_set_function(TX_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(TX_PIN, GPIO_IN); // High impedance

    // 3. Read data
    spi_read_blocking(SPI_PORT, 0, buffer, len);

    // 4. Turn TX back on (switch back to SPI function)
    gpio_set_function(TX_PIN, GPIO_FUNC_SPI);

    // 5. Pull CS High (End Transfer)
    gpio_put(CS_PIN, 1);
}

#endif

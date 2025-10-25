#include "stdio.h" // for printf
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1

const uint8_t REG_NUM_IP_BASE = 0x00; // input port
const uint8_t REG_NUM_OP_BASE = 0x08; // output port 
const uint8_t REG_NUM_PI_BASE = 0x10; // polarity inversion 
const uint8_t REG_NUM_IOC_BASE = 0x18; // io control
const uint8_t REG_NUM_MSK_BASE = 0x20; // mask interrupt
const uint8_t CMD_AUTO_INCREMENT_BIT = (1 << 7);

#define PORT1_PCA9505_ADDR 0xFF
#define PORT1_PCA9505_ADDR 0xFF

void init_i2c() {
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

uint64_t read_all_pins(uint8_t addr) {
    // expect this to have orders messed up (havent tested + turned brain off)
    uint8_t command_reg = (REG_NUM_IOC_BASE & 0b00111111) | (CMD_AUTO_INCREMENT_BIT);
    uint8_t data[5];
    i2c_write_blocking(I2C_PORT, addr, &command_reg, 1, true);
    i2c_read_blocking(I2C_PORT, addr, data, 5, false);

    uint64_t ret = 0;
    for (int i = 0; i < 5; i++) {
        ret |= (uint64_t) data[i] << (8 * i);
    }
    return ret;
}

void pca9505_set_pins_hi_z(uint8_t addr) {
    // set up PCA9505 gpios as inputs, pulled up
    uint8_t command_reg = (REG_NUM_IOC_BASE & 0b00111111) | (CMD_AUTO_INCREMENT_BIT);
    uint8_t data[] = {
        command_reg,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    i2c_write_blocking(I2C_PORT, addr, data, 5, false);
}

void set_single_pin_state(uint8_t addr, int pin, bool state) {
    // expect this to completely be wrong pin
    int bank = pin / 8; // floor(pin / 8) = location of pin (banks)
    uint8_t command_reg = (REG_NUM_IOC_BASE & 0b00111111) + bank;
    int bit = pin - bank * 8;
    uint8_t pin_data = (1 << bit);
    uint8_t data[] = {command_reg, pin_data};
    i2c_write_blocking(I2C_PORT, addr, data, 2, false);
}

int main() {
    
    stdio_init_all(); // for printf

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    init_i2c();


    while(1) {
        gpio_put(25, 1);
        sleep_ms(200);
        gpio_put(25, 0);
        sleep_ms(200);
    }
}
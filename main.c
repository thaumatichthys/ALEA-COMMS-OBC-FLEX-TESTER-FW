#include "stdio.h" // for printf
#include "stdint.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "SevSeg.h"
#include "config.h"

#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1

const uint8_t REG_NUM_IP_BASE = 0x00; // input port
const uint8_t REG_NUM_OP_BASE = 0x08; // output port 
const uint8_t REG_NUM_PI_BASE = 0x10; // polarity inversion 
const uint8_t REG_NUM_IOC_BASE = 0x18; // io control
const uint8_t REG_NUM_MSK_BASE = 0x20; // mask interrupt
const uint8_t CMD_AUTO_INCREMENT_BIT = (1 << 7);

#define PORT1_PCA9505_ADDR (0b0100000 | (0b001))
#define PORT2_PCA9505_ADDR (0b0100000 | (0b000))

// repeating_timer seven_seg_timer;
struct SevSeg4Obj sevseg;

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

int pca9505_set_pins_hi_z(uint8_t addr) {
    // set up PCA9505 gpios as inputs, pulled up
    uint8_t command_reg = (REG_NUM_IOC_BASE & 0b00111111) | (CMD_AUTO_INCREMENT_BIT);
    uint8_t data[] = {
        command_reg,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    return i2c_write_blocking(I2C_PORT, addr, data, 5, false);
}

int set_single_pin_state(uint8_t addr, int pin, bool state) {
    // expect this to completely be wrong pin
    int bank = pin / 8; // floor(pin / 8) = location of pin (banks)
    uint8_t command_reg = (REG_NUM_IOC_BASE & 0b00111111) + bank;
    int bit = pin - bank * 8;
    uint8_t pin_data = (1 << bit);
    uint8_t data[] = {command_reg, pin_data};
    return i2c_write_blocking(I2C_PORT, addr, data, 2, false);
}

void test_hardware() {
    int val1 = pca9505_set_pins_hi_z(PORT1_PCA9505_ADDR);
    int val2 = pca9505_set_pins_hi_z(PORT2_PCA9505_ADDR);
    printf("testing PCA9505s\n");
    if (val1 > 0 && val2 > 0) {
        printf("both PCA9505 ACK\n");
    }
    else {
        if (val1 > 0) printf("only PORT1 ACK\n");
        if (val2 > 0) printf("only PORT2 ACK\n");
    }
}

void SevenSegment_timer_ISR() {

}

int main() {
    
    stdio_init_all(); // for printf

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    init_i2c();

    // init seven segment display
    sevseg.com_pins[0] = DISP_COM_1;
    sevseg.com_pins[1] = DISP_COM_2;
    sevseg.com_pins[2] = DISP_COM_3;
    sevseg.com_pins[3] = DISP_COM_4;
    sevseg.segment_pins[0] = DISP_SEG_A;
    sevseg.segment_pins[1] = DISP_SEG_B;
    sevseg.segment_pins[2] = DISP_SEG_C;
    sevseg.segment_pins[3] = DISP_SEG_D;
    sevseg.segment_pins[4] = DISP_SEG_E;
    sevseg.segment_pins[5] = DISP_SEG_F;
    sevseg.segment_pins[6] = DISP_SEG_G;
    sevseg.number_of_segments_used = 7;
    sevseg.is_common_cathode = true;
    


    while(1) {
        gpio_put(25, 1);
        sleep_ms(200);
        gpio_put(25, 0);
        sleep_ms(200);
        test_hardware();
    }
}
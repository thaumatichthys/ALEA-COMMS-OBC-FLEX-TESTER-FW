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

#define PORT1_PCA9505_ADDR ((0b0100000 | (0b001)))
#define PORT2_PCA9505_ADDR ((0b0100000 | (0b000)))

struct repeating_timer seven_seg_timer;
struct SevSeg4Obj sevseg;

void init_i2c() {
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 100*1000);

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
uint8_t reverse_bits(uint8_t n) {
    n = (n >> 4) | (n << 4);             // swap nibbles
    n = ((n & 0xCC) >> 2) | ((n & 0x33) << 2); // swap bit pairs
    n = ((n & 0xAA) >> 1) | ((n & 0x55) << 1); // swap adjacent bits
    return n;
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
    // if (val1 > 0 && val2 > 0) {
    //     printf("both PCA9505 ACK\n");
    // }
    // else {
    //     printf("neither responded\n");
    //     if (val1 > 0) printf("only PORT1 ACK\n");
    //     if (val2 > 0) printf("only PORT2 ACK\n");
    // }

    // for (uint8_t i = 0; i < 128; i++) {
    //     int ret = pca9505_set_pins_hi_z(i);
    //     if (ret != -1) {
    //         printf("address %d ACKed\n", i);
    //     }
    // }

    printf("return vals: %d\n", val1);
}

bool SevenSegment_timer_ISR(struct repeating_timer *t) {
    SevSeg_UpdateFSM(sevseg);
    return true;
}

int main() {
    
    stdio_init_all(); // for printf

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_init(PCA9505_RST);
    gpio_set_dir(PCA9505_RST, GPIO_OUT);
    gpio_put(PCA9505_RST, 1);

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

    sevseg.frame_buffer_[0] = 0xFA;
    sevseg.frame_buffer_[1] = 0xFB;
    sevseg.frame_buffer_[2] = 0xFC;
    sevseg.frame_buffer_[3] = 0xFD;

    SevSeg_InitGPIOs(sevseg);
    

    add_repeating_timer_us(500, SevenSegment_timer_ISR, NULL, &seven_seg_timer);


    while(1) {
        gpio_put(25, 1);
        sleep_ms(200);
        gpio_put(25, 0);
        sleep_ms(200);
        test_hardware();
    }
}
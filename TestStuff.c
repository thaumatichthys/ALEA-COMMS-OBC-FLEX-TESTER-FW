#include "TestStuff.h"
#include "config.h"
#include "hardware/i2c.h"


#define IO(a, b) ((a)*8 + (b))

const uint8_t REG_NUM_IP_BASE = 0x00; // input port
const uint8_t REG_NUM_OP_BASE = 0x08; // output port 
const uint8_t REG_NUM_PI_BASE = 0x10; // polarity inversion 
const uint8_t REG_NUM_IOC_BASE = 0x18; // io control
const uint8_t REG_NUM_MSK_BASE = 0x20; // mask interrupt
const uint8_t CMD_AUTO_INCREMENT_BIT = (1 << 7);

/*
    Pin mapping scheme
    pin numbers the Altium Pn_m pins 

    PORT1 and PORT2 have their own mappings, and then there is another map that is between the two.

    what it does:
    given a pin number from 0 to 44: (this number is the PORT1 pin)
    has functions that can set the pins on port1, and read the coresponding one on port2.
*/
const uint16_t BIT_DIRECT_GPIO = (1 << 15);

// if direct gpio, then number is pin number.
// if not direct, then number is the PCA9505 pin number after flattening all the IO registers
const uint16_t PORT1_PINMAPPING[] = {
    IO(4,7), // P1_1
    IO(0,0), // P1_2
    IO(4,6), // P1_3
    IO(0,1), // P1_4
    IO(4,5), // P1_5
    IO(0,2), // P1_6
    IO(4,4), // P1_7
    IO(0,3), // P1_8
    IO(4,3), // P1_9
    IO(0,4), // P1_10
    IO(4,2), // P1_11
    IO(0,5), // P1_12
    IO(4,1), // P1_13
    IO(0,6), // P1_14
    IO(4,0), // P1_15
    IO(0,7), // P1_16
    IO(3,7), // P1_17
    IO(1,0), // P1_18
    IO(3,6), // P1_19
    IO(1,1), // P1_20
    IO(3,5), // P1_21
    IO(1,2), // P1_22
    IO(3,4), // P1_23
    IO(1,3), // P1_24
    IO(3,3), // P1_25
    IO(1,4), // P1_26
    IO(3,2), // P1_27
    IO(1,5), // P1_28
    IO(3,1), // P1_29
    IO(1,6), // P1_30
    IO(3,0), // P1_31
    IO(1,7), // P1_32
    IO(2,7), // P1_33
    IO(2,0), // P1_34
    IO(2,6), // P1_35
    IO(2,1), // P1_36
    IO(2,5), // P1_37
    IO(2,2), // P1_38
    IO(2,4), // P1_39
    IO(2,3), // P1_40
    11  | BIT_DIRECT_GPIO, // P1_41
    7   | BIT_DIRECT_GPIO, // P1_42
    10  | BIT_DIRECT_GPIO, // P1_43
    8   | BIT_DIRECT_GPIO, // P1_44
    9   | BIT_DIRECT_GPIO  // P1_45
};

const uint16_t PORT2_PINMAPPING[] = {
    IO(4,7), // P2_1
    IO(0,0), // P2_2
    IO(4,6), // P2_3
    IO(0,1), // P2_4
    IO(4,5), // P2_5
    IO(0,2), // P2_6
    IO(4,4), // P2_7
    IO(0,3), // P2_8
    IO(4,3), // P2_9
    IO(0,4), // P2_10
    IO(4,2), // P2_11
    IO(0,5), // P2_12
    IO(4,1), // P2_13
    IO(0,6), // P2_14
    IO(4,0), // P2_15
    IO(0,7), // P2_16
    IO(3,7), // P2_17
    IO(1,0), // P2_18
    IO(3,6), // P2_19
    IO(1,1), // P2_20
    IO(3,5), // P2_21
    IO(1,2), // P2_22
    IO(3,4), // P2_23
    IO(1,3), // P2_24
    IO(3,3), // P2_25
    IO(1,4), // P2_26
    IO(3,2), // P2_27
    IO(1,5), // P2_28
    IO(3,1), // P2_29
    IO(1,6), // P2_30
    IO(3,0), // P2_31
    IO(1,7), // P2_32
    IO(2,7), // P2_33
    IO(2,0), // P2_34
    IO(2,6), // P2_35
    IO(2,1), // P2_36
    IO(2,5), // P2_37
    IO(2,2), // P2_38
    IO(2,4), // P2_39
    IO(2,3), // P2_40
    6   | BIT_DIRECT_GPIO, // P2_41
    2   | BIT_DIRECT_GPIO, // P2_42
    5   | BIT_DIRECT_GPIO, // P2_43
    3   | BIT_DIRECT_GPIO, // P2_44
    4   | BIT_DIRECT_GPIO  // P2_45
};

bool read_from_port(uint pin, uint8_t port_num) {
    uint16_t mapping;
    uint8_t addr;
    if (port_num == 1) {
        mapping = PORT1_PINMAPPING[pin];
        addr = PORT1_PCA9505_ADDR;
    }
        
    else if (port_num == 2) {
        mapping = PORT2_PINMAPPING[pin];
        addr = PORT2_PCA9505_ADDR;
    }  

    uint16_t pin_number = mapping & ~BIT_DIRECT_GPIO;
    bool is_direct = mapping & BIT_DIRECT_GPIO;
    if (is_direct) {
        return gpio_get(pin_number);
    } else {
        // must read from PCA9505
        // timing is not important -- just read all 5 registers for convenience
        uint8_t command_reg = (REG_NUM_IP_BASE & 0b00111111) | (CMD_AUTO_INCREMENT_BIT);
        uint8_t data[5];
        i2c_write_blocking(I2C_PORT, addr, &command_reg, 1, true);
        i2c_read_blocking(I2C_PORT, addr, data, 5, false);

        return data[pin_number / 8] & (1 << (pin_number % 8));
    }
}
uint64_t read_all_from_port(uint8_t port_num) {
    uint16_t* mapping_ptr;
    uint8_t addr;
    if (port_num == 1) {
        mapping_ptr = PORT1_PINMAPPING;
        addr = PORT1_PCA9505_ADDR;
    }
        
    else if (port_num == 2) {
        mapping_ptr = PORT2_PINMAPPING;
        addr = PORT2_PCA9505_ADDR;
    }  
    // read from PCA9505
    uint8_t command_reg = (REG_NUM_IP_BASE & 0b00111111) | (CMD_AUTO_INCREMENT_BIT);
    uint8_t data[5];
    i2c_write_blocking(I2C_PORT, addr, &command_reg, 1, true);
    i2c_read_blocking(I2C_PORT, addr, data, 5, false);


    uint64_t result = 0; 

    for (int i = 0; i < 45; i++) {
        uint16_t mapping = mapping_ptr[i];
        uint16_t pin_number = mapping & ~BIT_DIRECT_GPIO;
        bool is_direct = mapping & BIT_DIRECT_GPIO;

        if (!is_direct) {
            result |= (uint64_t)((bool)(data[pin_number / 8] & (1 << (pin_number % 8)))) << i;
        }
        else {
            result |= (uint64_t)((bool)gpio_get(pin_number)) << i;
        }
    }
    return result;
}
void write_to_port(uint pin, uint8_t port_num, bool state) {
    uint16_t mapping;
    uint8_t addr;
    if (port_num == 1) {
        mapping = PORT1_PINMAPPING[pin];
        addr = PORT1_PCA9505_ADDR;
    }
        
    else if (port_num == 2) {
        mapping = PORT2_PINMAPPING[pin];
        addr = PORT2_PCA9505_ADDR;
    } 


    uint16_t pin_number = mapping & ~BIT_DIRECT_GPIO;
    // should probably check pin_number bounds
    bool is_direct = mapping & BIT_DIRECT_GPIO;
    if (is_direct) {
        gpio_set_dir(pin_number, GPIO_OUT);
        gpio_put(pin_number, state);
    } else {
        uint8_t command_reg = (REG_NUM_IOC_BASE & 0b00111111) | (CMD_AUTO_INCREMENT_BIT);
        uint8_t data[6];
        // first, set that specific pin to low impedance
        i2c_write_blocking(I2C_PORT, addr, &command_reg, 1, true);
        i2c_read_blocking(I2C_PORT, addr, &data[1], 5, false);

        // low Z
        data[pin_number / 8 + 1] &= ~(1 << (pin_number % 8));
        data[0] = command_reg;

        // dont have two separate writes for cmd and data, so put cmd in data[0]
        i2c_write_blocking(I2C_PORT, addr, data, 6, false);

        // then set the output port value
        command_reg = (REG_NUM_OP_BASE & 0b00111111) | (CMD_AUTO_INCREMENT_BIT);

        i2c_write_blocking(I2C_PORT, addr, &command_reg, 1, true);
        i2c_read_blocking(I2C_PORT, addr, &data[1], 5, false);

        if (state) {
            data[pin_number / 8 + 1] |= (1 << (pin_number % 8));
        }
        else {
            data[pin_number / 8 + 1] &= ~(1 << (pin_number % 8));
        }
        data[0] = command_reg;
        i2c_write_blocking(I2C_PORT, addr, data, 6, false);
    }
}

int pca9505_set_pins_hi_z(uint8_t port_num) {
    uint8_t addr;
    if (port_num == 1)
        addr = PORT1_PCA9505_ADDR;
    else if (port_num == 2)
        addr = PORT2_PCA9505_ADDR;
    // set up PCA9505 gpios as inputs, pulled up
    uint8_t command_reg = (REG_NUM_IOC_BASE & 0b00111111) | (CMD_AUTO_INCREMENT_BIT);
    uint8_t data[] = {
        command_reg,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    
    // then, set the direct GPIOs high Z too
    for (int i = 0; i < 45; i++) {
        uint16_t mapping;
        if (port_num == 1)
            mapping = PORT1_PINMAPPING[i];
        else if (port_num == 2)
            mapping = PORT2_PINMAPPING[i];
        
        bool is_direct = mapping & BIT_DIRECT_GPIO;
        uint16_t pin_number = mapping & ~BIT_DIRECT_GPIO;

        if (is_direct) {
            gpio_set_dir(pin_number, GPIO_IN);
            gpio_pull_up(pin_number);
        }
    }

    return i2c_write_blocking(I2C_PORT, addr, data, 6, false);
}

// int pca9505_set_single_pin_state(uint8_t addr, int pin, bool state) {
//     // expect this to completely be wrong pin
//     int bank = pin / 8; // floor(pin / 8) = location of pin (banks)
//     uint8_t command_reg = (REG_NUM_IOC_BASE & 0b00111111) + bank;
//     int bit = pin - bank * 8;
//     uint8_t pin_data = (1 << bit);
//     uint8_t data[] = {command_reg, pin_data};
//     return i2c_write_blocking(I2C_PORT, addr, data, 2, false);
// }

void pca9505_init_i2c() {
    // pull the reset line high first 
    gpio_init(PCA9505_RST);
    gpio_set_dir(PCA9505_RST, GPIO_OUT);
    gpio_put(PCA9505_RST, 1);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 100*1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    for (int i = 0; i < 45; i++) {
        uint16_t mapping = PORT1_PINMAPPING[i];
        bool is_direct = mapping & BIT_DIRECT_GPIO;
        if (is_direct) {
            uint16_t pin_number = mapping & ~BIT_DIRECT_GPIO;
            gpio_init(pin_number);
        }
    }
    for (int i = 0; i < 45; i++) {
        uint16_t mapping = PORT2_PINMAPPING[i];
        bool is_direct = mapping & BIT_DIRECT_GPIO;
        if (is_direct) {
            uint16_t pin_number = mapping & ~BIT_DIRECT_GPIO;
            gpio_init(pin_number);
        }
    }
    pca9505_set_pins_hi_z(1);
    pca9505_set_pins_hi_z(2);
    // assert OE
    gpio_init(PCA9505_OE);
    gpio_set_dir(PCA9505_OE, GPIO_OUT);
    gpio_put(PCA9505_OE, 0);
}

void pca9505_is_alive_questionmark() {
    int val1 = pca9505_set_pins_hi_z(1);
    int val2 = pca9505_set_pins_hi_z(2);
    printf("Testing PCA9505s\n");
    if (val1 > 0 && val2 > 0) {
        printf("both PCA9505 ACK\n");
    }
    else {
        
        if (val1 > 0) printf("only PORT1 ACK\n");
        else if (val2 > 0) printf("only PORT2 ACK\n");
        else printf("neither responded.. gg bro\n");
    }
}
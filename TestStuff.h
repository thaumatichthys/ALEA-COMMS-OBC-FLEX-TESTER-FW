#pragma once 
#include "pico/stdlib.h"
#include "stdio.h"


bool read_from_port(uint pin, uint8_t port_num);
uint64_t read_all_from_port(uint8_t port_num);

void write_to_port(uint pin, uint8_t port_num, bool state);


int pca9505_set_pins_hi_z(uint8_t addr);
//int pca9505_set_single_pin_state(uint8_t addr, int pin, bool state);
void pca9505_init_i2c();
void pca9505_is_alive_questionmark();

#pragma once 
#include "pico/stdlib.h"
#include "stdio.h"

bool read_port1(uint pin);
bool read_port2(uint pin);
bool read_port_generic(uint16_t mapping);
void write_port1(uint pin, bool state);
void write_port2(uint pin, bool state);
void write_port_generic(uint16_t mapping, bool state);
int pca9505_set_pins_hi_z(uint8_t addr);
int pca9505_set_single_pin_state(uint8_t addr, int pin, bool state);
void pca9505_init_i2c();
void pca9505_is_alive_questionmark();

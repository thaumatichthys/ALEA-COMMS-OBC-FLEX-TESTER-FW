#pragma once 
#include "pico/stdlib.h"

#define NUM_DIGITS 4

struct SevSeg4Cfg {
    // uint pin_A;
    // uint pin_B;
    // uint pin_C;
    // uint pin_D;
    // uint pin_E;
    // uint pin_F;
    // uint pin_G;

    // uint pin_com_1;
    // uint pin_com_2;
    // uint pin_com_3;
    // uint pin_com_4;
    /*
        FORMAT: 
        segment_pins[8] is an array that stores the segment pins.
        segment_pins[8] = { pin_A, pin_B, pin_C, pin_D, pin_E, pin_F, pin_G, pin_DP };

        number_of_segments_used is for when some segments are not used such as decimal point. Only supports the last ones unused (can't have pin_A then pin_C)
    
        com_pins is same concept as segment pins, but for the common pins.

        is_common_cathode: set to true when using common cathode, and false for common anode
    */
    uint segment_pins[8]; // this is bc 8 bits in a byte so not gunna make this arbitrary
    uint number_of_segments_used; // this is, for example, if you don't have a decimal point on your display so you use only 7 segments instead of all 8
    uint com_pins[NUM_DIGITS];
    
    bool is_common_cathode;
};

class SevSeg4 {
    int state_ = 0;
    SevSeg4Cfg cfg_;
    void SetGPIO(uint pin, bool state); // these two are for portability reasons
    void InitGPIOOutput(uint pin);

    bool seg_active_state_;
    bool com_active_state_;
public:
    /*
        frame buffer stores NUM_DIGITS number of 8 bit uint8s. Each element is an 8 element bitfield for the segments.
        The ordering is as follows:
         |------ UINT8_T -----|
         7  6  5  4  3  2  1  0
        DP, G, F, E, D, C, B, A
        MSB                 LSB
    */
    uint8_t frame_buffer_[NUM_DIGITS];
    SevSeg4(SevSeg4Cfg cfg);
    void UpdateFSM();
};

#include "SevSeg.h"
#include "hardware/gpio.h"


SevSeg4::SevSeg4(SevSeg4Cfg cfg) {
    cfg_ = cfg;

    if (cfg_.is_common_cathode) {
        seg_active_state_ = 1;
        com_active_state_ = 0;
    }
    else {
        seg_active_state_ = 0;
        com_active_state_ = 1;
    }

    for (int i = 0; i < cfg_.number_of_segments_used; i++) {
        InitGPIOOutput(cfg_.segment_pins[i]);
        SetGPIO(cfg_.segment_pins[i], !seg_active_state_);
    }
    for (int i = 0; i < NUM_DIGITS; i++) {
        InitGPIOOutput(cfg_.com_pins[i]);
        SetGPIO(cfg_.com_pins[i], !com_active_state_);
    }
}

void SetGPIO(uint pin, bool state) {
    gpio_put(pin, state);
}
void InitGPIOOutput(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
}

void SevSeg4::UpdateFSM() {
    // de-activate current common pin
    SetGPIO(cfg_.com_pins[state_], !com_active_state_);

    state_++;
    if (state_ >= NUM_DIGITS) state_ = 0;

    // update segment pins
    for (int i = 0; i < cfg_.number_of_segments_used; i++) {
        bool is_active = frame_buffer_[state_] & (1 << i);
        SetGPIO(cfg_.segment_pins[i], is_active == seg_active_state_);
    }

    // activate new common pin
    SetGPIO(cfg_.com_pins[state_], com_active_state_);
}
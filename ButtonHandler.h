#pragma once 
#include "pico/stdlib.h"


struct ButtonHandlerObj {
    uint32_t previous_activation_ms_;
    uint32_t held_start_ms_; 
    int hold_time_ms_; // beyond this, it is registered as a hold
    void (*regular_callback_) (int);
    void (*hold_callback_) (int);
    uint pin_num_;
    int debounce_ms_;
    bool previous_active_;
    bool button_active_state_;
    bool activated_hold_;
};

void InitButton(struct ButtonHandlerObj* obj, uint pin, bool active_state, void (*regular_cb) (int), void (*hold_cb) (int));
void UpdateButton(struct ButtonHandlerObj* obj);


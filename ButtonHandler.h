#pragma once 
#include "pico/stdlib.h"


struct ButtonHandlerObj {
    uint32_t previous_activation_ms_;
    void (*callback_) (int);
    uint pin_num_;
    int debounce_ms_;
    bool previous_active_;
    bool button_active_state_;
};

void InitButton(struct ButtonHandlerObj* obj, uint pin, bool active_state, void (*cb) (int));
void UpdateButton(struct ButtonHandlerObj* obj);


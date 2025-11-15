#include "ButtonHandler.h"



void InitButton(struct ButtonHandlerObj* obj, uint pin, bool active_state, void (*cb) (int)) {
    obj->previous_activation_ms_ = 0;
    obj->previous_active_ = false;
    obj->callback_ = cb;
    obj->pin_num_ = pin;
    obj->button_active_state_ = active_state;
    obj->debounce_ms_ = 50;

    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
}

void UpdateButton(struct ButtonHandlerObj* obj) {
    uint32_t time = to_ms_since_boot(get_absolute_time());
    bool current_val = gpio_get(obj->pin_num_);
    
    if (current_val != obj->button_active_state_) {
        // button released or not pressed
        if (obj->previous_activation_ms_ + obj->debounce_ms_ < time && obj->previous_active_) {
            obj->previous_activation_ms_ = time;
            // the point of putting pin num as an arg is to be able to reuse the callback function for multiple buttons
            obj->callback_(obj->pin_num_); 
        }
        obj->previous_active_ = false;
    }
    else {
        // button is held 
        obj->previous_active_ = true;
    }
}

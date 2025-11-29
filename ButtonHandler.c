#include "ButtonHandler.h"


void InitButton(struct ButtonHandlerObj* obj, uint pin, bool active_state, void (*regular_cb) (int), void (*hold_cb) (int)) {
    obj->previous_active_ = false;
    obj->activated_hold_ = false;
    obj->regular_callback_ = regular_cb;
    obj->hold_callback_ = hold_cb;
    obj->pin_num_ = pin;
    obj->button_active_state_ = active_state;
    obj->debounce_ms_ = 50;
    obj->held_start_ms_ = 0;
    obj->hold_time_ms_ = 1000;

    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
}


void UpdateButton(struct ButtonHandlerObj* obj) {
    uint32_t time = to_ms_since_boot(get_absolute_time());
    bool active_now = (gpio_get(obj->pin_num_) == obj->button_active_state_);

    if (active_now) {
        // Button is currently being held
        if (!obj->previous_active_) {
            // Just pressed now
            obj->held_start_ms_ = time;
            obj->activated_hold_ = false;
        } else {
            // Still held — check for long hold
            if (!obj->activated_hold_ &&
                (time - obj->held_start_ms_) >= obj->hold_time_ms_)
            {
                obj->hold_callback_(obj->pin_num_);
                obj->activated_hold_ = true;
            }
        }

        obj->previous_active_ = true;
    } 
    else {
        // Button is released
        if (obj->previous_active_) {
            // It WAS pressed before and just got released now
            if (!obj->activated_hold_) {
                // Only call regular if hold callback wasn't triggered
                obj->regular_callback_(obj->pin_num_);
            }
        }

        obj->previous_active_ = false;
        obj->activated_hold_ = false;
    }
}
/*
void UpdateButton(struct ButtonHandlerObj* obj) {
    uint32_t time = to_ms_since_boot(get_absolute_time());
    bool current_val = gpio_get(obj->pin_num_);
    
    if (current_val != obj->button_active_state_) {
        // button released or not pressed
        if (obj->previous_activation_ms_ + obj->debounce_ms_ < time && obj->previous_active_) {
            obj->previous_activation_ms_ = time;
            if (obj->held_start_ms_ + obj->hold_time_ms_ < time) {
                // the point of putting pin num as an arg is to be able to reuse the callback function for multiple buttons
                obj->hold_callback_(obj->pin_num_); 
            }
            else {
                obj->regular_callback_(obj->pin_num_); 
            } 
        }
        obj->previous_active_ = false;
        obj->held_start_ms_ = time;
    }
    else {
        // button is held 
        obj->previous_active_ = true;
    }
}
*/
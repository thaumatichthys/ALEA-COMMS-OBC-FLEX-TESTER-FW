#include "SevSeg.h"
#include "hardware/gpio.h"


void SevSeg_InitGPIOs(struct SevSeg4Obj* obj) {
    if (obj->is_common_cathode) {
        obj->seg_active_state_ = 1;
        obj->com_active_state_ = 0;
    }
    else {
        obj->seg_active_state_ = 0;
        obj->com_active_state_ = 1;
    }

    for (int i = 0; i < obj->number_of_segments_used; i++) {
        SevSeg_InitGPIOOutput(obj->segment_pins[i]);
        SevSeg_SetGPIO(obj->segment_pins[i], !obj->seg_active_state_);
    }
    for (int i = 0; i < NUM_DIGITS; i++) {
        SevSeg_InitGPIOOutput(obj->com_pins[i]);
        SevSeg_SetGPIO(obj->com_pins[i], !obj->com_active_state_);
    }
}

void SevSeg_SetGPIO(uint pin, bool state) {
    gpio_put(pin, state);
}
void SevSeg_InitGPIOOutput(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
}

void SevSeg_UpdateFSM(struct SevSeg4Obj* obj) {
    // de-activate current common pin
    SevSeg_SetGPIO(obj->com_pins[obj->state_], !obj->com_active_state_);

    obj->state_++;
    if (obj->state_ >= NUM_DIGITS) obj->state_ = 0;

    // update segment pins
    for (int i = 0; i < obj->number_of_segments_used; i++) {
        bool is_active = obj->frame_buffer_[obj->state_] & (1 << i);
        SevSeg_SetGPIO(obj->segment_pins[i], is_active == obj->seg_active_state_);
    }

    // activate new common pin
    SevSeg_SetGPIO(obj->com_pins[obj->state_], obj->com_active_state_);


    /////////////////////////////////////////////////////////////////////////////////////////////

}
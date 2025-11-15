#include "stdio.h" // for printf
#include "stdint.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "SevSeg.h"
#include "config.h"
#include "TestStuff.h"
#include "ButtonHandler.h"



struct repeating_timer seven_seg_timer;
struct repeating_timer button_poll_timer;

struct SevSeg4Obj sevseg;
struct ButtonHandlerObj button;


// uint64_t read_all_pins(uint8_t addr) {
//     // expect this to have orders messed up (havent tested + turned brain off)
//     uint8_t command_reg = (REG_NUM_IOC_BASE & 0b00111111) | (CMD_AUTO_INCREMENT_BIT);
//     uint8_t data[5];
//     i2c_write_blocking(I2C_PORT, addr, &command_reg, 1, true);
//     i2c_read_blocking(I2C_PORT, addr, data, 5, false);

//     uint64_t ret = 0;
//     for (int i = 0; i < 5; i++) {
//         ret |= (uint64_t) data[i] << (8 * i);
//     }
//     return ret;
// }

bool SevenSegment_timer_ISR(struct repeating_timer *t) {
    SevSeg_UpdateFSM(&sevseg);
    return true;
}

void Button_callback(int pin) {
    printf("button was presseed\n");

    pca9505_set_pins_hi_z(1);
    pca9505_set_pins_hi_z(2);
    
    write_port1(41, 0);
    write_port1(44, 0);
    

    for (int i = 0; i < 45; i++) {
        bool state = read_port2(i);
        
        printf("%d: %d\n", i, state);

        //sleep_ms(10);
    }
}

bool Button_poll_ISR() {
    UpdateButton(&button);
    return true;
}

int main() {
    
    stdio_init_all(); // for printf

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    pca9505_init_i2c();

    // init seven segment display
    sevseg.com_pins[0] = DISP_COM_1;
    sevseg.com_pins[1] = DISP_COM_2;
    sevseg.com_pins[2] = DISP_COM_3;
    sevseg.com_pins[3] = DISP_COM_4;
    sevseg.segment_pins[0] = DISP_SEG_A;
    sevseg.segment_pins[1] = DISP_SEG_B;
    sevseg.segment_pins[2] = DISP_SEG_C;
    sevseg.segment_pins[3] = DISP_SEG_D;
    sevseg.segment_pins[4] = DISP_SEG_E;
    sevseg.segment_pins[5] = DISP_SEG_F;
    sevseg.segment_pins[6] = DISP_SEG_G;
    sevseg.number_of_segments_used = 7;
    sevseg.is_common_cathode = true;

    InitButton(&button, BUTTON_PIN, 0, Button_callback);

    SevSeg_InitGPIOs(&sevseg);
    

    add_repeating_timer_us(500, SevenSegment_timer_ISR, NULL, &seven_seg_timer);
    //add_repeating_timer_us(1000, Button_poll_ISR, NULL, &button_poll_timer);




    int dummy = 0; 
    while(1) {
        gpio_put(25, 1);
        sleep_ms(2);
        gpio_put(25, 0);
        sleep_ms(2);
        //pca9505_is_alive_questionmark();
        Button_poll_ISR();

        uint8_t thing = SEG7_LUT[dummy++];
        if (dummy >= 10) dummy = 0;

        for (int i = 0; i < 4; i++) {
            sevseg.frame_buffer_[i] = thing;
        }
    }
}
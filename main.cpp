#include "stdio.h" // for printf
#include "pico/stdlib.h"
#include "hardware/gpio.h"

/*
    In case of build issues after copying the template, delete the entire build directory and in VSCode:
    Restart VSCode, click on "Terminal" on the top bar, and select GCC 10.2.1 arm-none-eabi

    Overclocking: https://youtu.be/G2BuoFNLoDM (The "catch" is already fixed in CMakeLists.txt, uncomment the three lines at the bottom)
    PIO: https://youtu.be/JSis2NU65w8
    Explaining PIO ASM instructions: https://youtu.be/yYnQYF_Xa8g
*/


int main() {
    
    // stdio_init_all(); // for printf

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    while(1) {
        gpio_put(25, 1);
        sleep_ms(200);
        gpio_put(25, 0);
        sleep_ms(200);
    }
}
#pragma once 


#define DISP_COM_1      15 // 22
#define DISP_COM_2      16 // 26
#define DISP_COM_3      19 // 27
#define DISP_COM_4      20 // 28

#define DISP_SEG_A      26 // 15
#define DISP_SEG_B      21 // 16
#define DISP_SEG_C      27 // 17
#define DISP_SEG_D      17 // 18
#define DISP_SEG_E      18 // 19
#define DISP_SEG_F      28 // 20
#define DISP_SEG_G      22 // 21

#define BUTTON_PIN      13

#define COMMON_CATHODE  true
#define PCA9505_RST     14
#define PCA9505_OE      12

#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1

#define PORT1_PCA9505_ADDR ((0b0100000 | (0b001)))
#define PORT2_PCA9505_ADDR ((0b0100000 | (0b000)))

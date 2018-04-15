#ifndef PINS_H
#define PINS_H

#include <avr/io.h>

#define CONCAT(x,y)         x ## y

#define DDR(x)              CONCAT(DDR,x)
#define PORT(x)             CONCAT(PORT,x)
#define PIN(x)              CONCAT(PIN,x)

#define OUT(x)              (DDR(x) |= x ## _LINE)
#define IN(x)               (DDR(x) &= ~x ## _LINE)
#define SET(x)              (PORT(x) |= x ## _LINE)
#define CLR(x)              (PORT(x) &= ~x ## _LINE)
#define READ(x)             (PIN(x) & x ## _LINE)

// KS0066 Data port
#define KS0066_D4           D
#define KS0066_D4_LINE      (1<<7)
#define KS0066_D5           B
#define KS0066_D5_LINE      (1<<0)
#define KS0066_D6           B
#define KS0066_D6_LINE      (1<<1)
#define KS0066_D7           B
#define KS0066_D7_LINE      (1<<2)
// KS0066 control port
#define KS0066_RS           D
#define KS0066_RS_LINE      (1<<5)
#define KS0066_E            D
#define KS0066_E_LINE       (1<<6)

// Backlight port
#define BCKL                C
#define BCKL_LINE           (1<<3)

// RC5 definitions
#define RC                  D
#define RC_LINE             (1<<3)

// I2C port definitions
#define I2C_SDA             C
#define I2C_SDA_LINE        (1<<4)
#define I2C_SCL             C
#define I2C_SCL_LINE        (1<<5)

// AVR ISP definitions
#define ISP_MOSI            B
#define ISP_MOSI_LINE       (1<<3)
#define ISP_SCK             B
#define ISP_SCK_LINE        (1<<5)
#define ISP_MISO            B
#define ISP_MISO_LINE       (1<<4)

// EXT outputs definitions
#define EXT_0               ISP_MOSI
#define EXT_0_LINE          ISP_MOSI_LINE
#define EXT_1               ISP_SCK
#define EXT_1_LINE          ISP_SCK_LINE
#define EXT_2               ISP_MISO
#define EXT_2_LINE          ISP_MISO_LINE

// Software SPI definitions
#define SPISW_DI            ISP_MOSI
#define SPISW_DI_LINE       ISP_MOSI_LINE
#define SPISW_CLK           ISP_SCK
#define SPISW_CLK_LINE      ISP_SCK_LINE
#define SPISW_CE            ISP_MISO
#define SPISW_CE_LINE       ISP_MISO_LINE

// SI470X Reset pins
#define SI470X_B_SDIO       I2C_SDA
#define SI470X_B_SDIO_LINE  I2C_SDA_LINE
#define SI470X_B_SCLK       I2C_SCL
#define SI470X_B_SCLK_LINE  I2C_SCL_LINE
#define SI470X_B_RST        EXT_2
#define SI470X_B_RST_LINE   EXT_2_LINE

// Standby/Mute port definitions
#define STMU_STBY           B
#define STMU_STBY_LINE      (1<<7)

// Encoder definitions
#define ENCODER_A           C
#define ENCODER_A_LINE      (1<<0)
#define ENCODER_B           C
#define ENCODER_B_LINE      (1<<1)

// Buttons definitions
#define BUTTON_1            B
#define BUTTON_1_LINE       (1<<6)
#define BUTTON_2            D
#define BUTTON_2_LINE       (1<<4)
#define BUTTON_3            D
#define BUTTON_3_LINE       (1<<2)
#define BUTTON_4            D
#define BUTTON_4_LINE       (1<<1)
#define BUTTON_5            D
#define BUTTON_5_LINE       (1<<0)

#endif // PINS_H

#!/bin/sh

for DISPLAY in KS0108A KS0108B ST7920 SSD1306 SH1106 KS0066_16X2 KS0066_16X2_PCF8574 LS020
do
	make clean
	make -j4 MCU=atmega32 DISPLAY=${DISPLAY}
done

for DISPLAY in KS0108A ST7920 SSD1306 SH1106
do
	make clean
	make -j4 MCU=atmega328p DISPLAY=${DISPLAY}
done

make clean

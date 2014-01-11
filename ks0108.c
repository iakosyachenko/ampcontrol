#include "ks0108.h"
#include "font.h"

#include <util/delay.h>

static uint8_t column = 0;	/* Current column (0..128) */
static uint8_t row = 0;		/* Current row (0..8) */
static uint8_t dig[6];		/* Array for num->string convert */

void gdWriteData(uint8_t data, uint8_t cs)
{
	/* Wait for controller is ready */
	_delay_us(10);
	/* Set data byte */
	GD_DPORT = data;
	/* Select controller, data mode and perform strobe */
	GD_CPORT |= (cs | GD_DI | GD_E);
	/* Unselect controller */
	GD_CPORT &= ~GD_CTRL;
	column++;
	return;
}

void gdWriteCommand(uint8_t command, uint8_t cs)
{
	/* Wait for controller is ready */
	_delay_us(10);
	/* Set command byte */
	GD_DPORT = command;
	/* Select controller and perform strobe */
	GD_CPORT |= cs | GD_E;
	GD_CPORT &= ~GD_CTRL;
	return;
}

void gdFill(uint8_t data, uint8_t cs)
{
	uint8_t i, j;
	gdWriteCommand(KS0108_SET_ADDRESS, cs);
	for (i = 0; i < GD_ROWS; i++) {
		gdWriteCommand(KS0108_SET_PAGE + i, cs);
		for (j = 0; j < GD_COLS; j++)
			gdWriteData(data, cs);
	}
	return;
}

void gdInit(void)
{
	/* Set data and control lines as outputs */
	GD_DDDR = 0xFF;
	GD_CDDR |= GD_CTRL;
	/* Unselect controller */
	GD_CPORT &= ~GD_CTRL;
	/* Reset */
	_delay_us(10);
	GD_CPORT |= GD_RES;
	/* Clear display  and reset addresses */
	gdFill(0x00, GD_CS1 | GD_CS2);
	gdWriteCommand(KS0108_DISPLAY_ON, GD_CS1 | GD_CS2);
	gdWriteCommand(KS0108_DISPLAY_START_LINE, GD_CS1 | GD_CS2);
	return;
}

int8_t gdSetXY(uint8_t x, uint8_t y)
{
	if (x >= (GD_COLS << 1))
		return 1;
	if (y >= GD_ROWS)
		return 1;
	if (x < GD_COLS) {
		gdWriteCommand(KS0108_SET_ADDRESS + x, GD_CS1);
		gdWriteCommand(KS0108_SET_ADDRESS, GD_CS2);
	} else {
		gdWriteCommand(KS0108_SET_ADDRESS, GD_CS1);
		gdWriteCommand(KS0108_SET_ADDRESS - GD_COLS + x, GD_CS2);
	}
	column = x;
	gdWriteCommand(KS0108_SET_PAGE + y, GD_CS1 | GD_CS2);
	row = y;
	return 0;
}

void gdWriteChar(uint8_t code, uint8_t inv)
{
	uint8_t cs;
	uint8_t i;
	uint16_t index;
	index = code * 5;
	uint8_t pgmData;

	for (i = 0; i < 6; i++) {
		if (column < (GD_COLS << 1)) {
			if (column < GD_COLS) {
				cs = GD_CS1;
			} else {
				cs = GD_CS2;
			}
			if (i == 5) {
				if (inv) {
					gdWriteData(0xFF, cs);
				} else {
					gdWriteData(0x00, cs);
				}
			} else {
				pgmData = pgm_read_byte(&k1013vg6_0[index + i]);
				if (pgmData != 0x5A) {
					if (inv) {
						gdWriteData(~pgmData, cs);
					} else {
						gdWriteData(pgmData, cs);
					}
				}
			}
		}
	}
	return;
}

void gdWriteString(uint8_t *string, uint8_t inv)
{
	while(*string)
		gdWriteChar(*string++, inv);
	return;
}

void gdWriteStringProgmem(const uint8_t *string, uint8_t inv)
{
	uint8_t i = 0, ch;
	do {
		ch = pgm_read_byte(&string[i++]);
		if (ch)
			gdWriteChar(ch, inv);
	} while (ch);
	return;
}

uint8_t *mkNumString(int16_t number, uint8_t width, uint8_t lead)
{
	uint8_t sign = lead;
	if (number < 0) {
		sign = '-';
		number = -number;
	}
	int8_t i;
	for (i = 0; i < width; i++)
		dig[i] = lead;
	dig[width] = '\0';
	i = width - 1;
	while (number > 0 || i == width - 1) {
		dig[i--] = number % 10 + 0x30;
		number /= 10;
	}
	if (i >= 0)
		dig[i] = sign;
	return dig;
}

void gdWriteCharScaled(uint8_t code, uint8_t scX, uint8_t scY, uint8_t inv)
{
	uint8_t cs;

	uint8_t i, j;
	uint16_t index;
	index = code * 5;

	uint8_t xpos = column;
	uint8_t ypos = row;

	uint8_t pgmData;
	uint8_t wrData;

	uint8_t bit;
	uint8_t shift = 0;

	for (j = 0; j < scY; j++) {
		gdSetXY(xpos, ypos + j);
		for (i = 0; i < 6 * scX; i++) {
			if (column < (GD_COLS << 1)) {
				if (column < GD_COLS) {
					cs = GD_CS1;
				} else {
					cs = GD_CS2;
				}
				if (i >= 5 * scX) {
					if (inv) {
						gdWriteData(0xFF, cs);
					} else {
						gdWriteData(0x00, cs);
					}
				} else {
					pgmData = pgm_read_byte(&k1013vg6_0[index + i / scX]);
					if (pgmData != 0x5A) {
						wrData = 0;

						for (bit = 0; bit < 8; bit++)
						{
							if (pgmData & (1 << ((shift+bit) / scY % 8)))
								wrData |= (1 << bit);
						}
						if (inv) {
							gdWriteData(~wrData, cs);
						} else {
							gdWriteData(wrData, cs);
						}
					}
				}
			}
		}
		shift += 8;
	}
	gdSetXY(column, ypos);
	return;
}

void gdWriteStringScaled(uint8_t *string, uint8_t scX, uint8_t scY, uint8_t inv)
{
	while(*string)
		gdWriteCharScaled(*string++, scX, scY, inv);
	return;
}

void gdWriteStringScaledProgmem(const uint8_t *string, uint8_t scX, uint8_t scY, uint8_t inv)
{
	uint8_t i = 0, ch;
	do {
		ch = pgm_read_byte(&string[i++]);
		if (ch)
			gdWriteCharScaled(ch, scX, scY, inv);
	} while (ch);
	return;
}

void gdWriteStringScaledEeprom(const uint8_t *string, uint8_t scX, uint8_t scY, uint8_t inv)
{
	uint8_t i = 0, ch;
	do {
		ch = eeprom_read_byte(&string[i++]);
		if (ch)
			gdWriteCharScaled(ch, scX, scY, inv);
	} while (ch);
	return;
}

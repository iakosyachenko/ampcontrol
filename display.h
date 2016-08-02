#ifndef DISPLAY_H
#define DISPLAY_H

#include <inttypes.h>

#include "audio/audio.h"
#include "display/ks0066.h"

// Timers for different screens (ms)
#define DISPLAY_TIME_TEST		15000U
#define DISPLAY_TIME_GAIN		2000U
#define DISPLAY_TIME_TIME		2000U
#define DISPLAY_TIME_TIME_EDIT	10000U
#define DISPLAY_TIME_FM_RADIO	5000U
#define DISPLAY_TIME_CHAN		2000U
#define DISPLAY_TIME_AUDIO		3000U
#define DISPLAY_TIME_SP			3000U
#define DISPLAY_TIME_BR			3000U

/* Backlight state */
#define BACKLIGHT_ON			1
#define BACKLIGHT_OFF			0

/* Radio tuning mode */
#define MODE_RADIO_TUNE			1
#define MODE_RADIO_CHAN			0

/* Data stored in user characters */
#define LCD_LEVELS				0
#define LCD_BAR					1

/* Display modes */
enum {
	MODE_SPECTRUM = MODE_SND_END,
	MODE_STANDBY,
	MODE_FM_RADIO,

	MODE_TIME,
	MODE_TIME_EDIT,
	MODE_MUTE,
	MODE_LOUDNESS,
	MODE_SURROUND,
	MODE_EFFECT_3D,
	MODE_TONE_DEFEAT,
	MODE_TEST,

	MODE_BR
};

/* Type of string printed (regular/eeprom/flash) */
#define STR_REG			0
#define STR_EEP			1
#define STR_PGM			2

#define STR_BUFSIZE		16

void showRC5Info(void);
void showRadio(uint8_t mode);
void showBoolParam(uint8_t value, uint8_t labelIndex);

void showBrWork(void);
void changeBrWork(int8_t diff);

void showSndParam(uint8_t mode);

void showTime(void);
void showSpectrum(void);

void setWorkBrightness(void);
void setStbyBrightness(void);

void displayInit(void);
void displayPowerOff(void);

#endif /* DISPLAY_H */

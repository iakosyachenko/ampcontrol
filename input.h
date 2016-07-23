#ifndef INPUT_H
#define INPUT_H

#include <inttypes.h>
#include "remote.h"

#define BTN_D0					0x01
#define BTN_D1					0x02
#define BTN_D2					0x04
#define BTN_D3					0x08
#define BTN_D4					0x10
#define BTN_D5					0x20
#define BTN_D6					0x40
#define BTN_D7					0x80

#define ENC_0					0x00
#define ENC_A					BTN_D6
#define ENC_B					BTN_D7
#define ENC_AB					(ENC_A | ENC_B)

typedef enum {
	CMD_RC_STBY,				// STBY
	CMD_RC_MUTE,				// MUTE
	CMD_RC_NEXT_SNDPAR,			// MENU
	CMD_RC_VOL_UP,				// VOL_UP
	CMD_RC_VOL_DOWN,			// VOL_DOWN
	CMD_RC_IN_0,				// RED
	CMD_RC_IN_1,				// GREEN
	CMD_RC_IN_2,				// YELLOW
	CMD_RC_IN_3,				// BLUE
	CMD_RC_IN_4,
	CMD_RC_IN_PREV,
	CMD_RC_IN_NEXT,				// NEXT
	CMD_RC_LOUDNESS,
	CMD_RC_SURROUND,
	CMD_RC_EFFECT_3D,
	CMD_RC_TONE_DEFEAT,

	CMD_RC_FM_RDS,				// TXT
	CMD_RC_FM_INC,				// CHAN_UP
	CMD_RC_FM_DEC,				// CHAN_DOWN
	CMD_RC_FM_MODE,				// UNIT
	CMD_RC_FM_MONO,				// ASPECT
	CMD_RC_FM_STORE,			// STORE
	CMD_RC_FM_0,				// 0
	CMD_RC_FM_1,				// 1
	CMD_RC_FM_2,				// 2
	CMD_RC_FM_3,				// 3
	CMD_RC_FM_4,				// 4
	CMD_RC_FM_5,				// 5
	CMD_RC_FM_6,				// 6
	CMD_RC_FM_7,				// 7
	CMD_RC_FM_8,				// 8
	CMD_RC_FM_9,				// 9

	CMD_RC_TIME,				// TIME
	CMD_RC_ALARM,				// QUESTION
	CMD_RC_TIMER,				// TIMER
	CMD_RC_BRIGHTNESS,			// PP
	CMD_RC_DEF_DISPLAY,			// TV
	CMD_RC_NEXT_SPMODE,			// AV
	CMD_RC_FALLSPEED,

	CMD_RC_END,

	CMD_BTN_1,
	CMD_BTN_2,
	CMD_BTN_3,
	CMD_BTN_4,
	CMD_BTN_5,
	CMD_BTN_1_LONG,
	CMD_BTN_2_LONG,
	CMD_BTN_3_LONG,
	CMD_BTN_4_LONG,
	CMD_BTN_5_LONG,
	CMD_BTN_12_LONG,
	CMD_BTN_13_LONG,

	CMD_END

} cmdID;

/* Handling long press actions */
#define SHORT_PRESS				100
#define LONG_PRESS				600
#define AUTOREPEAT				150

#define STBY_TIMER_OFF			-1
#define SILENCE_TIMER			180

#define TEMP_MEASURE_TIME		2
#define SENSOR_POLL_INTERVAL	5

void rcCodesInit(void);
void inputInit(void);

int8_t getEncoder(void);
cmdID getBtnCmd(void);

uint16_t getBtnBuf(void);
uint16_t getEncBuf(void);

void setDisplayTime(uint16_t value);
uint16_t getDisplayTime(void);

uint8_t getSensTimer(void);
void setSensTimer(uint8_t val);

int16_t getStbyTimer(void);
void setStbyTimer(int16_t val);

void setSecTimer(uint16_t val);
int16_t getSecTimer(void);

void setClockTimer(uint8_t value);
uint8_t getClockTimer(void);

void enableSilenceTimer(void);
void disableSilenceTimer(void);
int16_t getSilenceTimer(void);

#endif /* INPUT_H */

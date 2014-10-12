#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "eeprom.h"
#include "adc.h"
#include "input.h"
#include "rc5.h"
#include "i2c.h"

#include "audio/audio.h"
#include "display.h"
#include "tuner.h"

uint8_t *txtLabels[LABELS_COUNT];	/* Array with text label pointers */

/* Handle leaving standby mode */
static void powerOn(void)
{
	setWorkBrightness();
#if !defined(NOTUNER)
	loadTunerParams();
#endif
	unmuteVolume();

	return;
}

/* Handle entering standby mode */
static void powerOff(void)
{
	muteVolume();
	setStbyBrightness();
	stopEditTime();

	saveAudioParams();
	saveDisplayParams();
#if !defined(NOTUNER)
	saveTunerParams();
#endif

	return;
}

/* Load text labels from EEPROM */
static void loadLabels(uint8_t **txtLabels)
{
	uint8_t i;
	uint8_t *addr;

	addr = labelsAddr;
	i = 0;

	while (i < LABELS_COUNT && addr < (uint8_t*)EEPROM_SIZE) {
		if (eeprom_read_byte(addr) != '\0') {
			txtLabels[i] = addr;
			addr++;
			i++;
			while (eeprom_read_byte(addr) != '\0' &&
				   addr < (uint8_t*)EEPROM_SIZE) {
				addr++;
			}
		} else {
			addr++;
		}
	}

	return;
}

/* Hardware initialization */
static void hwInit(void)
{
	loadLabels(txtLabels);			/* Load text labels from EEPROM */

	I2CInit();						/* I2C bus */
	ks0066Init();

	rc5Init();						/* IR Remote control */
	adcInit();						/* Analog-to-digital converter */
	inputInit();					/* Buttons/encoder polling */
#if !defined(NOTUNER)
	tunerInit();					/* Tuner */
#endif

	STMU_DDR |= MUTE;				/* Standby/Mute port */
	BCKL_DDR |= BCKL;

	sei();							/* Gloabl interrupt enable */

	muteVolume();

	return;
}

int main(void)
{
	hwInit();

	uint8_t dispMode = MODE_STANDBY;
	uint8_t dispModePrev = dispMode;

	sndParam *curSndParam = sndParAddr(SND_VOLUME);

	int8_t encCnt = 0;
	uint8_t cmd = CMD_EMPTY;
	uint16_t rc5Buf = RC5_BUF_EMPTY;
	uint16_t rc5BufPrev = RC5_BUF_EMPTY;

#if !defined(NOTUNER)
	loadTunerParams();
#endif
	loadAudioParams(txtLabels);
	loadDispParams();
	setStbyBrightness();

	while (1) {
		encCnt = getEncoder();
		cmd = getBtnCmd();
		rc5Buf = getRC5Buf();

		/* Don't handle any command in test mode */
		if (dispMode == MODE_TEST) {
			if (cmd != CMD_EMPTY)
				setDisplayTime(DISPLAY_TIME_TEST);
			cmd = CMD_EMPTY;
		}

		/* Don't handle any command in standby mode except power on */
		if (dispMode == MODE_STANDBY) {
			if (cmd != CMD_BTN_1 && cmd != CMD_RC5_STBY &&
				cmd != CMD_BTN_TESTMODE)
				cmd = CMD_EMPTY;
		}

		/* Handle command */
		switch (cmd) {
		case CMD_BTN_1:
		case CMD_RC5_STBY:
			switch (dispMode) {
			case MODE_STANDBY:
				powerOn();
				dispMode = MODE_SPECTRUM;
				break;
			default:
				powerOff();
				dispMode = MODE_STANDBY;
				break;
			}
			break;
		case CMD_BTN_2:
		case CMD_RC5_NEXT_INPUT:
			switch (dispMode) {
			case MODE_GAIN:
				nextChan();
				ks0066Clear();
			default:
				curSndParam = sndParAddr(SND_GAIN0 + getChan());
				dispMode = MODE_GAIN;
				setDisplayTime(DISPLAY_TIME_GAIN);
				break;
			}
			break;
		case CMD_BTN_3:
		case CMD_RC5_TIME:
			switch (dispMode) {
			case MODE_TIME:
			case MODE_TIME_EDIT:
				editTime();
				dispMode = MODE_TIME_EDIT;
				setDisplayTime(DISPLAY_TIME_TIME_EDIT);
				if (!isETM())
					setDisplayTime(DISPLAY_TIME_TIME);
				break;
#if !defined(NOTUNER)
			case MODE_FM_RADIO:
				if (cmd == CMD_BTN_3) {
					tunerSetFreq(tunerGetFreq() - 10);
					setDisplayTime(DISPLAY_TIME_FM_RADIO);
					break;
				}
#endif
			default:
				stopEditTime();
				dispMode = MODE_TIME;
				setDisplayTime(DISPLAY_TIME_TIME);
				break;
			}
			break;
		case CMD_BTN_4:
		case CMD_RC5_MUTE:
			switch (dispMode) {
#if !defined(NOTUNER)
			case MODE_FM_RADIO:
				if (cmd == CMD_BTN_4) {
					tunerSetFreq(tunerGetFreq() + 10);
					setDisplayTime(DISPLAY_TIME_FM_RADIO);
					break;
				}
#endif
			default:
				ks0066Clear();
				switchMute();
				dispMode = MODE_MUTE;
				setDisplayTime(DISPLAY_TIME_CHAN);
				break;
			}
			break;
		case CMD_BTN_5:
		case CMD_RC5_MENU:
			if (dispMode >= MODE_VOLUME && dispMode < MODE_BALANCE) {
				curSndParam++;
				dispMode++;
			} else {
				curSndParam = sndParAddr(SND_VOLUME);
				dispMode = MODE_VOLUME;
			}
			setDisplayTime(DISPLAY_TIME_AUDIO);
			break;
		case CMD_BTN_1_LONG:
		case CMD_RC5_BACKLIGHT:
			dispMode = MODE_BR;
			setDisplayTime(DISPLAY_TIME_BR);
			break;
		case CMD_BTN_2_LONG:
		case CMD_RC5_DISPLAY:
			if (getChan() == 0) {
				setDisplayTime(DISPLAY_TIME_FM_RADIO);
				dispMode = MODE_FM_RADIO;
			}
			break;
		case CMD_BTN_3_LONG:
		case CMD_BTN_4_LONG:
		case CMD_BTN_5_LONG:
		case CMD_RC5_FM_STORE:
			if (dispMode == MODE_FM_RADIO) {
#if !defined(NOTUNER)
				if (cmd == CMD_BTN_3_LONG)
					scanStoredFreq(SEARCH_DOWN);
				else if (cmd == CMD_BTN_4_LONG)
					scanStoredFreq(SEARCH_UP);
				else
					storeStation();
				setDisplayTime(DISPLAY_TIME_FM_RADIO);
#endif
#if defined(TDA7313)
			} else if (cmd == CMD_BTN_4_LONG) {
				ks0066Clear();
				switchLoudness();
				dispMode = MODE_LOUDNESS;
				setDisplayTime(DISPLAY_TIME_AUDIO);
#endif
			}
			break;
		case CMD_BTN_TESTMODE:
			switch (dispMode) {
			case MODE_STANDBY:
				dispMode = MODE_TEST;
				setDisplayTime(DISPLAY_TIME_TEST);
				break;
			}
			break;
#if defined(TDA7313)
		case CMD_RC5_LOUDNESS:
			ks0066Clear();
			switchLoudness();
			dispMode = MODE_LOUDNESS;
			setDisplayTime(DISPLAY_TIME_AUDIO);
			break;
#endif
		case CMD_RC5_INPUT_0:
		case CMD_RC5_INPUT_1:
		case CMD_RC5_INPUT_2:
#if !defined(TDA7313)
		case CMD_RC5_INPUT_3:
#endif
			setChan(cmd - CMD_RC5_INPUT_0);
			ks0066Clear();
			curSndParam = sndParAddr(SND_GAIN0 + getChan());
			dispMode = MODE_GAIN;
			setDisplayTime(DISPLAY_TIME_GAIN);
			break;
#if !defined(NOTUNER)
		case CMD_RC5_FM_INC:
		case CMD_RC5_FM_DEC:
		case CMD_RC5_CHAN_UP:
		case CMD_RC5_CHAN_DOWN:
			setChan(0);
			if (dispMode == MODE_FM_RADIO) {
				switch (cmd) {
				case CMD_RC5_FM_INC:
					tunerSetFreq(tunerGetFreq() + 10);
					break;
				case CMD_RC5_FM_DEC:
					tunerSetFreq(tunerGetFreq() - 10);
					break;
				case CMD_RC5_CHAN_UP:
					scanStoredFreq(SEARCH_UP);
					break;
				case CMD_RC5_CHAN_DOWN:
					scanStoredFreq(SEARCH_DOWN);
					break;
				}
			}
			dispMode = MODE_FM_RADIO;
			setDisplayTime(DISPLAY_TIME_FM_RADIO);
			break;
		case CMD_RC5_FM_MONO:
			if (getChan() == 0) {
				tunerSwitchMono();
				dispMode = MODE_FM_RADIO;
				setDisplayTime(DISPLAY_TIME_FM_RADIO);
			}
			break;
		case CMD_RC5_1:
		case CMD_RC5_2:
		case CMD_RC5_3:
		case CMD_RC5_4:
		case CMD_RC5_5:
		case CMD_RC5_6:
		case CMD_RC5_7:
		case CMD_RC5_8:
		case CMD_RC5_9:
		case CMD_RC5_0:
			setChan(0);
			loadStation(cmd - CMD_RC5_1);
			dispMode = MODE_FM_RADIO;
			setDisplayTime(DISPLAY_TIME_FM_RADIO);
			break;
#endif
		}

		/* Emulate RC5 VOL_UP/VOL_DOWN as encoder actions */
		if (cmd == CMD_RC5_VOL_UP)
			encCnt++;
		if (cmd == CMD_RC5_VOL_DOWN)
			encCnt--;

		/* Handle encoder */
		if (encCnt) {
			switch (dispMode) {
			case MODE_STANDBY:
				break;
			case MODE_TEST:
				setDisplayTime(DISPLAY_TIME_TEST);
				break;
			case MODE_TIME_EDIT:
				changeTime(encCnt);
				setDisplayTime(DISPLAY_TIME_TIME_EDIT);
				break;
			case MODE_BR:
				changeBrWork(encCnt);
				setDisplayTime(DISPLAY_TIME_BR);
				break;
			case MODE_SPECTRUM:
			case MODE_TIME:
			case MODE_FM_RADIO:
				curSndParam = sndParAddr(SND_VOLUME);
				dispMode = MODE_VOLUME;
			default:
				changeParam(curSndParam, encCnt);
				setDisplayTime(DISPLAY_TIME_GAIN);
				break;
			}
		}

		/* Exit to default mode */
		if (getDisplayTime() == 0) {
			switch (dispMode) {
			case MODE_STANDBY:
				break;
			case MODE_TEST:
				setStbyBrightness();
				break;
			default:
				dispMode = MODE_SPECTRUM;
				break;
			}
		}

		/* Clear screen if mode has changed */
		if (dispMode != dispModePrev)
			ks0066Clear();

		/* Show things */
		switch (dispMode) {
		case MODE_STANDBY:
			showTime(txtLabels);
			if (dispModePrev == MODE_TEST)
				setStbyBrightness();
			break;
		case MODE_TEST:
			showRC5Info(rc5Buf);
			setWorkBrightness();
			if (rc5Buf != rc5BufPrev)
				setDisplayTime(DISPLAY_TIME_TEST);
			break;
		case MODE_SPECTRUM:
			drawSpectrum(getSpData());
			_delay_ms(20);
			break;
#if !defined(NOTUNER)
		case MODE_FM_RADIO:
			tunerReadStatus();
			showRadio();
			break;
#endif
		case MODE_MUTE:
			showBoolParam(getMute(), txtLabels[LABEL_MUTE], txtLabels);
			break;
#if defined(TDA7313)
		case MODE_LOUDNESS:
			showBoolParam(!getLoudness(), txtLabels[LABEL_LOUDNESS], txtLabels);
			break;
#endif
		case MODE_TIME:
		case MODE_TIME_EDIT:
			showTime(txtLabels);
			break;
		case MODE_BR:
			showBrWork(txtLabels);
			break;
		default:
			showSndParam(curSndParam, txtLabels);
			break;
		}

		/* Save current mode */
		dispModePrev = dispMode;
		/* Save current RC5 raw buf */
		rc5BufPrev = rc5Buf;
	}

	return 0;
}

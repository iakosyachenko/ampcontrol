#include "tuner.h"

#include <avr/eeprom.h>
#include "eeprom.h"

uint8_t bufFM[5];

#if defined(TUX032)
uint16_t freqFM;
#endif

void tunerInit()
{
#if defined(TEA5767)
	tea5767Init();
#elif defined(TUX032)
	tux032Init();
#endif
}

void tunerSetFreq(uint16_t freq)
{
	if (freq > FM_FREQ_MAX)
		freq = FM_FREQ_MIN;
	if (freq < FM_FREQ_MIN)
		freq = FM_FREQ_MAX;
#if defined(TEA5767)
	tea5767SetFreq(freq);
#elif defined(TUX032)
	tux032SetFreq(freq);
	freqFM = (freq);
#endif
}

void tunerReadStatus()
{
#if defined(TEA5767)
	tea5767ReadStatus(bufFM);
#elif defined(TUX032)
	tux032ReadStatus(bufFM);
#endif
}

uint16_t tunerGetFreq()
{
#if defined(TEA5767)
	return tea5767FreqAvail(bufFM);
#elif defined(TUX032)
	return freqFM;
#endif
}

uint8_t tunerReady()
{
#if defined(TEA5767)
	return TEA5767_BUF_READY(bufFM);
#elif defined(TUX032)
	return 1;
#endif
}

uint8_t tunerStereo()
{
#if defined(TEA5767)
	return TEA5767_BUF_STEREO(bufFM);
#elif defined(TUX032)
	return !TUX032_BUF_STEREO(bufFM);
#endif
}

uint8_t tunerLevel()
{
#if defined(TEA5767)
	return (bufFM[3] & TEA5767_LEV_MASK) >> 4;
#elif defined(TUX032)
	if (tunerStereo())
		return 13;
	else
		return 3;
#endif
}

/* Find station number (1..64) in EEPROM */
uint8_t stationNum(uint16_t freq)
{
	uint8_t i;

	for (i = 0; i < FM_COUNT; i++)
		if (eeprom_read_word(eepromStations + i) == freq)
			return i + 1;

	return 0;
}

/* Find nearest next/prev stored station */
void scanStoredFreq(uint16_t freq, uint8_t direction)
{
	uint8_t i;
	uint16_t freqCell;
	uint16_t freqFound = freq;

	for (i = 0; i < FM_COUNT; i++) {
		freqCell = eeprom_read_word(eepromStations + i);
		if (freqCell != 0xFFFF) {
			if (direction) {
				if (freqCell > freq) {
					freqFound = freqCell;
					break;
				}
			} else {
				if (freqCell < freq) {
					freqFound = freqCell;
				} else {
					break;
				}
			}
		}
	}

	tunerSetFreq(freqFound);

	return;
}

/* Load station by number */
void loadStation(uint8_t num)
{
	uint16_t freqCell = eeprom_read_word(eepromStations + num);

	if (freqCell != 0xFFFF)
		tunerSetFreq(freqCell);

	return;
}

/* Save/delete station from eeprom */
void storeStation(uint16_t freq)
{
	uint8_t i, j;
	uint16_t freqCell;

	for (i = 0; i < FM_COUNT; i++) {
		freqCell = eeprom_read_word(eepromStations + i);
		if (freqCell < freq)
			continue;
		if (freqCell == freq) {
			for (j = i; j < FM_COUNT; j++) {
				if (j == FM_COUNT - 1)
					freqCell = 0xFFFF;
				else
					freqCell = eeprom_read_word(eepromStations + j + 1);
				eeprom_update_word(eepromStations + j, freqCell);
			}
			break;
		} else {
			for (j = i; j < FM_COUNT; j++) {
				freqCell = eeprom_read_word(eepromStations + j);
				eeprom_update_word(eepromStations + j, freq);
				freq = freqCell;
			}
			break;
		}
	}

	return;
}

void loadTunerParams(uint16_t *freq)
{
	*freq = eeprom_read_word(eepromFMFreq);

#if defined(TUX032)
	tux032ExitStby();
#elif defined(TEA5767)
	tea5767SetFreq(*freq);
#endif
}

void saveTunerParams(uint16_t freq)
{
	eeprom_update_word(eepromFMFreq, freq);

#if defined(TUX032)
	tux032GoStby();
#elif defined(TEA5767)
#endif
	return;
}
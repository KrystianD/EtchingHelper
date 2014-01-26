#include "public.h"
#include "hardware.h"

volatile u16 timerTicks = 0; // updated in led.h
u8 secTimer = 0;
u16 secs = 0;

#include "led.h"
#include "ow.h"
#include "buttonmanager.h"

// 0 - no sensor, 1 - send convert, 2 - wait for conversion
uint8_t sensorState = 0;
uint8_t temp;
uint16_t convDelay = 0, sensorActivityTime;
uint8_t invalidReads = 0, haveValidRead = 0;

uint8_t desiredTemp = TEMP_START;

// relay
u8 isRelayEnabled = 0;

// screen
uint16_t settingsDisplay;

// security
u8 globalError = 0;
u16 relayOnTimeSecs, relayTurnOnTime;
u16 relayCheckTimeSecs;
u8 relayStartTemp;

void processSensor();
void enableRelay();
void disableRelay();

void do1s()
{
	secs++;
}
void do10ms()
{
	if (settingsDisplay) settingsDisplay--;
	if (convDelay) convDelay--;
	secTimer++;
	if (secTimer == 100)
	{
		do1s();
		secTimer = 0;
	}
}
void btnPress(uint8_t id)
{
	if (globalError)
	{
		globalError = 0;
		secs = 0;
	}
	else
	{
		if (settingsDisplay != 0)
		{
			if (id == BTN_LEFT)
			{
				if (desiredTemp > TEMP_MIN)
					desiredTemp--;
			}
			else
			{
				if (desiredTemp < TEMP_MAX)
					desiredTemp++;
			}
			eeprom_busy_wait();
			eeprom_update_byte((uint8_t*)0, 1);
			eeprom_update_byte((uint8_t*)1, desiredTemp);
		}
		settingsDisplay = 1000 / 10;
	}
}

int main()
{
	IO_PUSH_PULL(A1); IO_PUSH_PULL(B1); IO_PUSH_PULL(C1); IO_PUSH_PULL(D1); IO_PUSH_PULL(E1);
	IO_PUSH_PULL(F1); IO_PUSH_PULL(G1); IO_PUSH_PULL(A2); IO_PUSH_PULL(B2); IO_PUSH_PULL(C2);
	IO_PUSH_PULL(D2); IO_PUSH_PULL(E2); IO_PUSH_PULL(F2); IO_PUSH_PULL(G2);

	IO_PUSH_PULL(LED_RED);
	IO_PUSH_PULL(LED_GREEN);
	IO_PUSH_PULL(SWITCH);

	IO_INPUT_PP(BTN1);
	IO_INPUT_PP(BTN2);

	IO_PUSH_PULL(TX);
	IO_LOW(TX);
	IO_INPUT(RX);
	IO_LOW(RX);

	uint8_t tmp;
	eeprom_busy_wait();
	tmp = eeprom_read_byte(0);
	if (tmp != 0xff)
	{
		eeprom_busy_wait();
		desiredTemp = eeprom_read_byte((uint8_t*)1);
	}

	OSCCAL = 156;

	TCCR0 = _BV(CS01) | _BV(CS00);
	TIMSK |= _BV(TOIE0);

	UCSRB = _BV(RXEN) | _BV(TXEN);
	UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);
	u8 i;

	sei();

	uint16_t lastTick = 0;

	for (;;)
	{
		if (timerTicks - lastTick > 10 / 2)
		{
			do10ms();
			lastTick = timerTicks;
		}

		processSensor();

		uint8_t valid = 0;
		if (sensorState == 0)
		{
			ledOutChar('-', '-');
		}
		else
		{
			if (timerTicks - sensorActivityTime > 1000 / 2)
			{
				IO_TOGGLE(LED_GREEN);
				ledOutChar('o', 'o');
				sensorState = 0;
			}
			else
			{
				if (haveValidRead)
				{
					valid = 1;
				}
			}
		}

		if (!globalError)
		{
			if (valid)
			{
				if (settingsDisplay)
				{
					ledOutNum(desiredTemp);
					LED_GREEN_ON;
				}
				else
				{
					ledOutNum(temp);
					LED_GREEN_OFF;
				}

				if (desiredTemp > temp + 1)
				{
					enableRelay();
				}
				else if (desiredTemp < temp)
				{
					disableRelay();
				}
			}
			else
			{
				LED_GREEN_OFF;
				disableRelay();
			}
		}

		if (isRelayEnabled)
		{
			if (secs - relayCheckTimeSecs >= NO_TEMP_CHANGE_TIMEOUT)
			{
				if ((int16_t)temp - (int16_t)relayStartTemp < 1)
				{
					ledOutChar('-', 'o');
					disableRelay();
					globalError = 1;
				}
				else
				{
					relayStartTemp = temp;
					relayCheckTimeSecs = secs;
				}
			}
			if (secs - relayOnTimeSecs >= MAX_RELAY_ON_TIME)
			{
				disableRelay();
				relayTurnOnTime = secs + RELAY_DELAY_TIME;
			}
		}

		if (secs >= 3 * 3600)
		{
			ledOutChar('o', 'o');
			disableRelay();
			globalError = 1;
		}

		bmCheck();
	}
}

void processSensor()
{
	u8 i;

	switch (sensorState)
	{
	case 0:
		OW_UART_set9600();
		if (OW_UART_resetPulse())
		{
			OW_UART_set115200();
			OW_UART_writeByte(0xcc);
			OW_UART_writeByte(0x4e);
			OW_UART_writeByte(0x00);
			OW_UART_writeByte(0x00);
			OW_UART_writeByte(0x1f);
			sensorState = 1;
			sensorActivityTime = timerTicks;
			haveValidRead = 0;
			invalidReads = 0;
		}
		break;
	case 1:
		OW_UART_set9600();
		if (!OW_UART_resetPulse())
		{
			sensorState = 0;
			break;
		}
		OW_UART_set115200();
		OW_UART_writeByte(0xcc);
		OW_UART_writeByte(0x44);
		sensorState = 2;
		convDelay = 200 / 10;
		sensorActivityTime = timerTicks;
		break;
	case 2:
		if (convDelay == 0)
		{
			OW_UART_set9600();
			if (!OW_UART_resetPulse())
			{
				sensorState = 0;
				break;
			}
			OW_UART_set115200();
			OW_UART_writeByte(0xcc);
			OW_UART_writeByte(0xbe);
			uint8_t r[8];
			uint8_t crc = 0;
			for (i = 0; i < 8; i++)
			{
				r[i] = OW_UART_readByte();
				crc = OW_crc8_update(crc, r[i]);
			}
			uint8_t origcrc = OW_UART_readByte();
			if (crc == origcrc)
			{
				if (r[4] == 0x1f)
				{
					temp = (r[0] >> 4) |((r[1] & 0x07) << 4);
					sensorActivityTime = timerTicks;
					haveValidRead = 1;
					invalidReads = 0;
				}
				else
				{
					invalidReads++;
					if (invalidReads == 5)
					{
						sensorState = 0;
						break;
					}
				}
			}
			sensorState = 1;
		}
		break;
	}
}
void enableRelay()
{
	if (globalError || isRelayEnabled || secs < relayOnTimeSecs)
		return;
	LED_RED_ON;
	SWITCH_ON;
	relayOnTimeSecs = secs;
	relayCheckTimeSecs = secs;
	relayStartTemp = temp;
	isRelayEnabled = 1;
}
void disableRelay()
{
	LED_RED_OFF;
	SWITCH_OFF;
	isRelayEnabled = 0;
}

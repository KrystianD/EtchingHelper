#ifndef __BUTTONMANAGER_H__
#define __BUTTONMANAGER_H__

#define BM_STATE_RELEASED           0
#define BM_STATE_PRESSED_TODEBOUNCE 1
#define BM_STATE_PRESSED            2
#define BM_STATE_LONGPRESSED        3
#define BM_STATE_REPEATINGPRESS     4

uint8_t btnState[2] = { BM_STATE_RELEASED, BM_STATE_RELEASED };
uint16_t btnDebounce[2];
uint16_t btnClick[2];

void btnPress (uint8_t id);

void bmCheck ()
{
	uint8_t i;

	for (i = 0; i < 2; i++)
	{
		uint8_t pinState;
		if (i == 0) pinState = IO_IS_HIGH(BTN1);
		else pinState = IO_IS_HIGH(BTN2);

		if (btnState[i] == BM_STATE_RELEASED)
		{
			if (!pinState)
			{
				btnState[i] = BM_STATE_PRESSED_TODEBOUNCE;
				btnDebounce[i] = timerTicks;
			}
		}
		if (btnState[i] == BM_STATE_PRESSED_TODEBOUNCE)
		{
			if (timerTicks - btnDebounce[i] > 10 / 2)
			{
				if (!pinState)
				{
					btnState[i] = BM_STATE_PRESSED;
					btnPress (i);

					btnClick[i] = timerTicks;
				}
				else
				{
					btnState[i] = BM_STATE_RELEASED;
				}
			}
		}
		if (btnState[i] == BM_STATE_REPEATINGPRESS || btnState[i] == BM_STATE_PRESSED)
		{
			if (!pinState)
			{
				if (btnState[i] == BM_STATE_PRESSED)
				{
					if (timerTicks - btnClick[i] > 600 / 2)
					{
						btnPress (i);
						btnClick[i] = timerTicks;
						btnState[i] = BM_STATE_REPEATINGPRESS;
					}
				}
				if (btnState[i] == BM_STATE_REPEATINGPRESS)
				{
					if (timerTicks - btnClick[i] > 200 / 2)
					{
						btnPress (i);
						btnClick[i] = timerTicks;
					}
				}
			}
			else
			{
				btnState[i] = BM_STATE_RELEASED;
			}
		}
	}
}

#endif


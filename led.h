#ifndef __LED_H__
#define __LED_H__

uint8_t ledOut1, ledOut2;
uint8_t ledPart = 0;

const PROGMEM uint8_t digits[] = {
	0b00111111, // 0
	0b00000110, // 1
	0b01011011, // 2
	0b01001111, // 3
	0b01100110, // 4
	0b01101101, // 5
	0b01111101, // 6
	0b00000111, // 7
	0b01111111, // 8
	0b01101111, // 9

	// 0b01110111, // 10, A
	// 0b01111111, // 11, B
	// 0b00111001, // 12, C
	// 0b00111111, // 13, D
	// 0b01111001, // 14, E
	// 0b01110001, // 15, F
};

SIGNAL(TIMER0_OVF_vect)
{
	timerTicks++;
	IO_HIGH(A1); IO_HIGH(B1); IO_HIGH(C1); IO_HIGH(D1); IO_HIGH(E1);
	IO_HIGH(F1); IO_HIGH(G1); IO_HIGH(A2); IO_HIGH(B2); IO_HIGH(C2);
	IO_HIGH(D2); IO_HIGH(E2); IO_HIGH(F2); IO_HIGH(G2);

	switch (ledPart)
	{
	case 0: if (ledOut1 & 0x01) IO_LOW(A1); if (ledOut2 & 0x01) IO_LOW(A2); break;
	case 1: if (ledOut1 & 0x02) IO_LOW(B1); if (ledOut2 & 0x02) IO_LOW(B2); break;
	case 2: if (ledOut1 & 0x04) IO_LOW(C1); if (ledOut2 & 0x04) IO_LOW(C2); break;
	case 3: if (ledOut1 & 0x08) IO_LOW(D1); if (ledOut2 & 0x08) IO_LOW(D2); break;
	case 4: if (ledOut1 & 0x10) IO_LOW(E1); if (ledOut2 & 0x10) IO_LOW(E2); break;
	case 5: if (ledOut1 & 0x20) IO_LOW(F1); if (ledOut2 & 0x20) IO_LOW(F2); break;
	case 6: if (ledOut1 & 0x40) IO_LOW(G1); if (ledOut2 & 0x40) IO_LOW(G2); break;
	}

	ledPart++;
	if (ledPart == 7)
		ledPart = 0;
}

void ledOutNum (uint8_t num)
{
	if (num >= 100)
	{
		ledOut1 = 0x00;
		ledOut2 = 0x00;
	}
	else
	{
		ledOut1 = pgm_read_byte (digits + num / 10);
		ledOut2 = pgm_read_byte (digits + num % 10);
	}
}
uint8_t ledGetChar (char c)
{
	if (c >= '0' && c <= '9') return pgm_read_byte (digits + c - '0');
	switch (c)
	{
	case '-': return 0b01000000;
	case 'o': return 0b01011100;
	}
}
void ledOutChar (char c1, char c2)
{
	ledOut1 = ledGetChar (c1);
	ledOut2 = ledGetChar (c2);
}
// void ledOutHex (uint8_t num)
// {
	// uint8_t d1 = num >> 4;
	// out1 (digits[d1]);
	// uint8_t d2 = num & 0x0f;
	// out2 (digits[d2]);
// }

#endif

#ifndef __OW_H__
#define __OW_H__

#define UART_BAUD_SELECT(baud,freq)(freq/16/baud-1)
#define UART_BAUD_SELECT_DOUBLE(baud,freq)(freq/8/baud-1)
void OW_UART_set9600()
{
#define UART_BAUD1 UART_BAUD_SELECT(9600,F_CPU)
	UBRRH = (uint8_t)((UART_BAUD1) >> 8);
	UBRRL = (uint8_t)(UART_BAUD1);
}
void OW_UART_set115200()
{
#define UART_BAUD2 UART_BAUD_SELECT(115200,F_CPU)
	UBRRH = (uint8_t)((UART_BAUD2) >> 8);
	UBRRL = (uint8_t)(UART_BAUD2);
}
uint8_t OW_UART_resetPulse()
{
	uint8_t t = 0;
	UDR = 0xf0;
	while (bit_is_clear(UCSRA, RXC))
	{
		t++;
		if (t == 100)
			return 0;
		_delay_us(20);
	}
	uint8_t v = UDR;
	return v != 0xf0 ? 1 : 0;
}
uint8_t OW_UART_readBit()
{
	uint8_t t = 0;
	UDR = 0xff;
	while (bit_is_clear(UCSRA, RXC))
	{
		t++;
		if (t == 100)
			return 0;
		_delay_us(2);
	}
	uint8_t v = UDR;
	return v == 0xff ? 1 : 0;
}
uint8_t OW_UART_writeBit(uint8_t b)
{
	uint8_t t = 0;
	UDR = b ? 0xff : 0x00;
	while (bit_is_clear(UCSRA, RXC))
	{
		t++;
		if (t == 100)
			return 0;
		_delay_us(2);
	}
	uint8_t v = UDR;
}
void OW_UART_writeByte(uint8_t byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OW_UART_writeBit(byte & 0x01);
		byte >>= 1;
	}
}
uint8_t OW_UART_readByte()
{
	uint8_t i, v = 0;
	for (i = 0; i < 8; i++)
	{
		v >>= 1;
		if (OW_UART_readBit())
			v |= 0x80;
	}
	return v;
}
uint8_t OW_crc8_update(uint8_t crc, char d)
{
	uint8_t j;

	crc ^= d;
	for (j = 0; j < 8; j++)
	{
		if (crc & 0x1)
		{
			crc >>= 1;
			crc ^= 0x8c;
		}
		else
		{
			crc >>= 1;
		}
	}
	return crc;
}

#endif

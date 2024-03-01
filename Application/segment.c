#include "segment.h"

bool isDispShift = false;
uint8_t disp_step = 0;
const uint8_t digit[10] =
{
		0xC0,
		0xF9,
		0xA4,
		0xB0,
		0x99,
		0x92,
		0x82,
		0xF8,
		0x80,
		0x90
}; //numbers

const uint8_t letters[7] =
{
	0x88, // A
	0x83, // b
	0xc6, // C
	0xa1, // d
	0x86, // E
	0x8e, // F
	0xaf  // r,
}; //A, b, C, d, E, F

const uint8_t column[4] =
{
		0x01,
		0x02,
		0x04,
		0x08
}; //positions //,0x10,0x20,0x40,0x80  //8-digits position??

const uint8_t digit_pointed[10] =
{
		0x40,
		0x79,
		0x24,
		0x30,
		0x19,
		0x12,
		0x02,
		0x78,
		0x00,
		0x10
}; //numbers with dots

void shiftSegment(uint8_t data)
{
	uint8_t tmp[8] = {0,};

	for (uint8_t i=0; i<8; i++)
	{
		tmp[i] = (data >> (7-i)) & 0x01 ? 1 : 0;
		HAL_GPIO_WritePin(DIO_GPIO_Port, DIO_Pin, tmp[i] ? GPIO_PIN_SET : GPIO_PIN_RESET);
		// SCLK Low -> HIGH
		HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_SET);
	}
}

void dispData(uint8_t data, uint8_t pos)
{
	shiftSegment(data);
	shiftSegment(pos);
	HAL_GPIO_WritePin(RCLK_GPIO_Port, RCLK_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RCLK_GPIO_Port, RCLK_Pin, GPIO_PIN_SET);
}

bool dispCharacter(uint8_t data, uint8_t pos)
{
	switch (data)
	{
		case 'A':
			dispData(letters[0], pos);
			break;
		case 'b':
		case 'B':
			dispData(letters[1], pos);
			break;
		case 'c':
		case 'C':
			dispData(letters[2], pos);
			break;
		case 'd':
		case 'D':
			dispData(letters[3], pos);
			break;
		case 'e':
		case 'E':
			dispData(letters[4], pos);
			break;
		case 'f':
		case 'F':
			dispData(letters[5], pos);
			break;
		case 'r':
		case 'R':
			dispData(letters[6], pos);
			break;
		default:
			return false;
			break;
	}
	return true;
}

void dispError()
{
	uint8_t err[3] = {'E', 'r', 'r'};

	for (uint8_t i=0; i<3; i++)
	{
		dispCharacter(err[i], column[i]);
	}
}

void dispCountUntil9999Test()
{
	static uint32_t cnt_tick = 0;
	static uint16_t cnt = 0;
	dispIntNumber(cnt);
	if (HAL_GetTick() - cnt_tick >= 100)
	{
		cnt_tick = HAL_GetTick();
		cnt++;
	}
}

bool dispStatic(uint16_t data)
{
	uint8_t temp = 0xff;
	uint8_t shift_pos = 0x01;

	if (!isDispShift)
	{
		if (data < 10)
		{
			temp = digit[data];
			dispData(temp, shift_pos);
		}
		else if (data < 100 && data >= 10)
		{
			shift_pos = 0x02;
			temp = data/10;
			dispData(digit[temp], shift_pos);
			shift_pos >>= 1;
			temp = data%10;
			dispData(digit[temp], shift_pos);
		}
		else if (data < 1000 && data >= 100)
		{
			shift_pos = 0x04;
			temp = data/100;
			dispData(digit[temp], shift_pos);
			shift_pos >>= 1;
			temp = (data / 10) % 10;
			dispData(digit[temp], shift_pos);
			shift_pos >>= 1;
			temp = data % 10;
			dispData(digit[temp], shift_pos);
		}
		else if (data < 10000 && data >= 1000)
		{
			shift_pos = 0x08;
			temp = data/1000;
			dispData(digit[temp], shift_pos);

			shift_pos >>= 1;
			temp = (data / 100) % 10;
			dispData(digit[temp], shift_pos);

			shift_pos >>= 1;
			temp = (data / 10) % 10;
			dispData(digit[temp], shift_pos);

			shift_pos >>= 1;
			temp = data % 10;
			dispData(digit[temp], shift_pos);
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool dispDynamic(uint16_t data)
{
	const uint8_t thousand = data/1000;
	const uint8_t hundred = (data/100)%10;
	const uint8_t tens = (data%1000)%100/10;
	const uint8_t ones = (data%100)%10;
	uint8_t shift_obj[4] = {thousand, hundred, tens, ones};
	uint8_t shift_pos = 0x01;

	switch (disp_step)
	{
		case 0:
		{
			dispData(0xff, (shift_pos<<0));
		}
		break;
		case 1:
		{
			dispData(digit[shift_obj[0]], (shift_pos<<0));
		}
		break;
		case 2:
		{
			dispData(digit[shift_obj[0]], (shift_pos<<1));
			dispData(digit[shift_obj[1]], (shift_pos<<0));
		}
		break;
		case 3:
		{
			dispData(digit[shift_obj[0]], (shift_pos<<2));
			dispData(digit[shift_obj[1]], (shift_pos<<1));
			dispData(digit[shift_obj[2]], (shift_pos<<0));
		}
		break;
		case 4:
		{
			dispData(digit[shift_obj[0]], (shift_pos<<3));
			dispData(digit[shift_obj[1]], (shift_pos<<2));
			dispData(digit[shift_obj[2]], (shift_pos<<1));
			dispData(digit[shift_obj[3]], (shift_pos<<0));
		}
		break;
		case 5:
		{
			dispData(digit[shift_obj[1]], (shift_pos<<3));
			dispData(digit[shift_obj[2]], (shift_pos<<2));
			dispData(digit[shift_obj[3]], (shift_pos<<1));
		}
		break;
		case 6:
		{
			dispData(digit[shift_obj[2]], (shift_pos<<3));
			dispData(digit[shift_obj[3]], (shift_pos<<2));
		}
		break;
		case 7:
		{
			dispData(digit[shift_obj[3]], (shift_pos<<3));
		}
		break;
		case 8:
		{
			dispData(0xff, (shift_pos<<3));
		}
		break;
		default:
			return false;
			break;
	}

	return true;
}

bool dispIntNumber(uint16_t data)
{
	if (data > 9999)
	{
		return false;
	}
	else
	{
		if (!isDispShift)
		{
			dispStatic(data);
		}
		else
		{
			dispDynamic(data);
		}
		return true;
	}
	return false;
}

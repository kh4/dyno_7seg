#include "board.h"

/*
  Anodes (commons)
  PB7, PB6, PB5, PB4, PB3, PA15, PA12, PA11

  Segeents
  PB15, PB14, PB13, PB12, PB8, PB9, PB10, PB11(dp)
*/

const uint16_t PinsPortA = (GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15);
const uint16_t PinsPortB = (GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 |
			    GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
			    GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
			    GPIO_Pin_15);

volatile uint8_t segState[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

volatile uint8_t dno = 0;

// this will be called every 1ms from systick
void sevenSegHook()
{
  GPIO_ResetBits(GPIOA, PinsPortA);
  GPIO_ResetBits(GPIOB, PinsPortB);
  // enable segments
  GPIO_SetBits(GPIOB, ((uint16_t)segState[dno]) << 8);
  // enable anode
  switch (dno) {
  case 0: GPIO_SetBits(GPIOB, GPIO_Pin_7); break;
  case 1: GPIO_SetBits(GPIOB, GPIO_Pin_6); break;
  case 2: GPIO_SetBits(GPIOB, GPIO_Pin_5); break;
  case 3: GPIO_SetBits(GPIOB, GPIO_Pin_4); break;
  case 4: GPIO_SetBits(GPIOB, GPIO_Pin_3); break;
  case 5: GPIO_SetBits(GPIOA, GPIO_Pin_15); break;
  case 6: GPIO_SetBits(GPIOA, GPIO_Pin_12); break;
  case 7: GPIO_SetBits(GPIOA, GPIO_Pin_11); break;
  }
  dno = (dno + 1) & 7;
}

void sevenSegInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_ResetBits(GPIOA, PinsPortA);
  GPIO_ResetBits(GPIOB, PinsPortB);

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = PinsPortA;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = PinsPortB;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  systickAddHook(sevenSegHook);
}

// segments
// DP     0x08
// Up (a) 0x80
// UR (b) 0x40
// LR (c) 0x20
// Lo (d) 0x10
// LL (e) 0x01
// UL (f) 0x02
// Mi (g) 0x04

uint8_t charToSegs(char c, bool dp)
{
  uint8_t r = dp ? 0x08 : 0;
  switch (c) {
  case '0':
    r |= 0xf3;
    break;
  case '1':
    r |= 0x60;
    break;
  case '2':
    r |= 0xd5;
    break;
  case '3':
    r |= 0xf4;
    break;
  case '4':
    r |= 0x66;
    break;
  case '5':
    r |= 0xb6;
    break;
  case '6':
    r |= 0xb7;
    break;
  case '7':
    r |= 0xe0;
    break;
  case '8':
    r |= 0xf7;
    break;
  case '9':
    r |= 0xf6;
    break;
  case '-':
    r |= 0x04;
    break;
  case 0x00:
  case ' ':
  default:
    break;
  }
  return r;
}

void sevenSegSet(uint8_t d, char c, bool dp) {
  if (d < 8)
    segState[d] = charToSegs(c, dp);
}


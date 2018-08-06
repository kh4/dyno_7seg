#include "board.h"
static void _putc(void *p, char c)
{
    uartWrite(c);
}

void checkBootLoaderEntry(bool wait)
{
  uint32_t start = millis();
  do {
    if (uartAvailable() && ('R' == uartRead())) {
      systemReset(true);
      while (1);
    }
  }  while (wait && ((millis() - start) < 2000));
}


uint16_t v1 = 0, v2 = 0;


void handleValuesFromADC(uint16_t *_values)
{
  v1 = _values[0];
  v2 = _values[1];
}


uint8_t modul = 0;

int main(void)
{
    systemInit();
    ledInit();
    init_printf(NULL, _putc);
    uartInit(115200);
    checkBootLoaderEntry(true);
    sevenSegInit();
    adcInit();
    // loopy
    while (1) {
      //     printf("uptime is %d",micros());
      ledSet(LED_TOGGLE);
      delay(100);
      /*
      for (int i = 0; i < 8; i++)
	sevenSegSet(i, '0' + ((i + modul) % 10), (modul&1));
      modul++;
      */
      {
	// calculate values to show
	uint32_t left = v1, right = v2;
	uint32_t bigger, differ;
	if (left > right) {
	  bigger = left;
	  differ = left - right;
	} else {
	  bigger = right;
	  differ = right - left;
	}
	differ = differ * 100 / bigger;
	if (differ > 99) differ = 99;

	if (left > 1000) {
	  sevenSegSet(0,'-',0);
	  sevenSegSet(1,'-',0);
	  sevenSegSet(2,'-',0);
	} else {
	  char s[4];
	  tfp_sprintf(s,"%3d",left);
	  sevenSegSet(0,s[0],0);
	  sevenSegSet(1,s[1],0);
	  sevenSegSet(2,s[2],0);
	}
	if (right > 1000) {
	  sevenSegSet(3,'-',0);
	  sevenSegSet(4,'-',0);
	  sevenSegSet(5,'-',0);
	} else {
	  char s[4];
	  tfp_sprintf(s,"%3d",right);
	  sevenSegSet(3,s[0],0);
	  sevenSegSet(4,s[1],0);
	  sevenSegSet(5,s[2],0);
	}
	char s[4];
	tfp_sprintf(s,"%2d",differ);
	sevenSegSet(6,s[0],0);
	sevenSegSet(7,s[1],0);
      }
    }
}

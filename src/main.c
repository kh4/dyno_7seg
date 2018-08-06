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



int main(void)
{
    systemInit();
    ledInit();
    init_printf(NULL, _putc);
    uartInit(115200);
    checkBootLoaderEntry(true);
    // loopy
    while (1) {
      printf("uptime is %d",micros());
      ledSet(LED_TOGGLE);
      delay(100);
    }
}

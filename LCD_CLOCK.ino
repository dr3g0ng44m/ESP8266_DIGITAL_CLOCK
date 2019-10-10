#include"LCD_CLOCK.h"
#include<Ticker.h>

Ticker test;
Ticker Press;
Ticker ChangeS;
Ticker ChangeV;
Ticker Disp;

void setup() {
  InitializeSystem();
  Press.attach_ms(200, PressButton);
  ChangeS.attach_ms(200, ChangeState);
  ChangeV.attach_ms(10, ChangeValue);
  Disp.attach_ms(100, DisplayClock);
}
void loop() {}

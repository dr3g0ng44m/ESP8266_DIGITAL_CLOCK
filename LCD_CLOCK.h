#pragma once
#ifndef LCD_CLOCK

void DisplayTimeLCD1602(uint8_t hour, bool Blink_hour, uint8_t minute, bool Blink_minute, uint8_t sec, bool Blink_sec);
void InitializeSystem(void);
void PressButton(void);
void ChangeState(void);
void ChangeValue(void);
void DisplayClock(void);
#endif LCD_CLOCK

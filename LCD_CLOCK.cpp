#include<Arduino.h>
#include"LCD_CLOCK.h"
#include<LiquidCrystal_I2C.h>
#include<Wire.h>

//------------------DEFINE PARAMETER----------------
#define BUTTON1_PIN 14 //PIN connected to button 1
#define BUTTON2_PIN 12 //PIN connected to button 2
#define ONE_SECOND 5          // CYCLE = 200ms
#define ONE_SECOND_VALUE 100  //CYCLE = 10ms
#define ONE_FIFTH_SECOND 1    //CYCLE = 200ms

//------------------BUTTON STATES-------------------
bool pButton1 = false;  // Button 1 is pressed??
bool pButton2 = false;  // Button 2 is pressed??
bool OneSecond = 0;     // One second lasted??
bool OneFifthSecond = false;  //Pressed enough 200ms extra
bool buttonPressed1s = false; //Pressed enough 1s;
bool newlyPressed2 = false;   //Newly press button2
bool firstPressed2 = false;   //Press button2 1st or 2nd time??

//------------------CLOCK VALUE-----------------------
uint8_t timeCounter = 0;        //count Button_pressed lasting time
uint8_t BlinkClockCounter = 1;  //count time for blink
uint8_t subTimeCounter = 0;     //count time for faster increasing value
uint8_t SignalRead1 = 0;        //Digital Value input button1
uint8_t SignalRead2 = 0;        //Digital Value input button2
uint8_t OldSignalRead1 = 0;     //Former Digital Value input button1
uint8_t OldSignalRead2 = 0;     //Former Digital Value input button2
uint8_t hour  = 0;              //HOUR value
uint8_t min_  = 0;              //MINUTE value
uint8_t sec   = 0;              //SECOND value
uint8_t hour_STOPWATCH  = 0 ;   //Hour Stopwatch value 
uint8_t min_STOPWATCH   = 0 ;   //Min  Stopwatch value     
uint8_t sec_STOPWATCH   = 0 ;   //Sec  Stopwatch value
  


//-----------------CLOCK STATES----------------------
enum State {
	CLOCK_DISPLAYING,   //display CLOCK
	HOUR_SETTING,       //set HOUR value
	MIN_SETTING,        //set MINUTE value
	SEC_SETTING,        //set SECOND value
  STOPWATCH,      //STOPWATCH
};
enum State NowState = CLOCK_DISPLAYING; //initial State
LiquidCrystal_I2C lcd(0x27, 16, 2); //Declare 1602 LCD

//------------------MODIFY FUNCITION--------------------
void InitializeSystem(void) {
  Serial.begin(115200); //Declare Baudrate
  lcd.clear();          //clear LCD screen
  lcd.begin();          //begin using LCD
  lcd.backlight();      // turn on LCD backlight
  lcd.setCursor(4, 1);  //print my nametag ^_^
  lcd.print("PHI LONG");
	pinMode(BUTTON1_PIN, INPUT_PULLUP); // button1 is INPUT_PULLUP
	pinMode(BUTTON2_PIN, INPUT_PULLUP); // button2 is INPUT_PULLUP
	pinMode(LED_BUILTIN, OUTPUT);       // used to alarm that some button is pressed
	digitalWrite(LED_BUILTIN, LOW);     // turn on LED to sign beginning
}

//----------------LCD DISPLAY TIME FUCTION ------------------------------------------------------------------------
// In first 0.5s, we print time by format hh:mm::ss normally;
// In last 0.5s, if whose value is required blinkly-displaying, we replace it by '__'
void DisplayTimeLCD1602(uint8_t hour, bool Blink_hour, uint8_t minute, bool Blink_minute, uint8_t sec, bool Blink_sec){
  lcd.setCursor(4, 0);
  if(BlinkClockCounter < 50) {
        if(hour < 10) { lcd.print(0); lcd.print(hour);}
        else lcd.print(hour);
        lcd.print(':');
        if(minute < 10) { lcd.print(0); lcd.print(minute);}
        else lcd.print(minute);
        lcd.print(':');
        if(sec < 10) { lcd.print(0); lcd.print(sec);}
        else lcd.print(sec);
        }
   else if(Blink_hour)    {lcd.setCursor(4, 0);   lcd.print("  ");}
   else if(Blink_minute)  {lcd.setCursor(7, 0);   lcd.print("  ");}
   else if(Blink_sec)     {lcd.setCursor(10, 0);  lcd.print("  ");}
}
//----------------------------------------------------------------------------

//----------------------Set States when Press Button--------------------------
void PressButton(void){
	SignalRead1 = digitalRead(BUTTON1_PIN); // read state of button1
	SignalRead2 = digitalRead(BUTTON2_PIN); // read state of button2
	 // If button1 is pressed
	 if (SignalRead1 == 1){
      digitalWrite(LED_BUILTIN, LOW); //turn on led to show button is pressed
	    // If button 1 is continously pressed
	    if(OldSignalRead1 == 1) pButton1 = false; //prevent from continously  state-changing;
      else  {
            pButton1 = true;  // signal button1 is pressed
            pButton2 = false; // signal button2 isnt pressed
            //-----reset state associate to button2----
            newlyPressed2 = false;    
            OneFifthSecond = false;
            buttonPressed1s = false;
            timeCounter = 0;
            subTimeCounter = 0;
            //------------------------------------------            
            }
    }
	 else if (SignalRead2 == 1) {
      digitalWrite(LED_BUILTIN, LOW);
      pButton1 = false; // signal button1 isnt pressed
		  pButton2 = true;  // signal button2 is presse;
      //If Button2 have been being pressed
		  if (OldSignalRead2 == 1) {
          // If button 2 havent been pressed enough 1s
			    if (timeCounter < ONE_SECOND) timeCounter++;
          //.......enough 1s
			    else {
				        buttonPressed1s = true; //signal
                // If button 2 is pressed enough 0.2s extra
				        if (subTimeCounter == ONE_FIFTH_SECOND) {
				                 OneFifthSecond = true; //signal
				                 subTimeCounter = 1;    //start/restart subcounter
				                 }
               // If button 2 is havent been pressed enough 0.2s extra
                else subTimeCounter++;
			        }
		      }
      //If Button2 havent been being pressed / is pressed again
      else{
          pButton1 =false;
          pButton2 =false;
          newlyPressed2 = true;
          buttonPressed1s = false;
          OneFifthSecond = false;
          timeCounter = 0;
          subTimeCounter = 0;
          }
	  }
   // If no button is pressed: reset all value of states
	 else {
      digitalWrite(LED_BUILTIN, HIGH);
      newlyPressed2 = false;
		  pButton1 = false;
		  pButton2 = false;
		  buttonPressed1s = false;
      OneFifthSecond = false;
		  timeCounter = 0;
		  subTimeCounter = 0;
	    }
 //Read current value for next button-processing
 OldSignalRead1 = SignalRead1; 
 OldSignalRead2 = SignalRead2;
}
//----------------------------------------------------------

//-----------  Change State Follow Cycle:------------------------------
//  1. CLOCK-DISPLAYING
//  2. HOUR-SETTING
//  3. MIN-SETTING
//  4. SEC-SETTING
//  5. STOPWATCH  
void ChangeState(void){
  switch (NowState) {
  case CLOCK_DISPLAYING:
      if (pButton1) NowState = HOUR_SETTING;
      break;
  case HOUR_SETTING:
      if (pButton1) NowState = MIN_SETTING;
    break;
  case MIN_SETTING:
      if (pButton1) NowState = SEC_SETTING;
      break;
  case SEC_SETTING:
      if (pButton1) {
      NowState = STOPWATCH;
      lcd.setCursor(4, 0);
      lcd.print("00:00:00");
      firstPressed2 = false;
      // Reset all STOPWATCH value
        hour_STOPWATCH  = 0 ;
        min_STOPWATCH   = 0 ;
        sec_STOPWATCH   = 0 ;
      //------------------------
      }
      break;
  case STOPWATCH:
      if (pButton1) NowState = CLOCK_DISPLAYING;
      break;
  default:
      break;
  }
}
//-----------------------------------------------------------

//--------------------Change Value of Clock------------------
void ChangeValue(void){
	switch (NowState) {
	case CLOCK_DISPLAYING: // No Action
		break;
    // In next 3 states:
    // Increase corresponding value when:
    //  + Newly press button2
    //  + have pressed button 2 enough 1s and 0.2s extra
	case HOUR_SETTING:
		  if (pButton2)
			  if ((buttonPressed1s) && (OneFifthSecond)) {
			    hour = (hour + 1) % 24;
          OneFifthSecond = false;
			  }
        else if(newlyPressed2){
          hour = (hour + 1) % 24;
          newlyPressed2 = false;
        }
		break;
	case MIN_SETTING:
		  if (pButton2)
			  if ((buttonPressed1s) && (OneFifthSecond)){
			    min_ = (min_ + 1) % 60;
          OneFifthSecond = false;
	      }
        else if(newlyPressed2){
          min_ = (min_ + 1) % 60;
          newlyPressed2 = false;
        }
		  break;
	case SEC_SETTING:
		  if (pButton2){
			  if ((buttonPressed1s) && (OneFifthSecond)) {
			    sec = (sec + 1) % 60;
          OneFifthSecond = false;
          }
        else if(newlyPressed2){
          sec = (sec + 1) % 60;
          newlyPressed2 = false;
          }
		    }
		  break;
  case STOPWATCH:
  //1st press: start/countinue STOPWATCH;
  //2nd press: stop Stopwatch
  //STOPWATCH display in the same cycle with Normal Clock
      //distinguish 1st or 2nd press
      if (newlyPressed2){
        firstPressed2 = !firstPressed2 ;
        newlyPressed2 = false;
        }
      if((firstPressed2)&&(OneSecond)){
        if ((sec_STOPWATCH == 59) && (min_STOPWATCH == 59)) hour_STOPWATCH = (hour_STOPWATCH + 1) % 24;
        if (sec_STOPWATCH == 59) min_STOPWATCH = (min_STOPWATCH + 1) % 60;
        sec_STOPWATCH = (sec_STOPWATCH + 1) % 60;
        }
      break;
  }
  //Identity 1 Second
  if (BlinkClockCounter == 0) OneSecond = true;
    else OneSecond =  false;
  // Change value of Time
  if (OneSecond) {
        if ((sec == 59) && (min_ == 59)) hour = (hour + 1) % 24;
        if (sec == 59) min_ = (min_ + 1) % 60;
        sec = (sec + 1) % 60;
  }
  //When enough 1 second, restart
  BlinkClockCounter = (BlinkClockCounter + 1) % ONE_SECOND_VALUE;
}
//---------------------------------------------------------------------

//------------------Display Clock-----------------------------------
//  Print order number of each state;
//  Print time allow format with blink-require of each state
void DisplayClock(void) {
  switch (NowState) {
  case CLOCK_DISPLAYING:  // No Blink
      lcd.setCursor(0, 0);
      lcd.print(1);
      DisplayTimeLCD1602(hour, false, min_, false, sec, false);
      break;
  case HOUR_SETTING:      //Hour blink
      lcd.setCursor(0, 0);
      lcd.print(2);
      DisplayTimeLCD1602(hour, true, min_, false, sec, false);
      break;
  case MIN_SETTING:       //Minute blink
      lcd.setCursor(0, 0);
      lcd.print(3);    
      DisplayTimeLCD1602(hour, false, min_, true, sec, false);  
      break;
  case SEC_SETTING:       //Second blink
      lcd.setCursor(0, 0);
      lcd.print(4);    
      DisplayTimeLCD1602(hour, false, min_, false, sec, true);
      break;
   case STOPWATCH:        //No blink
      lcd.setCursor(0, 0);
      lcd.print(5);
      if(firstPressed2){
          DisplayTimeLCD1602( hour_STOPWATCH, false,
                              min_STOPWATCH,  false,
                              sec_STOPWATCH,  false);
          }
  }
}
//---------------------------------------------------------------------

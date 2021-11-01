#include <RTClib.h>
#include <LiquidCrystal.h>
#include <EnableInterrupt.h>

#define BUTTON_UP_PIN       A0
#define BUTTON_DOWN_PIN     A1
#define BUTTON_LEFT_PIN     A2
#define BUTTON_RIGHT_PIN    A3
#define BUTTON_NONE         0xFF
#define ALARM_BUZZER_PIN    7

#define MAX_DAY_STR_SIZE    9

const uint8_t buttons[] = 
{
  BUTTON_UP_PIN,
  BUTTON_DOWN_PIN,
  BUTTON_LEFT_PIN,
  BUTTON_RIGHT_PIN
};

const char daysOfTheWeek[7][10] = 
{
  "Sunday", 
  "Monday", 
  "Tuesday", 
  "Wednesday", 
  "Thursday", 
  "Friday", 
  "Saturday"
};

char timestr[20];
char datestr[20];

uint8_t strLength; 
char strBuffer[32];

uint8_t serialLength; 
char serialBuffer[32];

uint8_t rtc_status;
volatile uint8_t pressCount;
volatile uint8_t pressedButton = BUTTON_NONE;
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
RTC_DS1307 rtc;
DateTime now;

void setup() 
{
  Serial.begin(9600);
  Serial.println("DS1307 Digital Clock");

  lcd.begin(20, 4);
  CreateCustomChars();

  lcd.setCursor(4, 0);
  lcd.print("DS1307 RTC");
  lcd.setCursor(3, 1);
  lcd.print("Digital Clock");
  
  // set button pin mode as input pullup
  for(uint8_t i; i < sizeof(buttons); i++)
  {
    pinMode(buttons[i], INPUT_PULLUP);
    enableInterrupt(buttons[i], scan_buttons, CHANGE);
  }

  // set buzzer pin as ouput
  pinMode(ALARM_BUZZER_PIN, OUTPUT);

  rtc_status = rtc.begin();
  if(!rtc_status) Serial.println("Couldn't begin RTC!");
  if (rtc_status && !rtc.isrunning()) 
  {
    Serial.println("RTC is Not running, setting the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  delay(2500);
  lcd.clear();
}

void loop() 
{
  HandleButtonOperations();  
  ProcessSerialCommand();
  UpdateDisplay();
  delay(1000);
}

void scan_buttons()
{
  for(uint8_t i; i < sizeof(buttons); i++)
  {
    if(digitalRead(buttons[i]) == LOW)
    {
      pressedButton = buttons[i];
      break;
    }
  }
}

void HandleButtonOperations()
{
  if(pressedButton != BUTTON_NONE)
  {
    pressCount++;
    Serial.print("Button Pressed: ");
    Serial.println(pressedButton);
    lcd.setCursor(7, 2);
    lcd.print("B:");
    lcd.print(pressedButton);
    lcd.setCursor(12, 2);
    lcd.print(pressCount);
  }

  switch(pressedButton)
  {
    case BUTTON_UP_PIN:
      break;
    case BUTTON_DOWN_PIN:
      break;
    case BUTTON_LEFT_PIN:
      break;
    case BUTTON_RIGHT_PIN:
      break;
  }

  pressedButton = BUTTON_NONE;
}

void UpdateDisplay()
{
  now = rtc.now();  
  sprintf(datestr, "%.2u/%.2u/%.4u", now.day(), now.month(), now.year());
  sprintf(timestr, "%.2u:%.2u:%.2u %s", now.twelveHour(), now.minute(), now.second(), now.isPM() ? "PM" : "AM");  

  //update time display
  printBigString(2, &timestr[0], 2); //hour
  lcd.setCursor(8, 0);
  lcd.write('.');
  lcd.setCursor(8, 1);
  lcd.write('.');
  printBigString(9, &timestr[3], 2); //minute
  lcd.setCursor(16, 0);
  lcd.write(&timestr[9], 2); //AM/PM
  lcd.setCursor(16, 1);
  lcd.write(&timestr[6], 2); //second

  //update date display
  strLength = sprintf(strBuffer,"%s", daysOfTheWeek[now.dayOfTheWeek()]);
  str_padding_at_end(strBuffer, MAX_DAY_STR_SIZE - strLength);
  lcd.setCursor(0, 3);
  lcd.print(strBuffer);
  lcd.setCursor(10, 3);
  lcd.print(datestr);
}

void str_padding_at_end(char *str, uint8_t paddinglen)
{
  while(paddinglen--) 
    strcat(str, " ");
}

void ProcessSerialCommand()
{
  int rxlen = Serial.available();
  if(rxlen < 4) return;

  serialLength = 0;
  while(serialLength < rxlen)
  {
    serialBuffer[serialLength++] = Serial.read();
  }
  
  if(strstr(serialBuffer, "\r")) return;

  //"SET 22:29:12 Jan 14 2021\r"
  if(strstr(serialBuffer, "SET"))
  {
    if(serialLength < 25) return;
    rtc.adjust(DateTime(&serialBuffer[13], &serialBuffer[4]));
    Serial.println("OK");
  }
  else if(strstr(serialBuffer, "GET"))
  {
    now = rtc.now();
    sprintf(timestr, "%.2u:%.2u:%.2u", now.hour(), now.minute(), now.second());  
    sprintf(datestr, "%.2u %.2u %.4u", now.month(), now.day(), now.year());    
    Serial.print(timestr);
    Serial.print(" ");
    Serial.println(datestr);
  } 
}

// the 8 arrays that form each segment of the custom numbers
uint8_t bar1[8] = 
{
        B11100,
        B11110,
        B11110,
        B11110,
        B11110,
        B11110,
        B11110,
        B11100
};

uint8_t bar2[8] =
{
        B00111,
        B01111,
        B01111,
        B01111,
        B01111,
        B01111,
        B01111,
        B00111
};

uint8_t bar3[8] =
{
        B11111,
        B11111,
        B00000,
        B00000,
        B00000,
        B00000,
        B11111,
        B11111
};

uint8_t bar4[8] =
{
        B11110,
        B11100,
        B00000,
        B00000,
        B00000,
        B00000,
        B11000,
        B11100
};

uint8_t bar5[8] =
{
        B01111,
        B00111,
        B00000,
        B00000,
        B00000,
        B00000,
        B00011,
        B00111
};

uint8_t bar6[8] =
{
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B11111,
        B11111
};

uint8_t bar7[8] =
{
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00111,
        B01111
};

uint8_t bar8[8] =
{
        B11111,
        B11111,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000
};

void CreateCustomChars()
{
  // assignes each segment a write number
  lcd.createChar(1, bar1);
  lcd.createChar(2, bar2);
  lcd.createChar(3, bar3);
  lcd.createChar(4, bar4);
  lcd.createChar(5, bar5);
  lcd.createChar(6, bar6);
  lcd.createChar(7, bar7);
  lcd.createChar(8, bar8);
}

// uses segments to build the number 0
void custom0(int col)
{ 
  lcd.setCursor(col, 0); 
  lcd.write(2);  
  lcd.write(8); 
  lcd.write(1);
  lcd.setCursor(col, 1); 
  lcd.write(2);  
  lcd.write(6);  
  lcd.write(1);
}

void custom1(int col)
{
  lcd.setCursor(col,0);
  lcd.write(32);
  lcd.write(32);
  lcd.write(1);
  lcd.setCursor(col,1);
  lcd.write(32);
  lcd.write(32);
  lcd.write(1);
}

void custom2(int col)
{
  lcd.setCursor(col,0);
  lcd.write(5);
  lcd.write(3);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(2);
  lcd.write(6);
  lcd.write(6);
}

void custom3(int col)
{
  lcd.setCursor(col,0);
  lcd.write(5);
  lcd.write(3);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(7);
  lcd.write(6);
  lcd.write(1); 
}

void custom4(int col)
{
  lcd.setCursor(col,0);
  lcd.write(2);
  lcd.write(6);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(32);
  lcd.write(32);
  lcd.write(1);
}

void custom5(int col)
{
  lcd.setCursor(col,0);
  lcd.write(2);
  lcd.write(3);
  lcd.write(4);
  lcd.setCursor(col, 1);
  lcd.write(7);
  lcd.write(6);
  lcd.write(1);
}

void custom6(int col)
{
  lcd.setCursor(col,0);
  lcd.write(2);
  lcd.write(3);
  lcd.write(4);
  lcd.setCursor(col, 1);
  lcd.write(2);
  lcd.write(6);
  lcd.write(1);
}

void custom7(int col)
{
  lcd.setCursor(col,0);
  lcd.write(2);
  lcd.write(8);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(32);
  lcd.write(32);
  lcd.write(1);
}

void custom8(int col)
{
  lcd.setCursor(col, 0); 
  lcd.write(2);  
  lcd.write(3); 
  lcd.write(1);
  lcd.setCursor(col, 1); 
  lcd.write(2);  
  lcd.write(6);  
  lcd.write(1);
}

void custom9(int col)
{
  lcd.setCursor(col, 0); 
  lcd.write(2);  
  lcd.write(3); 
  lcd.write(1);
  lcd.setCursor(col, 1); 
  lcd.write(7);  
  lcd.write(6);  
  lcd.write(1);
}

void printBigNumber(int col, int value) 
{
  if (value == 0) {
    custom0(col);
  } if (value == 1) {
    custom1(col);
  } if (value == 2) {
    custom2(col);
  } if (value == 3) {
    custom3(col);
  } if (value == 4) {
    custom4(col);
  } if (value == 5) {
    custom5(col);
  } if (value == 6) {
    custom6(col);
  } if (value == 7) {
    custom7(col);
  } if (value == 8) {
    custom8(col);
  } if (value == 9) {
    custom9(col);
  }      
}  

void printBigString(int col, char* str, int size) 
{
  for(int i = 0; i < size; i++)
  {
    printBigNumber(col, str[i] - '0');
    col += 3;
  }
}
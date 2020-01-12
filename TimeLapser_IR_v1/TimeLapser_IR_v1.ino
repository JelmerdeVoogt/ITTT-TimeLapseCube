#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_64X32_1F_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//Period to elapse (for the timed function using 'Millis()')
unsigned long period = 0;
//A long in which the current time can be saved
unsigned long time_now = 0;
//Pin for the switch (Pullup button) and the led for displaying activity
const int Switch = 12, LED = 3;

//Pin to which the potmeter is connectet to
int potpin = 0;
//A long I save the (further calculated) current potmeter input to
unsigned long val;
//Interval in seconds
int seconds;
//Interval in minutes
int minutes;
//Bool to let the program know whether to have minute intervals or second intervals
bool shootingMinutes = false;
//Bool to be able to stop and start the time lapse
bool onOff;
//Bool to let the program know whether the button should be active or not
bool canChange;

void setup()
{
  u8g2.begin();
  u8g2.setCursor(0, 0);
  pinMode(Switch, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
}
void loop()
{
  setInterval();
  displayInterval();
  buttonToggle();
  
  if (onOff) 
  {
    digitalWrite(LED, HIGH);
    regulateInterval();
  } else if (!onOff) 
  {
    digitalWrite(LED, LOW);
  }
}

//Send IR code to receiver (Nikon D50)
void Pulse(void) 
{
  int i;
  for (i = 0; i < 76; i++) 
  {
    digitalWrite(2, HIGH);
    delayMicroseconds(7);
    digitalWrite(2, LOW);
    delayMicroseconds(7);
  }
  delay(27);
  delayMicroseconds(810);
  for (i = 0; i < 16; i++) 
  {
    digitalWrite(2, HIGH);
    delayMicroseconds(7);
    digitalWrite(2, LOW);
    delayMicroseconds(7);
  }
  delayMicroseconds(1540);
  for (i = 0; i < 16; i++) 
  {
    digitalWrite(2, HIGH);
    delayMicroseconds(7);
    digitalWrite(2, LOW);
    delayMicroseconds(7);
  }
  delayMicroseconds(3545);
  for (i = 0; i < 16; i++) 
  {
    digitalWrite(2, HIGH);
    delayMicroseconds(7);
    digitalWrite(2, LOW);
    delayMicroseconds(7);
  }
}

void displayInterval() 
{
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_inb24_mr);  
  if (shootingMinutes) 
  {
    if (minutes >= 10) 
    {
      u8g2.setCursor(1, 24);
    } else {
      u8g2.setCursor(11, 24);
    }
    u8g2.print(String(minutes) + "M");
  } else 
  {
    if (seconds >= 10) 
    {
      u8g2.setCursor(11, 24);
    } else {
      u8g2.setCursor(23, 24);
    }
    u8g2.print(seconds);
  }
  u8g2.sendBuffer();
}

void setInterval() 
{
  val = 1000 + ((analogRead(potpin) * 29.31) * 2);
  seconds = val / 1000;
  if (seconds > 30) 
  {
    shootingMinutes = true;
    minutes = (seconds - 30);
  }else
  {
    shootingMinutes = false;
  }
}

void buttonToggle()
{
if (digitalRead(Switch) == LOW && canChange) 
  {
    onOff = !onOff;
    canChange = false;
  }
  if (digitalRead(Switch) == HIGH) 
  {
    canChange = true;
  }
}

void regulateInterval()
{
    if (shootingMinutes) 
    {
      period = (minutes * 60000);
    } else 
    {
      period = (seconds * 1000);
    }

    if (millis() > time_now + period) 
    {
      time_now = millis();
      Pulse();
    }
}

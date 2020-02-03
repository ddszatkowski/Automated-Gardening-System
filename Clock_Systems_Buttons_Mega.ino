#include <DHT.h>
#include <Wire.h>
#include <Streaming.h>
#include <DS1307RTC.h>
#include <TimeAlarms.h>
#include <Time.h>
#include <TimeLib.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define fan 4
#define spkr 7

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int rightButton = 8;     // the number of the pushbutton pin
const int leftButton = 3;     // the number of the pushbutton pin
const int upButton = 5;     // the number of the pushbutton pin
const int downButton = 6;     // the number of the pushbutton pin

int buttonStateRight = 0;         // variable for reading the pushbutton status
int buttonStateLeft = 0;
int buttonStateUp = 0;
int buttonStateDown = 0;

bool right = 0;
bool left = 0;
bool up = 0;
bool down = 0;

int hold = 0;

int clockCursor = -1;
const int freq = 2700;
int fanCount = 11;
int maxHum = 60;
int maxTemp = 74;
int daycount = 1;
int sensorPin = A0; //moisture sensor
int sensorValue;  //moisture sensor 
int limit = 600; //moisture sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {     
  lcd.init();
  lcd.backlight();
  setTime(19,0,0,11,21,19);//set time for 4pm October 30th, 2019
  Alarm.alarmRepeat(13,0,0,FreqOn);
  Alarm.alarmRepeat(17,0,0,FreqOff);  // 4:00pm every day
  Alarm.alarmRepeat(10,0,0,WaterOn);  // 10:00am every day
  Alarm.timerRepeat(3600, FanOn);     //Repeats every hour 3600 secs
  Alarm.timerRepeat(60, FanOff); //Checks every minute if X minutes have passed since FanOn
  Alarm.timerRepeat(60, PrintLoop); //Prints shit every min
  pinMode(fan, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(spkr, OUTPUT);
  pinMode(rightButton, INPUT);
  pinMode(leftButton, INPUT);
  pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);
  Serial.begin(9600); 
  dht.begin();
}

void loop() {
  ButtonCheck();
  Alarm.delay(100);
  
}

void ButtonCheck(){
  buttonStateRight = digitalRead(rightButton);
  buttonStateLeft = digitalRead(leftButton);
  buttonStateUp = digitalRead(upButton);
  buttonStateDown = digitalRead(downButton);
  
  
  if (buttonStateRight == HIGH) {
    RightButton();
  } else if (buttonStateRight == LOW) {
    right = 0;
  }

  if (buttonStateLeft == HIGH) {
    LeftButton();
  } else {
    left = 0;
  }

  if (buttonStateUp == HIGH) {
    UpButton();
  } else {
    up = 0;
  }

  if (buttonStateDown == HIGH) {
      DownButton();
  } else {
    down = 0;
  }
  digitalClockDisplay();
}
void PrintLoop()
{
  //digitalClockDisplay();
  
  sensorValue = analogRead(sensorPin); 
  Serial.println("Analog Value : ");
  Serial.println(sensorValue);
    
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  float TempF = (t*1.8)+32;
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  if(h > maxHum || TempF > maxTemp) {
      digitalWrite(fan, HIGH);
      Serial.print("Turning on Fan it's hot! ");
  } else if(fanCount > 10){
        digitalWrite(fan, LOW);
  } 
  Serial.print("Day count: ");
  Serial.print(daycount);
  Serial.print(" \t");
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(TempF);
  Serial.println(" *F ");
}

void RightButton(){
    if (right == 0) {
      if(clockCursor < 7){
        clockCursor++;
        if(clockCursor == 2 or clockCursor == 5)
          clockCursor++;
      }
      else
        clockCursor = -1;
    }
    right = 1;
}

void LeftButton(){
    if (left == 0) {
      if(clockCursor > -1){
        clockCursor--;
        if(clockCursor == 2 or clockCursor == 5)
          clockCursor--;
      }
      else
        clockCursor = 7;
    }
    left = 1;
}

void UpButton(){
    if (up == 0) {
      if(clockCursor == 0)
        adjustTime(36000);
      else if(clockCursor == 1)
        adjustTime(3600);
      else if(clockCursor == 3)
        adjustTime(600);
      else if(clockCursor == 4)
        adjustTime(60);
      else if(clockCursor == 6)
        adjustTime(10);
      else if(clockCursor == 7)
        adjustTime(1);
    }
    up = 1;
}

void DownButton(){
  if (down == 0) {
      if(clockCursor == 0)
        adjustTime(-36000);
      else if(clockCursor == 1)
        adjustTime(-3600);
      else if(clockCursor == 3)
        adjustTime(-600);
      else if(clockCursor == 4)
        adjustTime(-60);
      else if(clockCursor == 6)
        adjustTime(-10);
      else if(clockCursor == 7)
        adjustTime(-1);
    }
    down = 1;
}
void FanOn() {
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  float TempF = (t*1.8)+32;
  Serial.print("Turning on Fan");
  digitalWrite(fan, HIGH);

  fanCount = 0;
}

void FanOff(){
  if(fanCount == 10){
    digitalWrite(fan, LOW);
    Serial.print("Fan Turned Off");
  }
  if(fanCount <= 11)
  {
    Serial.print("Fan on for ");
    Serial.print(fanCount);
    Serial.print("minutes.");
    fanCount++;
  }
}
//Timer for speaker
void FreqOn() {
  // write here the task to perform every morning
  Serial.println("Sound on");
  
  tone(spkr, freq);
}

void FreqOff() {
  //write here the task to perform every evening
  Serial.println("Sound off");
  noTone(spkr);
}

//Timer for watering system
void WaterOn() {
  Serial.println("Alarm: - its Raining!");
  Serial.println("daycount: ");
  Serial.println(daycount);
  if (daycount == 1 or 4) {
    if (sensorValue>limit) {
      digitalWrite(13, HIGH);
      delay(30000);
      digitalWrite(13, LOW); 
    }
    else {
      Serial.println("Plants have enough water");
    }
  }
  daycount = (daycount + 1);
  if (daycount > 7){
  daycount = 1;
  }
  Serial.println("daycount: ");
  Serial.println(daycount);
}


void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  lcd.setCursor(0,0);
  if(hour() < 10)
  {
    lcd.print(0);
    lcd.setCursor(1,0);
  }
  lcd.print(hour());
  printLCDmin(minute());
  printLCDsec(second());
  Serial.println();
  lcd.setCursor(0, 1);
  lcd.print("                ");
  if(clockCursor >= 0)
  {
    lcd.setCursor(clockCursor, 1);
    lcd.print('^');
  }
}

void printDigits(int digits) {
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void printLCDmin(int digits) {
  lcd.setCursor(2,0);
  lcd.print(":");
  lcd.setCursor(3,0);
  if (digits < 10)
  {
    lcd.print('0');
    lcd.setCursor(4,0);
  }
  lcd.print(digits);
}

void printLCDsec(int digits) {
  lcd.setCursor(5,0);
  lcd.print(":");
  lcd.setCursor(6,0);
  if (digits < 10)
  {
    lcd.print('0');
    lcd.setCursor(7,0);
  }
  lcd.print(digits);
}

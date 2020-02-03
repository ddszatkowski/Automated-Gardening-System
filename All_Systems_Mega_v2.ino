#include <DHT.h>
#include <Wire.h>
#include <Streaming.h>
#include <DS1307RTC.h>
#include <TimeAlarms.h>
#include <Time.h>
#include <TimeLib.h>

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define fan 4
#define spkr 7

const int freq = 2700;
int maxHum = 60;
int maxTemp = 74;
int daycount = 1;
int sensorPin = A0; //moisture sensor
int sensorValue;  //moisture sensor 
int limit = 600; //moisture sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {     
  
  setTime(12,57,0,11,4,19);//set time for 4pm October 30th, 2019
  Alarm.alarmRepeat(13,0,0,FreqOn);
  Alarm.alarmRepeat(17,0,0,FreqOff);  // 4:00pm every day
  Alarm.alarmRepeat(10,0,0,WaterOn);  // 10:00am every day
  Alarm.timerRepeat(6, FanOn);     //Repeats every hour 3600 secs
  Alarm.timerRepeat(60, PrintLoop); //Prints shit every min
  pinMode(fan, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(spkr, OUTPUT);
  Serial.begin(9600); 
  dht.begin();
}

void loop() {
 
  Alarm.delay(100);
  
}

void PrintLoop()
{
  digitalClockDisplay();
  
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
  } else{
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


void FanOn() {
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  float TempF = (t*1.8)+32;
  Serial.print("Turning on Fan");
  digitalWrite(fan, HIGH);

  //15 minutes is 900000 miliseconds
  delay(900000);
  
  digitalWrite(fan, LOW);
  Serial.print("Fan Turned Off");
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
  Serial.println();
}

void printDigits(int digits) {
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

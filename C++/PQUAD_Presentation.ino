#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Time.h>
#include <SD.h>
#include <SPI.h>
#include <avr/interrupt.h>

#include <dht11.h>
#include <stdlib.h>
#include <DS3231.h> //real time clock module 
#include <RTClib.h>

LiquidCrystal lcd(23, 24, 25, 26, 27, 28); //PCB pins used

dht11 DHT; //Create DHT object
#define DHT11_PIN 22 //PCB pin for DATA = 22

SoftwareSerial bluetooth(17, 16); // (RX2, TX2) = (17, 16) :: (RX3, TX3) = (15, 14)
int bt_EN = 43; //bt_enable = PIN43

RTC_DS3231 rtc; //RTC object
const byte rtcTimerIntPin = 2; //need to figure out what pin this should tie to for the mega/PCB
volatile byte flag = 0; //needs to be volitaile and init to false

//Interrupt Pins
//const byte sendPin = 16;
const byte printPin = 18;


//Pin declarations
int lightPin = A0; //Light data pin = 0
int soilPin = A2; //Soil data pin = 2

//These variables will be used as the most recent readings
static volatile int tempRead = 0;
static volatile int humRead = 0;
static volatile int lightRead = 0;
static volatile int soilRead = 0;

const byte intPin = 3;
void setup() {
  Serial.begin(9600); //9600 baud rate i presume might need to happen first

  //Init lcd and remove garbage values on screen. setup = .begin(col, row)
  lcd.begin(16, 2); //
  lcd.clear();

  //Parity check on DHT11 sensor
  checkDHT();

  //Setup for Bluetooth module
  bluetooth.begin(9600);
  pinMode(bt_EN, OUTPUT);
  digitalWrite(bt_EN, LOW);

  //Use compile time for the calibration of RTC module
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  if (!rtc.begin()) {
    lcd.println("Couldn't find RTC");
  }
  attachInterrupt(digitalPinToInterrupt(rtcTimerIntPin), rtc_interrupt, CHANGE);

  //Button display setup
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(intPin), pressed, CHANGE);
  lcd.clear();

}

void rtc_interrupt ()
{
  flag = 1; // end of rtc_interrupt
}

void loop() {

  DateTime current = rtc.now();
  if (current.second() % 30 == 0) {
    printInterrupt();
    sendInterrupt();
  }
  lcd.clear();
  delay(1000);
}

void getSensorData ( void ) {
  tempRead = DHT.temperature;
  humRead = DHT.humidity;
  lightRead = getLight();
  soilRead = getSoil();
}

//Attach interrupt here
void sendInterrupt ( void ) {
  getSensorData();
  char buffer[10];
  bluetooth.write(itoa((lightRead), buffer, 10));
  bluetooth.write(",");
  bluetooth.write(itoa((soilRead), buffer, 10));
  bluetooth.write(",");
  bluetooth.write(itoa((tempRead), buffer, 10));
  bluetooth.write(",");
  bluetooth.write(itoa((humRead), buffer, 10));
  bluetooth.write(",");
  
  //Check for conditions that might harm the plant
  if (  lightRead < 10  ) {
      bluetooth.write("1"); //send the signal for light 
  }
  else {
    bluetooth.write("0");
  }

  if ( soilRead < 30 || soilRead > 90) {
      bluetooth.write("1"); //send the signal for soil
  }
  else {
    bluetooth.write("0");
  }
  
  if ( (tempRead > 32) || (tempRead < 16) ) {
      bluetooth.write("1"); //send the signal for temp
  }
  else {
    bluetooth.write("0");
  }
  if ( humRead < 10 || humRead > 70 ) {
      bluetooth.write("1"); //send the signal for humidity
  }
  else {
    bluetooth.write("0");
  }
  
  bluetooth.write("\n");
  delay(1000);
}

//Function scrolls LCD screen to the left.
void scrollLeft () {
  delay(500);
  for (int i = 0; i < 16 ; i++)
  {
    delay(200);
    lcd.scrollDisplayLeft();
  }
  lcd.scrollDisplayLeft();
}

void printInterrupt ( void ) {
  DateTime today = rtc.now(); //record time of loop start

  lcd.setCursor(0, 0);
  lcd.print(today.month());
  lcd.print("/");
  lcd.print(today.day());
  lcd.print("/");
  lcd.print(today.year());
  delay(1000);
  lcd.clear();
  lcd.print("Light: ");
  lcd.print(getLight());
  lcd.setCursor(0, 1);
  lcd.print("Moisture: ");
  lcd.print(getSoil());
  scrollLeft();
  lcd.clear();
  lcd.setCursor(0, 0); //intialize cursor to postion col 0 row 0
  lcd.print("Temperature: "); //not sure if ardunio allows %d or concatination
  lcd.print(DHT.temperature);
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(DHT.humidity);
  scrollLeft();
  lcd.clear();
  delay(2000);
}

//Attach interrupt here
void sleepMode ( void ) {
  //Insert Code here
}


int getLight(void) {
  lightRead = analogRead(lightPin); //reads the sensor and stores the reading
  return lightRead;
}

int getSoil(void) {
  soilRead = analogRead(soilPin); //get val
  return soilRead;
}

void checkDHT ( void ) {
  int check;
  Serial.print("DHT11 STATUS – \t");
  check = DHT.read(DHT11_PIN);
  //check status
  switch (check)
  {
    case DHTLIB_OK:
      Serial.print("OK\n");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print("Checksum error\n");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print("Timeout error\n");
      break;
    default:
      Serial.print("Unknown error\n");
      break;
  }
}
void pressed () {
  lcd.print("FUCK");
  
}

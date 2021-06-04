#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
//setup software serial
SoftwareSerial btooth(3,4);
char BluetoothData;
char tempMode;
char curMode;

// Setup LCD
LiquidCrystal_I2C lcd(0x27,20,4);

//setting up pins for connected buttons
int upButton = 5;
int downButton = 6;
int stateUpButton = 0;
int stateDownButton = 0;

//setting up pin for indicator LED (this will be changed to relay for heaters)
const int ledPin =  7;
bool heaterOn = false;


//set target temperature on boot
int targetTemp = 37;

// Digital pin connected to the DHT sensor
#define DHTPIN 2

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    tempMode = 'f';
    //setting up connected hardware buttons
    pinMode(ledPin, OUTPUT);
    pinMode(upButton, INPUT);
    pinMode(downButton, INPUT);

    //initialize sensor and lCD

    dht.begin();
    lcd.begin();
    lcd.backlight();

//init serial
    btooth.begin(9600);
    btooth.println("Bluetooth On...");

    // Turn on backlight
    lcd.backlight();

    curMode = 'a';
}

void loop() {

    //read up and down buttons and adjust the temperature
    stateUpButton = digitalRead(upButton);
    stateDownButton = digitalRead(downButton);
    if (stateUpButton == HIGH) {
        targetTemp++;
        delay(50);
    }
    if (stateDownButton == HIGH) {
        targetTemp--;
        delay(50);
    }

    if (btooth.available()) {
        BluetoothData=btooth.read();
    }
    if (BluetoothData == 'f') {
        tempMode = 'f';
    }
    if (BluetoothData == 'c') {
        tempMode = 'c';
    }
    // add 1 second delay for slow sensor
    delay(200);

    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);


    // Check if any reads failed and exit early (to try again).
    if (isnan(f)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }

    //compare target temp to current temp and activate LED (heater)
    if (targetTemp <= f) {
        digitalWrite(ledPin, HIGH);
        heaterOn = true;
    }
    else {
        digitalWrite(ledPin, LOW);
        heaterOn=false;
    }




    lcd.setCursor(0,0);
    lcd.print("Target Temp: ");
    lcd.print(targetTemp);
    lcd.print("F");
    lcd.setCursor(0,1);
    lcd.print("Current Temp: ");
    lcd.print(f);
    lcd.print("F");
    lcd.setCursor(0,3);
    if (heaterOn) {
        lcd.print("            Cooling");
    }
    if (!heaterOn) {
        lcd.print("                   ");

    }
}
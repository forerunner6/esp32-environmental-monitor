#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long previousSensorTime = 0;
const unsigned long sensorInterval = 1000;
const unsigned long oneSecondInterval = 1000;

unsigned long previousDisplayTime = 0;
const unsigned long displayInterval = 250;

unsigned long previousConnectionCheckTime = 0;

float temperature = 0.0;
float humidity = 0.0;
float pressure = 0.0;

bool bmeDataValid = false;
bool bmeInitialized = false;
bool displayInitialized = false;
bool bmeConnected = false;
bool displayConnected = false;
bool bmeWasDisconnected = false;
bool displayWasDisconnected = false;

Adafruit_BME280 bme; 

void readSensorValues();
void printValues();
void updateDisplay();
void configureDisplay();
bool checkI2C(uint8_t address);

void setup() {

    Serial.begin(115200);
    Wire.begin(14,13);

    bmeInitialized = bme.begin(0x76);
    displayInitialized = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    bmeConnected = checkI2C(0x76);
    displayConnected = checkI2C(0x3C);


    if (!displayInitialized) {
        Serial.println("DISPLAY ERROR");
    }

    if (displayInitialized){
        configureDisplay();
        display.display();

    }  

    if (!bmeInitialized) {
        Serial.println("BME280 SENSOR FAILED");
    }

    Serial.println();
}


void loop() { 
    unsigned long currentTime = millis();

    if ((currentTime - previousConnectionCheckTime) >= oneSecondInterval){
        bmeConnected = checkI2C(0x76);
        displayConnected = checkI2C(0x3C);

        if (!bmeConnected){
            bmeInitialized = false;
            bmeDataValid = false;
        }
        if (!displayConnected){
            displayInitialized = false;
        }
        if (bmeConnected){
            if (bmeWasDisconnected){
                Serial.println("SENSOR RECONNECTED");
                bmeWasDisconnected = false;
            }
            if (!bmeInitialized){
                Serial.println("Sensor detected, attempting initialization...");
                bmeInitialized = bme.begin(0x76);
                if (bmeInitialized){
                    Serial.println("Sensor Initialization Successful");
                }
            }
        }
        if (displayConnected){
            if (displayWasDisconnected){
                Serial.println("DISPLAY CONNECTED");
                displayWasDisconnected = false;
            }
            if (!displayInitialized){
                Serial.println("Display detected, attempting initialization...");
                displayInitialized = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
                if (displayInitialized){
                    Serial.println("Display Initialization Successful");
                    configureDisplay();
                    display.display();

                }
            }
        }
        previousConnectionCheckTime = currentTime;
    }

    if (bmeConnected){
        if ((currentTime - previousSensorTime) >= sensorInterval){
            if (bmeInitialized){
                readSensorValues();
                printValues();
            }
        previousSensorTime = currentTime;
        }
    }
    else{
        if (!bmeWasDisconnected){
            Serial.println("SENSOR DISCONNECTED");
            bmeWasDisconnected = true;
        }
    }


    if (displayConnected){
        if ((currentTime - previousDisplayTime) >= displayInterval){
            if (displayInitialized){
                updateDisplay();
            }
        previousDisplayTime = currentTime;
        }
    }
    else{
        if (!displayWasDisconnected){
            Serial.println("DISPLAY DISCONNECTED");
            displayWasDisconnected = true;
        }

    }

}

void readSensorValues(){
    float temperatureCheck = (bme.readTemperature() * 1.8) + 32;
    float humidityCheck = bme.readHumidity();
    float pressureCheck = bme.readPressure() / 100.0F;

    if ((isfinite(temperatureCheck)) && (isfinite(humidityCheck)) && (isfinite(pressureCheck))){
        bmeDataValid = true;
    }
    else {
        bmeDataValid = false;
    }

    if (bmeDataValid){
        temperature = temperatureCheck;
        humidity = humidityCheck;
        pressure = pressureCheck;
    }


}


void printValues() {

    if (bmeDataValid){
        Serial.print("Temperature = ");
        Serial.print(temperature);
        Serial.println(" °F");

        Serial.print("Humidity = ");
        Serial.print(humidity);
        Serial.println(" %");

        Serial.print("Pressure = ");
        Serial.print(pressure);
        Serial.println(" hPa");

        Serial.println();
    }
    else {
        Serial.println("Invalid Read! Showing last valid read.");

        Serial.print("Temperature = ");
        Serial.print(temperature);
        Serial.println(" °F");

        Serial.print("Humidity = ");
        Serial.print(humidity);
        Serial.println(" %");

        Serial.print("Pressure = ");
        Serial.print(pressure);
        Serial.println(" hPa");

        Serial.println();
    }
}

void updateDisplay(){
    configureDisplay();
    
    if (bmeInitialized && bmeConnected){

        if (bmeDataValid){

            display.print("Temp: ");
            display.print(temperature);
            display.println(" F");

            display.print("Humidity: ");
            display.print(humidity);
            display.println(" %");

            display.print("Pressure: ");
            display.print(pressure);
            display.println(" hPa");
        }
        else{
            display.println("INVALID READ");
            display.println("Last Valid: ");

            display.print("Temp: ");
            display.print(temperature);
            display.println(" F");

            display.print("Humidity: ");
            display.print(humidity);
            display.println(" %");

            display.print("Pressure: ");
            display.print(pressure);
            display.println(" hPa");
        }
    }

    else {
        display.print("SENSOR FAILED");
    }
    display.display();
}

void configureDisplay(){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
}

bool checkI2C(uint8_t address){

    Wire.beginTransmission(address);
    uint8_t status = Wire.endTransmission();

    if (!status){
        return true;
    }
    else{
        return false;
    }
}
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
const uint8_t BME_ADDRESS = 0x76;
const uint8_t DISPLAY_ADDRESS = 0x3C;
const uint8_t SDA_PIN = 14;
const uint8_t SCL_PIN = 13;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long previousBmeReadTime = 0;
const unsigned long bmeReadInterval = 1000;
const unsigned long connectionCheckInterval = 1000;

unsigned long previousDisplayTime = 0;
const unsigned long displayInterval = 250;

unsigned long previousConnectionCheckTime = 0;

float temperature = 0.0;
float humidity = 0.0;
float pressure = 0.0;

const float BME_TEMP_MIN_C = -40.0F;
const float BME_TEMP_MAX_C = 85.0F;
const float BME_HUMIDITY_MIN = 0.0F;
const float BME_HUMIDITY_MAX = 100.0F;
const float BME_PRESSURE_MIN_HPA = 300.0F;
const float BME_PRESSURE_MAX_HPA = 1100.0F;

bool bmeDataValid = false;
bool bmeInitialized = false;
bool displayInitialized = false;
bool bmeConnected = false;
bool displayConnected = false;
bool bmeWasDisconnected = false;
bool displayWasDisconnected = false;

Adafruit_BME280 bme; 

void readBmeValues();
void printValues();
void updateDisplay();
void configureDisplay();
void updateConnections(unsigned long currentTime);
void updateBmeSensor(unsigned long currentTime);
void updateDisplayTask(unsigned long currentTime);
bool checkI2C(uint8_t address);
bool isBmeReadingValid(float temperatureC, float humidityPercent, float pressureHpa);

void setup() {

    Serial.begin(115200);
    Wire.begin(SDA_PIN,SCL_PIN);

    bmeInitialized = bme.begin(BME_ADDRESS);
    displayInitialized = display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
    bmeConnected = checkI2C(BME_ADDRESS);
    displayConnected = checkI2C(DISPLAY_ADDRESS);


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

    updateConnections(currentTime);
    updateBmeSensor(currentTime);
    updateDisplayTask(currentTime);

}

void updateConnections(unsigned long currentTime){

    if ((currentTime - previousConnectionCheckTime) < connectionCheckInterval){
        return;
    }

    bmeConnected = checkI2C(BME_ADDRESS);
    displayConnected = checkI2C(DISPLAY_ADDRESS);
    
    if (!bmeConnected){
        bmeInitialized = false;
        bmeDataValid = false;
        if (!bmeWasDisconnected){
            Serial.println("SENSOR DISCONNECTED");
            bmeWasDisconnected = true;
        }
    }
    if (!displayConnected){
        displayInitialized = false;
        if (!displayWasDisconnected){
            Serial.println("DISPLAY DISCONNECTED");
            displayWasDisconnected = true;
        }
    }
    if (bmeConnected){
        if (bmeWasDisconnected){
            Serial.println("SENSOR RECONNECTED");
            bmeWasDisconnected = false;
        }
        if (!bmeInitialized){
            Serial.println("Sensor detected, attempting initialization...");
            bmeInitialized = bme.begin(BME_ADDRESS);
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
            displayInitialized = display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
            if (displayInitialized){
                Serial.println("Display Initialization Successful");
                configureDisplay();
                display.display();

            }
        }
    }
    previousConnectionCheckTime = currentTime; 
}

void updateBmeSensor(unsigned long currentTime){
    if (bmeConnected &&
        ((currentTime - previousBmeReadTime) >= bmeReadInterval) &&
        bmeInitialized){

        readBmeValues();
        printValues();
            
        previousBmeReadTime = currentTime;
    }
    
}

void updateDisplayTask(unsigned long currentTime){
    if (displayConnected &&
        ((currentTime - previousDisplayTime) >= displayInterval) &&
        displayInitialized){

        updateDisplay();
        previousDisplayTime = currentTime;
    }
}


void readBmeValues(){
    float temperatureCheck = bme.readTemperature();
    float humidityCheck = bme.readHumidity();
    float pressureCheck = bme.readPressure() / 100.0F;

    bmeDataValid = isBmeReadingValid(temperatureCheck, humidityCheck, pressureCheck);

    if (bmeDataValid){
        temperature = (temperatureCheck * 1.8) + 32;
        humidity = humidityCheck;
        pressure = pressureCheck;
    }


}

bool isBmeReadingValid(float temperatureC, float humidityPercent, float pressureHpa){
    
    if (!((isfinite(temperatureC)) && (isfinite(humidityPercent)) && (isfinite(pressureHpa)))){
        return false;
    }
    if (!((temperatureC >= BME_TEMP_MIN_C && temperatureC <= BME_TEMP_MAX_C) && 
        (humidityPercent >= BME_HUMIDITY_MIN && humidityPercent <= BME_HUMIDITY_MAX) && 
        (pressureHpa >= BME_PRESSURE_MIN_HPA && pressureHpa <= BME_PRESSURE_MAX_HPA))){
        return false;
    }

    return true;
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
    display.clearDisplay();
    display.setCursor(0, 0);

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
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.clearDisplay();
    display.setCursor(0, 0);
}

bool checkI2C(uint8_t address){

    Wire.beginTransmission(address);
    uint8_t status = Wire.endTransmission();

    return status == 0;
}
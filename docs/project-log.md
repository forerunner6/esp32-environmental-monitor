# Project Log

## Prototype Bring-Up

### ESP32 Setup
- Configured Arduino IDE for ESP32-S3.
- Verified USB-UART upload over COM3.
- Tested Serial communication at 115200 baud.
- Practiced GPIO input/output using the onboard BOOT button and LED.
- Replaced blocking `delay()` timing with `millis()`-based timing.

### BME280 Sensor
- Soldered the 6-pin header onto the BME280 breakout.
- Connected using I2C:
  - SDA: GPIO14
  - SCL: GPIO13
- Used an I2C scanner to identify the sensor at address `0x76`.
- Adafruit BME280 example initially failed because it used the default address.
- Changed initialization to `bme.begin(0x76)`.
- Successfully read temperature, humidity, and pressure.

### SSD1306 OLED
- Added a 128x64 SSD1306 OLED to the same I2C bus.
- I2C scanner identified the OLED at address `0x3C`.
- Added Adafruit GFX and SSD1306 libraries.
- Display now shows temperature, humidity, and pressure.
- Sensor readings update every 1000 ms.
- Display refreshes independently every 250 ms.

### I2C Fault Detection and Recovery
- Added independent I2C presence checks for the BME280 at `0x76` and the OLED at `0x3C` every 1000 ms.
- Tracked connection and initialization state separately for each device.
- When a device stops responding, its connection and initialization states are cleared and a single disconnection message is sent to Serial.
- When the BME280 is unavailable, the working OLED displays `SENSOR FAILED` instead of stale readings.
- When a device responds again, the firmware reports the reconnection, retries initialization, and resumes normal operation after initialization succeeds.
- OLED configuration is restored after the display reconnects and is reinitialized.
- Tested the recovery paths by disconnecting and reconnecting each device's SDA jumper separately while the system was running.
- Confirmed successful BME280 and OLED fault detection, reinitialization, and recovery without restarting the ESP32.

### Fan Driver Experiment
- Built a 5 V fan driver using:
  - S8050 NPN transistor
  - 220 ohm base resistor
  - 10 kohm base pull-down resistor
  - flyback diode
  - GPIO4 control
- Verified the ESP32 can switch the fan on and off.
- Fan experiment was primarily used to learn transistor switching and inductive-load protection.

## Current Direction
The project is shifting toward a compact consumer-style indoor environmental monitor with:
- environmental sensing
- CO2 sensing
- Wi-Fi data logging
- battery operation
- custom PCB
- enclosure design
- manufacturability and BOM constraints
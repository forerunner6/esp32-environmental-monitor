# ESP32 Environmental Monitor

A developing ESP32-S3 environmental monitoring project focused on embedded systems, sensor integration, power efficiency, manufacturability, and eventual custom PCB design.

## Current Prototype

The current breadboard prototype includes:

- ESP32-S3
- BME280 temperature, humidity, and pressure sensor
- 128x64 SSD1306 OLED
- Shared I2C bus
- Non-blocking timing using `millis()`
- Experimental 5 V fan driver using an S8050 transistor

## Current Measurements

- Temperature
- Humidity
- Atmospheric pressure

## Project Direction

The final goal is a compact consumer-style indoor environmental monitor with:

- CO2 / air-quality sensing
- Wi-Fi data logging
- battery operation
- custom PCB
- enclosure design
- attention to BOM cost and manufacturability

## Development Log

Detailed progress, debugging, and design decisions are recorded in:

`docs/project-log.md`
# ESP-IR-Automation

This code is designed for deployment on an ESP-32-based system equipped with a temperature sensor, SD card module, and RTC module (refer to the provided connection schematic). The system seamlessly records temperature data at regular intervals onto the SD card. It boasts a feature to serve a 24-hour graph of temperature readings via the ESP-32 web server, offers remote control over infrared (IR) appliances through a companion app, and allows users to establish rules and instructions for scheduled activation/deactivation or temperature adjustments of appliances such as air conditioners and fans.

## Features

- **Temperature Logging**: Periodically records temperature data onto the SD card.
- **24-Hour Graph**: Provides a graphical representation of temperature readings over the past 24 hours.
- **Remote IR Appliance Control**: Allows remote control of IR appliances through a companion app.
- **Scheduled Rules**: Enables users to set rules and instructions for scheduled activation/deactivation or temperature adjustments of appliances.

## Companion APP

The companion app is responsible for serving the 24 hour graphs, adding rules and directly controlling the IP appliances.
More info here : [_ESP-IR-Companion-App_](https://github.com/Tombstone2K/ESP-IR-Companion-App)

## Getting Started

### Schematic

![esp_breaboard_connections_bb_latest](https://github.com/Tombstone2K/ESP-IR-Automation/assets/74809929/c52c87e1-4b34-4f9d-9d66-2d2926f47925)

### Extract IR Codes

Extract IR Codes of various appliances using [_this code_](https://github.com/Tombstone2K/Get-IR-Codes-Arduino/)

### Installation

Clone the repository and upload to the ESP-32 through PlatformIO on VSCode

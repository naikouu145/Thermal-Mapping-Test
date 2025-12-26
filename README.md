# Thermal-Mapping-Test

A Python application for real-time thermal imaging visualization using the MLX90640 infrared thermal camera connected via Arduino.

## Overview

This project reads thermal data from an MLX90640 32x24 pixel infrared thermal sensor connected to an Arduino and displays it as a real-time thermal heatmap using Matplotlib.

## Features

- Real-time thermal image display (24x32 resolution)
- Dynamic temperature range adjustment
- Multiple colormap options for visualization (inferno, hot, jet, etc.)
- Live temperature colorbar with °C scale

## Hardware Requirements

- Arduino (with MLX90640 library)
- MLX90640 IR Thermal Camera Sensor
- USB connection to PC

## Arduino Libraries

The following libraries must be installed in the Arduino IDE:

1. **Adafruit MLX90640** - Main driver for the thermal sensor
   - Install via: `Sketch > Include Library > Manage Libraries > Search "Adafruit MLX90640"`
   - GitHub: https://github.com/adafruit/Adafruit_MLX90640

2. **Adafruit BusIO** - Required dependency for I2C communication
   - Install via: `Sketch > Include Library > Manage Libraries > Search "Adafruit BusIO"`

3. **Wire** - Built-in Arduino I2C library (included with Arduino IDE)

### Arduino Wiring

| MLX90640 Pin | Arduino Pin |
|--------------|-------------|
| VIN          | 3.3V        |
| GND          | GND         |
| SDA          | SDA (G1)    |
| SCL          | SCL (G2)    |

> **Note:** The MLX90640 operates at 3.3V logic. If using a 5V Arduino, ensure your sensor breakout board has level shifting or use a 3.3V Arduino variant.

## Software Requirements

- Python 3.x
- Dependencies listed in `requirements.txt`

## Installation

1. Clone this repository
2. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```
3. Upload the appropriate Arduino sketch to your board (must output data in `START,...768 values...,END` format)

## Usage

1. Connect your Arduino with the MLX90640 sensor
2. Update `SERIAL_PORT` in `thermal_viewer.py` to match your Arduino's COM port
3. Run the viewer:
   ```bash
   python thermal_viewer.py
   ```

## Serial Protocol

The Arduino should send data in the following format:
- Send `READY` when the sensor is initialized
- Send thermal frames as: `START,<768 comma-separated temperature values>,END`
- Send `ERROR:<message>` if initialization fails

## Configuration

In `thermal_viewer.py`, you can adjust:
- `SERIAL_PORT` - Your Arduino's COM port
- `BAUD_RATE` - Serial communication speed (default: 115200)
- `cmap` - Colormap for thermal visualization
- `vmin/vmax` - Initial temperature range bounds
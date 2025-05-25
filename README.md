# ESP32 Sensor Monitoring Firmware

Firmware for ESP32 that reads sensor data and sends it to the API. Supports multiple sensor types and provides stable WiFi connection.

## Supported Sensors

- Temperature (DHT22)
- Humidity (DHT22)
- Light (LDR)
- Ammonia (MQ-137)
- CO2 (MQ-135)
- RFID (RC522)

## Hardware Requirements

- ESP32
- Sensors:
  - DHT22 (Temperature and Humidity)
  - LDR (Light)
  - MQ-137 (Ammonia)
  - MQ-135 (CO2)
  - RC522 (RFID)

## Connections

### DHT22
- VCC -> 3.3V
- DATA -> GPIO 4
- GND -> GND

### LDR
- VCC -> 3.3V
- OUT -> GPIO 34
- GND -> GND

### MQ-137 (Ammonia)
- VCC -> 5V
- OUT -> GPIO 35
- GND -> GND

### MQ-135 (CO2)
- VCC -> 5V
- OUT -> GPIO 36
- GND -> GND

### RC522 (RFID)
- VCC -> 3.3V
- RST -> GPIO 22
- GND -> GND
- MISO -> GPIO 19
- MOSI -> GPIO 23
- SCK -> GPIO 18
- SDA -> GPIO 21

## Installation

1. Install Arduino IDE
2. Install ESP32 support:
   - Open Arduino IDE
   - Go to File -> Preferences
   - Add URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Go to Tools -> Board Manager
   - Search for "esp32" and install

3. Install required libraries:
   - DHT sensor library
   - ArduinoJson

4. Configure the code:
   - Open `esp32_sensor_client.ino`
   - Modify WiFi variables:
     ```cpp
     const char* ssid = "YOUR_WIFI_SSID";
     const char* password = "YOUR_WIFI_PASSWORD";
     const char* serverName = "http://YOUR_API_IP:8000/enviroment_readings/";
     ```

5. Upload the code:
   - Connect ESP32
   - Select correct board in Tools
   - Select correct port
   - Click Upload

## Configuration

### Reading Interval
By default, sensors are read every 5 seconds. To modify:
```cpp
const int readingInterval = 5000; // 5 seconds
```

### Sensor Calibration
MQ sensors require calibration. Adjust values in:
```cpp
const float MQ137_R0 = 1.0; // Adjust according to calibration
const float MQ135_R0 = 1.0; // Adjust according to calibration
```

## Troubleshooting

1. **If WiFi doesn't connect**
   - Verify credentials are correct
   - Check if ESP32 is in range
   - Verify router accepts the connection

2. **If data is not sent**
   - Verify API is accessible
   - Check if API IP is correct
   - Verify sensors are connected properly

3. **If readings are incorrect**
   - Calibrate MQ sensors
   - Verify connections
   - Check power supply

## Contact

For support or questions, contact:
- GitHub: [difermon09](https://github.com/difermon09) 
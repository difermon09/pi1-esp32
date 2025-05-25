# Firmware ESP32 para Monitoreo de Sensores

Firmware para ESP32 que lee datos de sensores y los envía a la API. Soporta múltiples tipos de sensores y proporciona una conexión WiFi estable.

## Sensores Soportados

- Temperatura (DHT22)
- Humedad (DHT22)
- Luz (LDR)
- Amoniaco (MQ-137)
- CO2 (MQ-135)
- RFID (RC522)

## Requisitos de Hardware

- ESP32
- Sensores:
  - DHT22 (Temperatura y Humedad)
  - LDR (Luz)
  - MQ-137 (Amoniaco)
  - MQ-135 (CO2)
  - RC522 (RFID)

## Conexiones

### DHT22
- VCC -> 3.3V
- DATA -> GPIO 4
- GND -> GND

### LDR
- VCC -> 3.3V
- OUT -> GPIO 34
- GND -> GND

### MQ-137 (Amoniaco)
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

## Instalación

1. Instalar el IDE de Arduino
2. Instalar el soporte para ESP32:
   - Abrir Arduino IDE
   - Ir a Archivo -> Preferencias
   - Añadir URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Ir a Herramientas -> Gestor de Tarjetas
   - Buscar "esp32" e instalar

3. Instalar las librerías necesarias:
   - DHT sensor library
   - ArduinoJson

4. Configurar el código:
   - Abrir `esp32_sensor_client.ino`
   - Modificar las variables de WiFi:
     ```cpp
     const char* ssid = "TU_WIFI_SSID";
     const char* password = "TU_WIFI_PASSWORD";
     const char* serverName = "http://TU_IP_API:8000/enviroment_readings/";
     ```

5. Subir el código:
   - Conectar la ESP32
   - Seleccionar la placa correcta en Herramientas
   - Seleccionar el puerto correcto
   - Hacer clic en Subir

## Configuración

### Intervalo de Lectura
Por defecto, los sensores se leen cada 5 segundos. Para modificar:
```cpp
const int readingInterval = 5000; // 5 segundos
```

### Calibración de Sensores
Los sensores MQ requieren calibración. Ajustar los valores en:
```cpp
const float MQ137_R0 = 1.0; // Ajustar según calibración
const float MQ135_R0 = 1.0; // Ajustar según calibración
```

## Solución de Problemas

1. **Si no se conecta al WiFi**
   - Verificar que las credenciales son correctas
   - Comprobar que la ESP32 está en rango
   - Verificar que el router acepta la conexión

2. **Si no envía datos**
   - Verificar que la API está accesible
   - Comprobar que la IP de la API es correcta
   - Verificar que los sensores están conectados correctamente

3. **Si las lecturas son incorrectas**
   - Calibrar los sensores MQ
   - Verificar las conexiones
   - Comprobar la alimentación

## Contacto

Para soporte o preguntas, contacta con:
- GitHub: [difermon09](https://github.com/difermon09) 
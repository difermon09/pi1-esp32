#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <DHT.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ---------- Configuración de tiempos ----------
const unsigned long INTERVALO_LECTURA_SENSORES = 30000; // Intervalo de lectura (1 minuto)
const unsigned long INTERVALO_LECTURA_RFID = 30000;     // Intervalo de lectura RFID (30 segundos)
// 60000 = 1 minuto
// 300000 = 5 minutos
// 900000 = 15 minutos
// 1800000 = 30 minutos
// 3600000 = 1 hora
// 7200000 = 2 horas
// 14400000 = 4 horas
// 28800000 = 8 horas
// 43200000 = 12 horas
// 86400000 = 24 horas

unsigned long ultimaLectura = 0;
unsigned long ultimaLecturaRFID = 0;

// ---------- WiFi ----------
const char* ssid = "Aaa";
const char* password = "Barcelona";
const char* api_url = "http://10.48.50.55:8000";  // IP local del ordenador

// --- SENSOR DE LLUM (I2C) ---
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(0x39);

// --- SENSOR DE TEMPERATURA I HUMITAT ---
const int DHT_pin = 13;
const uint8_t DHT_type = DHT11;
DHT dht(DHT_pin, DHT_type);

// --- CONFIGURACIÓ DEL MODUL RFID RC522 (SPI) ---
#define SS_PIN 5     // SDA del RC522
#define RST_PIN 4    // RST del RC522
#define SCK_PIN 14
#define MISO_PIN 12
#define MOSI_PIN 23
MFRC522 rfid(SS_PIN, RST_PIN); 

// ========================= FUNCIONES API =========================
void enviarDatosSensor(int sensor_id, float valor) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(api_url) + "/enviroment_readings/";
        
        StaticJsonDocument<100> doc;
        doc["sensor_id"] = sensor_id;
        doc["value"] = valor;
        
        String jsonString;
        serializeJson(doc, jsonString);
        
        http.begin(url);
        http.addHeader("Content-Type", "application/json");
        int httpCode = http.POST(jsonString);
        
        if (httpCode > 0) {
            String response = http.getString();
            Serial.print("[API SENSORES] Código de respuesta: ");
            Serial.println(httpCode);
        } else {
            Serial.print("[API SENSORES] Error en la conexión: ");
            Serial.println(http.errorToString(httpCode));
        }
        http.end();
    } else {
        Serial.println("[API SENSORES] Error: Sin conexión WiFi");
        Serial.print("[API SENSORES] IP local: ");
        Serial.println(WiFi.localIP().toString());
    }
}

void enviarLecturaRFID(String uid) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(api_url) + "/tag_readings/";
        
        // Convertir el UID hexadecimal a decimal
        unsigned long long tag_id = strtoull(uid.c_str(), NULL, 16);
        
        StaticJsonDocument<50> doc;
        doc["tag_id"] = (int64_t)tag_id;  // Forzar conversión a int64_t
        
        String jsonString;
        serializeJson(doc, jsonString);
        
        Serial.print("[API RFID] Enviando tag_id: ");
        Serial.println(tag_id);
        Serial.print("[API RFID] JSON enviado: ");
        Serial.println(jsonString);
        
        http.begin(url);
        http.addHeader("Content-Type", "application/json");
        int httpCode = http.POST(jsonString);
        
        if (httpCode > 0) {
            Serial.print("[API RFID] Código de respuesta: ");
            Serial.println(httpCode);
            if (httpCode == 404) {
                String response = http.getString();
                Serial.print("[API RFID] Respuesta: ");
                Serial.println(response);
            }
        } else {
            Serial.print("[API RFID] Error: ");
            Serial.println(http.errorToString(httpCode));
        }
        http.end();
    }
}

// ========================= SETUP =========================
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=== INICIANDO ===");

    // Conectar WiFi
    WiFi.begin(ssid, password);
    Serial.print("Conectando a ");
    Serial.println(ssid);
    
    int intentos = 0;
    while (WiFi.status() != WL_CONNECTED && intentos < 20) {
        delay(500);
        Serial.print(".");
        intentos++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi ready");
        Serial.println("Conectado IP: " + WiFi.localIP().toString());
    } else {
        Serial.println("Error de conexión WiFi");
    }

    // Inicializar sensores
    Wire.begin(19, 18);  // Pines I2C personalizados
    if (!tsl.begin()) {
        Serial.println("Error al inicializar sensor luz");
        while(1);
    }
    tsl.enableAutoRange(true);
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
    
    dht.begin();
    
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
    rfid.PCD_Init();
}

// ========================= LOOP =========================
void loop() {
    unsigned long tiempoActual = millis();

    // Lectura RFID cada 30 segundos
    if (tiempoActual - ultimaLecturaRFID >= INTERVALO_LECTURA_RFID || ultimaLecturaRFID == 0) {
        if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
            ultimaLecturaRFID = tiempoActual;
            String uid = "";
            for (byte i = 0; i < rfid.uid.size; i++) {
                uid += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
                uid += String(rfid.uid.uidByte[i], HEX);
            }
            
            // Mostrar el valor decimal del UID
            unsigned long long decimalValue = strtoull(uid.c_str(), NULL, 16);
            Serial.print("[RFID] Detectado: " + uid + "= ");
            Serial.println(decimalValue);
            
            enviarLecturaRFID(uid);
            rfid.PICC_HaltA();
        }else{
            ultimaLecturaRFID = 0;
            Serial.println("[RFID] No detectado: ");
        }
    }

    // Lectura periódica de sensores
    if (tiempoActual - ultimaLectura >= INTERVALO_LECTURA_SENSORES || ultimaLectura == 0) {
        ultimaLectura = tiempoActual;
        
        // Lectura luz
        sensors_event_t event;
        tsl.getEvent(&event);
        if (event.light) {
            Serial.print("[Luz]: ");
            Serial.print(event.light);
            Serial.println(" Lux");
            enviarDatosSensor(3, event.light);
        }
        
        // Lectura DHT11
        float temp = dht.readTemperature();
        float hum = dht.readHumidity();
        if (!isnan(temp) && !isnan(hum)) {
            Serial.print("[Temperatura]: ");
            Serial.print(temp);
            Serial.println("°C");
            enviarDatosSensor(1, temp);
            Serial.print("[Humedad]: ");
            Serial.print(hum);
            Serial.println("%");
            enviarDatosSensor(2, hum);
        }
        
        // Sensores simulados
        float amoniaco = random(0, 20) + random(0, 100)/100.0;
        int co2 = random(400, 2000);
        Serial.print("[Amoniaco]: ");
        Serial.print(amoniaco);
        Serial.println("ppm");
        enviarDatosSensor(4, amoniaco);
        Serial.print("[CO2]: ");
        Serial.print(co2);
        Serial.println("ppm");
        enviarDatosSensor(5, co2);
    }
    delay(1000);
}
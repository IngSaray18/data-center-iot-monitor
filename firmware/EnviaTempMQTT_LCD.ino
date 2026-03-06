#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



// --- Configuración de la Pantalla ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// El -1 indica que la pantalla no tiene pin de reset dedicado
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);



// --- Configuración del Sensor DHT ---
#define DHTPIN 2  
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);
const float UMBRAL_TEMP = 28.0;


// --- TUS DATOS ---
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "test.mosquitto.org";

const int ledPin =12; // Tu LED

WiFiClient espClient;
PubSubClient client(espClient);

// Variables para el cronómetro (millis)
unsigned long ultimoTiempo = 0;
const long intervalo = 5000; // Enviar datos cada 5000ms (5 seg)

void callback(char* topic, byte* payload, unsigned int length) {
  // Esta función se ejecuta ASINCRÓNAMENTE (cuando llega algo)
  Serial.print("Comando recibido: ");
  char caracter = (char)payload[0];
  
  if (caracter == '1') {
    digitalWrite(ledPin, HIGH);
    Serial.println("LED ON");
    // Confirmamos al servidor que lo hicimos
    client.publish("mi_casa/estado", "LED Encendido"); 
  } else {
    digitalWrite(ledPin, LOW);
    Serial.println("LED OFF");
    client.publish("mi_casa/estado", "LED Apagado");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando MQTT...");
    String clientId = "ESP32-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("¡Listo!");
      client.subscribe("mi_casa/led"); // Nos volvemos a suscribir
    } else {
      delay(5000); // Aquí sí podemos esperar porque no estamos conectados
    }
  }
}

void setup() {
  pinMode(12, OUTPUT);
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
    dht.begin();

     // Inicializar pantalla con dirección I2C 0x3C
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Error: No se encontró la pantalla OLED"));
    for(;;); // Bloquear si no hay pantalla
  }

   

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
     display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(10, 25);
  display.println("Conectando...");
  display.display();
  }
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop(); // Mantiene la escucha activa (IMPORTANTE)

   

  // --- LÓGICA NO BLOQUEANTE ---
  unsigned long tiempoActual = millis();
  
  // Si han pasado 5 segundos desde la última vez...
  if (tiempoActual - ultimoTiempo >= intervalo) {
    ultimoTiempo = tiempoActual; // Reiniciamos el cronómetro
    
    // 1. Simulamos una lectura de sensor (ej: 20 a 30 grados)
     float h = dht.readHumidity();
     float t = dht.readTemperature();
     float f = dht.readTemperature(true);    
    // 2. Convertimos el número a texto
    char tempString[8];
    dtostrf( t , 1, 2, tempString);

    char humString[8];
    dtostrf( h , 1, 2, humString);
    
    // 3. Publicamos
    Serial.print("Enviando temperatura: ");
    Serial.println(tempString);
   // Serial.println(t);
    client.publish("mi_casa/temperatura", tempString);
    client.publish("mi_casa/humedad", humString);

  display.clearDisplay();

     // Encabezado
  display.setTextSize(1);
  display.setCursor(20, 0);
  display.println("DATA CENTER MON");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  // Sección de Temperatura
  display.setCursor(0, 20);
  display.print("TEMP: ");
  display.setTextSize(2); // Texto más grande para el valor
  display.print(t, 1);
  display.setTextSize(1);
  display.println(" C");

  // Sección de Humedad
  display.setCursor(0, 45);
  display.setTextSize(1);
  display.print("HUM:  ");
  display.setTextSize(2);
  display.print(h, 0);
  display.setTextSize(1);
  display.println(" %");

  display.display();
  }
}
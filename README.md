# Data Center Environmental Monitor (IoT)

## Descripción
Este proyecto surge de la necesidad de monitorear en tiempo real las condiciones críticas (temperatura y humedad) de un rack de servidores. Implementa una arquitectura IoT completa desde la adquisición de datos en el "edge" hasta la visualización y alertas remotas.

## Tecnologías Utilizadas
- **Hardware:** ESP32 DevKit V1, Sensor DHT11, Pantalla OLED SSD1306.
- **Protocolo de Comunicación:** MQTT (Broker: Mosquitto).
- **Backend:** Node-RED (Orquestación y Lógica).
- **Dashboard:** Node-RED Dashboard.
- **Notificaciones:** Telegram Bot API.
- **Entorno de Desarrollo:** Arduino IDE / C++.

## Arquitectura del Sistema
El ESP32 captura los datos y los publica en un broker MQTT. Node-RED actúa como suscriptor, procesa la información, actualiza el Dashboard web y ejecuta la lógica de alertas: si la temperatura > 28°C, se dispara una notificación push a Telegram.


# Código del controlador
Este es el código del módulo controlador del Invernadero inteligente, diseñado para una placa basada en el microcontrolador ESP32, con sensores de humedad del suelo capacitivos, así como también de temperatura y humedad del aire DHT22.

### Estructura
El archivo main.cpp contiene las funciones setup() y loop(), y refleja el flujo del código mediante llamadas a funciones definidas en los archivos header ".h". Los mismos utilizan variables globales y definen funciones declaradas en el archivo Declaraciones.h, que también contiene todas las librerías utilizadas.  
De este modo, para cambiar una funcionalidad del controlador, casi siempre bastará modificando el archivo Declaraciones.h y el header correspondiente a la característica.

### Diagrama de flujo
_TODO: además de quizás agregar explicaciones de las variables guardadas (EEPROM) y los comandos._

### Librerías utilizadas (sujeto a cambios y dependiente de la versión)
+ [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)
+ [Adafruit_Sensor](https://github.com/adafruit/Adafruit_Sensor)
+ [Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
+ [Adafruit DHT](https://github.com/adafruit/DHT-sensor-library)
+ Arduino
+ Arduino EEPROM
+ Arduino Json
+ Arduino Wire
+ [CTBot (Stefano Ledda)](https://github.com/shurillu/CTBot)
+ [ESP32 Servo (Kevin Harrington)](https://github.com/jkb-git/ESP32Servo)
+ [ThingSpeak (MathWorks)](https://github.com/mathworks/thingspeak-arduino)  
\+ cualquier dependencia.

### Changelog
¡Primera versión!
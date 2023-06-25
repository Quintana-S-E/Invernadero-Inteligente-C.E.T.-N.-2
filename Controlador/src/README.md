# Código del controlador
Este es el código del módulo controlador del Invernadero inteligente, diseñado para una placa basada en el microcontrolador ESP32, con sensores de humedad del suelo capacitivos, así como también de temperatura y humedad del aire AHT10.

### Estructura
El archivo [main.cpp](https://github.com/Quintana-S-E/Invernadero-inteligente-C.E.T.-N.-2/blob/master/Controlador/src/main.cpp) contiene las funciones `setup()` y `loop()`, y refleja el flujo del código mediante llamadas a funciones definidas en los archivos header ".h". Los mismos utilizan variables globales y definen funciones declaradas en el archivo [Declaraciones.h](https://github.com/Quintana-S-E/Invernadero-inteligente-C.E.T.-N.-2/blob/master/Controlador/src/Declaraciones.h), que también contiene todas las librerías utilizadas.  
De este modo, para cambiar una funcionalidad del controlador, casi siempre bastará modificando el archivo Declaraciones.h y el header correspondiente a la característica.

### Diagrama de flujo
*TODO: además de quizás agregar explicaciones de las variables guardadas (EEPROM) y los comandos.*

### Librerías utilizadas (sujeto a cambios y dependiente de la versión)
+ [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)
+ [Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
+ [AHT10 (enjoyneering)](https://github.com/enjoyneering/AHT10)
+ Arduino
+ Arduino EEPROM
+ Arduino Json
+ Arduino Wire
+ [BluetoothSerial (arduino-esp32)](https://github.com/espressif/arduino-esp32/tree/master/libraries/BluetoothSerial)
+ [CTBot (Stefano Ledda)](https://github.com/shurillu/CTBot)
+ [WiFi (arduino-esp32)](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi)
\+ cualquier dependencia.

### Changelog
¡Primera versión!
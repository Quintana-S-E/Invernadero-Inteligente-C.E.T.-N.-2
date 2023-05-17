#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>	  // display OLED
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h> // display OLED
#include <ArduinoJson.h>
#include <CTBot.h>
#include <DHT.h>
#include <EEPROM.h>
#include <ESP32Servo.h>
#include <ThingSpeak.h>
#include <Wire.h> // I2C
#include <WiFi.h> // Gráficos.h

//#define DEBUGserial // Comentar para eliminar los Serial.print
#ifdef DEBUGserial
	#define imprimir(x) Serial.print(x)
	#define imprimirln(x) Serial.println(x)
#else
	#define imprimir(x)
	#define imprimirln(x)
#endif

// Constantes de funcionamiento generales
#define DELAY_ACTIVIDAD_INVERNADERO 0UL // (ms) tiempo de espera para el loop del invernadero

// Pines
// de salida
#ifndef LED_BUILTIN
	#define LED_BUILTIN 2
#endif
#define LED_ROJO 4
#define PIN_BOMBA 18
#define PIN_VENTILADOR 19
#define PIN_SERVO 23
// de los sensores DHT
#define DHT_INT_LOW_PIN 27
#define DHT_INT_HIGH_PIN 14
#define DHT_EXT_PIN 13 //#define DHT_INT_MID_PIN // Para el domo
// de los sensores humedad suelo
#define SOIL_EXT_PIN A0
#define SOIL_INT_PIN A3 //#define SOIL_INT_2_PIN A6 // Para el domo

// Variables de tiempo generales
unsigned long ultima_vez_invernadero_funciono = 0;
unsigned long ultima_vez_display_cambio = 0;

// Flags de estado generales
/* -------------------------IDEA:-------------------------
Posiblemente crear class Salida con flags: activada, desactivada, forzada, ultima_vez_activada/desactivada.
Y crear child classes con los métodos propios: abrir(), chequear(), prender(), apagar(), abrir un ángulo(), esperando, temp_max, etc;
declarando Riego, Calefa, Ventilación y quizás Alarma. Ver cómo incorporar los valores de la EEPROM (fácil, pero declararlos todos
en el mismo lugar que los otros valores de la EEPROM [humedad suelo es el único no relacionado con una salida]).
*/
bool ventilacion_forzada	= false; // si el estado de ventilación está siendo forzado por telegram
bool ventilando				= false;
bool esperando_riego		= false; // para chequearRiego()

// Tiempo.h
String mensajeSegundosATiempo(unsigned long segundos);


// Sensores.h
#define MUESTRAS_HUMEDAD_SUELO 16		// 16 máximo
void leerSensores();
void leerDHT22Interiores();
void leerSoilInteriores();
void leerDHT22Exteriores();
void leerSoilExteriores();
// variables
// DHTs interiores
float temp_interior_promedio;
float humedad_aire_interior_promedio;
// DHTs exteriores
float temp_exterior;
float humedad_aire_exterior;
// soil moisture sensors
int humedad_suelo_interior;
int humedad_suelo_exterior;


// Control.h
unsigned long ultima_vez_bomba_encendio = 0;
void chequearVentilacion();
void chequearRiego();
void activarVentilacion();
void desactivarVentilacion();
//void abrirVentana();
//void cerrarVentana();
//void chequear_iluminacion();
#define ANGULO_APERTURA	90		// posición de apertura de la ventana
#define ANGULO_CERRADO	0		// posición de cerrado de la ventana


// Display.h
unsigned long ultima_vez_display_actualizo = 0;
void inicializarDisplay();
void cambiarDatoDisplay();
void displayConectandoWIFI();
void displayErrorWIFI();
void displayConexionWIFI(String Amensaje_conectado_a, String Assid_conectada);
void actualizarDisplay();
void displayHumedadAire();
void displayHumedadSuelo();
void displayTemperatura();
#define DELAY_CAMBIO_DISPLAY		10000UL
#define DELAY_ACTUALIZACION_DISPLAY	500UL
#define SCREEN_WIDTH				128		// ancho del display OLED display, en píxeles
#define SCREEN_HEIGHT				64		// alto del display OLED display, en píxeles
enum DatoMostradoEnDisplay
{
	Temperatura,
	HumedadAire,
	HumedadSuelo
};
DatoMostradoEnDisplay DatoDelDisplay = Temperatura;


// Graficos.h
unsigned long ultima_vez_thingspeak_actualizo = 0;
void actualizarGraficos();
inline bool inicializarThingSpeak();
#define FIELD_TEMP_INT		1
#define FIELD_TEMP_EXT		2
#define FIELD_HUM_AIRE_INT	3
#define FIELD_HUM_AIRE_EXT	4
#define FIELD_HUM_SUELO_INT	5
#define FIELD_HUM_SUELO_EXT	6

// Telegram.h
// variables
#define TELEGRAM_TIEMPO_MAX_CONFIGURACION	15000UL
#define DELAY_COMPROBACION_WIFI				60000UL		// cada un minuto comprueba la conexión a WiFi
unsigned long ultima_vez_alarma_funciono = 0;
unsigned long ultima_vez_comprobacion_WIFI = 0;
String		chat_rpta; // necesariamente global para cambiarla en evaluarMensajeFloat() y evaluarMensajeInt()
uint64_t 	chat_id = 0; // comienza en 0 para comprobaciones en chequearAlarma()
uint16_t 	chat_numero_entrada_int;	// cuando preguntamos por un número entero de entrada
float		chat_numero_entrada_float;	// cuando preguntamos por un número con decimal de entrada
bool		chat_primer_mensaje = true; // para chequearMensajesRecibidosTelegram()
// WiFi
void conectarWIFI(bool parar_programa);
bool conectarWIFICon(const String& Assid, const String& Apassword);
void chequearConexion();
// funciones varias
void chequearMensajesRecibidosTelegram();
void chequearAlarma();
void enviarMensaje(const uint64_t Aid, const String& Amensaje);
bool evaluarMensajeInt(uint16_t Avalor_min, uint16_t Avalor_max, String Aunidad);
bool evaluarMensajeFloat(float Avalor_min, float Avalor_max, String Aunidad);
// comandos
void comandoStart();
void comandoLecturas();
void comandoInfo();
void comandoProg();
void comandoVentilar();
void comandoTiempoAl();
void comandoTiempoRiego();
void comandoTiempoEspera();
void comandoTmax();
void comandoTmin();
void comandoTvent();
void comandoAlarma();
void comandoHum();
void comandoLed();
void comandoReprog();


// EEMPROM_manejo.h
// funciones
void chequearEEPROMProgramada();
void setDireccionesEEPROM();
void leerEEPROMProgramada();
void cargarValoresPorDefecto();
void imprimirEEPROMValsDirsReads();
template <typename T>
void escribirEEPROM(int Adireccion, T Adato); // template <typename T> T leerEEPROM(int Adireccion, T Adato);
// variables de la EEPROM con sus valores por defecto
#define TEMP_MAXIMA_ALARMA_DEFECTO		45.0F
#define TEMP_MINIMA_ALARMA_DEFECTO		-5.0F
#define TEMP_MAXIMA_VENTILACION_DEFECTO	35.0F
#define HUMEDAD_SUELO_MINIMA_DEFECTO	60
#define LAPSO_ALARMA_MINUTOS_DEFECTO	60
#define ALARMA_ACTIVADA_DEFECTO			true
#define TIEMPO_BOMBEO_SEGUNDOS_DEFECTO	10
#define TIEMPO_ESPERA_MINUTOS_DEFECTO	15
bool		EEPROM_programada;			// 0.	para verificar si está programada o no la EEPROM
float		temp_maxima_alarma;			// 1.
float		temp_minima_alarma;			// 2.
float		temp_maxima_ventilacion;	// 3.
uint8_t		humedad_suelo_minima;		// 4.	70 % es vaso de agua, 29 % es el aire
uint16_t	lapso_alarma_minutos;		// 5.	60 minutos (máx 65535 min o 1092 horas o 45 días)
bool		alarma_activada;			// 6.
uint16_t	tiempo_bombeo_segundos;		// 7.	4 segundos (máx 65535 seg o 18,2 horas)
uint16_t	tiempo_espera_minutos;		// 8.	15 minutos (máx 65535 min)
// manejo de las direcciones de la EEPROM
enum EEPROMDireccionesVariables
{
	DIR_EEPROM_PROGRAMADA,
	DIR_TEMP_MAXIMA_ALARMA,
	DIR_TEMP_MINIMA_ALARMA,
	DIR_TEMP_MAXIMA_VENTILACION,
	DIR_HUMEDAD_SUELO_MINIMA,
	DIR_LAPSO_ALARMA_MINUTOS,
	DIR_ALARMA_ACTIVADA,
	DIR_TIEMPO_BOMBEO_SEGUNDOS,
	DIR_TIEMPO_ESPERA_MINUTOS,
	CANT_VARIABLES_EEPROM
};
										// bool, float, float, float, int8, int, bool, int, int
const int LONGITUD_DATO_EEPROM[CANT_VARIABLES_EEPROM] = {1, 4, 4, 4, 1, 2, 1, 2, 2};
int direccion[CANT_VARIABLES_EEPROM];
int espacios_EEPROM;


// Clases
CTBot Bot;
Servo Ventana;
WiFiClient Cliente;
Adafruit_SSD1306 Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT DhtInteriorLow(DHT_INT_LOW_PIN, DHT22);
DHT DhtInteriorHigh(DHT_INT_HIGH_PIN, DHT22);
DHT DhtExterior(DHT_EXT_PIN, DHT22);
//DHT DhtInteriorMid(DHT_INT_MID_PIN, DHT22); // Para el domo
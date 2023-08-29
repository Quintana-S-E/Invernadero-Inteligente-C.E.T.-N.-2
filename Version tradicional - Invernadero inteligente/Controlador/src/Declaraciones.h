#pragma once

#include "Claves.h"

#include <Arduino.h>
#include <Adafruit_GFX.h>	  // display OLED
#include <Adafruit_SSD1306.h> // display OLED
#include <AHT10.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <Firebase_ESP_Client.h>
	#include "addons/TokenHelper.h" // Provide the token generation process info.
	#include "addons/RTDBHelper.h" // Provide the RTDB payload printing info and other helper functions.
#include <SD.h>
#include <SPI.h>
#include "time.h" // tiempo unix
#include <Wire.h> // I2C
#include <WiFi.h>
#include <WiFiMulti.h>

/*
TODO IMPORTANTE: al tener una versión ya compilada, ver de cambiar todos los headers a .cpp. Que todos incluyan solamente a
este archivo (Declaraciones.h) y que en main.cpp también solo se incluya este archivo. Así quedarían más claras la función de los
headers (definir funciones declaradas acá) y la función de este archivo (declarar todas las funciones).
Lo intenté el 22/8, pareciera que iba todo bien pero hubo un problema en AHT10.cpp (creo que de la librería, pero qsy por ahí
tenía que ver con haberlo cambiado todo). Sólo una vez que todo funcione hacer este cambio.
*/

#define DEBUG_SERIAL // Comentar para eliminar los Serial.print
#ifdef DEBUG_SERIAL
	#define imprimir(x) Serial.print(x)
	#define imprimirln(x) Serial.println(x)
#else
	#define imprimir(x)
	#define imprimirln(x)
#endif



// Constantes de funcionamiento generales
#define DELAY_ACTIVIDAD_INVERNADERO 0UL // (ms) tiempo de espera para el loop del invernadero
#define W_SSID_SIZE		33 // 32 caracteres + null terminator
#define W_PASS_SIZE		64 // 63 caracteres + null terminator
#define F_EMAIL_SIZE	86 // 63 de domain + 21 de @cet2bariloche.edu.ar (o menos de @gmail y @hotmail.com) + null terminator
#define F_PASS_SIZE		31 // 30 caracteres + null terminator
#define F_API_KEY_SIZE	40 // 39 caracteres + null terminator
#define F_URL_SIZE		68 // "https://" (8 chars) + 30 chars name + "-default-rtdb.firebaseio.com/" (29 chars) + null terminator
// Pines
#define SD_CS			5
// ledes y botón
#define PIN_BTN			4
#ifndef LED_BUILTIN
	#define LED_BUILTIN	2
#endif
#define LED_VENTILACION		15 // Active low
#define LED_WIFI			12 // Active low
// de los relés
#define PIN_BOMBA1			17
#define PIN_BOMBA2			16
#define PIN_BOMBA3			26
#define PIN_VENTILACION		27
// del multiplexor
#define MUX_A 32
#define MUX_B 33
#define MUX_C 25
// de los sensores AHT10 (desde el MUX)
enum class PinesAHT10MUX : uint8_t
{
	INT_HIGH,
	INT_MID,
	INT_LOW,
	AGUA1,
	AGUA2,
	AGUA3,
	GEOTERMICO,
};
// de los sensores humedad suelo
#define SOIL1_PIN A0
#define SOIL2_PIN A3
#define SOIL3_PIN A6
#define SOIL4_PIN A7

// Variables de tiempo generales
unsigned long ultima_vez_invernadero_funciono = 0;
unsigned long ultima_vez_display_cambio = 0;

// Flags de estado generales
/* -------------------------IDEA:-------------------------
Posiblemente crear class Salida con flags: activada, desactivada, forzada, ultima_vez_activada/desactivada.
Y crear child classes con los métodos propios: abrir(), controlar(), prender(), apagar(), abrir un ángulo(), esperando, temp_max, etc;
declarando Riego, Calefa, Ventilación y quizás Alarma. Ver cómo incorporar los valores de la EEPROM (fácil, pero declararlos todos
en el mismo lugar que los otros valores de la EEPROM [humedad suelo es el único no relacionado con una salida]).

-------------------------2DA IDEA:-------------------------
Cambiar esto por un struct FlagsSalidas. Las funciones de cada salida pueden quedar sueltas. No es muy buen encapsulado pero meh.

bool ventilacion_forzada	= false; // si el estado de ventilación está siendo forzado por telegram
bool ventilando				= false;*/
bool esperando_riego		= false; // para controlarRiego()



// Tiempo.h
char SERVIDOR_NTP[] = "pool.ntp.org";
bool tiempo_unix_configurado = false;
inline void inicializarTiempoUnix();
unsigned long obtenerTiempoUnix();
String mensajeSegundosATiempo(unsigned long segundos);


// Sensores.h
#define MUESTRAS_HUMEDAD_SUELO 16		// 16 máximo
void establecerSalidaMUX(uint8_t salida);
void inicializarSensores();
void leerSensores();
void leerAHT10Interiores();
void leerSoilInteriores();
void leerAHT10Exteriores();
void leerSoilExteriores();
// variables
// AHTs interiores
float temp_interior_promedio;
float humedad_aire_interior_promedio;
// AHTs exteriores
float temp_exterior;
float humedad_aire_exterior;
float temp_exterior_geotermica;
// soil moisture sensors
int humedad_suelo_interior;
int humedad_suelo_exterior;
class AHT10Mux
{
	private:
		uint8_t salida_del_mux;

	public:
		AHT10Mux(PinesAHT10MUX salida_del_mux);
		bool     begin();
		float    readTemperature(bool readI2C = AHT10_FORCE_READ_DATA);
		float    readHumidity(bool readI2C = AHT10_FORCE_READ_DATA);
};


// Control.h
unsigned long ultima_vez_bomba_encendio = 0;
void controlarVentilacion();
void controlarRiego();
void activarVentilacion();
void desactivarVentilacion();
//void abrirVentana();
//void cerrarVentana();
//void controlarIluminacion();
#define ANGULO_APERTURA	90		// posición de apertura de la ventana
#define ANGULO_CERRADO	0		// posición de cerrado de la ventana
// parte del botón (no utilizado, bueno tenerlo)
#define TIEMPO_MIN_BTN_MANTENIDO 800UL
enum class EstadoBoton : uint8_t
{
	Suelto,
	Clickeado,
	Mantenido,
	DobleClickeado
};
EstadoBoton leerBoton(unsigned long timeout_lectura);

enum class SalidaModos : uint8_t
{
	Automatica,
	Deshabilitada,
	Forzada,
	Temporizada
};
class SalidaOnOff
{
	public:
		SalidaModos modo = SalidaModos::Automatica;
		bool encendida = false;
		//unsigned long ultima_vez_encendido;	LO OBVIAMOS, ponemos un datalog channel por cada salida y dataloggeamos 1 si
		//unsigned long ultima_vez_apagado;		está encendida y 0 si está apagada
	private:
		uint8_t pin_rele;
		uint8_t pin_led;

	public:
		SalidaOnOff(uint8_t pin_rele, uint8_t pin_led);
		void encender();
		void apagar();
};
class SalidaVentilacion
{
	public:
		SalidaModos modo = SalidaModos::Automatica;
	private:
		uint8_t pin_rele1;
		uint8_t pin_rele2;
		uint8_t pin_led;

	public:
		SalidaVentilacion(uint8_t pin_rele1, uint8_t pin_rele2, uint8_t pin_led);
		// TODO: que las funciones de abrir/cerrar accionen el LED
	// A decidir método de movimiento
};


// Display.h
unsigned long ultima_vez_display_actualizo = 0;
void inicializarDisplay();
void cambiarDatoDisplay();
void displayConectandoWiFi();
void displayErrorWiFi();
void displayConetadoA(String ssid_conectada);
void actualizarDisplay();
void displayHumedadAire();
void displayHumedadSuelo();
void displayTemperatura();
void displayError();
void displayNoSD();
void displayErrorSD();
// TODO:
void displayLogo(/*SE VALE PONER DELAY (1-2 seg)*/); // Invernadero inteligente que esté centrado, nada más


#define DELAY_CAMBIO_DISPLAY		10000UL
#define DELAY_ACTUALIZACION_DISPLAY	500UL
#define SCREEN_WIDTH				128		// ancho del display OLED display, en píxeles
#define SCREEN_HEIGHT				64		// alto del display OLED display, en píxeles
enum class DisplayDato : uint8_t
{
	Temperatura,
	HumedadAire,
	HumedadSuelo
};
DisplayDato DatoDelDisplay = DisplayDato::Temperatura;


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


// Conectividad.h
#define CANT_REDES_WIFI 3
class LocalWiFi
{
	public:
		bool hay_conexion;
		uint8_t cant_redes = 0;
		char ssid[CANT_REDES_WIFI][W_SSID_SIZE];
		char pass[CANT_REDES_WIFI][W_PASS_SIZE];

	public:
		void inicializarWiFi();
		bool guardarRedWiFi(const char* ssid, const char* password);
		bool guardarRedWiFi(const char* ssid);
		bool correr();
} LCWF;


// Firebase.h
class LocalFirebase
{
	public:
		char email		[	F_EMAIL_SIZE	];
		char pass		[	F_PASS_SIZE		];
		char url		[	F_URL_SIZE		];
		char api_key	[	F_API_KEY_SIZE	];
		bool tiene_firebase = false;
	private:
		FirebaseData data;
		FirebaseData stream;
		FirebaseAuth auth;
		FirebaseConfig config;
		FirebaseJson json;

	public:
		//funcs
	private:
		//funcs
} LCFB;


// SD_manejo.h
enum class ResultadoLecturaSD : uint8_t
{
	NO_ARCHIVO,
	NO_CONTENIDO,
	EXITOSO
};
class LocalSD
{
	private:
		// NOMBRES DE LOS FOLDERS (Y.TXT)
		//char STRINGS_PATH[]			= "controlador/strings/";
		char CONFIG_FOLDER_PATH[20]		= "controlador/config/";
		char WIFI_FOLDER_PATH[6]		= "wifi/";
		char FIREBASE_FOLDER_PATH[10]	= "firebase/";
		//char PARAMETROS_FOLDER_PATH[]	= "parametros/";
		char TXT[5]						= ".txt";
		// NOMBRES DE FOLDER WIFI
		char NOMBRE_ARCHIVO_WSSID[5]	= "ssid";
		char NOMBRE_ARCHIVO_WPASS[5]	= "pass";
		// NOMBRES DE FOLDER FIREBASE
		char NOMBRE_ARCHIVO_FEMAIL[6]	= "email";
		char NOMBRE_ARCHIVO_FPASS[5]	= "pass";
		char NOMBRE_ARCHIVO_FURL[4]		= "url";
		char NOMBRE_ARCHIVO_FAPIKEY[7]	= "apikey";
	public:
		void inicializar();
		void leerConfigWiFi();
		void leerConfigFirebase();
		void leerConfigParametros();
	private:
		ResultadoLecturaSD leerStringA(char *buffer, const uint8_t caracteres, const char *path);
} LCSD;


// Telegram.h
// variables
#define TELEGRAM_TIEMPO_MAX_CONFIGURACION	15000UL
unsigned long ultima_vez_alarma_funciono = 0;
unsigned long ultima_vez_comprobacion_wifi = 0;
String		chat_rpta; // necesariamente global para cambiarla en evaluarMensajeFloat() y evaluarMensajeInt()
uint64_t 	chat_id = 0; // comienza en 0 para comprobaciones en controlarAlarma()
uint16_t 	chat_numero_entrada_int;	// cuando preguntamos por un número entero de entrada
float		chat_numero_entrada_float;	// cuando preguntamos por un número con decimal de entrada
bool		chat_primer_mensaje = true; // para controlarMensajesRecibidosTelegram()
// WiFi
void conectarWiFi(bool parar_programa);
bool conectarWiFiCon(const String& Assid, const String& Apassword);
void controlarConexion();
// funciones varias
void controlarMensajesRecibidosTelegram();
void controlarAlarma();
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
// variables de la EEPROM con sus valores por defecto
#define ALARMA_ACTIVADA_DEFECTO			true
#define TEMP_MAXIMA_ALARMA_DEFECTO		45.0F
#define TEMP_MINIMA_ALARMA_DEFECTO		-5.0F
#define TEMP_MAXIMA_VENTILACION_DEFECTO	35.0F
#define HUMEDAD_SUELO_MINIMA_DEFECTO	60
#define LAPSO_ALARMA_MINUTOS_DEFECTO	60
#define TIEMPO_BOMBEO_SEGUNDOS_DEFECTO	10
#define TIEMPO_ESPERA_MINUTOS_DEFECTO	15
bool		eeprom_programada;			// 0.	para verificar si está programada o no la EEPROM
bool		alarma_activada;			// 1.
float		temp_maxima_alarma;			// 2.
float		temp_minima_alarma;			// 3.
float		temp_maxima_ventilacion;	// 4.
uint8_t		humedad_suelo_minima;		// 5.	70 % es vaso de agua, 29 % es el aire
uint16_t	lapso_alarma_minutos;		// 6.	60 minutos (máx 65535 min o 1092 horas o 45 días)
uint16_t	tiempo_bombeo_segundos;		// 7.	4 segundos (máx 65535 seg o 18,2 horas)
uint16_t	tiempo_espera_minutos;		// 8.	15 minutos (máx 65535 min)
class LocalEEPROM
{
	public:
		enum OrdenParametros : uint8_t
		{
			PROGRAMADA,
			ALARMA_ACTIVADA,
			TEMP_MAXIMA_ALARMA,
			TEMP_MINIMA_ALARMA,
			TEMP_MAXIMA_VENTILACION,
			HUMEDAD_SUELO_MINIMA,
			LAPSO_ALARMA_MINUTOS,
			TIEMPO_BOMBEO_SEGUNDOS,
			TIEMPO_ESPERA_MINUTOS,
			CANT_VARIABLES
		};											// bool, bool, float, float, float, int8, int, int, int
		const int LONGITUD_DATO[CANT_VARIABLES] = {1, 1, 4, 4, 4, 1, 2, 2, 2};
		int direccion[CANT_VARIABLES];
	private:
		int espacios;

	public:
		void inicializar();
		void leerCompleta();
		void cargarValoresPorDefecto();
		template <typename T>
		void escribir(int Adireccion, T Adato);
	private:
		void setDirecciones();
		void imprimirValsDirsReads();
} LCEE;


// Clases
File DatalogSD;
WiFiMulti WiFiMultiO;
Adafruit_SSD1306 Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
AHT10 AhtSeleccionado(AHT10_ADDRESS_0X38);
AHT10Mux AhtInteriorHigh(PinesAHT10MUX::INT_HIGH);
AHT10Mux AhtInteriorMid(PinesAHT10MUX::INT_MID);
AHT10Mux AhtInteriorLow(PinesAHT10MUX::INT_LOW);
AHT10Mux AhtAgua1(PinesAHT10MUX::AGUA1);
AHT10Mux AhtAgua2(PinesAHT10MUX::AGUA2);
AHT10Mux AhtAgua3(PinesAHT10MUX::AGUA3);
AHT10Mux AhtGeotermico(PinesAHT10MUX::GEOTERMICO);
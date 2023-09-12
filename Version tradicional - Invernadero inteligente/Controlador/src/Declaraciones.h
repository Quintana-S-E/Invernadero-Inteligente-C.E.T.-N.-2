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
#define DELAY_ACTIVIDAD_INVERNADERO 0UL		// (ms) tiempo de espera para el loop del invernadero
#define DELTA_T_VENTILACION			1.0F	// ver Control.h
#define DELTA_T_CALEFA				DELTA_T_VENTILACION
#define W_SSID_SIZE		33 // 32 caracteres + null terminator
#define W_PASS_SIZE		64 // 63 caracteres + null terminator
#define F_EMAIL_SIZE	86 // 63 de domain + 21 de @cet2bariloche.edu.ar (o menos de @gmail y @hotmail.com) + null terminator
#define F_PASS_SIZE		31 // 30 caracteres + null terminator
#define F_API_KEY_SIZE	40 // 39 caracteres + null terminator
#define F_URL_SIZE		68 // "https://" (8 chars) + 30 chars name + "-default-rtdb.firebaseio.com/" (29 chars) + null terminator
// Pines
#define SD_CS			5
#ifndef LED_BUILTIN
	#define LED_BUILTIN	2
#endif
enum class PinsOut : uint8_t
{
	// de los relés
	RIEGO			= 17,
	CALEFA			= 15,
	MARCHA			= 16,
	CONTRAMARCHA	= 2,
	// del MUX
	MUX_A	= 14,
	MUX_B	= 27,
	MUX_C	= 26
};
// de los sensores humedad suelo
enum class PinsIn : uint8_t
{
	SOIL1 = A7,
	SOIL2 = A0,
	SOIL3 = A3,
};
// de los sensores AHT10 (desde el MUX)
enum class PinsAHT10MUX : uint8_t
{
	INT_MID		= 1,
	INT_HIGH,
	INT_LOW,
	GEOTERMICO	= 5,
};

// Variables de tiempo y flags generales
unsigned long ultima_vez_invernadero_funciono = 0;
unsigned long ultima_vez_display_cambio = 0;
bool esperando_riego = false; // para controlarRiego()



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
		AHT10Mux(PinsAHT10MUX salida_del_mux);
		bool     begin();
		float    readTemperature(bool readI2C = AHT10_FORCE_READ_DATA);
		float    readHumidity(bool readI2C = AHT10_FORCE_READ_DATA);
};


// Control.h
//#define TIEMPO_MIN_BTN_MANTENIDO 800UL
/*parte del botón (no utilizado, bueno tenerlo)
enum class EstadoBoton : uint8_t
{
	Suelto,
	Clickeado,
	Mantenido,
	DobleClickeado
};
EstadoBoton leerBoton(unsigned long timeout_lectura);*/
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
		SalidaModos modo;
		bool encendida = false;
		unsigned long ultima_vez_encendida = 0; // TODO: poner un datalog channel con encendida/apagada
		//unsigned long ultima_vez_apagado;
	private:
		uint8_t pin_mosfet;

	public:
		SalidaOnOff(PinsOut pin_mosfet);
		void encender(unsigned long millis_actual);
		void apagar();
};
class SalidaVentilacion
{
	public:
		SalidaModos modo;
		bool abierta = false;
		unsigned long ultima_vez_abierta = 0; // TODO: poner un datalog channel con encendida/apagada
	private:
		uint8_t pin_marcha;
		uint8_t pin_contramarcha;

	public:
		SalidaVentilacion(PinsOut pin_marcha, PinsOut pin_contramarcha);
		void abrir(unsigned long millis_actual);
		void cerrar();
};
class LocalControl
{
	public:
	private:
	public:
		void configurarModosSalidas();
		void controlarRiego();
		void controlarCalefa();
		void controlarVentilacion();
	private:
		void riegoTemporizado();
		void riegoAutomatico();
		void monitorearBombeoRiego(unsigned long millis_actual);
		void calefaTemporizada();
		void calefaAutomatica();
		void ventilacionTemporizada();
		void ventilacionAutomatica();
} LCCT;


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
		bool correr();
		void controlarAlarma();
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
		char PARAMETROS_FOLDER_PATH[12]	= "parametros/";
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
#define MODOS_SALIDAS_DEFECTO				0b00000000	// todas en automático
// alarma
#define ALARMA_ACTIVADA_DEFECTO				true
#define LAPSO_ALARMA_MIN_DEFECTO			60
#define TEMP_MAXIMA_ALARMA_DEFECTO			45.0F
#define TEMP_MINIMA_ALARMA_DEFECTO			-5.0F
// riego
#define HUMEDAD_SUELO_MINIMA_DEFECTO		60
#define LAPSO_RIEGOS_MIN_DEFECTO			720			// 12 h
#define TIEMPO_BOMBEO_SEG_DEFECTO			10
#define TIEMPO_ESPERA_MIN_DEFECTO			15
// calefa
#define TEMP_MINIMA_CALEFA_DEFECTO			5.0F
#define LAPSO_CALEFAS_MIN_DEFECTO			1440		// 24 h
#define TIEMPO_ENCENDIDO_CALEFA_MIN_DEFECTO 60
// ventilación
#define TEMP_MAXIMA_VENTILACION_DEFECTO		32.0F
#define LAPSO_VENTILACIONES_MIN_DEFECTO 	1440		// 24 h
#define TIEMPO_APERTURA_VENT_MIN_DEFECTO	30
#define TIEMPO_MARCHA_VENT_SEG_DEFECTO		7
bool		eeprom_programada;			// 0.	para verificar si está programada o no la EEPROM
uint8_t		modos_salidas;			// 1.
// alarma
bool		alarma_activada;			// 2.
uint16_t	lapso_alarma_min;			// 3.
float		temp_maxima_alarma;			// 4.
float		temp_minima_alarma;			// 5.
// riego
uint8_t		humedad_suelo_minima;		// 6.	70 % es vaso de agua, 29 % es el aire
uint16_t	lapso_riegos_min;			// 7.	60 minutos (máx 65535 min o 1092 horas o 45 días)
uint16_t	tiempo_bombeo_seg;			// 8.	10 segundos (máx 65535 seg o 18,2 horas)
uint16_t	tiempo_espera_min;			// 9.	15 minutos (máx 65535 min)
// calefa
float		temp_minima_calefa;			// 10.
uint16_t	lapso_calefas_min;			// 11.
uint16_t	tiempo_encendido_calefa_min;// 12.
// ventilación
float		temp_maxima_ventilacion;	// 13.
uint16_t	lapso_ventilaciones_min;	// 14.
uint16_t	tiempo_apertura_vent_min;	// 15.
uint8_t		tiempo_marcha_vent_seg;		// 16.
class LocalEEPROM
{
	public:
		enum OrdenParametros : uint8_t
		{
			PROGRAMADA,
			MODOS_SALIDAS,
			ALARMA_ACTIVADA,
			LAPSO_ALARMA_MIN,
			TEMP_MAXIMA_ALARMA,
			TEMP_MINIMA_ALARMA,
			HUMEDAD_SUELO_MINIMA,
			LAPSO_RIEGOS_MIN,
			TIEMPO_BOMBEO_SEG,
			TIEMPO_ESPERA_MIN,
			TEMP_MINIMA_CALEFA,
			LAPSO_CALEFAS_MIN,
			TIEMPO_ENCENDIDO_CALEFA_MIN,
			TEMP_MAXIMA_VENTILACION,
			LAPSO_VENTILACIONES_MIN,
			TIEMPO_APERTURA_VENT_MIN,
			TIEMPO_MARCHA_VENT_SEG,
			CANT_VARIABLES
		};//									   bool, uint8_t = 1;		uint16_t = 2;		float = 4
		const int LONGITUD_DATO[CANT_VARIABLES] = {1, 1, 1, 2, 4, 4, 1, 2, 2, 2, 4, 2, 2, 4, 2, 2, 1};
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
AHT10Mux AhtInteriorHigh(PinsAHT10MUX::INT_HIGH);
AHT10Mux AhtInteriorMid(PinsAHT10MUX::INT_MID);
AHT10Mux AhtInteriorLow(PinsAHT10MUX::INT_LOW);
AHT10Mux AhtGeotermico(PinsAHT10MUX::GEOTERMICO);
SalidaOnOff Riego(PinsOut::RIEGO);
SalidaOnOff Calefa(PinsOut::CALEFA);
SalidaVentilacion Ventilacion(PinsOut::MARCHA, PinsOut::CONTRAMARCHA);
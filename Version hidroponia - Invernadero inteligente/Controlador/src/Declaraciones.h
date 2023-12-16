#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>	  // display OLED
#include <Adafruit_SSD1306.h> // display OLED
#include <AHTxx.h>
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
const unsigned long DELAY_ACTIVIDAD_INVERNADERO = 0;		// (ms) tiempo de espera para el loop del invernadero
const unsigned long DELAY_DATALOG = 10000; // 9,97 años hasta alcanzar el máximo de renglones SD (1048576)
const uint8_t W_SSID_SIZE		= 33; // 32 caracteres + null terminator
const uint8_t W_PASS_SIZE		= 64; // 63 caracteres + null terminator
const uint8_t F_EMAIL_SIZE		= 86; // 63 de domain + 21 de @cet2bariloche.edu.ar (o menos de @gmail y @hotmail.com) + null terminator
const uint8_t F_PASS_SIZE		= 31; // 30 caracteres + null terminator
const uint8_t F_API_KEY_SIZE	= 40; // 39 caracteres + null terminator
const uint8_t F_URL_SIZE		= 68; // "https://" (8 chars) + 30 chars name + "-default-rtdb.firebaseio.com/" (29 chars) + null terminator
// Pines
const uint8_t SD_CS			= 5;
enum class PinsOut : uint8_t
{
	// de los relés
	BOMBA1			= 17,
	BOMBA2			= 15,
	BOMBA3			= 12,
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
	SOIL4 = A6
};
// de los sensores AHT10 (desde el MUX)
enum class PinsAHT10MUX : uint8_t
{
	INT_MID = 1,
	INT_HIGH,
	INT_LOW,
	AGUA1,
	GEOTERMICO,
	AGUA2,
	AGUA3,
};

// Variables de tiempo y flags generales
unsigned long ultima_vez_invernadero_funciono = 0;



// Tiempo.h
char SERVIDOR_NTP[] = "pool.ntp.org";
bool tiempo_unix_configurado = false;
inline void inicializarTiempoUnix();
unsigned long obtenerTiempoUnix();
String mensajeSegundosATiempo(unsigned long segundos);


// Sensores.h
void inicializarSensores();
void leerSensores();
void leerSensoresAHT10();
void leerSensoresSoil();
// AHT10
float humedad_int_high;
float humedad_int_mid;
float humedad_int_low;
// soil moisture sensors
int humedad_suelo1;
int humedad_suelo2;
int humedad_suelo3;
int humedad_suelo4;
class AHT10Mux
{
	public:
		float temperatura;
	private:
		uint8_t salida_del_mux;

	public:
		AHT10Mux(PinsAHT10MUX salida_del_mux);
		bool     begin();
		float    readTemperature(bool readI2C = AHTXX_FORCE_READ_DATA);
		float    readHumidity(bool readI2C = AHTXX_FORCE_READ_DATA);
	private:
		void establecerSalidaMUX();
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
		unsigned long ultima_vez_encendida = 0;
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
		unsigned long ultima_vez_abierta = 0;
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
	private:
		unsigned long ultima_actualizacion_alarma = 0;
		const float DELTA_T_VENTILACION	= 1;
	public:
		void configurarModosSalidas();
		void controlarBomba1();
		void controlarBomba2();
		void controlarBomba3();
		void controlarVentilacion();
		void controlarAlarma();
	private:
		void bomba1Temporizada();
		void bomba2Temporizada();
		void bomba3Temporizada();
		void ventilacionTemporizada();
		void ventilacionAutomatica();
} LCCT;


// Display.h
const unsigned long DELAY_CAMBIO_DISPLAY		= 6000;
enum class DisplayDato : uint8_t
{
	Temperatura1,
	Temperatura2,
	Temperatura3,
	Temperatura4,
	HumedadAire1,
	HumedadAire2,
	HumedadSuelo1,
	HumedadSuelo2,
	WiFi
};
DisplayDato DatoDelDisplay = DisplayDato::Temperatura1;
class LocalDisplay
{
	public:
		unsigned long ultima_actualizacion = 0;
		unsigned long ultima_vez_cambio = 0;
	private:
		const unsigned long DELAY_ACTUALIZACION_DISPLAY	= 500;
		char msg_temp_sup[21] = "Temperatura superior";
		char msg_temp_mid[18] = "Temperatura medio";
		char msg_temp_inf[21] = "Temperatura inferior";
		char msg_temp_geo[22] = "Temperatura suelo ext";
		char msg_temp_agua1[21] = "Temperatura tanque 1";
		char msg_temp_agua2[21] = "Temperatura tanque 2";
		char msg_temp_agua3[21] = "Temperatura tanque 3";
		char msg_hum_aire_sup[22] = "Humedad aire superior";
		char msg_hum_aire_mid[19] = "Humedad aire medio";
		char msg_hum_aire_inf[22] = "Humedad aire inferior";
		char msg_vacio[1] = "";
		char msg_hum_suelo1[20] = "Humedad del suelo 1";
		char msg_hum_suelo2[20] = "Humedad del suelo 2";
		char msg_hum_suelo3[20] = "Humedad del suelo 3";
		char msg_hum_suelo4[20] = "Humedad del suelo 4";

		char msg_conectando[22] 			= "Conectando\n a WiFi...";
		char msg_no_se_encuentra[17]		= "No se\n encuentra";
		char msg_conectado_a[21]			= "Conectado a la red:\n";
		char msg_error_al_iniciar[20]		= "Error al iniciar el";
		char msg_controlador_motivo[21]		= "controlador. Motivo:";
		//char msg_sd_ausente[19]			= "Tarjeta SD ausente";
		char msg_error_sd[20]				= "Error en tarjeta SD";

	public:
		void inicializar();
		void displayLogo();
		void actualizar();
		void cambiarDato();
		void displayConectandoWiFi();
		void displayErrorWiFi();
		void displayConetadoA(String ssid_conectada);
		void displayNoSD();
		void displayErrorSD();
		void displayVentana(bool abriendo);
	private:
		void displayTemperatura(uint8_t pantalla);
		void displayHumedadAire(uint8_t pantalla);
		void displayHumedadSuelo(uint8_t pantalla);
		void display2temperaturas(char msg_arriba[22], float valor_arriba, char msg_abajo[22], float valor_abajo = 0);
		void display2porcentajes(char msg_arriba[22], float valor_arriba, char msg_abajo[22], float valor_abajo = 0);
		void displayError();
} LCDP;


// Conectividad.h
const uint8_t CANT_REDES_WIFI = 3;
class LocalWiFi
{
	public:
		bool hay_conexion = false;
		uint8_t cant_redes = 0;
		char ssid[CANT_REDES_WIFI][W_SSID_SIZE];
		char pass[CANT_REDES_WIFI][W_PASS_SIZE];

	public:
		void inicializarWiFi();
		bool guardarRedWiFi(const char* ssid, const char* password);
		bool guardarRedWiFi(const char* ssid);
		bool correr();
		void correrDisplayWiFi();
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
		bool inicializado = false;
		const uint8_t CARACTERES_NODO_ESCUCHAR = 10;
		uint8_t i_datalog = 0;
		const char NOMBRES_DATOS[23][5] =
		{"T","Ts","Tm","Ti","Tg","Ta1","Ta2","Ta3","HAs","HAm","HAi","HS1","HS2","HS3","HS4",
		"mBO1","BOM1","mBO2","BOM2","mBO3","BOM3","mVEN","VENT"};
		const char HEADLINE_DATALOG[94] =
		"T,Ts,Tm,Ti,Tg,Ta1,Ta2,Ta3,HAs,HAm,HAi,HS1,HS2,HS3,HS4,mBO1,BOM1,mBO2,BOM2,mBO3,BOM3,mVEN,VENT";
		const char NOMBRE_NODO_COMAPP_BOMBA1[7]	= "bomba1";
		const char NOMBRE_NODO_COMAPP_BOMBA2[7]	= "bomba2";
		const char NOMBRE_NODO_COMAPP_BOMBA3[7]	= "bomba3";
		const char NOMBRE_NODO_COMAPP_VENT[5]	= "vent";
	private:
		#define CARACTERES_PATH_LECTURAS 45	// lo de abajo + caracteres necesarios para la timestamp y el '/'
		const char PATH_LECTURAS[33]			= "/Invernadero/lecturas/";
		const char PATH_ESCUCHAR[38]			= "/Invernadero/comApp/";
		const char PATH_ESCRITURA[24]			= "/Invernadero/comModulo/";
		const char NOMBRE_NODO_RTA[5] 			= "rta";
		const char NOMBRE_NODO_ALARMA_ALTA[12]	= "alarmaAlta";
		const char NOMBRE_NODO_ALARMA_BAJA[12]	= "alarmaBaja";
		FirebaseData data;
		FirebaseData stream;
		FirebaseAuth auth;
		FirebaseConfig config;

	public:
		void inicializar();
		inline void setAlarmasFalse();
		void correr();
		void datalog(const char* path_sd);
		void responderOk();
		void enviarAlarmaCaliente();
		void enviarAlarmaFrio();
		inline void comandoSalidaOnOff(SalidaOnOff Salida, uint8_t valor);
        inline void comandoVent(uint8_t valor);
        inline void cambiarAlarmaActivada(bool valor);
        inline void cambiarLapsoAlarma(uint16_t valor);
        inline void cambiarTMaxAlarma(float valor);
        inline void cambiarTMinAlarma(float valor);

		inline void cambiarModoBomba1(uint8_t valor);
		inline void cambiarLapsoBombeo1(uint8_t valor);
		inline void cambiarTiempoEncendidoBomba1(uint8_t valor);
		inline void cambiarModoBomba2(uint8_t valor);
		inline void cambiarLapsoBombeo2(uint8_t valor);
		inline void cambiarTiempoEncendidoBomba2(uint8_t valor);
		inline void cambiarModoBomba3(uint8_t valor);
		inline void cambiarLapsoBombeo3(uint8_t valor);
		inline void cambiarTiempoEncendidoBomba3(uint8_t valor);
		
		inline void cambiarModoVent(uint8_t valor);
        inline void cambiarTMaxVent(float valor);
        inline void cambiarLapsoVent(uint16_t valor);
        inline void cambiarTiempoAperturaVent(uint16_t valor);
        inline void cambiarTiempoMarchaVent(uint8_t valor);
	private:
		void enviarParametros();
		inline bool enviarJson(FirebaseData* data, const char* path, FirebaseJson* json);
} LCFB;
void appInput(FirebaseStream data);
void appInputTimeout(bool comandoTimeout);


// SD_manejo.h
enum class ResultadoLecturaSD : uint8_t
{
	NO_ARCHIVO,
	NO_CONTENIDO,
	EXITOSO
};
class LocalSD
{
	public:
		const char DATALOG_PATH[23]				= "/controlador/datos.txt";
	private:
		unsigned long ultimo_datalog = 0;
		const char TXT[5]						= ".txt";
		// NOMBRES DE LOS FOLDERS Y ARCHIVOS
		const char WIFI_FOLDER_PATH[26]			= "/controlador/config/wifi/";
		const char FIREBASE_FOLDER_PATH[30]		= "/controlador/config/firebase/";
		const char PARAMETROS_FOLDER_PATH[32]	= "/controlador/config/parametros/";
		// NOMBRES DE FOLDER WIFI
		const char NOMBRE_ARCHIVO_WSSID[5]		= "ssid";
		const char NOMBRE_ARCHIVO_WPASS[5]		= "pass";
		// NOMBRES DE FOLDER FIREBASE
		const char NOMBRE_ARCHIVO_FEMAIL[6]		= "email";
		const char NOMBRE_ARCHIVO_FPASS[5]		= "pass";
		const char NOMBRE_ARCHIVO_FURL[4]		= "url";
		const char NOMBRE_ARCHIVO_FAPIKEY[7]	= "apikey";
	public:
		void inicializar();
		void leerConfigWiFi();
		void leerConfigFirebase();
		void leerConfigParametros();
		void datalog();
		template <typename T>
		void escribirFBySD(const char* path, String &string, bool coma, T dato, FirebaseJson* json);
	private:
		void escribir(const char* path, String string);
		ResultadoLecturaSD leerStringA(char* buffer, const uint8_t caracteres, const char* path);
} LCSD;


// EEMPROM_manejo.h
class LocalEEPROM
{
	public:
		// alarma
		const bool		ALARMA_ACTIVADA_DEFECTO				= true;
		const uint16_t	LAPSO_ALARMA_MIN_DEFECTO			= 60;
		const float		TEMP_MAXIMA_ALARMA_DEFECTO			= 45.0;
		const float		TEMP_MINIMA_ALARMA_DEFECTO			= -5.0;
		// bomba1
		const uint8_t	MODO_BOMBA1_DEFECTO					= static_cast<uint8_t>(SalidaModos::Forzada);
		const uint16_t	LAPSO_BOMBEO1_MIN_DEFECTO			= 1440;			// 24 h
		const uint16_t	TIEMPO_ENCENDIDO_BOMBA1_MIN_DEFECTO = 60;
		// bomba2
		const uint8_t	MODO_BOMBA2_DEFECTO					= static_cast<uint8_t>(SalidaModos::Forzada);
		const uint16_t	LAPSO_BOMBEO2_MIN_DEFECTO			= 1440;			// 24 h
		const uint16_t	TIEMPO_ENCENDIDO_BOMBA2_MIN_DEFECTO = 60;
		// bomba3
		const uint8_t	MODO_BOMBA3_DEFECTO					= static_cast<uint8_t>(SalidaModos::Forzada);
		const uint16_t	LAPSO_BOMBEO3_MIN_DEFECTO			= 1440;			// 24 h
		const uint16_t	TIEMPO_ENCENDIDO_BOMBA3_MIN_DEFECTO = 60;
		// ventilación
		const uint8_t	MODO_VENT_DEFECTO					= static_cast<uint8_t>(SalidaModos::Automatica);
		const float		TEMP_MAXIMA_VENTILACION_DEFECTO		= 32.0;
		const uint16_t	LAPSO_VENTILACIONES_MIN_DEFECTO 	= 1440;			// 24 h
		const uint16_t	TIEMPO_APERTURA_VENT_MIN_DEFECTO	= 30;
		const uint8_t	TIEMPO_MARCHA_VENT_SEG_DEFECTO		= 7;

		bool		eeprom_programada;			// 0.	para verificar si está programada o no la EEPROM
		// alarma
		bool		alarma_activada;			// 1.
		uint16_t	lapso_alarma_min;			// 2.
		float		temp_maxima_alarma;			// 3.
		float		temp_minima_alarma;			// 4.
		// bomba1
		uint8_t		modo_bomba1;				// 5.
		uint16_t	lapso_bombeo1_min;			// 6.
		uint16_t	tiempo_encendido_bomba1_min;// 7.
		// bomba2
		uint8_t		modo_bomba2;				// 8.
		uint16_t	lapso_bombeo2_min;			// 9.
		uint16_t	tiempo_encendido_bomba2_min;// 10.
		// bomba1
		uint8_t		modo_bomba3;				// 11.
		uint16_t	lapso_bombeo3_min;			// 12.
		uint16_t	tiempo_encendido_bomba3_min;// 13.
		// ventilación
		uint8_t		modo_vent;					//   14
		float		temp_maxima_ventilacion;	//a  15.
		uint16_t	lapso_ventilaciones_min;	// t 16.
		uint16_t	tiempo_apertura_vent_min;	// t 17.
		uint8_t		tiempo_marcha_vent_seg;		//at 18.
		enum OrdenParametros : uint8_t
		{
			PROGRAMADA,
			ALARMA_ACTIVADA,
			LAPSO_ALARMA_MIN,
			TEMP_MAXIMA_ALARMA,
			TEMP_MINIMA_ALARMA,
			MODO_BOMBA1,
			LAPSO_BOMBEO1_MIN,
			TIEMPO_ENCENDIDO_BOMBA1_MIN,
			MODO_BOMBA2,
			LAPSO_BOMBEO2_MIN,
			TIEMPO_ENCENDIDO_BOMBA2_MIN,
			MODO_BOMBA3,
			LAPSO_BOMBEO3_MIN,
			TIEMPO_ENCENDIDO_BOMBA3_MIN,
			MODO_VENT,
			TEMP_MAXIMA_VENTILACION,
			LAPSO_VENTILACIONES_MIN,
			TIEMPO_APERTURA_VENT_MIN,
			TIEMPO_MARCHA_VENT_SEG,
			CANT_VARIABLES
		};//									   bool, uint8_t = 1;		uint16_t = 2;		float = 4
		const uint8_t LONGITUD_DATO[CANT_VARIABLES] = { 1, 1, 2, 4, 4, 1, 2, 2, 1, 2, 2, 1, 2, 2, 1, 4, 2, 2, 1};
		int direccion[CANT_VARIABLES];
	private:
		int espacios;

	public:
		void inicializar();
		void leerCompleta();
		void cargarValoresPorDefecto();
		template <typename T>
		void escribir(int Adireccion, T Adato);
		template <typename T>
		T leer(int Adireccion);
	private:
		void setDirecciones();
		void imprimirValsDirsReads();
} LCEE;


// Clases
File DatalogSD;
WiFiMulti WiFiMultiO;
Adafruit_SSD1306 Display(128, 64, &Wire, -1); // 128 px de ancho por 64 px de alto
AHTxx AhtSeleccionado(AHTXX_ADDRESS_X38, AHT1x_SENSOR);
AHT10Mux AhtInteriorHigh(PinsAHT10MUX::INT_HIGH);
AHT10Mux AhtInteriorMid(PinsAHT10MUX::INT_MID);
AHT10Mux AhtInteriorLow(PinsAHT10MUX::INT_LOW);
AHT10Mux AhtAgua1(PinsAHT10MUX::AGUA1);
AHT10Mux AhtAgua2(PinsAHT10MUX::AGUA2);
AHT10Mux AhtAgua3(PinsAHT10MUX::AGUA3);
AHT10Mux AhtGeotermico(PinsAHT10MUX::GEOTERMICO);
SalidaOnOff Bomba1(PinsOut::BOMBA1);
SalidaOnOff Bomba2(PinsOut::BOMBA2);
SalidaOnOff Bomba3(PinsOut::BOMBA3);
SalidaVentilacion Ventilacion(PinsOut::MARCHA, PinsOut::CONTRAMARCHA);
#pragma once

#include "Claves.h"

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

//#define DEBUG_SERIAL // Comentar para eliminar los Serial.print
#ifdef DEBUG_SERIAL
	#define imprimir(x) Serial.print(x)
	#define imprimirln(x) Serial.println(x)
#else
	#define imprimir(x)
	#define imprimirln(x)
#endif



// Constantes de funcionamiento generales
const unsigned long DELAY_ACTIVIDAD_INVERNADERO = 0;		// (ms) tiempo de espera para el loop del invernadero
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
const unsigned long DELAY_DATALOG = 5000; // 9,97 años hasta alcanzar el máximo de renglones SD (1048576)



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
//float temp_interior_promedio;			// TODO: ver si son necesarias
//float humedad_aire_interior_promedio;
float humedad_int_high;
float humedad_int_mid;
float humedad_int_low;
// soil moisture sensors
int humedad_suelo1;
int humedad_suelo2;
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
const float DELTA_T_VENTILACION	= 1;	// ver Control.h
const float DELTA_T_CALEFA		= DELTA_T_VENTILACION;
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
	private:
		bool esperando_riego = false; // para riegoAutomatico()
		unsigned long ultima_actualizacion_alarma = 0;
	public:
		void configurarModosSalidas();
		void controlarRiego();
		void controlarCalefa();
		void controlarVentilacion();
		void controlarAlarma();
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
const unsigned long DELAY_CAMBIO_DISPLAY		= 6000;
const unsigned long DELAY_ACTUALIZACION_DISPLAY	= 500;
const uint8_t SCREEN_WIDTH	= 128;		// ancho del display OLED display, en píxeles
const uint8_t SCREEN_HEIGHT	= 64;		// alto del display OLED display, en píxeles
enum class DisplayDato : uint8_t
{
	Temperatura1,
	Temperatura2,
	HumedadAire1,
	HumedadAire2,
	HumedadSuelo,
	WiFi
};
DisplayDato DatoDelDisplay = DisplayDato::Temperatura1;
class LocalDisplay
{
	public:
		unsigned long ultima_actualizacion = 0;
		unsigned long ultima_vez_cambio = 0;
	private:
		char msg_temp_sup[21] = "Temperatura superior";
		char msg_temp_mid[18] = "Temperatura medio";
		char msg_temp_inf[21] = "Temperatura inferior";
		char msg_temp_geo[22] = "Temperatura suelo ext";
		char msg_hum_aire_sup[22] = "Humedad aire superior";
		char msg_hum_aire_mid[19] = "Humedad aire medio";
		char msg_hum_aire_inf[22] = "Humedad aire inferior";
		char msg_vacio[1] = "";
		char msg_hum_suelo1[20] = "Humedad del suelo 1";
		char msg_hum_suelo2[20] = "Humedad del suelo 2";

		char msg_conectando[22] 			= "Conectando\n a WiFi...";
		char msg_no_se_encuentra[17]		= "No se\n encuentra";
		char msg_conectado_a[21]			= "Conectado a la red:\n";
		char msg_error_al_iniciar[20]		= "Error al iniciar el";
		char msg_controlador_motivo[21]		= "controlador. Motivo:";
		//char msg_sd_ausente[19]			= "Tarjeta SD ausente";
		char msg_error_sd[20]				= "Error en tarjeta SD";

	public:
		void inicializar();
		void displayLogo(/*TODO: SE VALE PONER DELAY (1-2 seg)*/); // Invernadero inteligente que esté centrado, nada más
		void actualizar();
		void cambiarDato();
		void displayConectandoWiFi();
		void displayErrorWiFi();
		void displayConetadoA(String ssid_conectada);
		void displayNoSD();
		void displayErrorSD();
	private:
		void displayTemperatura(uint8_t pantalla);
		void displayHumedadAire(uint8_t pantalla);
		void displayHumedadSuelo();
		void display2temperaturas(char msg_arriba[22], float valor_arriba, char msg_abajo[22], float valor_abajo = 0);
		void display2porcentajes(char msg_arriba[22], float valor_arriba, char msg_abajo[22], float valor_abajo = 0);
		void displayError();
} LCDP;


// Graficos.h



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
		uint8_t i_datalog = 0;	// AMBOS
		FirebaseJson json;		// AMBOS PÚBLICOS POR SER REFERENCIADOS EN SD.h, Y EL DE ABAJO TAMBIÉN
		const char NOMBRES_DATOS[13][8]	= {"T(s)","Ts","Tm","Ti","Tg(°C)","HAs","HAm","HAi","HS1","HS2(%)","RIE","CAL","VENT"};
	private:
		#define CARACTERES_PATH_LECTURAS 33 // lo de abajo + caracteres necesarios para la timestamp
		const char PATH_LECTURAS[CARACTERES_PATH_LECTURAS]	= "/Invernadero/lecturas/";
		const char PATH_ESCUCHAR[38]						= "/Invernadero/comApp/";
		const char PATH_ESCRITURA[24]						= "/Invernadero/comModulo/";
		FirebaseData data;
		FirebaseData stream;
		FirebaseAuth auth;
		FirebaseConfig config;

	public:
		void inicializar();
		void correr();
		void datalog(File sd);
		void responderOk();
		void enviarAlarmaCaliente();
		void enviarAlarmaFrio();
		
		inline void comandoRiego(uint8_t valor);
        inline void comandoCalefa(uint8_t valor);
        inline void comandoVent(uint8_t valor);
        inline void cambiarModosSalidas(uint8_t valor);
        inline void cambiarAlarmaActivada(bool valor);
        inline void cambiarLapsoAlarma(uint16_t valor);
        inline void cambiarTMaxAlarma(float valor);
        inline void cambiarTMinAlarma(float valor);
        inline void cambiarHumSueloMin(uint8_t valor);
        inline void cambiarLapsoRiegos(uint16_t valor);
        inline void cambiarTiempoBombeo(uint16_t valor);
        inline void cambiarTiempoEspera(uint16_t valor);
        inline void cambiarTMinCalefa(float valor);
        inline void cambiarLapsoCalefas(uint16_t valor);
        inline void cambiarTiempoEncendidoCalefa(uint16_t valor);
        inline void cambiarTMaxVent(float valor);
        inline void cambiarLapsoVent(uint16_t valor);
        inline void cambiarTiempoAperturaVent(uint16_t valor);
        inline void cambiarTiempoMarchaVent(uint8_t valor);
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
	private:
		unsigned long ultimo_datalog = 0;
		const char TXT[5]						= ".txt";
		const char DATALOG_PATH[22]				= "controlador/datos.txt";
		const char DATALOG_HEADLINE[57]		  	= "T(s),Ts,Tm,Ti,Tg(°C),HAs,HAm,HAi,HS1,HS2(%),RIE,CAL,VEN";
		// NOMBRES DE LOS FOLDERS Y ARCHIVOS
		const char WIFI_FOLDER_PATH[25]			= "controlador/config/wifi/";
		const char FIREBASE_FOLDER_PATH[29]		= "controlador/config/firebase/";
		const char PARAMETROS_FOLDER_PATH[31]	= "controlador/config/parametros/";
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
		void escribirFBySDabierta(File Archivo, T dato, bool coma);
	private:
		ResultadoLecturaSD leerStringA(char *buffer, const uint8_t caracteres, const char *path);
} LCSD;


// Telegram.h
// variables
#define TELEGRAM_TIEMPO_MAX_CONFIGURACION	15000UL
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
//void comandoInfo();
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
class LocalEEPROM
{
	public:
		const uint8_t	MODOS_SALIDAS_DEFECTO				= 0b00000000;	// todas en automático
		// alarma
		const bool		ALARMA_ACTIVADA_DEFECTO				= true;
		const uint16_t	LAPSO_ALARMA_MIN_DEFECTO			= 60;
		const float		TEMP_MAXIMA_ALARMA_DEFECTO			= 45.0;
		const float		TEMP_MINIMA_ALARMA_DEFECTO			= -5.0;
		// riego
		const uint8_t	HUMEDAD_SUELO_MINIMA_DEFECTO		= 60;
		const uint16_t	LAPSO_RIEGOS_MIN_DEFECTO			= 720;			// 12 h
		const uint16_t	TIEMPO_BOMBEO_SEG_DEFECTO			= 10;
		const uint16_t	TIEMPO_ESPERA_MIN_DEFECTO			= 15;
		// calefa
		const float		TEMP_MINIMA_CALEFA_DEFECTO			= 5.0;
		const uint16_t	LAPSO_CALEFAS_MIN_DEFECTO			= 1440;		// 24 h
		const uint16_t	TIEMPO_ENCENDIDO_CALEFA_MIN_DEFECTO = 60;
		// ventilación
		const float		TEMP_MAXIMA_VENTILACION_DEFECTO		= 32.0;
		const uint16_t	LAPSO_VENTILACIONES_MIN_DEFECTO 	= 1440;		// 24 h
		const uint16_t	TIEMPO_APERTURA_VENT_MIN_DEFECTO	= 30;
		const uint8_t	TIEMPO_MARCHA_VENT_SEG_DEFECTO		= 7;

		bool		eeprom_programada;			// 0.	para verificar si está programada o no la EEPROM
		uint8_t		modos_salidas;				// 1.
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
AHTxx AhtSeleccionado(AHTXX_ADDRESS_X38, AHT1x_SENSOR);
AHT10Mux AhtInteriorHigh(PinsAHT10MUX::INT_HIGH);
AHT10Mux AhtInteriorMid(PinsAHT10MUX::INT_MID);
AHT10Mux AhtInteriorLow(PinsAHT10MUX::INT_LOW);
AHT10Mux AhtGeotermico(PinsAHT10MUX::GEOTERMICO);
SalidaOnOff Riego(PinsOut::RIEGO);
SalidaOnOff Calefa(PinsOut::CALEFA);
SalidaVentilacion Ventilacion(PinsOut::MARCHA, PinsOut::CONTRAMARCHA);
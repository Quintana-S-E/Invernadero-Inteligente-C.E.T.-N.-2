/*------------------------------------------------------------------------------------------------------------------*\
	Nombre:				Invernadero Inteligente maqueta ESP32 Node32s
	Desarrollo: 		octubre - diciembre de 2022
	Primer lanzamiento: 14 de noviembre de 2022
	Creado por:			Pulido Norberto N., Quintana Santiago E., Riha Fabio, Sacchero Fidel, Inticito.
\*------------------------------------------------------------------------------------------------------------------*/

#include "Control.h"
#include "Conectividad.h"
#include "Declaraciones.h" // contiene <Arduino.h> y todas las librerías
#include "Display.h"
#include "EEPROM_manejo.h"
#include "Firebase.h"
#include "Graficos.h"
#include "Sensores.h"
#include "SD_manejo.h"
#include "Telegram.h"

#include "Claves.h"

void setup()
{
	// inicializaciones varias
	#ifdef DEBUG_SERIAL
		Serial.begin(9600);
	#endif
	pinMode(PIN_BTN, INPUT_PULLUP);
	pinMode(LED_VENTILACION, OUTPUT);
	pinMode(LED_WIFI, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(PIN_BOMBA1, OUTPUT);
	pinMode(PIN_BOMBA2, OUTPUT);
	pinMode(PIN_BOMBA3, OUTPUT);
	pinMode(PIN_VENTILACION, OUTPUT);
	digitalWrite(PIN_BOMBA1, HIGH);
	digitalWrite(PIN_BOMBA2, HIGH);
	digitalWrite(PIN_BOMBA3, HIGH);
	digitalWrite(PIN_VENTILACION, HIGH);
	pinMode(MUX_A, OUTPUT);
	pinMode(MUX_B, OUTPUT);
	pinMode(MUX_C, OUTPUT);
	// ver, para el futuro https://forum.arduino.cc/t/digitalwritefast-digitalreadfast-pinmodefast-etc/47037

	// inicializar display
	inicializarDisplay();
	displayLogo();

	// inicializar sensores y SD
	inicializarSensores();
	LCSD.inicializar();

	// leer los archivos de configuración de la tarjeta SD
	LCSD.leerConfigWiFi();
	LCSD.leerConfigFirebase();

	// leer o escribir la EEPROM
	LCEE.inicializar();

	delay(3500);

	// Conectarse a WiFi
	imprimirln("Conectando a WiFi...");
	LCWF.inicializarWiFi();
	imprimirln("Setup finalizado!");
	delay(2000);
}

//==============================================aquí se encuentra la función loop()==============================================//

void loop()
{
	if (millis() - ultima_vez_invernadero_funciono >= DELAY_ACTIVIDAD_INVERNADERO)
	{ // idealmente, en lugar de esperas pondríamos al uC en un estado de bajo consumo por un período fijo
		ultima_vez_invernadero_funciono = millis();
		
		LCWF.correr();

		// Leer sensores
		leerSensores();

		// Manejar Telegram
		controlarMensajesRecibidosTelegram();
		controlarAlarma();

		// Actualizar datos mostrables
		actualizarDisplay();
		actualizarGraficos();

		// Tomar decisiones
		controlarVentilacion();
		controlarRiego();
		//controlar_iluminacion();

		digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // parpadeamos el LED de la placa
	}

	// cambiamos el contenido de la pantalla
	if (millis() - ultima_vez_display_cambio >= DELAY_CAMBIO_DISPLAY)
	{
		ultima_vez_display_cambio = millis();
		cambiarDatoDisplay();
	}
}
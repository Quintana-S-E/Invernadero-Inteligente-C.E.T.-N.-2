/*------------------------------------------------------------------------------------------------------------------*\
	Nombre:				Version tradicional - Invernadero Inteligente
	Desarrollo: 		octubre 2022 - septiembre 2023
	Primer lanzamiento: -
	Creado por:			Quintana Santiago Esteban
\*------------------------------------------------------------------------------------------------------------------*/

#include "Control.h"
#include "Conectividad.h"
#include "Declaraciones.h" // contiene <Arduino.h> y todas las librerías
#include "Display.h"
#include "EEPROM_manejo.h"
#include "Firebase.h"
//#include "Graficos.h"
#include "Sensores.h"
#include "SD_manejo.h"
#include "Tiempo.h"
//#include "Telegram.h"

#include "Claves.h"

void setup()
{
	// inicializaciones varias
	#ifdef DEBUG_SERIAL
		Serial.begin(9600);
	#endif
	for (PinsOut pin : {PinsOut::RIEGO, PinsOut::CALEFA,
						PinsOut::MARCHA, PinsOut::CONTRAMARCHA,
						PinsOut::MUX_A, PinsOut::MUX_B,	 PinsOut:: MUX_C})
	{
		digitalWrite(static_cast<uint8_t>(pin), LOW);
		pinMode(static_cast<uint8_t>(pin), OUTPUT);
  	}

	// inicializar display
	LCDP.inicializar();
	LCDP.displayLogo();

	// inicializar sensores y SD
	inicializarSensores();
	LCSD.inicializar();

	// leer los archivos de configuración de la tarjeta SD
	LCSD.leerConfigWiFi();
	LCSD.leerConfigFirebase();

	LCEE.inicializar();

	LCCT.configurarModosSalidas();

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

		// Leer sensores
		leerSensores();
		
		// Manejar conexiones y comunicaciones
		LCWF.correr();
		LCCT.controlarAlarma();
		//LCFB.correr();

		// Datalog
		//LCFB.datalog();
		LCSD.datalog();

		// Tomar decisiones
		LCCT.controlarRiego();
		LCCT.controlarCalefa();
		LCCT.controlarVentilacion();

		// Actualizar datos mostrables
		LCDP.actualizar();
		//actualizarGraficos();
	}

	// cambiamos el contenido de la pantalla
	if (millis() - LCDP.ultima_vez_cambio >= DELAY_CAMBIO_DISPLAY)
	{
		LCDP.ultima_vez_cambio = millis();
		LCDP.cambiarDato();
	}
}
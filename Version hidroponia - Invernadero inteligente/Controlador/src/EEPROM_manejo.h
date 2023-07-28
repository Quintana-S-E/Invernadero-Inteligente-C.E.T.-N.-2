#pragma once

#include "Declaraciones.h"

void setDireccionesEEPROM() // en "setup()"
{
	// hay que hacer EEPROM.put a las direcciones indicadas, según la longitud de los datos escritos anteriormente
	int i;
	direccion[0] = 0;
	for (i = 1; i < CANT_VARIABLES_EEPROM; ++i)
		direccion[i] = direccion[i - 1] + LONGITUD_DATO_EEPROM[i - 1];
	espacios_EEPROM = direccion[i - 1] + LONGITUD_DATO_EEPROM[i - 1];
}

//==================================================================================================================//

void chequearEEPROMProgramada() // en "setup()"
{
	setDireccionesEEPROM();
	EEPROM.begin(espacios_EEPROM); // la abrimos
	eeprom_programada = (EEPROM.read(0) == 255 || EEPROM.read(0) == 0) ? false : true;
	EEPROM.end(); // la cerramos

	if (eeprom_programada)
	{
		imprimirln("Hay que leer la EEPROM");
		leerEEPROMProgramada();
	}
	else
	{
		imprimirln("Hay que escribir la EEPROM");
		cargarValoresPorDefecto(); // TODO: que primero vea si tiene WiFi
	}

#ifdef DEBUGserial
	imprimirEEPROMValsDirsReads();
#endif
}

//==================================================================================================================//

void leerEEPROMProgramada() // en "chequearEEPROMProgramada()"
{
	EEPROM.begin(espacios_EEPROM);
	EEPROM.get(direccion[DIR_EEPROM_PROGRAMADA],		eeprom_programada); //	*1
	EEPROM.get(direccion[DIR_TEMP_MAXIMA_ALARMA],		temp_maxima_alarma);
	EEPROM.get(direccion[DIR_TEMP_MINIMA_ALARMA],		temp_minima_alarma);
	EEPROM.get(direccion[DIR_TEMP_MAXIMA_VENTILACION],	temp_maxima_ventilacion);
	EEPROM.get(direccion[DIR_HUMEDAD_SUELO_MINIMA],		humedad_suelo_minima);
	EEPROM.get(direccion[DIR_LAPSO_ALARMA_MINUTOS],		lapso_alarma_minutos);
	EEPROM.get(direccion[DIR_ALARMA_ACTIVADA],			alarma_activada);
	EEPROM.get(direccion[DIR_TIEMPO_BOMBEO_SEGUNDOS],	tiempo_bombeo_segundos);
	EEPROM.get(direccion[DIR_TIEMPO_ESPERA_MINUTOS],	tiempo_espera_minutos);
	EEPROM.end();
}
//	*1 no es necesario, ya que en chequearEEPROMProgramada se le asignó true para poder entrar a esta función, y
//	ya que está programada va a volver a dar 1. Sólo se pone para una mejor legibilidad del código
//==================================================================================================================//

// TODO: que tome un argumento para ver si setear eeprom_programada o no. Si al iniciar el controlador
// no le pusieron redes wifi en la SD, que llame a esta función con el argumento false.
void cargarValoresPorDefecto()
{
	eeprom_programada =			true; // ahora va a estar programada
	temp_maxima_alarma =		TEMP_MAXIMA_ALARMA_DEFECTO;
	temp_minima_alarma =		TEMP_MINIMA_ALARMA_DEFECTO;
	temp_maxima_ventilacion =	TEMP_MAXIMA_VENTILACION_DEFECTO;
	humedad_suelo_minima =		HUMEDAD_SUELO_MINIMA_DEFECTO;
	lapso_alarma_minutos =		LAPSO_ALARMA_MINUTOS_DEFECTO;
	alarma_activada =			ALARMA_ACTIVADA_DEFECTO;
	tiempo_bombeo_segundos =	TIEMPO_BOMBEO_SEGUNDOS_DEFECTO;
	tiempo_espera_minutos =		TIEMPO_ESPERA_MINUTOS_DEFECTO;

	EEPROM.begin(espacios_EEPROM);
	EEPROM.put(direccion[DIR_EEPROM_PROGRAMADA],		eeprom_programada);
	EEPROM.put(direccion[DIR_TEMP_MAXIMA_ALARMA],		temp_maxima_alarma);
	EEPROM.put(direccion[DIR_TEMP_MINIMA_ALARMA],		temp_minima_alarma);
	EEPROM.put(direccion[DIR_TEMP_MAXIMA_VENTILACION],	temp_maxima_ventilacion);
	EEPROM.put(direccion[DIR_HUMEDAD_SUELO_MINIMA],		humedad_suelo_minima);
	EEPROM.put(direccion[DIR_LAPSO_ALARMA_MINUTOS],		lapso_alarma_minutos);
	EEPROM.put(direccion[DIR_ALARMA_ACTIVADA],			alarma_activada);
	EEPROM.put(direccion[DIR_TIEMPO_BOMBEO_SEGUNDOS],	tiempo_bombeo_segundos);
	EEPROM.put(direccion[DIR_TIEMPO_ESPERA_MINUTOS],	tiempo_espera_minutos);
	EEPROM.commit(); // efectivamente escribir
	EEPROM.end();
}

//==================================================================================================================//

template <typename T> // puede aceptar cualquier tipo de dato de entrada
void escribirEEPROM(int Adireccion, T Adato) // en chequearMensajesRecibidosTelegram()
{
	EEPROM.begin(espacios_EEPROM);
	EEPROM.put(Adireccion, Adato);
	EEPROM.commit(); // efectivamente escribir
	EEPROM.end();
}

//==================================================================================================================//

void imprimirEEPROMValsDirsReads()
{
	Serial.println();
	Serial.println("Valores recuperados de la EEPROM:");
	Serial.println(eeprom_programada);
	Serial.println(temp_maxima_alarma);
	Serial.println(temp_minima_alarma);
	Serial.println(temp_maxima_ventilacion);
	Serial.println(humedad_suelo_minima);
	Serial.println(lapso_alarma_minutos);
	Serial.println(alarma_activada);
	Serial.println(tiempo_bombeo_segundos);
	Serial.println(tiempo_espera_minutos);
	Serial.println();

	Serial.println("Valores de las direcciones:");
	for (int i = 0; i < CANT_VARIABLES_EEPROM; ++i)
	{
		Serial.print("Dir ");
		Serial.print(i);
		Serial.print(": ");
		Serial.println(direccion[i]);
	}
	Serial.println();

	EEPROM.begin(30);
	for (int j = 0; j < 30; ++j)
	{
		Serial.print("Dirección ");
		Serial.print(j);
		Serial.print(": ");
		Serial.println(EEPROM.read(j));
	}
	EEPROM.end();
}

//=================================== ANTERIOR SISTEMA PARA setDireccionesEEPROM ===================================
/*
acá todas las variables solamente y después:

#define CANT_VARIABLES_EEPROM 10
int direccion[CANT_VARIABLES_EEPROM];
int espacios_EEPROM;

void setDireccionesEEPROM()
{
	// hay que hacer .put a las direcciones indicadas, según la longitud de los datos escritos anteriormente
	direccion[0] = 0;
	direccion[1] = direccion[0] + 1;//bool eeprom_programada
	direccion[2] = direccion[1] + 4;//float	temp_maxima_alarma
	direccion[3] = direccion[2] + 4;//float	temp_minima_alarma
	direccion[4] = direccion[3] + 4;//float	temp_maxima_ventilacion
	direccion[5] = direccion[4] + 1;//int	humedad_suelo_minima
	direccion[6] = direccion[5] + 2;//int	lapso_alarma_minutos
	direccion[7] = direccion[6] + 1;//bool	alarma_activada
	direccion[8] = direccion[7] + 2;//int	tiempo_bombeo_segundos
	direccion[9] = direccion[8] + 2;//int	tiempo_espera_minutos
 espacios_EEPROM = direccion[9] + x;//sizeof(x)	nombrede_x
}
*/
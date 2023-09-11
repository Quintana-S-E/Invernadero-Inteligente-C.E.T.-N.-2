#pragma once

#include "Declaraciones.h"

// hay que hacer EEPROM.put a las direcciones indicadas, según la longitud de los datos escritos anteriormente
void LocalEEPROM::setDirecciones() // en "inicializar()"
{
	int i;
	this->direccion[0] = 0;
	for (i = 1; i < this->CANT_VARIABLES; ++i)
		this->direccion[i] = this->direccion[i - 1] + this->LONGITUD_DATO[i - 1];
	this->espacios = this->direccion[i - 1] + this->LONGITUD_DATO[i - 1];
}

//==================================================================================================================//

void LocalEEPROM::inicializar() // en "setup()"
{
	setDirecciones();

	EEPROM.begin(this->espacios);
	eeprom_programada = (EEPROM.read(0) == 255 || EEPROM.read(0) == 0) ? false : true;
	EEPROM.end();

	if (!eeprom_programada)
	{
		imprimirln("Hay que escribir la EEPROM");
		LCSD.leerConfigParametros();
	}
	else
	{
		imprimirln("Hay que leer la EEPROM");
		if (LCWF.cant_redes > 0)
			leerCompleta();
		else
			LCSD.leerConfigParametros();
	}

#ifdef DEBUGserial
	imprimirEEPROMValsDirsReads();
#endif
}

//==================================================================================================================//

void LocalEEPROM::leerCompleta()
{
	EEPROM.begin(this->espacios);
	EEPROM.get(this->direccion[PROGRAMADA],					eeprom_programada); //	*1
	EEPROM.get(this->direccion[ALARMA_ACTIVADA],			alarma_activada);
	EEPROM.get(this->direccion[TEMP_MAXIMA_ALARMA],			temp_maxima_alarma);
	EEPROM.get(this->direccion[TEMP_MINIMA_ALARMA],			temp_minima_alarma);
	EEPROM.get(this->direccion[TEMP_MAXIMA_VENTILACION],	temp_maxima_ventilacion);
	EEPROM.get(this->direccion[HUMEDAD_SUELO_MINIMA],		humedad_suelo_minima);
	EEPROM.get(this->direccion[LAPSO_ALARMA_MINUTOS],		lapso_alarma_minutos);
	EEPROM.get(this->direccion[TIEMPO_BOMBEO_SEGUNDOS],		tiempo_bombeo_segundos);
	EEPROM.get(this->direccion[TIEMPO_ESPERA_MINUTOS],		tiempo_espera_minutos);

	EEPROM.get(this->direccion[ESTADOS_SALIDAS],				estados_salidas);
	EEPROM.get(this->direccion[LAPSO_VENTILACIONES_MIN],		lapso_ventilaciones_min);
	EEPROM.get(this->direccion[TIEMPO_APERTURA_VENT_MIN],		tiempo_apertura_vent_min);
	EEPROM.get(this->direccion[LAPSO_RIEGOS_MIN],				lapso_riegos_min);
	EEPROM.get(this->direccion[TEMP_MINIMA_CALEFA],				temp_minima_calefa);
	EEPROM.get(this->direccion[LAPSO_CALEFAS_MIN],				lapso_calefas_min);
	EEPROM.get(this->direccion[TIEMPO_ENCENDIDO_CALEFA_MIN],	tiempo_encendido_calefa_min);
	EEPROM.get(this->direccion[TIEMPO_MARCHA_VENT_SEG],			tiempo_marcha_vent_seg);
	EEPROM.end();
}
//	*1 no es necesario, ya que en inicializar se le asignó true para poder entrar a esta función, y
//	ya que está programada va a volver a dar 1. Sólo se pone para una mejor legibilidad del código
//==================================================================================================================//

void LocalEEPROM::cargarValoresPorDefecto()
{
	eeprom_programada =			true; // ahora va a estar programada
	alarma_activada =			ALARMA_ACTIVADA_DEFECTO;
	temp_maxima_alarma =		TEMP_MAXIMA_ALARMA_DEFECTO;
	temp_minima_alarma =		TEMP_MINIMA_ALARMA_DEFECTO;
	temp_maxima_ventilacion =	TEMP_MAXIMA_VENTILACION_DEFECTO;
	humedad_suelo_minima =		HUMEDAD_SUELO_MINIMA_DEFECTO;
	lapso_alarma_minutos =		LAPSO_ALARMA_MINUTOS_DEFECTO;
	tiempo_bombeo_segundos =	TIEMPO_BOMBEO_SEGUNDOS_DEFECTO;
	tiempo_espera_minutos =		TIEMPO_ESPERA_MINUTOS_DEFECTO;

	estados_salidas =				ESTADOS_SALIDAS_DEFECTO;
	lapso_ventilaciones_min =		LAPSO_VENTILACIONES_MIN_DEFECTO;
	tiempo_apertura_vent_min =		TIEMPO_APERTURA_VENT_MIN_DEFECTO;
	lapso_riegos_min =				LAPSO_RIEGOS_MIN_DEFECTO;
	temp_minima_calefa =			TEMP_MINIMA_CALEFA_DEFECTO;
	lapso_calefas_min =				LAPSO_CALEFAS_MIN_DEFECTO;
	tiempo_encendido_calefa_min =	TIEMPO_ENCENDIDO_CALEFA_MIN_DEFECTO;
	tiempo_marcha_vent_seg =		TIEMPO_MARCHA_VENT_SEG_DEFECTO;

	EEPROM.begin(this->espacios);
	EEPROM.put(this->direccion[PROGRAMADA],					eeprom_programada);
	EEPROM.put(this->direccion[ALARMA_ACTIVADA],			alarma_activada);
	EEPROM.put(this->direccion[TEMP_MAXIMA_ALARMA],			temp_maxima_alarma);
	EEPROM.put(this->direccion[TEMP_MINIMA_ALARMA],			temp_minima_alarma);
	EEPROM.put(this->direccion[TEMP_MAXIMA_VENTILACION],	temp_maxima_ventilacion);
	EEPROM.put(this->direccion[HUMEDAD_SUELO_MINIMA],		humedad_suelo_minima);
	EEPROM.put(this->direccion[LAPSO_ALARMA_MINUTOS],		lapso_alarma_minutos);
	EEPROM.put(this->direccion[TIEMPO_BOMBEO_SEGUNDOS],		tiempo_bombeo_segundos);
	EEPROM.put(this->direccion[TIEMPO_ESPERA_MINUTOS],		tiempo_espera_minutos);

	EEPROM.put(this->direccion[ESTADOS_SALIDAS],				estados_salidas);
	EEPROM.put(this->direccion[LAPSO_VENTILACIONES_MIN],		lapso_ventilaciones_min);
	EEPROM.put(this->direccion[TIEMPO_APERTURA_VENT_MIN],		tiempo_apertura_vent_min);
	EEPROM.put(this->direccion[LAPSO_RIEGOS_MIN],				lapso_riegos_min);
	EEPROM.put(this->direccion[TEMP_MINIMA_CALEFA],				temp_minima_calefa);
	EEPROM.put(this->direccion[LAPSO_CALEFAS_MIN],				lapso_calefas_min);
	EEPROM.put(this->direccion[TIEMPO_ENCENDIDO_CALEFA_MIN],	tiempo_encendido_calefa_min);
	EEPROM.put(this->direccion[TIEMPO_MARCHA_VENT_SEG],			tiempo_marcha_vent_seg);
	EEPROM.commit(); // efectivamente escribir
	EEPROM.end();
}

//==================================================================================================================//

template <typename T>
void LocalEEPROM::escribir(int Adireccion, T Adato)
{
	EEPROM.begin(this->espacios);
	EEPROM.put(Adireccion, Adato);
	EEPROM.commit(); // efectivamente escribir
	EEPROM.end();
}

//==================================================================================================================//

void LocalEEPROM::imprimirValsDirsReads()
{
	Serial.println();
	Serial.println("Valores recuperados de la EEPROM:");
	Serial.println(eeprom_programada);
	Serial.println(alarma_activada);
	Serial.println(temp_maxima_alarma);
	Serial.println(temp_minima_alarma);
	Serial.println(temp_maxima_ventilacion);
	Serial.println(humedad_suelo_minima);
	Serial.println(lapso_alarma_minutos);
	Serial.println(tiempo_bombeo_segundos);
	Serial.println(tiempo_espera_minutos);

	Serial.println(estados_salidas);
	Serial.println(lapso_ventilaciones_min);
	Serial.println(tiempo_apertura_vent_min);
	Serial.println(lapso_riegos_min);
	Serial.println(temp_minima_calefa);
	Serial.println(lapso_calefas_min);
	Serial.println(tiempo_encendido_calefa_min);
	Serial.println(tiempo_marcha_vent_seg);
	Serial.println();

	Serial.println("Valores de las direcciones:");
	for (int i = 0; i < this->CANT_VARIABLES; ++i)
	{
		Serial.print("Dir ");
		Serial.print(i);
		Serial.print(": ");
		Serial.println(this->direccion[i]);
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

#define CANT_VARIABLES 10
int direccion[CANT_VARIABLES];
int espacios;

void setDireccionesEEPROM()
{
	// hay que hacer .put a las direcciones indicadas, según la longitud de los datos escritos anteriormente
	this->direccion[0] = 0;
	this->direccion[1] = this->direccion[0] + 1;//bool eeprom_programada
	this->direccion[2] = this->direccion[1] + 1;//bool	alarma_activada
	this->direccion[3] = this->direccion[2] + 4;//float	temp_maxima_alarma
	this->direccion[4] = this->direccion[3] + 4;//float	temp_minima_alarma
	this->direccion[5] = this->direccion[4] + 4;//float	temp_maxima_ventilacion
	this->direccion[6] = this->direccion[5] + 1;//int	humedad_suelo_minima
	this->direccion[7] = this->direccion[6] + 2;//int	lapso_alarma_minutos
	this->direccion[8] = this->direccion[7] + 2;//int	tiempo_bombeo_segundos
	this->direccion[9] = this->direccion[8] + 2;//int	tiempo_espera_minutos
 this->espacios = this->direccion[9] + x;//sizeof(x)	nombrede_x
}
*/
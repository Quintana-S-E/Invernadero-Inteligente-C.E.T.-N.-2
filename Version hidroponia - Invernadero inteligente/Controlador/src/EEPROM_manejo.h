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
	this->setDirecciones();

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
		if (LCWF.cant_redes > 0  &&  LCFB.tiene_firebase)
			leerCompleta();
		else
			LCSD.leerConfigParametros();
	}

#ifdef DEBUG_SERIAL
	this->imprimirValsDirsReads();
#endif
}

//==================================================================================================================//

void LocalEEPROM::leerCompleta()
{
	EEPROM.begin(this->espacios);
	EEPROM.get(this->direccion[PROGRAMADA],						this->eeprom_programada); //	*1
	EEPROM.get(this->direccion[ALARMA_ACTIVADA],				this->alarma_activada);
	EEPROM.get(this->direccion[LAPSO_ALARMA_MIN],				this->lapso_alarma_min);
	EEPROM.get(this->direccion[TEMP_MAXIMA_ALARMA],				this->temp_maxima_alarma);
	EEPROM.get(this->direccion[TEMP_MINIMA_ALARMA],				this->temp_minima_alarma);
	EEPROM.get(this->direccion[MODO_BOMBA1],					this->modo_bomba1);
	EEPROM.get(this->direccion[LAPSO_BOMBEO1_MIN],				this->lapso_bombeo1_min);
	EEPROM.get(this->direccion[TIEMPO_ENCENDIDO_BOMBA1_MIN],	this->tiempo_encendido_bomba1_min);
	EEPROM.get(this->direccion[MODO_BOMBA2],					this->modo_bomba2);
	EEPROM.get(this->direccion[LAPSO_BOMBEO2_MIN],				this->lapso_bombeo2_min);
	EEPROM.get(this->direccion[TIEMPO_ENCENDIDO_BOMBA2_MIN],	this->tiempo_encendido_bomba2_min);
	EEPROM.get(this->direccion[MODO_BOMBA3],					this->modo_bomba3);
	EEPROM.get(this->direccion[LAPSO_BOMBEO3_MIN],				this->lapso_bombeo3_min);
	EEPROM.get(this->direccion[TIEMPO_ENCENDIDO_BOMBA3_MIN],	this->tiempo_encendido_bomba3_min);
	EEPROM.get(this->direccion[MODO_VENT],						this->modo_vent);
	EEPROM.get(this->direccion[TEMP_MAXIMA_VENTILACION],		this->temp_maxima_ventilacion);
	EEPROM.get(this->direccion[LAPSO_VENTILACIONES_MIN],		this->lapso_ventilaciones_min);
	EEPROM.get(this->direccion[TIEMPO_APERTURA_VENT_MIN],		this->tiempo_apertura_vent_min);
	EEPROM.get(this->direccion[TIEMPO_MARCHA_VENT_SEG],			this->tiempo_marcha_vent_seg);
	EEPROM.end();
}
//	*1 no es necesario, ya que en inicializar se le asignó true para poder entrar a esta función, y
//	ya que está programada va a volver a dar 1. Sólo se pone para una mejor legibilidad del código
//==================================================================================================================//

void LocalEEPROM::cargarValoresPorDefecto()
{
	EEPROM.begin(this->espacios);
	EEPROM.put(this->direccion[PROGRAMADA],						true); // ahora va a estar programada
	EEPROM.put(this->direccion[ALARMA_ACTIVADA],				this->ALARMA_ACTIVADA_DEFECTO);
	EEPROM.put(this->direccion[LAPSO_ALARMA_MIN],				this->LAPSO_ALARMA_MIN_DEFECTO);
	EEPROM.put(this->direccion[TEMP_MAXIMA_ALARMA],				this->TEMP_MAXIMA_ALARMA_DEFECTO);
	EEPROM.put(this->direccion[TEMP_MINIMA_ALARMA],				this->TEMP_MINIMA_ALARMA_DEFECTO);
	EEPROM.put(this->direccion[MODO_BOMBA1],					this->MODO_BOMBA1_DEFECTO);
	EEPROM.put(this->direccion[LAPSO_BOMBEO1_MIN],				this->LAPSO_BOMBEO1_MIN_DEFECTO);
	EEPROM.put(this->direccion[TIEMPO_ENCENDIDO_BOMBA1_MIN],	this->TIEMPO_ENCENDIDO_BOMBA1_MIN_DEFECTO);
	EEPROM.put(this->direccion[MODO_BOMBA2],					this->MODO_BOMBA2_DEFECTO);
	EEPROM.put(this->direccion[LAPSO_BOMBEO2_MIN],				this->LAPSO_BOMBEO2_MIN_DEFECTO);
	EEPROM.put(this->direccion[TIEMPO_ENCENDIDO_BOMBA2_MIN],	this->TIEMPO_ENCENDIDO_BOMBA2_MIN_DEFECTO);
	EEPROM.put(this->direccion[MODO_BOMBA3],					this->MODO_BOMBA3_DEFECTO);
	EEPROM.put(this->direccion[LAPSO_BOMBEO3_MIN],				this->LAPSO_BOMBEO3_MIN_DEFECTO);
	EEPROM.put(this->direccion[TIEMPO_ENCENDIDO_BOMBA3_MIN],	this->TIEMPO_ENCENDIDO_BOMBA3_MIN_DEFECTO);
	EEPROM.put(this->direccion[MODO_VENT],						this->MODO_VENT_DEFECTO);
	EEPROM.put(this->direccion[TEMP_MAXIMA_VENTILACION],		this->TEMP_MAXIMA_VENTILACION_DEFECTO);
	EEPROM.put(this->direccion[LAPSO_VENTILACIONES_MIN],		this->LAPSO_VENTILACIONES_MIN_DEFECTO);
	EEPROM.put(this->direccion[TIEMPO_APERTURA_VENT_MIN],		this->TIEMPO_APERTURA_VENT_MIN_DEFECTO);
	EEPROM.put(this->direccion[TIEMPO_MARCHA_VENT_SEG],			this->TIEMPO_MARCHA_VENT_SEG_DEFECTO);
	EEPROM.commit(); // efectivamente escribir
	EEPROM.end();

	this->leerCompleta(); // asignar valores a las globales
}

//==================================================================================================================//

template <typename T>
void LocalEEPROM::escribir(int direccion, T dato)
{
	EEPROM.begin(this->espacios);
	EEPROM.put(direccion, dato);
	EEPROM.commit(); // efectivamente escribir
	EEPROM.end();
}

template <typename T>
T LocalEEPROM::leer(int direccion)
{
	T valor;
	EEPROM.begin(this->espacios);
	EEPROM.get(direccion, valor);
	EEPROM.end();
	return valor;
}

//==================================================================================================================//

void LocalEEPROM::imprimirValsDirsReads()
{
	Serial.println();
	Serial.println("Valores recuperados de la EEPROM:");
	Serial.println(this->eeprom_programada);
	Serial.println(this->alarma_activada);
	Serial.println(this->lapso_alarma_min);
	Serial.println(this->temp_maxima_alarma);
	Serial.println(this->temp_minima_alarma);
	Serial.println(this->modo_bomba1);
	Serial.println(this->lapso_bombeo1_min);
	Serial.println(this->tiempo_encendido_bomba1_min);
	Serial.println(this->modo_bomba2);
	Serial.println(this->lapso_bombeo2_min);
	Serial.println(this->tiempo_encendido_bomba2_min);
	Serial.println(this->modo_bomba3);
	Serial.println(this->lapso_bombeo3_min);
	Serial.println(this->tiempo_encendido_bomba3_min);
	Serial.println(this->modo_vent);
	Serial.println(this->temp_maxima_ventilacion);
	Serial.println(this->lapso_ventilaciones_min);
	Serial.println(this->tiempo_apertura_vent_min);
	Serial.println(this->tiempo_marcha_vent_seg);
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

	EEPROM.begin(100);
	for (int j = 0; j < 37; ++j)
	{
		Serial.print("Dirección ");
		Serial.print(j);
		Serial.print(": ");
		Serial.println(EEPROM.read(j));
	}
	EEPROM.end();
}
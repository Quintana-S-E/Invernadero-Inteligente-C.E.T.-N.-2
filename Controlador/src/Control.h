#pragma once

#include "Declaraciones.h"
#include "EEPROM_manejo.h"

// Abrir la ventilación y activar ventiladores
void chequearVentilacion() // en "loop()"
{
	if (ventilacion_forzada)
		return; // si la ventilación es forzada por telegram, ignorar lo automático

	if (temp_exterior >= temp_maxima_ventilacion && !ventilando)
	{
		ventilando = true;
		activarVentilacion();
	}
	else if (temp_exterior < temp_maxima_ventilacion && ventilando)
	{
		ventilando = false;
		desactivarVentilacion();
	}
}

//==================================================================================================================//

// Regar y esperar el tiempo necesario para la filtración del agua antes de medir de nuevo
void chequearRiego() // en "loop()"
{
	// apagar la bomba después del tiempo definido
	if (millis() - ultima_vez_bomba_encendio >= (tiempo_bombeo_segundos * 1000UL))
		digitalWrite(PIN_BOMBA, HIGH);

	// si se está esperando, comprobar si pasó el tiempo desde ultima_vez_bomba_encendio. De ser así, dejar de esperar
	if (esperando_riego)
	{
		if (millis() - ultima_vez_bomba_encendio >= (tiempo_espera_minutos * 60000UL))
		{
			esperando_riego = false;
			imprimirln("La espera desde el riego finalizó");
		}
	}

	// chequear la humedad y regar (si no se está esperando la filtración del agua)
	if (humedad_suelo_exterior <= humedad_suelo_minima && !esperando_riego)
	{
		ultima_vez_bomba_encendio = millis();
		digitalWrite(PIN_BOMBA, LOW);
		esperando_riego = true; // hay que esperar desde el tiempo 0 (ultima_vez_bomba_encendio)
	}
}

//==================================================================================================================//

void activarVentilacion()
{
	Ventana.write(ANGULO_APERTURA); // sacar cuando separemos ventiladores de ventana
	digitalWrite(PIN_VENTILADOR, LOW);
}

//==================================================================================================================//

void desactivarVentilacion()
{
	Ventana.write(ANGULO_CERRADO); // sacar cuando separemos ventiladores de ventana
	digitalWrite(PIN_VENTILADOR, HIGH);
}

//================================================FUTURAS VERSIONES=================================================//
// Identifica la necesidad de iluminar, basándose en la lectura de un sensor LDR
void chequear_iluminacion()
{
	// ...
}
//==========================HABILITAR PARA CUANDO SEPAREMOS LOS VENTILADORES DE LA VENTANA==========================//
/*
void abrirVentana()
{
	Ventana.write(ANGULO_APERTURA);
}

//==========================HABILITAR PARA CUANDO SEPAREMOS LOS VENTILADORES DE LA VENTANA==========================//

void cerrarVentana()
{
	Ventana.write(ANGULO_CERRADO);
}
*/
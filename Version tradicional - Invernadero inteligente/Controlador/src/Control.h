#pragma once

#include "Declaraciones.h"

/*
Para determinar la temperatura de activación/desactivación de ventilación utilizamos la lógica de un Schmitt trigger: Activamos a
T(elegida) + ΔT;  y luego desactivamos en T(elegida) - ΔT. La temp. haría un ripple centrado en T(elegida), con una variación
de temperatura de 2ΔT (por ejemplo, T(elegida) = 25 °C  y ΔT = 0,5 °C. La temp. variaría entre 25,5 y 24,5 °C o un poquito
más, debido a un overshoot desde que se activa hasta que empieza a bajar; o desactiva y empieza a subir [inercia térmica]).
	https://forum.allaboutcircuits.com/threads/schmitt-trigger-vs-rc-low-pass-filter-for-signal-conditioning.187898/
*/

// Regar y esperar el tiempo necesario para la filtración del agua antes de medir de nuevo
void LocalControl::controlarRiego() // en "loop()"
{
	switch (Riego.modo)
	{
	case SalidaModos::Deshabilitada:
		Riego.apagar();
	case SalidaModos::Forzada:
		return;
	case SalidaModos::Temporizada:
		riegoTemporizado();
	case SalidaModos::Automatica:
		riegoAutomatico();
	}
}

void LocalControl::riegoTemporizado()
{
	unsigned long millis_actual = millis();
	// si pasó "lapso_riegos_min" desde "Riego.ultima_vez_encendida"
	if (millis_actual - Riego.ultima_vez_encendida >= (unsigned long) lapso_riegos_min * 60000UL)
		Riego.encender(millis_actual);
	monitorearBombeoRiego(millis_actual);
}

void LocalControl::riegoAutomatico()
{
	unsigned long millis_actual = millis();
	monitorearBombeoRiego(millis_actual);

	// si se está esperando, comprobar si pasó el tiempo desde ultima_vez_encendida. De ser así, dejar de esperar
	if (esperando_riego)
	{
		if (millis_actual - Riego.ultima_vez_encendida >= (unsigned long) tiempo_espera_min * 60000UL)
		{
			esperando_riego = false;
			imprimirln("La espera desde el riego finalizó");
		}
		else return;
	}

	// controlar la humedad y regar (si no se está esperando la filtración del agua)
	if (humedad_suelo1 <= humedad_suelo_minima	||	humedad_suelo2 <= humedad_suelo_minima)
	{
		Riego.encender(millis_actual);
		esperando_riego = true; // hay que esperar desde el tiempo 0 (ultima_vez_encendida)
	}
}

void LocalControl::monitorearBombeoRiego(unsigned long millis_actual)
{
	if (millis_actual - Riego.ultima_vez_encendida >= (unsigned long) tiempo_bombeo_seg * 1000UL)
		Riego.apagar();
}

//===============================================================================================================================//

void LocalControl::controlarCalefa() // en "loop()"
{
	switch (Calefa.modo)
	{
	case SalidaModos::Deshabilitada:
		Calefa.apagar();
	case SalidaModos::Forzada:
		return;
	case SalidaModos::Temporizada:
		calefaTemporizada();
	case SalidaModos::Automatica:
		calefaAutomatica();
	}
}

void LocalControl::calefaTemporizada()
{
	unsigned long millis_actual = millis();
	// si pasó "lapso_calefas_min" desde "Calefa.ultima_vez_abierta"
	if (millis_actual - Calefa.ultima_vez_encendida >= (unsigned long) lapso_calefas_min * 60000UL)
		Calefa.encender(millis_actual);
	if (millis_actual - Calefa.ultima_vez_encendida >= (unsigned long) tiempo_encendido_calefa_min * 60000UL)
		Calefa.apagar();
}

void LocalControl::calefaAutomatica()
{
	if (AhtInteriorMid.temperatura <= temp_minima_calefa - DELTA_T_CALEFA)
		Calefa.encender(millis());
	else if (AhtInteriorMid.temperatura >= temp_minima_calefa + DELTA_T_CALEFA)
		Calefa.apagar();
}

//===============================================================================================================================//

void LocalControl::controlarVentilacion() // en "loop()"
{
	switch (Ventilacion.modo)
	{
	case SalidaModos::Deshabilitada:
		Ventilacion.cerrar();
	case SalidaModos::Forzada:
		return;
	case SalidaModos::Temporizada:
		ventilacionTemporizada();
	case SalidaModos::Automatica:
		ventilacionAutomatica();
	}
}

void LocalControl::ventilacionTemporizada()
{
	unsigned long millis_actual = millis();
	// si pasó "lapso_ventilaciones_min" desde "Ventilacion.ultima_vez_abierta"
	if (millis_actual - Ventilacion.ultima_vez_abierta >= (unsigned long) lapso_ventilaciones_min * 60000UL)
		Ventilacion.abrir(millis_actual);
	if (millis_actual - Ventilacion.ultima_vez_abierta >= (unsigned long) tiempo_apertura_vent_min * 60000UL)
		Ventilacion.cerrar();
}

void LocalControl::ventilacionAutomatica()
{
	if (AhtInteriorHigh.temperatura >= temp_maxima_ventilacion + DELTA_T_VENTILACION)
		Ventilacion.abrir(millis());
	else if (AhtInteriorHigh.temperatura < temp_maxima_ventilacion - DELTA_T_VENTILACION)
		Ventilacion.cerrar();
}

//===============================================================================================================================//

SalidaOnOff::SalidaOnOff(PinsOut pin_mosfet)	{	this->pin_mosfet = static_cast<int>(pin_mosfet);	}

void SalidaOnOff::encender(unsigned long millis_actual)
{
	digitalWrite(this->pin_mosfet, HIGH);
	this->ultima_vez_encendida = millis_actual;
	this->encendida = true;
}

void SalidaOnOff::apagar()
{
	digitalWrite(this->pin_mosfet, LOW);
	this->encendida = false;
}

//===============================================================================================================================//

SalidaVentilacion::SalidaVentilacion(PinsOut pin_marcha, PinsOut pin_contramarcha)
{
	this->pin_marcha = static_cast<int>(pin_marcha);
	this->pin_contramarcha = static_cast<int>(pin_contramarcha);
}

void SalidaVentilacion::abrir(unsigned long millis_actual)
{
	if (this->abierta)
		return;
	// si llegamos acá ambos están LOW
	digitalWrite(this->pin_marcha, HIGH);

	delay(tiempo_marcha_vent_seg * 1000UL);	// ATENCIÓN: ZONA CRÍTICA, NO DEBE APAGARSE. EN MOVIMIENTO

	digitalWrite(this->pin_contramarcha, HIGH); // ambos relés activados para encender lámpara
	this->ultima_vez_abierta = millis_actual;
	// terminan ambos HIGH
	this->abierta = true;
}

void SalidaVentilacion::cerrar()
{
	if (!this->abierta)
		return;
	// si llegamos acá ambos están HIGH
	digitalWrite(this->pin_marcha, LOW);

	delay(tiempo_marcha_vent_seg * 1000UL);	// ATENCIÓN: ZONA CRÍTICA, NO DEBE APAGARSE. EN MOVIMIENTO

	digitalWrite(this->pin_contramarcha, LOW); // ambos relés desactivados para apagar lámpara
	// terminan ambos LOW
	this->abierta = false;
}

//===============================================================================================================================//

void LocalControl::configurarModosSalidas()
{
	//Riego1.modo =		static_cast<SalidaModos>((modos_salidas >> 6) & 0b00000011); primeros dos bits son para salida no usada
	Riego.modo =		static_cast<SalidaModos>((modos_salidas >> 4) & 0b00000011);
	Calefa.modo =		static_cast<SalidaModos>((modos_salidas >> 2) & 0b00000011);
	Ventilacion.modo =	static_cast<SalidaModos>(modos_salidas & 0b00000011);
}

//===============================================================================================================================//

/*// Devuelve clickeado, mantenido, o suelto si pasó el timeout
EstadoBoton leerBoton(unsigned long timeout_lectura)
{
	bool btn_presionado_anterior = false;
	unsigned long t_0_btn_mantenido = 0;

	unsigned long millis_actual = millis();
	unsigned long t_0_espera = millis_actual;

	while (millis_actual - t_0_espera < timeout_lectura)
	{
		millis_actual = millis();
		bool btn_presionado = !digitalRead(PIN_BTN); // input_pullup
		// Si se presionó (falling edge)
		if (btn_presionado  &&  !btn_presionado_anterior)
		{
			t_0_btn_mantenido = millis_actual;
			btn_presionado_anterior = true;
			delay(50); // por el bouncing, no leer por accidente que lo soltamos
		}
		else if (!btn_presionado  &&  btn_presionado_anterior) // Se soltó
			return EstadoBoton::Clickeado;

		if (btn_presionado)
		{
			if (millis_actual - t_0_btn_mantenido >= TIEMPO_MIN_BTN_MANTENIDO)
				return EstadoBoton::Mantenido;
		}
	}

	return EstadoBoton::Suelto;
}*/
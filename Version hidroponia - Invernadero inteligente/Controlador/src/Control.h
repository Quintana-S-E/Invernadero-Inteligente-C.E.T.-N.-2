#pragma once

#include "Declaraciones.h"

/*
Para determinar la temperatura de activación/desactivación de ventilación utilizamos la lógica de un Schmitt trigger: Activamos a
T(elegida) + ΔT;  y luego desactivamos en T(elegida) - ΔT. La temp. haría un ripple centrado en T(elegida), con una variación
de temperatura de 2ΔT (por ejemplo, T(elegida) = 25 °C  y ΔT = 0,5 °C. La temp. variaría entre 25,5 y 24,5 °C o un poquito
más, debido a un overshoot desde que se activa hasta que empieza a bajar; o desactiva y empieza a subir [inercia térmica]).
	https://forum.allaboutcircuits.com/threads/schmitt-trigger-vs-rc-low-pass-filter-for-signal-conditioning.187898/
*/

void LocalControl::controlarBomba1()
{
	switch (Bomba1.modo)
	{
	case SalidaModos::Deshabilitada:
		Bomba1.apagar();
	case SalidaModos::Forzada:
		return;
	case SalidaModos::Temporizada:
		this->bomba1Temporizada();
		break;
	}
}

void LocalControl::bomba1Temporizada()
{
	unsigned long millis_actual = millis();
	// si pasó "lapso_bombeo1_min" desde "Bomba1.ultima_vez_encendida"
	if (millis_actual - Bomba1.ultima_vez_encendida >= (unsigned long) LCEE.lapso_bombeo1_min * 60000UL)
		Bomba1.encender(millis_actual);
	if (millis_actual - Bomba1.ultima_vez_encendida >= (unsigned long) LCEE.tiempo_encendido_bomba1_min * 60000UL)
		Bomba1.apagar();
}

//===============================================================================================================================//

void LocalControl::controlarBomba2()
{
	switch (Bomba2.modo)
	{
	case SalidaModos::Deshabilitada:
		Bomba2.apagar();
	case SalidaModos::Forzada:
		return;
	case SalidaModos::Temporizada:
		this->bomba2Temporizada();
		break;
	}
}

void LocalControl::bomba2Temporizada()
{
	unsigned long millis_actual = millis();
	// si pasó "lapso_bombeo2_min" desde "Bomba2.ultima_vez_encendida"
	if (millis_actual - Bomba2.ultima_vez_encendida >= (unsigned long) LCEE.lapso_bombeo2_min * 60000UL)
		Bomba2.encender(millis_actual);
	if (millis_actual - Bomba2.ultima_vez_encendida >= (unsigned long) LCEE.tiempo_encendido_bomba2_min * 60000UL)
		Bomba2.apagar();
}

//===============================================================================================================================//

void LocalControl::controlarBomba3()
{
	switch (Bomba3.modo)
	{
	case SalidaModos::Deshabilitada:
		Bomba3.apagar();
	case SalidaModos::Forzada:
		return;
	case SalidaModos::Temporizada:
		this->bomba3Temporizada();
		break;
	}
}

void LocalControl::bomba3Temporizada()
{
	unsigned long millis_actual = millis();
	// si pasó "lapso_bombeo3_min" desde "Bomba3.ultima_vez_encendida"
	if (millis_actual - Bomba3.ultima_vez_encendida >= (unsigned long) LCEE.lapso_bombeo3_min * 60000UL)
		Bomba3.encender(millis_actual);
	if (millis_actual - Bomba3.ultima_vez_encendida >= (unsigned long) LCEE.tiempo_encendido_bomba3_min * 60000UL)
		Bomba3.apagar();
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
		this->ventilacionTemporizada();
		break;
	case SalidaModos::Automatica:
		this->ventilacionAutomatica();
		break;
	}
}

void LocalControl::ventilacionTemporizada()
{
	unsigned long millis_actual = millis();
	// si pasó "lapso_ventilaciones_min" desde "Ventilacion.ultima_vez_abierta"
	if (millis_actual - Ventilacion.ultima_vez_abierta >= (unsigned long) LCEE.lapso_ventilaciones_min * 60000UL)
		Ventilacion.abrir(millis_actual);
	if (millis_actual - Ventilacion.ultima_vez_abierta >= (unsigned long) LCEE.tiempo_apertura_vent_min * 60000UL)
		Ventilacion.cerrar();
}

void LocalControl::ventilacionAutomatica()
{
	imprimirln("vent auto...");
	if (AhtInteriorHigh.temperatura >= LCEE.temp_maxima_ventilacion + DELTA_T_VENTILACION)
		Ventilacion.abrir(millis());
	else if (AhtInteriorHigh.temperatura < LCEE.temp_maxima_ventilacion - DELTA_T_VENTILACION)
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
	LCDP.displayVentana(true);

	while (millis() - millis_actual < (LCEE.tiempo_marcha_vent_seg * 1000UL))
		; // ATENCIÓN: ZONA CRÍTICA, NO DEBE APAGARSE. VENTANA EN MOVIMIENTO

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

	LCDP.displayVentana(false);	// TODO IMPORTANTE: HACER UN WHILE LOOP EN VEZ DE DELAY
	delay(LCEE.tiempo_marcha_vent_seg * 1000UL);	// ATENCIÓN: ZONA CRÍTICA, NO DEBE APAGARSE EN MOVIMIENTO

	digitalWrite(this->pin_contramarcha, LOW); // ambos relés desactivados para apagar lámpara
	// terminan ambos LOW
	this->abierta = false;
}

//===============================================================================================================================//

void LocalControl::configurarModosSalidas()
{
	Bomba1.modo =		static_cast<SalidaModos>(LCEE.modo_bomba1);
	Bomba2.modo =		static_cast<SalidaModos>(LCEE.modo_bomba2);
	Bomba3.modo =		static_cast<SalidaModos>(LCEE.modo_bomba3);
	Ventilacion.modo =	static_cast<SalidaModos>(LCEE.modo_vent);
}

//===============================================================================================================================//

void LocalControl::controlarAlarma()
{
	if (!LCFB.tiene_firebase  ||  !LCWF.hay_conexion  ||  !LCEE.alarma_activada)
		return;
	if (millis() - this->ultima_actualizacion_alarma < (unsigned long) LCEE.lapso_alarma_min * 60000UL)
		return;
	this->ultima_actualizacion_alarma = millis();

	// evaluamos la temperatura
	if (AhtInteriorMid.temperatura >= LCEE.temp_maxima_alarma)
		LCFB.enviarAlarmaCaliente();
	else if (AhtInteriorMid.temperatura <= LCEE.temp_minima_alarma)
		LCFB.enviarAlarmaFrio();
}


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
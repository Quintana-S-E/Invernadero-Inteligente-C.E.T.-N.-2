#pragma once

#include "Declaraciones.h"
#include "EEPROM_manejo.h"

/*
TODO: Para determinar la temperatura de activación/desactivación de ventilación utilizar la lógica de un Schmitt trigger: Activar a
T(elegida) + ΔT;  y luego desactivar en T(elegida) - ΔT. La temp. haría un ripple centrado en T(elegida), con una variación
de temperatura de 2ΔT (por ejemplo, T(elegida) = 25 °C  y ΔT = 0,5 °C. La temp. variaría entre 25,5 y 24,5 °C o un poquito
más, debido a un overshoot desde que se activa hasta que empieza a bajar; o desactiva y empieza a subir [inercia térmica]).
	https://forum.allaboutcircuits.com/threads/schmitt-trigger-vs-rc-low-pass-filter-for-signal-conditioning.187898/
Es muy fácil de implementar, solo una suma y resta.
*/
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


	/*if (riego_forzado) // idea centralita
		return;*/


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

//==================================================================================================================//

// Devuelve clickeado, mantenido, o suelto si pasó el timeout
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
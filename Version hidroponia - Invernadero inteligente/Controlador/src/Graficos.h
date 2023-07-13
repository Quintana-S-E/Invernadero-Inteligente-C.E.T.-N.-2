#pragma once

#include "Declaraciones.h"
#include "Claves.h"

inline bool inicializarThingSpeak() { return ThingSpeak.begin(Cliente); }

void actualizarGraficos()
{
	// hay que actualizar cada 15 segundos máx, porque si no el sitio no responde
	if (millis() - ultima_vez_thingspeak_actualizo >= 20000UL)
	{
		ultima_vez_thingspeak_actualizo = millis();

		// no escribimos los -50 °C de la primera lectura después del reinicio, vamos a los otros datos
		if (temp_interior_promedio == -50  ||  temp_exterior == -50)
			goto SinTemperaturas;
		// escribimos todos los valores
		ThingSpeak.setField(1, temp_interior_promedio);
		ThingSpeak.setField(2, temp_exterior);
SinTemperaturas:
		ThingSpeak.setField(3, humedad_aire_interior_promedio);
		ThingSpeak.setField(4, humedad_aire_exterior);
		ThingSpeak.setField(5, humedad_suelo_interior);
		ThingSpeak.setField(6, humedad_suelo_exterior);

		int x = ThingSpeak.writeFields(1, CLAVES_THINGSPEAK_API_KEY);
		if (x != 200)
		{
			imprimir("Fallo en ThingSpeak: ");
			imprimir("error ");
			imprimirln(x);
		}
	}
}
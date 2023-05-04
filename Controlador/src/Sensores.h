#pragma once

#include "Declaraciones.h"

void leerSensores() // en "loop()"
{
	// setea   humedad_aire_interior_promedio  y  temp_interior_promedio
	leerDHT22Interiores();
	// setea   humedad_suelo_interior
	leerSoilInteriores();

	// setea   humedad_aire_exterior  y  temp_exterior
	leerDHT22Exteriores();
	// setea   humedad_suelo_exterior
	leerSoilExteriores();
}

//==================================================================================================================//

void leerDHT22Interiores() // en leerSensores()
{
	// cada lectura toma alrededor de 250 ms, y puede tener hasta 2 segundos de antigüedad
	float humedad_aire_interior[2];
	float temp_interior[2];

	// leer a DhtInteriorLow
	humedad_aire_interior[0] = DhtInteriorLow.readHumidity(); // %
	temp_interior[0] = DhtInteriorLow.readTemperature();	  // Celsius

	/* leer a DhtInteriorMid
	humedad_aire_interior[0] = DhtInteriorMid.readHumidity();
	temp_interior[0] = DhtInteriorMid.readTemperature();
	*/

	// leer a DhtInteriorHigh
	humedad_aire_interior[1] = DhtInteriorHigh.readHumidity();
	temp_interior[1] = DhtInteriorHigh.readTemperature();

	// mirar si alguna lectura falló y finalizar antes (para empezar de nuevo).
	/*
	if (isnan(humedad_aire_interior[0]) || isnan(temp_interior[0]))
		imprimirln("Fallo al leer el DHT22 interior bajo");
	if (isnan(humedad_aire_interior[1]) || isnan(temp_interior[1]))
		imprimirln("Fallo al leer el DHT22 interior alto");
	*/

	humedad_aire_interior_promedio = (humedad_aire_interior[0] + humedad_aire_interior[1]) / 2;
	temp_interior_promedio = (temp_interior[0] + temp_interior[1]) / 2;
}

//==================================================================================================================//

void leerSoilInteriores() // en leerSensores()
{
	int humedad_suelo_interior_raw[MUESTRAS_HUMEDAD_SUELO];
	unsigned int humedad_suelo_interior_suma = 0;
	for (int i = 0; i < MUESTRAS_HUMEDAD_SUELO; i++)
	{
		humedad_suelo_interior_raw[i] = analogRead(SOIL_INT_PIN);
		humedad_suelo_interior_suma += humedad_suelo_interior_raw[i];
	}
	humedad_suelo_interior = (humedad_suelo_interior_suma / MUESTRAS_HUMEDAD_SUELO);
	humedad_suelo_interior = map(humedad_suelo_interior, 0, 4095, 100, 0);
	// TODO: el 70 % del agua pura debería ser 100 %, y el 29 % del aire debería ser 0 % (ver en tierra verdadera)
}

//==================================================================================================================//

void leerDHT22Exteriores() // en leerSensores()
{
	// leer a DhtExterior
	humedad_aire_exterior = DhtExterior.readHumidity(); // %
	temp_exterior = DhtExterior.readTemperature();		// Celsius

	// mirar si alguna lectura falló y finalizar antes (para empezar de nuevo).
	/*
	if (isnan(humedad_aire_exterior) || isnan(temp_exterior))
		imprimirln("Fallo al leer el DHT22 exterior");
	*/

	// con más sensores exteriores aquí haríamos el promedio
}

//==================================================================================================================//

void leerSoilExteriores() // en leerSensores()
{
	int humedad_suelo_exterior_raw[MUESTRAS_HUMEDAD_SUELO];
	unsigned int humedad_suelo_exterior_suma = 0;
	for (int i = 0; i < MUESTRAS_HUMEDAD_SUELO; i++)
	{
		humedad_suelo_exterior_raw[i] = analogRead(SOIL_EXT_PIN);
		humedad_suelo_exterior_suma += humedad_suelo_exterior_raw[i];
	}
	humedad_suelo_exterior = (humedad_suelo_exterior_suma / MUESTRAS_HUMEDAD_SUELO);
	humedad_suelo_exterior = map(humedad_suelo_exterior, 0, 4095, 100, 0);
	// TODO: el 70 % del agua pura debería ser 100 %, y el 29 % del aire debería ser 0 % (ver en tierra verdadera)
}
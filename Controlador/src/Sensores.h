#pragma once

#include "Declaraciones.h"

AHT10Mux::AHT10Mux(uint8_t Asalida_del_mux) {	salida_del_mux = Asalida_del_mux;	}

bool AHT10Mux::begin()
{
	establecerSalidaMUX(salida_del_mux);
	return AhtSeleccionado.begin();
}

float AHT10Mux::readTemperature(bool readI2C = AHT10_FORCE_READ_DATA)
{
	establecerSalidaMUX(salida_del_mux);
	return AhtSeleccionado.readTemperature(readI2C);
}

float AHT10Mux::readHumidity(bool readI2C = AHT10_FORCE_READ_DATA)
{
	establecerSalidaMUX(salida_del_mux);
	return AhtSeleccionado.readHumidity(readI2C);
}

//==================================================================================================================//

void establecerSalidaMUX(uint8_t salida)
{
	bool bits[3]; // últimos 3 bits del número
	for (int i = 0;  i < 3;  ++i)
		bits[i] =  (salida >> i) & 1;

	digitalWrite(MUX_A, bits[0]);
	digitalWrite(MUX_B, bits[1]);
	digitalWrite(MUX_C, bits[2]);
}

//==================================================================================================================//

void inicializarSensores()
{
	const uint8_t CANTIDAD_SENSORES_AHT = 5;
	bool sensor[CANTIDAD_SENSORES_AHT];
	sensor[0] = AhtInteriorHigh.begin();
	sensor[1] = AhtInteriorMid.begin();
	sensor[2] = AhtInteriorLow.begin();
	sensor[3] = AhtExterior.begin();
	sensor[4] = AhtExteriorGeotermico.begin();
	if (!sensor[0] || !sensor[1] || !sensor[2] || !sensor[3] || !sensor[4])
	{
		imprimir("Error inicializando AHT10: ");
		for (uint8_t i = 0; i < CANTIDAD_SENSORES_AHT; ++i)
			imprimir(sensor[i]);
		imprimirln();
	}
}

//==================================================================================================================//

void leerSensores() // en "loop()"
{
	// setea   humedad_aire_interior_promedio  y  temp_interior_promedio
	leerAHT10Interiores();
	// setea   humedad_suelo_interior
	leerSoilInteriores();

	// setea   humedad_aire_exterior  y  temp_exterior
	leerAHT10Exteriores();
	// setea   humedad_suelo_exterior
	leerSoilExteriores();
}

//==================================================================================================================//

void leerAHT10Interiores() // en leerSensores()
{
	float humedad_aire_interior[3];
	float temp_interior[3];

	// leer a AhtInteriorHigh
	humedad_aire_interior[0] = AhtInteriorHigh.readHumidity(); // %
	temp_interior[0] = AhtInteriorHigh.readTemperature();	  // Celsius
	// leer a AhtInteriorMid
	humedad_aire_interior[1] = AhtInteriorMid.readHumidity();
	temp_interior[1] = AhtInteriorMid.readTemperature();
	// leer a AhtInteriorLow
	humedad_aire_interior[2] = AhtInteriorLow.readHumidity();
	temp_interior[2] = AhtInteriorLow.readTemperature();

	humedad_aire_interior_promedio = (humedad_aire_interior[0] + humedad_aire_interior[1] + humedad_aire_interior[2]) / 3;
	temp_interior_promedio = (temp_interior[0] + temp_interior[1] + temp_interior[2]) / 3;
}

//==================================================================================================================//

void leerSoilInteriores() // en leerSensores()
{
	int humedad_suelo_interior_raw[MUESTRAS_HUMEDAD_SUELO];
	unsigned int humedad_suelo_interior_suma = 0;
	for (int i = 0; i < MUESTRAS_HUMEDAD_SUELO; ++i)
	{
		humedad_suelo_interior_raw[i] = analogRead(SOIL_INT_PIN);
		humedad_suelo_interior_suma += humedad_suelo_interior_raw[i];
	}
	humedad_suelo_interior = (humedad_suelo_interior_suma / MUESTRAS_HUMEDAD_SUELO);
	humedad_suelo_interior = map(humedad_suelo_interior, 0, 4095, 100, 0);
	// TODO: el 70 % del agua pura debería ser 100 %, y el 29 % del aire debería ser 0 % (ver en tierra verdadera)
}

//==================================================================================================================//

void leerAHT10Exteriores() // en leerSensores()
{
	// leer a AhtExterior
	humedad_aire_exterior = AhtExterior.readHumidity(); // %
	temp_exterior = AhtExterior.readTemperature();		// Celsius
	// leer a AhtExteriorGeotermico
	temp_exterior_geotermica = AhtExteriorGeotermico.readTemperature();
}

//==================================================================================================================//

void leerSoilExteriores() // en leerSensores()
{
	int humedad_suelo_exterior_raw[MUESTRAS_HUMEDAD_SUELO];
	unsigned int humedad_suelo_exterior_suma = 0;
	for (int i = 0; i < MUESTRAS_HUMEDAD_SUELO; ++i)
	{
		humedad_suelo_exterior_raw[i] = analogRead(SOIL_EXT_PIN);
		humedad_suelo_exterior_suma += humedad_suelo_exterior_raw[i];
	}
	humedad_suelo_exterior = (humedad_suelo_exterior_suma / MUESTRAS_HUMEDAD_SUELO);
	humedad_suelo_exterior = map(humedad_suelo_exterior, 0, 4095, 100, 0);
	// TODO: el 70 % del agua pura debería ser 100 %, y el 29 % del aire debería ser 0 % (ver en tierra verdadera)
}
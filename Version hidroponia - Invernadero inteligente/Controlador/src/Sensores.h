#pragma once

#include "Declaraciones.h"

AHT10Mux::AHT10Mux(PinsAHT10MUX salida_del_mux) {	this->salida_del_mux = static_cast<int>(salida_del_mux);	}

bool AHT10Mux::begin()
{
	this->establecerSalidaMUX();
	return AhtSeleccionado.begin();
}

float AHT10Mux::readTemperature(bool readI2C)
{
	this->establecerSalidaMUX();
	this->temperatura = AhtSeleccionado.readTemperature(readI2C);
	return this->temperatura;
}

float AHT10Mux::readHumidity(bool readI2C)
{
	this->establecerSalidaMUX();
	return AhtSeleccionado.readHumidity(readI2C);
}

void AHT10Mux::establecerSalidaMUX()
{
	bool bits[3]; // últimos 3 bits del número
	for (int i = 0;  i < 3;  ++i)
		bits[i] =  (this->salida_del_mux >> i) & 1;

	digitalWrite(static_cast<uint8_t>(PinsOut::MUX_A), bits[0]);
	digitalWrite(static_cast<uint8_t>(PinsOut::MUX_B), bits[1]);
	digitalWrite(static_cast<uint8_t>(PinsOut::MUX_C), bits[2]);
}

//==================================================================================================================//

void inicializarSensores()
{
	const uint8_t CANT_SENSORES = 7;
	bool sensor[CANT_SENSORES];
	sensor[0] = AhtInteriorHigh.begin();
	sensor[1] = AhtInteriorMid.begin();
	sensor[2] = AhtInteriorLow.begin();
	sensor[3] = AhtAgua1.begin();
	sensor[4] = AhtAgua2.begin();
	sensor[5] = AhtAgua3.begin();
	sensor[6] = AhtGeotermico.begin();
	if (!sensor[0] || !sensor[1] || !sensor[2] || !sensor[3] || !sensor[4] || !sensor[5] || !sensor[6])
	{
		imprimir("Error inicializando AHT10: ");
		for (uint8_t i = 0; i < CANT_SENSORES; ++i)
			imprimir(sensor[i]);
		imprimirln();
	}
}

//==================================================================================================================//

void leerSensores() // en "loop()"
{
	leerSensoresAHT10();
	leerSensoresSoil();
}

//===============================================================================================================================//

void leerSensoresAHT10() // en leerSensores()
{
	AhtInteriorHigh.readTemperature();					// Celsius
	humedad_int_high = AhtInteriorHigh.readHumidity();	// %
	AhtInteriorMid.readTemperature();
	humedad_int_mid = AhtInteriorMid.readHumidity();
	AhtInteriorLow.readTemperature();
	humedad_int_low = AhtInteriorLow.readHumidity();
	AhtGeotermico.readTemperature();
	AhtAgua1.readTemperature();
	AhtAgua2.readTemperature();
	AhtAgua3.readTemperature();
	//humedad_aire_interior_promedio = (humedad_aire_interior[0] + humedad_aire_interior[1] + humedad_aire_interior[2]) / 3;
	//temp_interior_promedio = (temp_interior[0] + temp_interior[1] + temp_interior[2]) / 3;
}

//===============================================================================================================================//

void leerSensoresSoil() // en leerSensores()
{
	humedad_suelo1 = analogRead(static_cast<uint8_t>(PinsIn::SOIL1));
	humedad_suelo1 = map(humedad_suelo1, 0, 4095, 100, 0);
	humedad_suelo2 = analogRead(static_cast<uint8_t>(PinsIn::SOIL2));
	humedad_suelo2 = map(humedad_suelo2, 0, 4095, 100, 0);
	humedad_suelo3 = analogRead(static_cast<uint8_t>(PinsIn::SOIL3));
	humedad_suelo3 = map(humedad_suelo3, 0, 4095, 100, 0);
	humedad_suelo4 = analogRead(static_cast<uint8_t>(PinsIn::SOIL4));
	humedad_suelo4 = map(humedad_suelo4, 0, 4095, 100, 0);
	// TODO: el 70 % del agua pura debería ser 100 %, y el 29 % del aire debería ser 0 % (ver en tierra verdadera)
}



/*
int humedad_suelo_raw[MUESTRAS_HUMEDAD_SUELO];
unsigned int humedad_suelo_suma = 0;
for (int i = 0; i < MUESTRAS_HUMEDAD_SUELO; ++i)
{
	humedad_suelo_raw[i] = analogRead(static_cast<uint8_t>(PinsIn::SOIL1));
	humedad_suelo_suma += humedad_suelo_raw[i];
}
humedad_suelo1 = (humedad_suelo_suma / MUESTRAS_HUMEDAD_SUELO);
humedad_suelo1 = map(humedad_suelo1, 0, 4095, 100, 0);
*/
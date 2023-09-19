#pragma once

#include "Declaraciones.h"

void LocalSD::inicializar()
{
	if (!SD.begin(SD_CS))
	{
		LCDP.displayNoSD();
		while (1)
			;
	}

	File DatalogSD = SD.open(this->DATALOG_PATH, FILE_WRITE);
	if (!DatalogSD)
	{
		LCDP.displayErrorSD();
		while (1)
			;
	}
	DatalogSD.println(this->DATALOG_HEADLINE);
	DatalogSD.close();
}

//===============================================================================================================================//

void LocalSD::leerConfigWiFi()
{
	for (uint8_t i = 0; i < CANT_REDES_WIFI; ++i)
	{
		ResultadoLecturaSD lectura;
		char i_txt[7];
		char path[45];
		sprintf(i_txt, "%i%s", i + 1, TXT);
		sprintf(path, "%s%s%s", this->WIFI_FOLDER_PATH, NOMBRE_ARCHIVO_WSSID, i_txt);

		// Rellena la fila i de LCWF.ssid con un caracter por columna [i][0] = 'H', [i][1] = 'e', [i][2] = 'l', [i][3] = 'l', etc
		lectura = leerStringA(LCWF.ssid[i], W_SSID_SIZE, path); // this-> pointer evitado para mejor lectura
		if (lectura == ResultadoLecturaSD::NO_ARCHIVO  ||  lectura == ResultadoLecturaSD::NO_CONTENIDO)
			continue;

		sprintf(path, "%s%s%s", this->WIFI_FOLDER_PATH, NOMBRE_ARCHIVO_WPASS, i_txt);

		lectura = leerStringA(LCWF.pass[i], W_PASS_SIZE, path);
		if (lectura == ResultadoLecturaSD::NO_ARCHIVO  ||  lectura == ResultadoLecturaSD::NO_CONTENIDO)
			LCWF.guardarRedWiFi(LCWF.ssid[i]);
		else
			LCWF.guardarRedWiFi(LCWF.ssid[i], LCWF.pass[i]);
		
		++LCWF.cant_redes;
	}
}

void LocalSD::leerConfigFirebase()
{
	char path[50];

	ResultadoLecturaSD lectura[4];
	sprintf(path, "%s%s%s", this->FIREBASE_FOLDER_PATH, NOMBRE_ARCHIVO_FEMAIL, TXT);
	lectura[0] = leerStringA(LCFB.email,	F_EMAIL_SIZE,	path);
	sprintf(path, "%s%s%s", this->FIREBASE_FOLDER_PATH, NOMBRE_ARCHIVO_FPASS, TXT);
	lectura[1] = leerStringA(LCFB.pass,		F_PASS_SIZE,	path);
	sprintf(path, "%s%s%s", this->FIREBASE_FOLDER_PATH, NOMBRE_ARCHIVO_FURL, TXT);
	lectura[2] = leerStringA(LCFB.url,		F_URL_SIZE,		path);
	sprintf(path, "%s%s%s", this->FIREBASE_FOLDER_PATH, NOMBRE_ARCHIVO_FAPIKEY, TXT);
	lectura[3] = leerStringA(LCFB.api_key,	F_API_KEY_SIZE,	path);	

	for (uint8_t i = 0; i < 4; ++i)
		if (lectura[i] == ResultadoLecturaSD::NO_ARCHIVO  ||  lectura[i] == ResultadoLecturaSD::NO_CONTENIDO)
			return;

	LCFB.tiene_firebase = true;
}

//===============================================================================================================================//
// Cargamos valores por defecto. Escribimos los de la SD en la EEPROM. Si uno no es válido (o no existe archivo), no lo escribimos.
// Luego de escribir los valores de la SD, leemos la EEPROM para asignar los valores a las variables globales
void LocalSD::leerConfigParametros()
{
	LCEE.cargarValoresPorDefecto();

	for (uint8_t i = 0; i < LCEE.CANT_VARIABLES; ++i)
	{
		char path[7];
		sprintf(path, "%s%02d%s", this->PARAMETROS_FOLDER_PATH, i + 1, TXT);

		File ArchivoSD = SD.open(path, FILE_READ);
		if (!ArchivoSD)
		{
			ArchivoSD.close(); // Por las dudas, es buena práctica
			continue;
		}

		if		(LCEE.LONGITUD_DATO[i] == 1)
			LCEE.escribir(LCEE.direccion[i], (uint8_t)	ArchivoSD.parseInt());
		else if	(LCEE.LONGITUD_DATO[i] == 4)
			LCEE.escribir(LCEE.direccion[i],			ArchivoSD.parseFloat());
		else if (LCEE.LONGITUD_DATO[i] == 2)
			LCEE.escribir(LCEE.direccion[i], (uint16_t)	ArchivoSD.parseInt());
		
		ArchivoSD.close();
	}

	LCEE.leerCompleta();
}

//===============================================================================================================================//

// Pone el contenido del archivo "path" dentro de "buffer". "caracteres" es la cantidad máxima de caracteres a rellenar
ResultadoLecturaSD LocalSD::leerStringA(char *buffer, const uint8_t caracteres, const char *path)
{
	uint8_t bytes_leidos = 0;
	char contenido[caracteres];

	File ArchivoSD = SD.open(path, FILE_READ);
	if (!ArchivoSD)
	{
		ArchivoSD.close(); // Por las dudas, es buena práctica
		return ResultadoLecturaSD::NO_ARCHIVO;
	}

	bytes_leidos = ArchivoSD.readBytes(contenido, caracteres);
	ArchivoSD.close();

	if (bytes_leidos == 0)
		return ResultadoLecturaSD::NO_CONTENIDO;
	strcpy(buffer, contenido);
	return ResultadoLecturaSD::EXITOSO;
}

//===============================================================================================================================//

void LocalSD::datalog()
{
	unsigned long millis_actual = millis();
	if (millis_actual - this->ultimo_datalog < DELAY_DATALOG)
		return;
	this->ultimo_datalog = millis_actual;

	File DatalogSD = SD.open(this->DATALOG_PATH, FILE_WRITE);
	if (!DatalogSD)
	{
		LCDP.displayErrorSD();
		DatalogSD.close();
		return;
	}

	this->escribirSDAbierta(DatalogSD, millis_actual/1000, true);
	this->escribirSDAbierta(DatalogSD, AhtInteriorHigh.temperatura,	true);
	this->escribirSDAbierta(DatalogSD, AhtInteriorMid.temperatura,	true);
	this->escribirSDAbierta(DatalogSD, AhtInteriorLow.temperatura,	true);
	this->escribirSDAbierta(DatalogSD, AhtGeotermico.temperatura,	true);
	this->escribirSDAbierta(DatalogSD, humedad_int_high,	true);
	this->escribirSDAbierta(DatalogSD, humedad_int_mid,		true);
	this->escribirSDAbierta(DatalogSD, humedad_int_low,		true);
	this->escribirSDAbierta(DatalogSD, humedad_suelo1, true);
	this->escribirSDAbierta(DatalogSD, humedad_suelo2, true);
	this->escribirSDAbierta(DatalogSD, Riego.encendida,		true);
	this->escribirSDAbierta(DatalogSD, Calefa.encendida,	true);
	this->escribirSDAbierta(DatalogSD, Ventilacion.abierta,	false);
	DatalogSD.println();

	DatalogSD.close();
}

//===============================================================================================================================//

template <typename T>
void LocalSD::escribirSDAbierta(File Archivo, T dato, bool coma)
{
	Archivo.print(dato);
	if (coma)
		Archivo.print(',');
	Archivo.flush();
}
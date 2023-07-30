#pragma once

#include "Declaraciones.h"

void inicializarSD()
{
	if (!SD.begin(SD_CS))
	{
		displayNoSD();
		while (1)
			;
	}

	DatalogSD = SD.open("Datos.txt", FILE_WRITE);
	if (!DatalogSD)
	{
		displayErrorSD();
		while (1)
			;
	}
}

//===============================================================================================================================//

void configWiFi()
{
	for (uint8_t i = 0; i < CANT_REDES_WIFI; ++i)
	{
		char path[50];
		ResultadoLecturaSD lectura;
		sprintf(path, "%s%s%s%i%s", CONFIG_FOLDER_PATH, WIFI_FOLDER_PATH, NOMBRE_ARCHIVO_WSSID, i + 1, TXT);

		// Rellena la fila i de LCWF.ssid con un caracter por columna [i][0] = 'H', [i][0] = 'e', [i][0] = 'l', [i][0] = 'l', etc
		lectura = leerArchivoSDA(LCWF.ssid[i], W_SSID_SIZE, path);
		if (lectura == ResultadoLecturaSD::NO_ARCHIVO  ||  lectura == ResultadoLecturaSD::NO_CONTENIDO)
			continue;

		char* pch = strstr(path, NOMBRE_ARCHIVO_WSSID);
		strncpy(pch, NOMBRE_ARCHIVO_WPASS, 6);

		lectura = leerArchivoSDA(LCWF.pass[i], W_PASS_SIZE, path);
		if (lectura == ResultadoLecturaSD::NO_ARCHIVO  ||  lectura == ResultadoLecturaSD::NO_CONTENIDO)
			LCWF.guardarRedWiFi(LCWF.ssid[i]);
		else
			LCWF.guardarRedWiFi(LCWF.ssid[i], LCWF.pass[i]);
		
		++cant_redes_wifi;
	}
}

void configFirebase()
{
	char p_path[35];
	char path[50];
	sprintf(p_path, "%s%s", CONFIG_FOLDER_PATH, FIREBASE_FOLDER_PATH);
	char* pch = strstr(path, NOMBRE_ARCHIVO_FEMAIL);

	ResultadoLecturaSD lectura[4];
	sprintf(path, "%s%s%s", p_path, NOMBRE_ARCHIVO_FEMAIL, TXT);
	lectura[0] = leerArchivoSDA(LCFB.email,	F_EMAIL_SIZE,	path);
	sprintf(path, "%s%s%s", p_path, NOMBRE_ARCHIVO_FPASS, TXT);
	lectura[1] = leerArchivoSDA(LCFB.pass,	F_PASS_SIZE,	path);
	sprintf(path, "%s%s%s", p_path, NOMBRE_ARCHIVO_FURL, TXT);
	lectura[2] = leerArchivoSDA(LCFB.url,	F_URL_SIZE,		path);
	sprintf(path, "%s%s%s", p_path, NOMBRE_ARCHIVO_FAPIKEY, TXT);
	lectura[3] = leerArchivoSDA(LCFB.api_key,F_API_KEY_SIZE,	path);	

	for (uint8_t i = 0; i < 4; ++i)
		if (lectura[i] == ResultadoLecturaSD::NO_ARCHIVO  ||  lectura[i] == ResultadoLecturaSD::NO_CONTENIDO)
			return;

	LCFB.tiene_firebase = true;
}

//===============================================================================================================================//

// Pone el contenido del archivo "path" dentro de "buffer". "caracteres" es la cantidad máxima de caracteres a rellenar
ResultadoLecturaSD leerArchivoSDA(char *buffer, const uint8_t caracteres, const char *path)
{
	uint8_t bytes_leidos = 0;
	char contenido[caracteres];

	File ArchivoSD = SD.open(path, FILE_READ);
	if (!ArchivoSD)
		return ResultadoLecturaSD::NO_ARCHIVO;

	bytes_leidos = ArchivoSD.readBytes(contenido, caracteres);
	ArchivoSD.close();

	if (bytes_leidos == 0)
		return ResultadoLecturaSD::NO_CONTENIDO;
	strcpy(buffer, contenido);
	return ResultadoLecturaSD::EXITOSO;
}

//===============================================================================================================================//

void escribirDatos(String dato)
{
	// Para los sensores
}
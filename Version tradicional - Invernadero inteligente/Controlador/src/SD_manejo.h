#pragma once

#include "Declaraciones.h"

void LocalSD::inicializar()
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

void LocalSD::leerConfigWiFi()
{
	for (uint8_t i = 0; i < CANT_REDES_WIFI; ++i)
	{
		char path[50];
		ResultadoLecturaSD lectura;
		sprintf(path, "%s%s%s%i%s", CONFIG_FOLDER_PATH, WIFI_FOLDER_PATH, NOMBRE_ARCHIVO_WSSID, i + 1, TXT);

		// Rellena la fila i de LCWF.ssid con un caracter por columna [i][0] = 'H', [i][1] = 'e', [i][2] = 'l', [i][3] = 'l', etc
		lectura = leerStringA(LCWF.ssid[i], W_SSID_SIZE, path);
		if (lectura == ResultadoLecturaSD::NO_ARCHIVO  ||  lectura == ResultadoLecturaSD::NO_CONTENIDO)
			continue;

		char* pch = strstr(path, NOMBRE_ARCHIVO_WSSID);
		strncpy(pch, NOMBRE_ARCHIVO_WPASS, 6);

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
	char p_path[35];
	char path[50];
	sprintf(p_path, "%s%s", CONFIG_FOLDER_PATH, FIREBASE_FOLDER_PATH);
	char* pch = strstr(path, NOMBRE_ARCHIVO_FEMAIL);

	ResultadoLecturaSD lectura[4];
	sprintf(path, "%s%s%s", p_path, NOMBRE_ARCHIVO_FEMAIL, TXT);
	lectura[0] = leerStringA(LCFB.email,	F_EMAIL_SIZE,	path);
	sprintf(path, "%s%s%s", p_path, NOMBRE_ARCHIVO_FPASS, TXT);
	lectura[1] = leerStringA(LCFB.pass,	F_PASS_SIZE,	path);
	sprintf(path, "%s%s%s", p_path, NOMBRE_ARCHIVO_FURL, TXT);
	lectura[2] = leerStringA(LCFB.url,	F_URL_SIZE,		path);
	sprintf(path, "%s%s%s", p_path, NOMBRE_ARCHIVO_FAPIKEY, TXT);
	lectura[3] = leerStringA(LCFB.api_key,F_API_KEY_SIZE,	path);	

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

	for (uint8_t i = 1; i < LCEE.CANT_VARIABLES; ++i)
	{
		char path[7];
		sprintf(path, "%02d%s", i, TXT);

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
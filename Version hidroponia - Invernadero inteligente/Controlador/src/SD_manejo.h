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
		sprintf(path, "%s%s%s%i%s", CONFIG_FOLDER_PATH, WIFI_FOLDER_PATH, NOMBRE_ARCHIVOS_WSSID, i + 1, TXT);

		// Rellena la fila i de w_ssid con un caracter por columna [i][0] = 'H', [i][0] = 'e', [i][0] = 'l', [i][0] = 'l', etc
		ResultadoLecturaSD lectura = leerArchivoSDA(w_ssid[i], W_SSID_SIZE, path);
		if (lectura == ResultadoLecturaSD::NO_ARCHIVO  ||  lectura == ResultadoLecturaSD::NO_CONTENIDO)
			continue;

		char* pch = strstr(path, NOMBRE_ARCHIVOS_WSSID);
		strncpy(pch, NOMBRE_ARCHIVOS_WPASS, 4);

		lectura = leerArchivoSDA(w_pass[i], W_PASS_SIZE, path);
		if (lectura == ResultadoLecturaSD::NO_ARCHIVO  ||  lectura == ResultadoLecturaSD::NO_CONTENIDO)
			guardarRedWiFi(w_ssid[i]);
		else
			guardarRedWiFi(w_ssid[i], w_pass[i]);
		
		++cant_redes_wifi;
	}
}

void configFirebase()
{
	char email_usuario[F_EMAIL_SIZE];
	char password_firebase[F_PASS_SIZE];
	// email_usuario = leer;
	// password_firebase = leer;

	// tiene_firebase = true;
	// escribirEEPROM(direccion[DIR_TIENE_FIREBASE], tiene_firebase);
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
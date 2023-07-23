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

//==================================================================================================================//

// SIN TERMINAR. FALTA RECONOCER PASSWORDS VACÍOS O INEXISTENTES.
void configWiFi()
{
	for (uint8_t i = 0; i < CANTIDAD_REDES_WIFI; ++i)
	{
		char path[50];
		char archivo[20];
		sprintf(archivo, "%s%i%s", "ssid", i + 1, ".txt");
		sprintf(path, "%s%s%s", CONFIG_FOLDER_PATH, WIFI_FOLDER_PATH, archivo);

		// Rellena la fila i de w_ssid con un caracter por columna [i][0] = 'H', [i][0] = 'e', [i][0] = 'l', [i][0] = 'l', etc
		leerArchivoSDA(w_ssid[i], W_SSID_SIZE, path);

		path[sizeof(WIFI_FOLDER_PATH) - 1] = 'p';
		path[sizeof(WIFI_FOLDER_PATH) + 0] = 'a';
		path[sizeof(WIFI_FOLDER_PATH) + 1] = 's';
		path[sizeof(WIFI_FOLDER_PATH) + 2] = 's';

		leerArchivoSDA(w_pass[i], W_PASS_SIZE, path);

		guardarRedWiFi(w_ssid[i], w_pass[i]);
	}

	tiene_wifi = true;
	escribirEEPROM(direccion[DIR_TIENE_WIFI], tiene_wifi);
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

//==================================================================================================================//

// Devuelve Arduino String con el contenido del archivo provisto en "path"
String leerArchivoSD(const char *path)
{
}

/*
// Rellena la fila del "array" "nro_network" con el contenido del archivo de "path". "caracteres" es cant. de columnas por fila
void leerArchivoSDA(char (*array)[], const uint8_t nro_network, const uint8_t caracteres, const char *path)
{
	File ArchivoMensaje = SD.open(path, FILE_READ);
	if (ArchivoMensaje)
	{
		uint8_t i = 0;
		while (ArchivoMensaje.available()  &&  i < caracteres)
		{
			array[num_red][i] = ArchivoMensaje.read();
			++i;
		}
		array[num_red][i] = '\0';
		ArchivoMensaje.close();
	}
}
*/
// Pone el contenido del archivo en "path" dentro de "buffer". "caracteres" es la cantidad máxima de caracteres a rellenar
void leerArchivoSDA(char *buffer, const uint8_t caracteres, const char *path)
{
	char contenido[caracteres];
	strcpy(buffer, contenido);
}



//==================================================================================================================//

void escribirDatos(String dato)
{
	// Para los sensores
}
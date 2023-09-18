#pragma once

#include "Declaraciones.h"

void inicializarTiempoUnix()
{
	if (!tiempo_unix_configurado)
	{
		configTime(0, 0, SERVIDOR_NTP);
		tiempo_unix_configurado = true;
	}
}

//==================================================================================================================//

unsigned long obtenerTiempoUnix()
{
	time_t ahora;
	struct tm timeinfo;
	if (!getLocalTime(&timeinfo)) {
		// no se obtuvo el tiempo
		return(0);
	}
	time(&ahora);
	return ahora;
}
/*
//==================================================================================================================//
// El buffer debe ser igual (o mayor, pero innecesario) a 45 bytes
void mensajeSegundosATiempo(char* buffer, uint8_t caracteres, unsigned long segundos)
{
	if (caracteres < 45)
		return;
	int minutos =			segundos / 60;
	int resto_seg =	segundos % 60;
	int horas =				minutos / 60;
	int resto_min =		minutos % 60;
	int dias =				horas / 24;
	int resto_horas =		horas % 24;
	char mensaje[45] = "";

	if (dias != 0)
	{
		if (dias == 1)
			strcat(mensaje, "1 día");
		else
			sprintf(mensaje, "%i%s", dias, " días");
	}

	if (resto_horas != 0)
	{
		if (strlen(mensaje) > 0)
			strcat(mensaje, ", ");
		if (resto_horas == 1)
			strcat(mensaje, "1 hora");
		else
		{
			char nro_horas[9];
			sprintf(nro_horas, "%d%s", resto_horas, " horas");
			strcat(mensaje, nro_horas);
		}
	}

	if (resto_min != 0)
	{
		if (strlen(mensaje) > 0)
			strcat(mensaje, ", ");
		if (resto_min == 1)
			strcat(mensaje, "1 minuto");
		else
		{
			char nro_minutos[10];
			sprintf(nro_minutos, "%d%s", resto_min, " minutos");
			strcat(mensaje, nro_minutos);
		}
	}

	if (resto_seg != 0)
	{
		if (strlen(mensaje) > 0)
			strcat(mensaje, ", ");
		if (resto_seg == 1)
			strcat(mensaje, "1 segundo");
		else
		{
			char nro_segundos[10];
			sprintf(nro_segundos, "%d%s", resto_seg, " segundos");
			strcat(mensaje, nro_segundos);
		}
	}

	strcpy(buffer, mensaje);
}
*/
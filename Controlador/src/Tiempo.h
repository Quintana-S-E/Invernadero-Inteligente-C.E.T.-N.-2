#pragma once

#include "Declaraciones.h"

String mensajeSegundosATiempo(unsigned long segundos)
{
	int minutos =			segundos / 60;
	int resto_segundos =	segundos % 60;
	int horas =				minutos / 60;
	int resto_minutos =		minutos % 60;
	int dias =				horas / 24;
	int resto_horas =		horas % 24;
	String mensaje = "";

	if (dias != 0)
	{
		if (dias == 1)
			mensaje += "1 día";
		else
			mensaje += String(dias) + " días";
	}

	if (resto_horas != 0)
	{
		if (mensaje.length() > 1)
			mensaje += ", ";
		if (resto_horas == 1)
			mensaje += "1 hora";
		else
			mensaje += String(resto_horas) + " horas";
	}

	if (resto_minutos != 0)
	{
		if (mensaje.length() > 1)
			mensaje += ", ";
		if (resto_minutos == 1)
			mensaje += "1 minuto";
		else
			mensaje += String(resto_minutos) + " minutos";
	}

	if (resto_segundos != 0)
	{
		if (mensaje.length() > 1)
			mensaje += ", ";
		if (resto_segundos == 1)
			mensaje += "1 segundo";
		else
			mensaje += String(resto_segundos) + " segundos";
	}

	return String(mensaje);
}
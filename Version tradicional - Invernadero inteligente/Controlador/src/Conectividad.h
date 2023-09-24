#pragma once

#include "Declaraciones.h"

//==================================================PARTE DE CONEXIONES A WiFi===================================================//


bool LocalWiFi::guardarRedWiFi(const char* ssid, const char* password)	{ imprimir("agregar red: "); imprimir(ssid); imprimir(", "); imprimirln(password);	return WiFiMultiO.addAP(ssid, password);	}
bool LocalWiFi::guardarRedWiFi(const char* ssid)						{	return WiFiMultiO.addAP(ssid);				}

//===============================================================================================================================//

void LocalWiFi::inicializarWiFi()
{
	LCDP.displayConectandoWiFi();
	if(!this->correr())
	{
		imprimirln("No se encuentra red WiFi.");
		LCDP.displayErrorWiFi();
		return;
	}

	LCDP.displayConetadoA( WiFi.SSID() );
}

//===============================================================================================================================//

// devuelve verdadero si hay conexión a WiFi. Falso si no. Maneja LED_WIFI, no maneja display
bool LocalWiFi::correr()
{
	if (this->cant_redes == 0)
		return false;

	WiFi.mode(WIFI_STA);

	if (WiFiMultiO.run() == WL_CONNECTED)
	{
		this->hay_conexion = true;
		inicializarTiempoUnix();
		LCFB.inicializar(); // se ejecutan una sola vez. Acá por si en setup() se cortó WiFi pero después hay
		return true;
	}
	this->hay_conexion = false;
	return false;
}

//===============================================================================================================================//

void LocalWiFi::correrDisplayWiFi()
{
	if (this->cant_redes == 0)
	{
		LCDP.cambiarDato();
		return;
	}
	if (this->hay_conexion)
		LCDP.displayConetadoA( WiFi.SSID() );
	else
		LCDP.displayErrorWiFi();
}


/*
Función conectarWiFi():
Queremos que inicialice bluetooth por un tiempo definido (2 minutos) y que espere a que le lleguen serial strings desde la app.
Necesitamos SSID, PASS, username y pass de firebase (usar wifiMulti.addAP()). Usaremos WiFiMulti Tutorial de la App:

https://youtu.be/l10NGNCGUBc (en vez de botones poner write boxes y enviar lo que se escriba)

Tutorial WiFiMulti:

https://randomnerdtutorials.com/esp32-wifimulti/

Y una vez lleguen los datos guardarlos en la memoria flash del controlador. Tutorial:

https://youtu.be/VnfX9YJbaU8?t=2380 (sí, que el código también esté en este archivo)
Si no sirve bien este tuto (mirarlo después de lograr la comunicación por bluetooth, buscar "save data in spiffs esp32" en yt)


También queremos definir controlarConexion(). Lo bueno es que con WiFiMulti es una sóla línea "if(wifiMulti.run() == WL_CONNECTED)"
*/
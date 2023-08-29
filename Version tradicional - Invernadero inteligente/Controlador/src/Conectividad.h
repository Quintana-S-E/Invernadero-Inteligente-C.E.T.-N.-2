#pragma once

#include "Declaraciones.h"

//==================================================PARTE DE CONEXIONES A WiFi===================================================//


bool LocalWiFi::guardarRedWiFi(const char* ssid, const char* password)	{	return WiFiMultiO.addAP(ssid, password);	}
bool LocalWiFi::guardarRedWiFi(const char* ssid)						{	return WiFiMultiO.addAP(ssid);				}

//===============================================================================================================================//

void LocalWiFi::inicializarWiFi()
{
	displayConectandoWiFi();
	if(!this->correr())
	{
		imprimirln("No se encuentra red WiFi.");
		displayErrorWiFi();
		return;
	}

	displayConetadoA( WiFi.SSID() );
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
		// TODO: Actualizar hay_conexion
		inicializarTiempoUnix(); // se ejecuta una sola vez. Acá por si en setup() se cortó WiFi pero después hay
		digitalWrite(LED_WIFI, LOW); // encender
		return true;
	}
	// TODO: Actualizar hay_conexion
	digitalWrite(LED_WIFI, HIGH); // apagar
	return false;
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
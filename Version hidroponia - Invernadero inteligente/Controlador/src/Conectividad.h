#pragma once

#include "Declaraciones.h"

//============================================PARTE DE CONEXIONES A WiFi============================================//


bool guardarRedWiFi(const char* ssid, const char* password)	{	return WiFiMultiO.addAP(ssid, password);	}
bool guardarRedWiFi(const char* ssid)						{	return WiFiMultiO.addAP(ssid);				}

//==================================================================================================================//

// NO FUNCIONA, UTILIZA MÉTODO ANTERIOR DE BLUETOOTH
void inicializarWiFi()
{
	if (cant_redes_wifi == 0)
	{
		if ( !quiereAgregarCredenciales() ) // maneja display
			return;

		if ( !recibirBTApp(true) )
			return;
	}

	WiFi.mode(WIFI_STA);

	// sólo se repite SI quiere cambiar las credenciales Y recibimos nuevos datos de la app. Breaks if nos pudimos conectar
	while (true)
	{
		displayConectandoWiFi();
		if(WiFiMultiO.run() != WL_CONNECTED) // acá llamar a conectarWiFi(); devuelve true o false
		{
			if ( !quiereCambiarCredenciales() ) // maneja display
				return;

			if ( !recibirBTApp(true) )
				return;
		}
		else break;
	}

	displayConetadoA( WiFi.SSID() );
}

//==================================================================================================================//

bool conectarWiFi()
{
	
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


También queremos definir chequearConexion(). Lo bueno es que con WiFiMulti es una sóla línea "if(wifiMulti.run() == WL_CONNECTED)"


TODO IMPORTANTE: AL CONECTARNOS/RECONECTARNOS A WIFI LLAMAR A LA FUNCIÓN inicializarTiempoUnix();
*/
#pragma once

#include "Declaraciones.h"

// Devuelve verdadero si se obtuvieron datos de la aplicación, falso si no.
bool configuracionInicial()
{
	if (tiene_config_inicial)
		return true;
	// Es recursiva. Se llama nuevamente si no se identifica el tipo de dato enviado
	intentos_bluetooth++;
    if (intentos_bluetooth == 10)
	{
		BTSerial.end();
		return false;
	}
	BTSerial.begin(BLUETOOTH_NOMBRE);
	unsigned long tiempo_0_bluetooth = millis();
	byte byte_prueba = 0;
	do
	{
		if (millis() - tiempo_0_bluetooth >= BLUETOOTH_TIEMPO_MAX_CONFIGURACION)
		{
			// timeout
			BTSerial.end();
			tiene_wifi = false; // si no tiene wifi?
			return false;
		}
		if (BTSerial.available() > 0)
			byte_prueba = BTSerial.read();
	} while (BTSerial.available() == 0 || byte_prueba == BLUETOOTH_TEST_BYTE  ||  byte_prueba == 0); // Por el \0
	if(!decodificarMensaje(byte_prueba))
		configuracionInicial();
	BTSerial.end();
	return true;
}

//==================================================================================================================//

// Devuelve verdadero si se decodificó qué tipo de mensaje envió la aplicación. Falso si no.
bool decodificarMensaje(byte primer_byte)
{
	BTSerial.read(); // deshacernos del \n
	// si es el byte de sin WiFi o si no se envió nada más que ese byte
	if (primer_byte == BLUETOOTH_PRIMER_BYTE_SIN_WIFI  ||  !BTSerial.available())
		configSinWiFi();
	else if (primer_byte == BLUETOOTH_PRIMER_BYTE_CON_WIFI)
		configConWiFi();
	else
		return false; // No se identificó el tipo de dato
	tiene_config_inicial = true;
	escribirEEPROM(direccion[DIR_TIENE_CONFIG_INICIAL], tiene_config_inicial);
	return true;
}

//==================================================================================================================//

void configSinWiFi()
{
	tiene_wifi = false;
	escribirEEPROM(direccion[DIR_TIENE_WIFI], tiene_wifi);
	// poner mensaje en display
}

//==================================================================================================================//

void configConWiFi()
{
	char ssid[32];
	char password_wifi[32];
	BTSerial.readBytesUntil('\n', ssid, sizeof(ssid));
	BTSerial.readBytesUntil('\n', password_wifi, sizeof(password_wifi));
	guardarRedWiFi(ssid, password_wifi);
}

//==================================================================================================================//

void guardarRedWiFi(const char *ssid, const char *password_wifi = "NULL")
{
	if (password_wifi == "NULL")
		WiFiMultiO.addAP(ssid);
	else
		WiFiMultiO.addAP(ssid, password_wifi);
	// y acá hacemos lo de JSON
}

//==================================================================================================================//



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

*/
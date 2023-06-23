#pragma once

#include "Declaraciones.h"

// Devuelve verdadero si se obtuvieron datos de la aplicación, falso si no.
bool configuracionInicial()
{
	if (tiene_config_inicial)
		return true;
	BTSerial.begin(BLUETOOTH_NOMBRE);
	unsigned long tiempo_0_bluetooth = millis();
	char caracter_prueba;
	do
	{
		if (BTSerial.available() > 0)
			caracter_prueba = BTSerial.read();
		if (millis() - tiempo_0_bluetooth >= BLUETOOTH_TIEMPO_MAX_CONFIGURACION)
		{
			// pasó el tiempo
			BTSerial.end();
			tiene_wifi = false; // si no tiene wifi?
			return false;
		}
	} while (BTSerial.available() == 0 || caracter_prueba == BLUETOOTH_TEST_CARACTER  ||  caracter_prueba == '\0');
	bool resultado = decodificarMensaje(caracter_prueba);
	BTSerial.end();
	return resultado;
}

//==================================================================================================================//

// Devuelve verdadero si se obtuvo el dato que se quería de la aplicación. Falso si no.
bool decodificarMensaje(char primer_caracter)
{
	bool resultado_esperado = false;
	char mensaje_primer_linea[2];
	mensaje_primer_linea[0] = primer_caracter;
	mensaje_primer_linea[1] = BTSerial.read();
	BTSerial.read(); // deshacernos del \n
	if (mensaje_primer_linea == BLUETOOTH_PRIMER_LINEA_SIN_WIFI)
	{
		configSinWiFi();
		resultado_esperado = true;
	}
	else if (mensaje_primer_linea == BLUETOOTH_PRIMER_LINEA_SOLO_WIFI)
	{
		configWiFi();
		resultado_esperado = true;
	}
	else if (mensaje_primer_linea == BLUETOOTH_PRIMER_LINEA_WIFI_FIREBASE)
	{
		configWiFiFirebase();
		resultado_esperado = true;
	}
	if (!resultado_esperado)
		return false;
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

void configWiFi()
{
	char ssid[32];
	char password_wifi[32];
	BTSerial.readBytesUntil('\n', ssid, sizeof(ssid));
	BTSerial.readBytesUntil('\n', password_wifi, sizeof(password_wifi));
	guardarRedWiFi(ssid, password_wifi);
}

//==================================================================================================================//

void configWiFiFirebase()
{
	configWiFi();
	char email_usuario[85]; // 63 de domain + 21 de @cet2bariloche.edu.ar (o menos de @gmail y @hotmail.com)
	char password_firebase[30];
	BTSerial.readBytesUntil('\n', email_usuario, sizeof(email_usuario));
	BTSerial.readBytesUntil('\n', password_firebase, sizeof(password_firebase));
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

void guardarDatosFirebase(String email, String password_firebase, String api_key)
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

*/
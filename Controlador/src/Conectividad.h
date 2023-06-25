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
	//Serial.println("Bluetooth encendido");
	unsigned long tiempo_0_bluetooth = millis();
	byte byte_prueba = 0;
	do
	{
		if (millis() - tiempo_0_bluetooth >= BLUETOOTH_TIEMPO_MAX_CONFIGURACION)
		{
			//Serial.println("Pasó demasiado tiempo");
			BTSerial.end();
			tiene_wifi = false;
			return false;
		}
		if (BTSerial.available() > 0)
			byte_prueba = BTSerial.read();
	} while (BTSerial.available() == 0 || byte_prueba == BLUETOOTH_TEST_BYTE);
	//Serial.println("Mensaje distinto del byte de prueba");
	if(!decodificarMensaje(byte_prueba))
	{
		limpiarBufferBluetooth();
		//Serial.println("no es ninguno, volviendo a llamar");
		configuracionInicial();
	}
	BTSerial.end();
	return true;
}

//==================================================================================================================//

// Devuelve verdadero si se decodificó qué tipo de mensaje envió la aplicación. Falso si no.
bool decodificarMensaje(byte primer_byte)
{
	//Serial.println(primer_byte);
	BTSerial.read(); // deshacernos del \n
	if (primer_byte == BLUETOOTH_PRIMER_BYTE_SIN_WIFI)
		decodificarSinWiFi();
	else if (primer_byte == BLUETOOTH_PRIMER_BYTE_CON_WIFI)
		decodificarConWiFi();
	else
		return false; // No se identificó el tipo de dato
	tiene_config_inicial = true;
	escribirEEPROM(direccion[DIR_TIENE_CONFIG_INICIAL], tiene_config_inicial);
	return true;
}

//==================================================================================================================//

void decodificarSinWiFi()
{
	//Serial.println("config sin wifi");
	tiene_wifi = false;
	escribirEEPROM(direccion[DIR_TIENE_WIFI], tiene_wifi);
	// poner mensaje en display
}

//==================================================================================================================//

void decodificarConWiFi()
{
	// Muy sucio, pero no me meto con pointers para hacer una función que lea el buffer. BTSerial.readBytesUntil no funcionaba :S
	char ssid[32];
	char password_wifi[32];
	static byte ndx = 0;
    char incom_char;
    bool llenando_password = false, listo_password = false;
    while (BTSerial.available() > 0 && !listo_password)
	{
        incom_char = BTSerial.read();
        if (incom_char != '\n')
		{
			if (llenando_password)
				password_wifi[ndx] = incom_char;
			else
            	ssid[ndx] = incom_char;
            ndx++;
            if (ndx >= 32)
                ndx = 32 - 1;
        }
        else
		{
			if (llenando_password)
			{
				password_wifi[ndx] = '\0';
				listo_password = true;
			}
			else
			{
            	ssid[ndx] = '\0';
				llenando_password = true;
				ndx = 0;
			}
        }
	}
	//Serial.print("SSID: ");
	//Serial.println(ssid);
	//Serial.print("PASS: ");
	//Serial.println(password_wifi);
	guardarRedWiFi(ssid, password_wifi);
}

//==================================================================================================================//

// Enviar "NULL" si es WiFi público
void guardarRedWiFi(const char *ssid, const char *password_wifi)
{
	if (password_wifi == "NULL")
		WiFiMultiO.addAP(ssid);
	else
		WiFiMultiO.addAP(ssid, password_wifi);
	// y acá hacemos lo de JSON
}

//==================================================================================================================//

void limpiarBufferBluetooth()
{
	while(BTSerial.available() > 0)
	{
		BTSerial.read();
	}
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
#pragma once

#include "Declaraciones.h"

void LocalDisplay::inicializar() // en "setup()"
{
	if (!Display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
	{
		imprimirln("Fallo en el display. Resetear el equipo.");
		while (true)	;
	}
	Display.clearDisplay();
	Display.setTextColor(WHITE);
}

//===============================================================================================================================//

void LocalDisplay::cambiarDato()
{
	switch (DatoDelDisplay)
	{
	case DisplayDato::Temperatura1:
		DatoDelDisplay = DisplayDato::Temperatura2;
		break;

	case DisplayDato::Temperatura2:
		DatoDelDisplay = DisplayDato::Temperatura3;
		break;
		
	case DisplayDato::Temperatura3:
		DatoDelDisplay = DisplayDato::Temperatura4;
		break;

	case DisplayDato::Temperatura4:
		DatoDelDisplay = DisplayDato::HumedadAire1;
		break;

	case DisplayDato::HumedadAire1:
		DatoDelDisplay = DisplayDato::HumedadAire2;
		break;

	case DisplayDato::HumedadAire2:
		DatoDelDisplay = DisplayDato::HumedadSuelo1;
		break;

	case DisplayDato::HumedadSuelo1:
		DatoDelDisplay = DisplayDato::HumedadSuelo2;
		break;

	case DisplayDato::HumedadSuelo2:
		DatoDelDisplay = DisplayDato::WiFi;
		break;

	case DisplayDato::WiFi:
		DatoDelDisplay = DisplayDato::Temperatura1;
		break;
	}
}

//===============================================================================================================================//

void LocalDisplay::actualizar() // en "loop()"
{
	// sólo actualizamos los números del display cada cierto tiempo (porque si no los números parpadean)
	if (millis() - LCDP.ultima_actualizacion < DELAY_ACTUALIZACION_DISPLAY)
		return;
	this->ultima_actualizacion = millis();

	switch (DatoDelDisplay)
	{
	case DisplayDato::Temperatura1:
		this->displayTemperatura(1);
		break;

	case DisplayDato::Temperatura2:
		this->displayTemperatura(2);
		break;

	case DisplayDato::Temperatura3:
		this->displayTemperatura(3);
		break;

	case DisplayDato::Temperatura4:
		this->displayTemperatura(4);
		break;

	case DisplayDato::HumedadAire1:
		this->displayHumedadAire(1);
		break;

	case DisplayDato::HumedadAire2:
		this->displayHumedadAire(2);
		break;

	case DisplayDato::HumedadSuelo1:
		this->displayHumedadSuelo(1);
		break;
	
	case DisplayDato::HumedadSuelo2:
		this->displayHumedadSuelo(2);
		break;

	case DisplayDato::WiFi:
		LCWF.correrDisplayWiFi();
	}
}

//===============================================================================================================================//

void LocalDisplay::displayTemperatura(uint8_t pantalla) // en "actualizar()"
{
	if (pantalla == 1)
		display2temperaturas(this->msg_temp_sup, AhtInteriorHigh.temperatura, this->msg_temp_mid, AhtInteriorMid.temperatura);
	else if (pantalla = 2)
		display2temperaturas(this->msg_temp_inf, AhtInteriorLow.temperatura, this->msg_temp_geo, AhtGeotermico.temperatura);
	else if (pantalla = 3)
		display2temperaturas(this->msg_temp_agua1, AhtAgua1.temperatura, this->msg_temp_agua2, AhtAgua2.temperatura);
	else if (pantalla = 4)
		display2temperaturas(this->msg_temp_agua3, AhtAgua3.temperatura, this->msg_vacio);
}

//===============================================================================================================================//

void LocalDisplay::displayHumedadAire(uint8_t pantalla) // en "actualizar()"
{
	if (pantalla == 1)
		this->display2porcentajes(this->msg_hum_aire_sup, humedad_int_high, this->msg_hum_aire_mid, humedad_int_mid);
	else
		this->display2porcentajes(this->msg_hum_aire_inf, humedad_int_low, this->msg_vacio);
}

//===============================================================================================================================//

void LocalDisplay::displayHumedadSuelo(uint8_t pantalla) // en "actualizar()"
{
	if (pantalla == 1)
		this->display2porcentajes(this->msg_hum_suelo1, humedad_suelo1, this->msg_hum_suelo2, humedad_suelo2);
	else
		this->display2porcentajes(this->msg_hum_suelo3, humedad_suelo3, this->msg_hum_suelo4, humedad_suelo4);
}

//===============================================================================================================================//
// para un solo valor enviar como parámetro "msg_abajo" un empty string ("")
void LocalDisplay::display2temperaturas(char msg_arriba[22], float valor_arriba, char msg_abajo[22], float valor_abajo)
{
	Display.clearDisplay();

	Display.setTextSize(1);
	Display.setCursor(0, 0);
	Display.print(msg_arriba);
	Display.setTextSize(2);
	Display.setCursor(0, 11);
	Display.print(valor_arriba);
	Display.print(" ");
	Display.setTextSize(1);
	Display.cp437(true);
	Display.write(167);
	Display.setTextSize(2);
	Display.print("C");
	if (strlen(msg_abajo) == 0) return;
	Display.setTextSize(1);
	Display.setCursor(0, 33);
	Display.print(msg_abajo);
	Display.setTextSize(2);
	Display.setCursor(0, 44);
	Display.print(valor_abajo);
	Display.print(" ");
	Display.setTextSize(1);
	Display.cp437(true);
	Display.write(167);
	Display.setTextSize(2);
	Display.print("C");

	Display.display();
}
// para un solo valor enviar como parámetro "msg_abajo" un empty string ("")
void LocalDisplay::display2porcentajes(char msg_arriba[22], float valor_arriba, char msg_abajo[22], float valor_abajo)
{
	Display.clearDisplay();

	Display.setTextSize(1);
	Display.setCursor(0, 0);
	Display.print(msg_arriba);
	Display.setTextSize(2);
	Display.setCursor(0, 11);
	Display.print(valor_arriba);
	Display.print(" %");
	if (strlen(msg_abajo) == 0) return;
	Display.setTextSize(1);
	Display.setCursor(0, 33);
	Display.print(msg_abajo);
	Display.setTextSize(2);
	Display.setCursor(0, 44);
	Display.print(valor_abajo);
	Display.print(" %");

	Display.display();
}

//===============================================================================================================================//

void LocalDisplay::displayConectandoWiFi() // en "setup()"
{
	Display.clearDisplay();
	Display.setTextSize(2);
	Display.setCursor(7, 15);
	Display.print(this->msg_conectando);
	Display.display();
}

//===============================================================================================================================//

void LocalDisplay::displayErrorWiFi() // en "setup()"
{
	Display.clearDisplay();
	Display.setTextSize(2);
	Display.setCursor(34, 7);
	Display.print(this->msg_no_se_encuentra);
  	Display.setCursor(20, 40);
  	Display.print("red WiFi");
	Display.display();
}

//===============================================================================================================================//

void LocalDisplay::displayConetadoA(String ssid_conectada) // en "conectarWiFiCon()"
{
	Display.clearDisplay();
	Display.setTextSize(2); // en grande:
	Display.setCursor(11, 5);
	Display.print(this->msg_conectado_a);
	Display.setTextSize(1);				 // en chiquito:
	Display.setCursor(11, 44);
	Display.print(ssid_conectada); // nombre de la red
	Display.display();
}

//===============================================================================================================================//

void LocalDisplay::displayError()
{
	Display.setTextSize(1);
	Display.setCursor(7, 7);
    Display.println(this->msg_error_al_iniciar);
    Display.setCursor(5, 17);
    Display.print(this->msg_controlador_motivo);
}

//===============================================================================================================================//
/*
void LocalDisplay::displayNoSD()
{
	Display.clearDisplay();
	this->displayError();
	Display.setCursor(10, 39);
	Display.print(this->msg_sd_ausente);
	Display.display();
}
*/

//===============================================================================================================================//

void LocalDisplay::displayErrorSD()
{
	Display.clearDisplay();
	this->displayError();
	Display.setCursor(7, 39);
	Display.print(this->msg_error_sd);
	Display.display();
}

//===============================================================================================================================//

void LocalDisplay::displayVentana(bool abriendo)
{
	Display.clearDisplay();
    Display.setTextSize(2);
    Display.setCursor(17, 13);
	if (abriendo)
    	Display.println("Abriendo");
	else
		Display.println("Cerrando");
    Display.setCursor(20, 34);
    Display.print("ventana");
    Display.display();
}

//===============================================================================================================================//

void LocalDisplay::displayLogo()
{
	Display.clearDisplay();
	Display.setTextSize(1);
	Display.setCursor(31, 7);
	Display.println("Invernadero");
	Display.setCursor(31, 18);
	Display.println("inteligente");
	Display.drawLine(5, 33, 123, 33, 1);
	Display.setCursor(28, 40);
	Display.println("Hecho por el");
	Display.setCursor(10, 50);
	Display.println("C.E.T. 2 Bariloche");
	Display.display();
}
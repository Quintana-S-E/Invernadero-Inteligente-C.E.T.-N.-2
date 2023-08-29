#pragma once

#include "Declaraciones.h"

void inicializarDisplay() // en "setup()"
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
// TODO: añadir un estado que sea EstadoWiFi y que diga conectado/desconectado y el nombre de la red
void cambiarDatoDisplay()
{
	switch (DatoDelDisplay)
	{
	case DisplayDato::Temperatura:
		DatoDelDisplay = DisplayDato::HumedadAire;
		break;

	case DisplayDato::HumedadAire:
		DatoDelDisplay = DisplayDato::HumedadSuelo;
		break;

	case DisplayDato::HumedadSuelo:
		DatoDelDisplay = DisplayDato::Temperatura;
		break;
	}
}

//===============================================================================================================================//

void displayConectandoWiFi() // en "setup()"
{
	Display.clearDisplay();
	Display.setTextSize(2);
	Display.setCursor(7, 15);
	Display.print("Conectando\n a WiFi...");
	Display.display();
}

//===============================================================================================================================//

void displayErrorWiFi() // en "setup()"
{
	Display.clearDisplay();
	Display.setTextSize(2);
	Display.setCursor(34, 7);
	Display.print("No se\n encuentra");
  	Display.setCursor(20, 40);
  	Display.print("red WiFi");
	Display.display();
}

//===============================================================================================================================//

void displayConetadoA(String ssid_conectada) // en "conectarWiFiCon()"
{
	Display.clearDisplay();
	Display.setTextSize(2); // en grande:
	Display.setCursor(11, 5);
	Display.print("Conectado a la red:\n");
	Display.setTextSize(1);				 // en chiquito:
	Display.setCursor(11, 44);
	Display.print(ssid_conectada); // nombre de la red
	Display.display();
}

//===============================================================================================================================//

void actualizarDisplay() // en "loop()"
{
	// sólo actualizamos los números del display cada cierto tiempo (porque si no los números parpadean)
	if (millis() - ultima_vez_display_actualizo >= DELAY_ACTUALIZACION_DISPLAY)
	{
		ultima_vez_display_actualizo = millis();

		switch (DatoDelDisplay)
		{
		case DisplayDato::Temperatura:
			displayTemperatura();
			break;

		case DisplayDato::HumedadAire:
			displayHumedadAire();
			break;

		case DisplayDato::HumedadSuelo:
			displayHumedadSuelo();
			break;
		}
	}
}

//===============================================================================================================================//

void displayHumedadAire() // en "actualizarDisplay()"
{
	// limpiar display
	Display.clearDisplay();

	// mostrar humedad aire exterior
	Display.setTextSize(1);
	Display.setCursor(0, 0);
	Display.print("Humedad aire exterior");
	Display.setTextSize(2);
	Display.setCursor(0, 11);
	Display.print(String(humedad_aire_exterior) + " %");

	// mostrar humedad aire interior
	Display.setTextSize(1);
	Display.setCursor(0, 33);
	Display.print("Humedad aire interior");
	Display.setTextSize(2);
	Display.setCursor(0, 44);
	Display.print(String(humedad_aire_interior_promedio) + " %");

	Display.display();
}

//===============================================================================================================================//

void displayHumedadSuelo() // en "actualizarDisplay()"
{
	// limpiar display
	Display.clearDisplay();

	// mostrar humedad suelo exterior (para varios sensores poner números en su lugar)
	Display.setTextSize(1);
	Display.setCursor(0, 0);
	Display.print("Humedad del suelo ext");
	Display.setTextSize(2);
	Display.setCursor(0, 11);
	Display.print(String(humedad_suelo_exterior) + " %");

	// mostrar humedad suelo interior (para varios sensores poner números en su lugar)
	Display.setTextSize(1);
	Display.setCursor(0, 33);
	Display.print("Humedad del suelo int");
	Display.setTextSize(2);
	Display.setCursor(0, 44);
	Display.print(String(humedad_suelo_interior) + " %");

	Display.display();
}

//===============================================================================================================================//

void displayTemperatura() // en "actualizarDisplay()"
{
	// limpiar display
	Display.clearDisplay();

	// mostrar temperatura aire exterior
	Display.setTextSize(1);
	Display.setCursor(0, 0);
	Display.print("Temperatura exterior");
	Display.setTextSize(2);
	Display.setCursor(0, 11);
	Display.print(temp_exterior);
	Display.print(" ");
	Display.setTextSize(1);
	Display.cp437(true);
	Display.write(167);
	Display.setTextSize(2);
	Display.print("C");

	// mostrar temperatura aire interior
	Display.setTextSize(1);
	Display.setCursor(0, 33);
	Display.print("Temperatura interior");
	Display.setTextSize(2);
	Display.setCursor(0, 44);
	Display.print(temp_interior_promedio);
	Display.print(" ");
	Display.setTextSize(1);
	Display.cp437(true);
	Display.write(167);
	Display.setTextSize(2);
	Display.print("C");

	Display.display();
}

//===============================================================================================================================//

void displayError()
{
	Display.setTextSize(1);
	Display.setCursor(7, 7);
    Display.println("Error al iniciar el");
    Display.setCursor(5, 17);
    Display.print("controlador. Motivo:");
}

//===============================================================================================================================//

void displayNoSD()
{
	Display.clearDisplay();
	displayError();
	Display.setCursor(10, 39);
	Display.print("Tarjeta SD ausente");
	Display.display();
}

//===============================================================================================================================//

void displayErrorSD()
{
	Display.clearDisplay();
	displayError();
	Display.setCursor(7, 39);
	Display.print("Error en tarjeta SD");
	Display.display();
}

//===============================================================================================================================//

void displayLogo()
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
#pragma once

#include "Declaraciones.h"

void inicializarDisplay() // en "setup()"
{
	if (!Display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
	{
		imprimirln("Fallo en el display. Resetear el equipo.");
		while (1)
			;
	}
	delay(2000);
	Display.clearDisplay();
	Display.setTextColor(WHITE);
}

//==================================================================================================================//

void cambiarDatoDisplay()
{
	switch (DatoDelDisplay)
	{
	case Temperatura:
		DatoDelDisplay = HumedadAire;
		break;

	case HumedadAire:
		DatoDelDisplay = HumedadSuelo;
		break;

	case HumedadSuelo:
		DatoDelDisplay = Temperatura;
		break;
	}
}

//==================================================================================================================//

void displayConectandoWIFI() // en "setup()"
{
	Display.clearDisplay();
	Display.setTextSize(2);
	Display.setCursor(0, 0);
	Display.print("Conectando\na WIFI...");
	Display.display();
}

//==================================================================================================================//

void displayErrorWIFI() // en "setup()"
{
	Display.clearDisplay();
	Display.setTextSize(2);
	Display.setCursor(0, 0);
	Display.print("No se\nencuentra\nred WIFI.");
	Display.display();
}

//==================================================================================================================//

void displayConexionWIFI(String Amensaje_conectado_a, String Assid_conectada) // en "conectarWIFICon()"
{
	Display.clearDisplay();
	Display.setTextSize(2); // en grande:
	Display.setCursor(0, 0);
	Display.print(Amensaje_conectado_a); // conectado a la red
	Display.setTextSize(1);				 // en chiquito:
	Display.setCursor(0, 40);
	Display.print(Assid_conectada); // nombre de la red
	Display.display();
}

//==================================================================================================================//

void actualizarDisplay() // en "loop()"
{
	// sólo actualizamos los números del display cada cierto tiempo (porque si no los números parpadean)
	if (millis() - ultima_vez_display_actualizo >= DELAY_ACTUALIZACION_DISPLAY)
	{
		ultima_vez_display_actualizo = millis();

		switch (DatoDelDisplay)
		{
		case Temperatura:
			displayTemperatura();
			break;

		case HumedadAire:
			displayHumedadAire();
			break;

		case HumedadSuelo:
			displayHumedadSuelo();
			break;
		}
	}
}

//==================================================================================================================//

void displayHumedadAire() // en "actualizarDisplay()"
{
	// limpiar display
	Display.clearDisplay();

	// mostrar humedad aire exterior
	Display.setTextSize(1);
	Display.setCursor(0, 0);
	Display.print("Humedad aire exterior");
	Display.setTextSize(2);
	Display.setCursor(0, 10);
	Display.print(String(humedad_aire_exterior) + " %");

	// mostrar humedad aire interior
	Display.setTextSize(1);
	Display.setCursor(0, 35);
	Display.print("Humedad aire interior");
	Display.setTextSize(2);
	Display.setCursor(0, 45);
	Display.print(String(humedad_aire_interior_promedio) + " %");

	Display.display();
}

//==================================================================================================================//

void displayHumedadSuelo() // en "actualizarDisplay()"
{
	// limpiar display
	Display.clearDisplay();

	// mostrar humedad suelo exterior (para varios sensores poner números en su lugar)
	Display.setTextSize(1);
	Display.setCursor(0, 0);
	Display.print("Humedad del suelo ext");
	Display.setTextSize(2);
	Display.setCursor(0, 10);
	Display.print(String(humedad_suelo_exterior) + " %");

	// mostrar humedad suelo interior (para varios sensores poner números en su lugar)
	Display.setTextSize(1);
	Display.setCursor(0, 35);
	Display.print("Humedad del suelo int");
	Display.setTextSize(2);
	Display.setCursor(0, 45);
	Display.print(String(humedad_suelo_interior) + " %");

	Display.display();
}

//==================================================================================================================//

void displayTemperatura() // en "actualizarDisplay()"
{
	// limpiar display
	Display.clearDisplay();

	// mostrar temperatura aire exterior
	Display.setTextSize(1);
	Display.setCursor(0, 0);
	Display.print("Temperatura exterior");
	Display.setTextSize(2);
	Display.setCursor(0, 10);
	Display.print(temp_exterior);
	Display.print(" ");
	Display.setTextSize(1);
	Display.cp437(true);
	Display.write(167);
	Display.setTextSize(2);
	Display.print("C");

	// mostrar temperatura aire interior
	Display.setTextSize(1);
	Display.setCursor(0, 35);
	Display.print("Temperatura interior");
	Display.setTextSize(2);
	Display.setCursor(0, 45);
	Display.print(temp_interior_promedio);
	Display.print(" ");
	Display.setTextSize(1);
	Display.cp437(true);
	Display.write(167);
	Display.setTextSize(2);
	Display.print("C");

	Display.display();
}
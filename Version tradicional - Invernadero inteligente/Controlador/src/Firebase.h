#pragma once

#include "Declaraciones.h"

void LocalFirebase::inicializar()
{
    if (!this->tiene_firebase)
        return;
    if (this->inicializado)
        return;
    inicializado = true;

    config.api_key = this->api_key;
    auth.user.email = this->email;
    auth.user.password = this->pass;
    config.database_url = this->url;

    Firebase.reconnectWiFi(true);
    data.setResponseSize(4096);

    // Assign the callback function for the long running token generation task
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
    config.max_token_generation_retry = 5;
    Firebase.begin(&config, &auth);
    
    while (auth.token.uid == "")
        delay(500);

    Firebase.RTDB.beginStream(&stream, PATH_ESCUCHAR);
    Firebase.RTDB.setStreamCallback(&stream, appInput, appInputTimeout);
}

//===============================================================================================================================//

void appInputTimeout(bool comandoTimeout) { return; } // TODO: error checking
void appInput(FirebaseStream data)
{
    int     inumero = data.intData();
    float   fnumero = data.floatData();

    char nodo[10];
    data.dataPath().substring(1).toCharArray(nodo, 10);
    if      (strcmp(nodo, "riego")  == 0)
        LCFB.comandoRiego(inumero);
    else if (strcmp(nodo, "calefa") == 0)
        LCFB.comandoCalefa(inumero);
    else if (strcmp(nodo, "vent")   == 0)
        LCFB.comandoVent(inumero);

    int numero = atoi(nodo);
	switch (numero)
    {
    case LCEE.MODOS_SALIDAS:
        LCFB.cambiarModosSalidas(inumero);
        break;
    
    case LCEE.ALARMA_ACTIVADA:
        LCFB.cambiarAlarmaActivada(inumero);
        break;

    case LCEE.LAPSO_ALARMA_MIN:
        LCFB.cambiarLapsoAlarma(inumero);
        break;
    
    case LCEE.TEMP_MAXIMA_ALARMA:
        LCFB.cambiarTMaxAlarma(fnumero);
        break;
    
	case LCEE.TEMP_MINIMA_ALARMA:
        LCFB.cambiarTMinAlarma(fnumero);
        break;
        
    case LCEE.HUMEDAD_SUELO_MINIMA:
        LCFB.cambiarHumSueloMin(inumero);
        break;
    
	case LCEE.LAPSO_RIEGOS_MIN:
        LCFB.cambiarLapsoRiegos(inumero);
        break;
			
    case LCEE.TIEMPO_BOMBEO_SEG:
        LCFB.cambiarTiempoBombeo(inumero);
        break;
    
    case LCEE.TIEMPO_ESPERA_MIN:
        LCFB.cambiarTiempoEspera(inumero);
    
    case LCEE.TEMP_MINIMA_CALEFA:
        LCFB.cambiarTMinCalefa(fnumero);
        break;
    
	case LCEE.LAPSO_CALEFAS_MIN:
        LCFB.cambiarLapsoCalefas(inumero);
        break;

	case LCEE.TIEMPO_ENCENDIDO_CALEFA_MIN:
        LCFB.cambiarTiempoEncendidoCalefa(inumero);
    
	case LCEE.TEMP_MAXIMA_VENTILACION:
        LCFB.cambiarTMaxVent(fnumero);
        break;
			
    case LCEE.LAPSO_VENTILACIONES_MIN:
        LCFB.cambiarLapsoVent(inumero);
        break;

	case LCEE.TIEMPO_APERTURA_VENT_MIN:
        LCFB.cambiarTiempoAperturaVent(inumero);
        break;
    
	case LCEE.TIEMPO_MARCHA_VENT_SEG:
        LCFB.cambiarTiempoMarchaVent(inumero);

    default:
        return; // para no responder ok
    }
    LCFB.responderOk();
}

inline void LocalFirebase::comandoRiego(uint8_t valor)
{
	if (valor)
		Riego.encender(millis());
	else
		Riego.apagar();
}
inline void LocalFirebase::comandoCalefa(uint8_t valor)
{
    if (valor)
        Calefa.encender(millis());
    else
        Calefa.apagar();
}
inline void LocalFirebase::comandoVent(uint8_t valor)
{
    if (valor)
        Ventilacion.abrir(millis());
    else
        Ventilacion.cerrar();
}
inline void LocalFirebase::cambiarModosSalidas(uint8_t valor)
{
    LCEE.escribir(LCEE.direccion[LCEE.MODOS_SALIDAS], valor);
    LCCT.configurarModosSalidas();
}
inline void LocalFirebase::cambiarAlarmaActivada(bool valor)    { LCEE.escribir(LCEE.direccion[LCEE.ALARMA_ACTIVADA], valor); }
inline void LocalFirebase::cambiarLapsoAlarma(uint16_t valor)   { LCEE.escribir(LCEE.direccion[LCEE.LAPSO_ALARMA_MIN], valor); }
inline void LocalFirebase::cambiarTMaxAlarma(float valor)       { LCEE.escribir(LCEE.direccion[LCEE.TEMP_MAXIMA_ALARMA], valor); }
inline void LocalFirebase::cambiarTMinAlarma(float valor)       { LCEE.escribir(LCEE.direccion[LCEE.TEMP_MINIMA_ALARMA], valor); }
inline void LocalFirebase::cambiarHumSueloMin(uint8_t valor)    { LCEE.escribir(LCEE.direccion[LCEE.HUMEDAD_SUELO_MINIMA], valor);}
inline void LocalFirebase::cambiarLapsoRiegos(uint16_t valor)   { LCEE.escribir(LCEE.direccion[LCEE.LAPSO_RIEGOS_MIN], valor);}
inline void LocalFirebase::cambiarTiempoBombeo(uint16_t valor)  { LCEE.escribir(LCEE.direccion[LCEE.TIEMPO_BOMBEO_SEG], valor);}
inline void LocalFirebase::cambiarTiempoEspera(uint16_t valor)  { LCEE.escribir(LCEE.direccion[LCEE.TIEMPO_ESPERA_MIN], valor);}
inline void LocalFirebase::cambiarTMinCalefa(float valor)       { LCEE.escribir(LCEE.direccion[LCEE.TEMP_MINIMA_CALEFA], valor);}
inline void LocalFirebase::cambiarLapsoCalefas(uint16_t valor)  { LCEE.escribir(LCEE.direccion[LCEE.LAPSO_CALEFAS_MIN], valor);}
inline void LocalFirebase::cambiarTiempoEncendidoCalefa(uint16_t valor) { LCEE.escribir(LCEE.direccion[LCEE.TIEMPO_ENCENDIDO_CALEFA_MIN], valor);}
inline void LocalFirebase::cambiarTMaxVent(float valor)         { LCEE.escribir(LCEE.direccion[LCEE.TEMP_MAXIMA_VENTILACION], valor);}
inline void LocalFirebase::cambiarLapsoVent(uint16_t valor)     { LCEE.escribir(LCEE.direccion[LCEE.LAPSO_VENTILACIONES_MIN], valor);}
inline void LocalFirebase::cambiarTiempoAperturaVent(uint16_t valor)    { LCEE.escribir(LCEE.direccion[LCEE.TIEMPO_APERTURA_VENT_MIN], valor);}
inline void LocalFirebase::cambiarTiempoMarchaVent(uint8_t valor)       { LCEE.escribir(LCEE.direccion[LCEE.TIEMPO_MARCHA_VENT_SEG], valor);}
//===============================================================================================================================//

void LocalFirebase::responderOk()
{
	json.set("rta", "ok");
    Firebase.RTDB.setJSON(&data, PATH_ESCRITURA, &json);
}

//===============================================================================================================================//

void LocalFirebase::correr()
{
	if (!this->tiene_firebase)
        return;
	if ( Firebase.isTokenExpired() )
        Firebase.refreshToken(&config);
}

//===============================================================================================================================//

void LocalFirebase::datalog(File sd)
{
    if (this->tiene_firebase)
	{
        char path[CARACTERES_PATH_LECTURAS];
        sprintf(path, "%s%d", this->PATH_LECTURAS, obtenerTiempoUnix());
        this->i_datalog = 0;
    }

    LCSD.escribirFBySDabierta(sd, millis()/1000, true);
	LCSD.escribirFBySDabierta(sd, AhtInteriorHigh.temperatura,	true);
	LCSD.escribirFBySDabierta(sd, AhtInteriorMid.temperatura,	true);
	LCSD.escribirFBySDabierta(sd, AhtInteriorLow.temperatura,	true);
	LCSD.escribirFBySDabierta(sd, AhtGeotermico.temperatura,	true);
	LCSD.escribirFBySDabierta(sd, humedad_int_high,	true);
	LCSD.escribirFBySDabierta(sd, humedad_int_mid,		true);
	LCSD.escribirFBySDabierta(sd, humedad_int_low,		true);
	LCSD.escribirFBySDabierta(sd, humedad_suelo1, true);
	LCSD.escribirFBySDabierta(sd, humedad_suelo2, true);
	LCSD.escribirFBySDabierta(sd, Riego.encendida,		true);
	LCSD.escribirFBySDabierta(sd, Calefa.encendida,	true);
	LCSD.escribirFBySDabierta(sd, Ventilacion.abierta,	false);
	sd.println();
	
    if (this->tiene_firebase  &&  Firebase.ready())
	    Firebase.RTDB.setJSON(&data, this->PATH_LECTURAS, &json);
}
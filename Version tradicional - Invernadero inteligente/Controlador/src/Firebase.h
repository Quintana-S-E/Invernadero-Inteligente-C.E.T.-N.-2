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
    data.keepAlive(5, 5, 1);

    // Assign the callback function for the long running token generation task
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
    config.max_token_generation_retry = 5;
    Firebase.begin(&config, &auth);
    
    while (auth.token.uid == "")
        delay(500);

    this->enviarParametros();
    Firebase.RTDB.beginStream(&stream, PATH_ESCUCHAR);
    Firebase.RTDB.setStreamCallback(&stream, appInput, appInputTimeout);
}

//===============================================================================================================================//

void appInputTimeout(bool comandoTimeout) { imprimirln("Firebase: timeout"); } // TODO: error checking
void appInput(FirebaseStream data)
{
    int     inumero = data.intData();
    float   fnumero = data.floatData();
    char nodo[LCFB.CARACTERES_NODO_ESCUCHAR];
    data.dataPath().substring(1).toCharArray(nodo, LCFB.CARACTERES_NODO_ESCUCHAR);

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
        break;
    case LCEE.TEMP_MINIMA_CALEFA:
        LCFB.cambiarTMinCalefa(fnumero);
        break;
	case LCEE.LAPSO_CALEFAS_MIN:
        LCFB.cambiarLapsoCalefas(inumero);
        break;
	case LCEE.TIEMPO_ENCENDIDO_CALEFA_MIN:
        LCFB.cambiarTiempoEncendidoCalefa(inumero);
        break;
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
        break;
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

inline bool LocalFirebase::enviarJson(FirebaseData* data, const char* path, FirebaseJson* json)
{
    if (Firebase.ready())
        Firebase.RTDB.setJSON(data, path, json);
}

//===============================================================================================================================//

void LocalFirebase::responderOk()
{
    FirebaseJson json;
	json.set(this->NOMBRE_NODO_RTA, "ok");
    this->enviarJson(&data, this->PATH_ESCRITURA, &json);
    imprimirln("Firebase: rta ok");
}

//===============================================================================================================================//

void LocalFirebase::enviarParametros()
{
    FirebaseJson json;
    char nodo[this->CARACTERES_NODO_ESCUCHAR];
    int i;
    for (i = 1; i < LCEE.CANT_VARIABLES; ++i)
    {
        itoa(i, nodo, 10);
        switch (LCEE.LONGITUD_DATO[i])
        {
        case 1:
            json.set( nodo, LCEE.leer<uint8_t>(LCEE.direccion[i]) );
            break;
        case 2:
            json.set( nodo, LCEE.leer<uint16_t>(LCEE.direccion[i]) );
            break;
        case 4:
            json.set( nodo, LCEE.leer<float>(LCEE.direccion[i]) );
            break;
        }
    }
    json.set(itoa(i, nodo, 10), Riego.encendida);
    ++i;
    json.set(itoa(i, nodo, 10), Calefa.encendida);
    ++i;
    json.set(itoa(i, nodo, 10), Ventilacion.abierta);

    this->enviarJson(&data, this->PATH_ESCUCHAR, &json);
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

void LocalFirebase::enviarAlarmaCaliente()
{
    FirebaseJson json;
    json.set(this->NOMBRE_NODO_ALARMA_ALTA, true);
    this->enviarJson(&data, this->PATH_ESCRITURA, &json);
}

void LocalFirebase::enviarAlarmaFrio()
{
    FirebaseJson json;
    json.set(this->NOMBRE_NODO_ALARMA_BAJA, true);
    this->enviarJson(&data, this->PATH_ESCRITURA, &json);
}

//===============================================================================================================================//

void LocalFirebase::datalog(File sd)
{
    this->i_datalog = 0;
    unsigned long tiempo_unix;
    if (this->tiene_firebase)
        tiempo_unix = obtenerTiempoUnix(); // en un if pq tiene 5 s de timeout

    char path[CARACTERES_PATH_LECTURAS];
    sprintf(path, "%s%u%c", this->PATH_LECTURAS, tiempo_unix, '/');
    FirebaseJson json;

    LCSD.escribirFBySDabierta(sd, millis()/1000, true, &json);
	LCSD.escribirFBySDabierta(sd, AhtInteriorHigh.temperatura,	true, &json);
	LCSD.escribirFBySDabierta(sd, AhtInteriorMid.temperatura,	true, &json);
	LCSD.escribirFBySDabierta(sd, AhtInteriorLow.temperatura,	true, &json);
	LCSD.escribirFBySDabierta(sd, AhtGeotermico.temperatura,	true, &json);
	LCSD.escribirFBySDabierta(sd, humedad_int_high,	true, &json);
	LCSD.escribirFBySDabierta(sd, humedad_int_mid,		true, &json);
	LCSD.escribirFBySDabierta(sd, humedad_int_low,		true, &json);
	LCSD.escribirFBySDabierta(sd, humedad_suelo1, true, &json);
	LCSD.escribirFBySDabierta(sd, humedad_suelo2, true, &json);
	LCSD.escribirFBySDabierta(sd, Riego.encendida,		true, &json);
	LCSD.escribirFBySDabierta(sd, Calefa.encendida,	true, &json);
	LCSD.escribirFBySDabierta(sd, Ventilacion.abierta,	false, &json);
	sd.println();

	
    if (this->tiene_firebase)
    {
        json.set("unix", tiempo_unix);
	    this->enviarJson(&data, path, &json);
    }
}
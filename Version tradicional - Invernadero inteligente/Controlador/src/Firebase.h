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
    int nodo_numero;
    data.dataPath().substring(1).toCharArray(nodo, LCFB.CARACTERES_NODO_ESCUCHAR);

    if      (strcmp(nodo, LCFB.NOMBRE_NODO_COMAPP_RIEGO)  == 0)
    {
        LCFB.comandoSalidaOnOff(Riego, inumero);
        goto responderOk;
    }
    else if (strcmp(nodo, LCFB.NOMBRE_NODO_COMAPP_CALEFA) == 0)
    {
        LCFB.comandoSalidaOnOff(Calefa, inumero);
        goto responderOk;
    }
    else if (strcmp(nodo, LCFB.NOMBRE_NODO_COMAPP_VENT)   == 0)
    {
        LCFB.comandoVent(inumero);
        goto responderOk;
    }

    nodo_numero = atoi(nodo);
	switch (nodo_numero)
    {
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
    case LCEE.MODO_RIEGO:
        LCFB.cambiarModoRiego(inumero);
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
    case LCEE.MODO_CALEFA:
        LCFB.cambiarModoCalefa(inumero);
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
    case LCEE.MODO_VENT:
        LCFB.cambiarModoVent(inumero);
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
responderOk:
    LCFB.responderOk();
}

//===============================================================================================================================//

inline void LocalFirebase::comandoSalidaOnOff(SalidaOnOff Salida, uint8_t valor)
{
    if (valor && !Salida.encendida)
        Salida.encender(millis());
    else if (!valor && Salida.encendida)
        Salida.apagar();
}

inline void LocalFirebase::comandoVent(uint8_t valor)
{
    if (valor && !Ventilacion.abierta)
        Ventilacion.abrir(millis());
    else if (!valor && Ventilacion.abierta)
        Ventilacion.cerrar();
}
inline void LocalFirebase::cambiarAlarmaActivada(bool valor)
{ LCEE.alarma_activada = valor;             LCEE.escribir(LCEE.direccion[LCEE.ALARMA_ACTIVADA], valor);}
inline void LocalFirebase::cambiarLapsoAlarma(uint16_t valor)
{ LCEE.lapso_alarma_min = valor;            LCEE.escribir(LCEE.direccion[LCEE.LAPSO_ALARMA_MIN], valor); }
inline void LocalFirebase::cambiarTMaxAlarma(float valor)
{ LCEE.temp_maxima_alarma = valor;          LCEE.escribir(LCEE.direccion[LCEE.TEMP_MAXIMA_ALARMA], valor); }
inline void LocalFirebase::cambiarTMinAlarma(float valor)
{ LCEE.temp_minima_alarma = valor;          LCEE.escribir(LCEE.direccion[LCEE.TEMP_MINIMA_ALARMA], valor); }
inline void LocalFirebase::cambiarModoRiego(uint8_t valor)
{
    LCEE.modo_riego = valor;
    LCEE.escribir(LCEE.direccion[LCEE.MODO_RIEGO], valor);
    LCCT.configurarModosSalidas();    
}
inline void LocalFirebase::cambiarHumSueloMin(uint8_t valor)
{ LCEE.humedad_suelo_minima = valor;        LCEE.escribir(LCEE.direccion[LCEE.HUMEDAD_SUELO_MINIMA], valor);}
inline void LocalFirebase::cambiarLapsoRiegos(uint16_t valor)
{ LCEE.lapso_riegos_min = valor;            LCEE.escribir(LCEE.direccion[LCEE.LAPSO_RIEGOS_MIN], valor);}
inline void LocalFirebase::cambiarTiempoBombeo(uint16_t valor)
{ LCEE.tiempo_bombeo_seg = valor;           LCEE.escribir(LCEE.direccion[LCEE.TIEMPO_BOMBEO_SEG], valor);}
inline void LocalFirebase::cambiarTiempoEspera(uint16_t valor)
{ LCEE.tiempo_espera_min = valor;           LCEE.escribir(LCEE.direccion[LCEE.TIEMPO_ESPERA_MIN], valor);}
inline void LocalFirebase::cambiarModoCalefa(uint8_t valor)
{
    LCEE.modo_calefa = valor;
    LCEE.escribir(LCEE.direccion[LCEE.MODO_CALEFA], valor);
    LCCT.configurarModosSalidas();    
}
inline void LocalFirebase::cambiarTMinCalefa(float valor)
{ LCEE.temp_minima_calefa = valor;          LCEE.escribir(LCEE.direccion[LCEE.TEMP_MINIMA_CALEFA], valor);}
inline void LocalFirebase::cambiarLapsoCalefas(uint16_t valor)
{ LCEE.lapso_calefas_min = valor;           LCEE.escribir(LCEE.direccion[LCEE.LAPSO_CALEFAS_MIN], valor);}
inline void LocalFirebase::cambiarTiempoEncendidoCalefa(uint16_t valor)
{ LCEE.tiempo_encendido_calefa_min = valor; LCEE.escribir(LCEE.direccion[LCEE.TIEMPO_ENCENDIDO_CALEFA_MIN], valor);}
inline void LocalFirebase::cambiarModoVent(uint8_t valor)
{
    LCEE.modo_vent = valor;
    LCEE.escribir(LCEE.direccion[LCEE.MODO_VENT], valor);
    LCCT.configurarModosSalidas();    
}
inline void LocalFirebase::cambiarTMaxVent(float valor)
{ LCEE.temp_maxima_ventilacion = valor;     LCEE.escribir(LCEE.direccion[LCEE.TEMP_MAXIMA_VENTILACION], valor);}
inline void LocalFirebase::cambiarLapsoVent(uint16_t valor)
{ LCEE.lapso_ventilaciones_min = valor;     LCEE.escribir(LCEE.direccion[LCEE.LAPSO_VENTILACIONES_MIN], valor);}
inline void LocalFirebase::cambiarTiempoAperturaVent(uint16_t valor)
{ LCEE.tiempo_apertura_vent_min = valor;    LCEE.escribir(LCEE.direccion[LCEE.TIEMPO_APERTURA_VENT_MIN], valor);}
inline void LocalFirebase::cambiarTiempoMarchaVent(uint8_t valor)
{ LCEE.tiempo_marcha_vent_seg = valor;      LCEE.escribir(LCEE.direccion[LCEE.TIEMPO_MARCHA_VENT_SEG], valor);}

//===============================================================================================================================//

inline bool LocalFirebase::enviarJson(FirebaseData* data, const char* path, FirebaseJson* json)
{
    if (Firebase.ready())
        return Firebase.RTDB.setJSON(data, path, json);
    return false;
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
        imprimir("Parametros. Subnodo: "); imprimir(nodo); imprimir(": ");
        switch (LCEE.LONGITUD_DATO[i])
        {
        case 1:
            json.set( nodo, LCEE.leer<uint8_t>(LCEE.direccion[i]) );
            imprimirln(LCEE.leer<uint8_t>(LCEE.direccion[i]));
            break;
        case 2:
            json.set( nodo, LCEE.leer<uint16_t>(LCEE.direccion[i]) );
            imprimirln(LCEE.leer<uint16_t>(LCEE.direccion[i]));
            break;
        case 4:
            json.set( nodo, LCEE.leer<float>(LCEE.direccion[i]) );
            imprimirln(LCEE.leer<float>(LCEE.direccion[i]));
            break;
        }
    }
    strcpy(nodo, this->NOMBRE_NODO_COMAPP_RIEGO);
    imprimir("Parametros. Subnodo: "); imprimir(nodo); imprimir(": "); imprimirln(Riego.encendida);
    json.set<char*, uint8_t>(nodo, Riego.encendida);
    strcpy(nodo, this->NOMBRE_NODO_COMAPP_CALEFA);
    imprimir("Parametros. Subnodo: "); imprimir(nodo); imprimir(": "); imprimirln(Calefa.encendida);
    json.set<char*, uint8_t>(nodo, Calefa.encendida);
    strcpy(nodo, this->NOMBRE_NODO_COMAPP_VENT);
    imprimir("Parametros. Subnodo: "); imprimir(nodo); imprimir(": "); imprimirln(Ventilacion.abierta);
    json.set<char*, uint8_t>(nodo, Ventilacion.abierta);

    this->enviarJson(&data, this->PATH_ESCUCHAR, &json);
    this->setAlarmasFalse();
}

inline void LocalFirebase::setAlarmasFalse()
{
    FirebaseJson json;
    json.set(this->NOMBRE_NODO_ALARMA_ALTA, false);
    json.set(this->NOMBRE_NODO_ALARMA_BAJA, false);
    this->enviarJson(&data, this->PATH_ESCRITURA, &json);
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

void LocalFirebase::datalog(const char* path_sd)
{
    this->i_datalog = 0;
    unsigned long tiempo_unix;
    if (this->tiene_firebase)
        tiempo_unix = obtenerTiempoUnix(); // en un if pq tiene 5 s de timeout

    char path[CARACTERES_PATH_LECTURAS];
    sprintf(path, "%s%u%c", this->PATH_LECTURAS, tiempo_unix, '/');
    FirebaseJson json;

    String string;
    imprimir("escribiendo SD: ");
    LCSD.escribirFBySD(path_sd, string, true, millis()/1000, &json);
	LCSD.escribirFBySD(path_sd, string, true, AhtInteriorHigh.temperatura, &json);
	LCSD.escribirFBySD(path_sd, string, true, AhtInteriorMid.temperatura, &json);
	LCSD.escribirFBySD(path_sd, string, true, AhtInteriorLow.temperatura, &json);
	LCSD.escribirFBySD(path_sd, string, true, AhtGeotermico.temperatura, &json);
	LCSD.escribirFBySD(path_sd, string, true, humedad_int_high, &json);
	LCSD.escribirFBySD(path_sd, string, true, humedad_int_mid, &json);
	LCSD.escribirFBySD(path_sd, string, true, humedad_int_low, &json);
	LCSD.escribirFBySD(path_sd, string, true, humedad_suelo1, &json);
	LCSD.escribirFBySD(path_sd, string, true, humedad_suelo2, &json);
	LCSD.escribirFBySD(path_sd, string, true, static_cast<uint8_t>(Riego.modo), &json);
	LCSD.escribirFBySD(path_sd, string, true, Riego.encendida, &json);
	LCSD.escribirFBySD(path_sd, string, true, static_cast<uint8_t>(Calefa.modo), &json);
	LCSD.escribirFBySD(path_sd, string, true, Calefa.encendida, &json);
	LCSD.escribirFBySD(path_sd, string, true, static_cast<uint8_t>(Ventilacion.modo), &json);
	LCSD.escribirFBySD(path_sd, string, false, Ventilacion.abierta, &json);
	
    if (this->tiene_firebase)
    {
        json.set("unix", tiempo_unix);
	    this->enviarJson(&data, path, &json);
    }
}
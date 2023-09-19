#pragma once

#include "Declaraciones.h"

void LocalFirebase::inicializar()
{
    if (this->inicializado)
        return;
    inicializado = true;
    if (!this->tiene_firebase)
        return;

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


/*
HASTA ACÁ LLEGUÉ
*/
    // Update database path
    databasePath = "/UsersData/" + uid + "/readings";
    listenerPath = "/UsersData/" + uid + "/commands/digital/";




    // Streaming (whenever data changes on a path)
    // Begin stream on a database path --> board1/outputs/digital
    if (!Firebase.RTDB.beginStream(&stream, listenerPath.c_str()))
    Serial.printf("stream begin error, %s\n\n", stream.errorReason().c_str());

    // Assign a calback function to run when it detects changes on the database
    Firebase.RTDB.setStreamCallback(&stream, streamCallback, streamTimeoutCallback);
}
// Manejar comandos, (ver pruebas/Firebase demo), enviar datos hacia realtime database,
// reconectar el cliente, etc
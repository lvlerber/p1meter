#pragma once

#include "Arduino.h"
#include "stdlib_noniso.h"
// #include <functional>


    #include "AsyncTCP.h"
    #include "ESPAsyncWebServer.h"
    #include "WiFi.h"




typedef std::function<void(uint8_t *data, size_t len)> RecvMsgHandler;

// Uncomment to enable webserial debug mode
#define WEBSERVER_DEBUG 1

class WebServices{

public:
    void begin(AsyncWebServer *server);



private:
    AsyncWebServer *_server;
    AsyncWebSocket *_ws;
    RecvMsgHandler _RecvFunc = NULL;
    
    #if defined(WEBSERVER_DEBUG)
        void DEBUG_WEB_SERIAL(const char* message);
    #endif
};


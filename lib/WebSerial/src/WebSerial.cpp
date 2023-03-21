#include "WebSerial.h"
#define DEBUG

void WebSerialClass::begin(AsyncWebServer *server, const char *url)
{

    _server = server;
    _ws = new AsyncWebSocket("/webserialws");

    _server->on(url, HTTP_GET, [](AsyncWebServerRequest *request)
                {
        // Send Webpage
            const String html="<!DOCTYPE html><html><head><script type = 'text/javascript'>const ws = new WebSocket('ws://'+location.hostname+'/webserialws');\
ws.onopen = function() {document.getElementById('div1').insertAdjacentHTML('beforeend','Connection opened<br>');};ws.onclose = function()  {document.getElementById('div1').insertAdjacentHTML('beforeend','Connection closed<br>');};\
ws.onmessage = function(event) {\
	for(var i=0;i<event.data.length;i++){var c=event.data.charAt(i);\
    if (c!='\\n') {const p = document.createTextNode(c); document.getElementById('div1').appendChild(p);}\
    else {const b = document.createElement('br');document.getElementById('div1').appendChild(b);}}\
document.getElementById('buttonsend').onclick = function(){	ws.send(document.getElementById('cmd').value); \
	document.getElementById('cmd').value='';};};</script><div> <input id='cmd' type='text' placeholder='type message and press the Send Button' style='width:90%'/>\
<input id='buttonsend' type='button' value='Send' /></div></head><body><div id='div1' /></body></html>";
//        Serial.printf("%s\n",html.c_str());
        // AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", WEBSERIAL_HTML, WEBSERIAL_HTML_SIZE);
 //       AsyncWebServerResponse *response = request->beginResponse(200, "text/html",html);
        // Serial.printf(" before call pointer %p length %i ",&html,html.length());
        AsyncWebServerResponse *response = new AsyncBasicResponse(200, "text/html",html);
        request->send(response); });

    _ws->onEvent([&](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) -> void
                 {
        if(type == WS_EVT_CONNECT){
#if defined(DEBUG)
                DEBUG_WEB_SERIAL("Client connection received");
#endif
        } else if(type == WS_EVT_DISCONNECT){
#if defined(DEBUG)
                DEBUG_WEB_SERIAL("Client disconnected");
#endif
        } else if(type == WS_EVT_DATA){
#if defined(DEBUG)
                DEBUG_WEB_SERIAL("Received Websocket Data");
#endif
            if(_RecvFunc != NULL){
                _RecvFunc(data, len);
            }
        } });

    _server->addHandler(_ws);

#if defined(WEBSERIAL_DEBUG)
    DEBUG_WEB_SERIAL("Attached AsyncWebServer along with Websockets");
#endif
}

void WebSerialClass::msgCallback(RecvMsgHandler _recv)
{
    _RecvFunc = _recv;
}

// Print
void WebSerialClass::print(String m)
{
    _ws->textAll(m);
}

void WebSerialClass::print(const char *m)
{
    _ws->textAll(m);
}

void WebSerialClass::print(char *m)
{
    _ws->textAll(m);
}

void WebSerialClass::print(int m)
{
    _ws->textAll(String(m));
}

void WebSerialClass::print(uint8_t m)
{
    _ws->textAll(String(m));
}

void WebSerialClass::print(uint16_t m)
{
    _ws->textAll(String(m));
}

void WebSerialClass::print(uint32_t m)
{
    _ws->textAll(String(m));
}

void WebSerialClass::print(double m)
{
    _ws->textAll(String(m));
}

void WebSerialClass::print(float m)
{
    _ws->textAll(String(m));
}

void WebSerialClass::print(const uint8_t *buffer, size_t size)
{
    _ws->textAll((char *)buffer, size);
}

// Print with New Line

void WebSerialClass::println(String m)
{
    _ws->textAll(m + "\n");
}

void WebSerialClass::println(const char *m)
{
    _ws->textAll(String(m) + "\n");
}

void WebSerialClass::println(char *m)
{
    _ws->textAll(String(m) + "\n");
}

void WebSerialClass::println(int m)
{
    _ws->textAll(String(m) + "\n");
}

void WebSerialClass::println(uint8_t m)
{
    _ws->textAll(String(m) + "\n");
}

void WebSerialClass::println(uint16_t m)
{
    _ws->textAll(String(m) + "\n");
}

void WebSerialClass::println(uint32_t m)
{
    _ws->textAll(String(m) + "\n");
}

void WebSerialClass::println(float m)
{
    _ws->textAll(String(m) + "\n");
}

void WebSerialClass::println(double m)
{
    _ws->textAll(String(m) + "\n");
}

#if defined(WEBSERIAL_DEBUG)
void WebSerialClass::DEBUG_WEB_SERIAL(const char *message)
{
    Serial.print("[WebSerial] ");
    Serial.println(message);
}
#endif

WebSerialClass WebSerial;
int multi_writefn(void *cookie, const char *data, int n)
{
    // Dit is de functie die opgeroepen wordt voor writes naar stdout en stderr
    //
    if (WebSerialActive)
    {
        /* redirect the bytes somewhere; writing to Serial just for an example */
        //   Serial.print("got ");
        //   Serial.print(n);
        //   Serial.print(" bytes:");
        //   Serial.print("    > ");
        char cmdstr[n];
        memcpy(cmdstr, data, n);
        cmdstr[n] = 0;

        Serial.print(cmdstr);
        //   WebSerial.println();
    }
    return n;
}
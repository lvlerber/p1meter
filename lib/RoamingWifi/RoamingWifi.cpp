#include "RoamingWifi.h"

#include "Arduino.h"
#include "WiFi.h"

RoamingWiFiClass::RoamingWiFiClass(){};

void RoamingWiFiClass::start(){
        WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("EspMower", NULL);
    WiFi.disconnect();
    delay(100);
    //  WiFi.mode(WIFI_STA);
    WiFi.onEvent(handleWiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
    WiFi.onEvent(handleWiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent(handleWiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
    WiFi.onEvent(handleOtherEvents, SYSTEM_EVENT_MAX);

    connect();
};

void RoamingWiFiClass::connect()
{
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0)
    {
        Serial.println("no networks found");
    }
    else
    {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i)
        {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
            delay(10);
        }
        if (0)
        { //static ip address
            // Set your Static IP address
            IPAddress local_IP(192, 168, 1, 184);
            // Set your Gateway IP address
            IPAddress gateway(192, 168, 1, 1);

            IPAddress subnet(255, 255, 0, 0);
            IPAddress primaryDNS(8, 8, 8, 8);   // optional
            IPAddress secondaryDNS(8, 8, 4, 4); // optional
            // Configures static IP address
            if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
            {
                Serial.println("STA Failed to configure");
            }
        }
        WiFi.begin("Duffelstraat", "SJiADgXg5Z");
        Serial.print("Connecting to WiFi ..");
        while (WiFi.status() != WL_CONNECTED)
        {
            Serial.print('.');
            delay(1000);
        }
        Serial.println(WiFi.localIP());

        Serial.println("Setup done");
    }
}

RoamingWiFiClass RoamingWiFi;

void handleOtherEvents(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("other event triggered");
    Serial.println(event);

}
void handleWiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("Connected to AP successfully!");
}

void handleWiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void handleWiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("Disconnected from WiFi access point");
    Serial.print("WiFi lost connection. Reason: ");
    Serial.println(info.disconnected.reason);
    Serial.println("Trying to Reconnect");
    RoamingWiFi.connect();
}

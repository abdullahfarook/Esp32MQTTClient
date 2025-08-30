#ifndef RECONNECT_HELPER_H
#define RECONNECT_HELPER_H

#include <Arduino.h>

class ReconnectHelper
{
private:
    unsigned long lastReconnectAttempt = 0;
    int reconnectFailures = 0;
    bool isConnected = false;
    unsigned long retryInterval = 1000;
    int maxFailures = 10;
    char *clientName;
    bool allowDelay = true;

public:
    void setClient(char *name, unsigned long intervalMs = 1000, int maxFail = 10,bool allowDelay = true)
    {
        retryInterval = intervalMs;
        maxFailures = maxFail;
        clientName = name;
        allowDelay = allowDelay;

    }

    bool handle(std::function<bool()> isConnectedFn)
    {
        while (!isConnectedFn())
        {
            if (millis() - lastReconnectAttempt > retryInterval)
            {
                isConnected = false;
                lastReconnectAttempt = millis();
                reconnectFailures++;
                Serial.print(".");

                if (reconnectFailures > maxFailures)
                {
                    Serial.printf("[%s] Too many failures, restarting ESP...\n", clientName);
                    ESP.restart();
                }
                if(allowDelay){
                    delay(retryInterval - 100);
                }else{
                    return false;
                }
            }
            else
            {
                if (!isConnected)
                {
                    isConnected = true;
                    reconnectFailures = 0; // reset on success
                    Serial.printf("\n[%s] Reconnected!\n", clientName);
                    return true;
                }
            }
        }
    }
};
#endif
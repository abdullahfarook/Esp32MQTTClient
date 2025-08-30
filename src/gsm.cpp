/*
TTGO T-Call V1.4 (AM-036) SIM800L Basic AT Test
  Uses UART1 at 9600 baud (SIM800L default)
*/

#include <HardwareSerial.h>
#include "gsm.h"

// TTGO T-Call V1.4 pinout
#define MODEM_RST         5
#define MODEM_PWRKEY      4
#define MODEM_POWER_ON    23
#define MODEM_TX          27
#define MODEM_RX          26

HardwareSerial SerialGSM(1);

static bool waitForKeyword(const char *keyword, unsigned long timeoutMs) {
    unsigned long start = millis();
    String buffer;
    while (millis() - start < timeoutMs) {
        while (SerialGSM.available()) {
            char c = (char)SerialGSM.read();
            buffer += c;
            if (buffer.indexOf(keyword) >= 0) {
                return true;
            }
        }
        delay(10);
    }
    Serial.print("Timeout waiting for: ");
    Serial.println(keyword);
    Serial.print("Buffer: ");
    Serial.println(buffer);
    return false;
}

void modemPowerOn() {
    // Power on the SIM800L
    digitalWrite(MODEM_POWER_ON, HIGH);
    delay(100);

    // Pulse PWRKEY to turn on module
    digitalWrite(MODEM_PWRKEY, HIGH);
    delay(100);
    digitalWrite(MODEM_PWRKEY, LOW);
    delay(1000);
    digitalWrite(MODEM_PWRKEY, HIGH);

    // Give time to boot
    delay(5000);
}
void sendAT(const char *cmd) {
    Serial.print(">>> ");
    Serial.println(cmd);
    SerialGSM.println(cmd);
    delay(1000);
    while (SerialGSM.available()) {
        Serial.write(SerialGSM.read());
    }
    Serial.println();
}

void setupGSM() {
    Serial.begin(9600);
    delay(2000);
    Serial.println("Booting TTGO T-Call and SIM800L...");

    // Setup control pins
    pinMode(MODEM_PWRKEY, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);
    pinMode(MODEM_RST, OUTPUT);

    digitalWrite(MODEM_RST, HIGH); // Keep reset inactive

    modemPowerOn();

    // Start UART1 for SIM800L
    SerialGSM.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);
    delay(3000);

    Serial.println("Testing AT commands...");

    sendAT("AT");
    sendAT("AT+CSQ");
    sendAT("AT+CCID");
    sendAT("AT+CREG?");
}

void loopGSM() {
    // Print modem responses
    if (SerialGSM.available()) {
        Serial.write(SerialGSM.read());
    }
    // Forward from USB Serial to modem
    if (Serial.available()) {
        SerialGSM.write(Serial.read());
    }
}

bool sendSms(const String &phoneNumber, const String &text) {
    bool sent = true;

    Serial.println("Preparing to send SMS...");

    SerialGSM.println("AT");
    if (!waitForKeyword("OK", 2000)){
        sent = false;
    }

    // SerialGSM.println("AT+CMGF=1");
    // if (!waitForKeyword("OK", 2000)){
    //     sent = false;
    // }

    // SerialGSM.print("AT+CMGS=\"");
    // SerialGSM.print(phoneNumber);
    // SerialGSM.println("\"");
    // if (!waitForKeyword(">", 5000)) return false;

    // SerialGSM.print(text);
    // SerialGSM.write(26); // CTRL+Z

    // bool sent = waitForKeyword("OK", 60000) || waitForKeyword("+CMGS:", 60000);
    // Serial.println(sent ? "SMS sent successfully" : "Failed to send SMS");
    return sent; 
}


/*
TTGO T-Call V1.4 (AM-036) SIM800L Basic AT Test
  Uses UART1 at 9600 baud (SIM800L default)
*/

#include <HardwareSerial.h>

// TTGO T-Call V1.4 pinout
#define MODEM_RST         5
#define MODEM_PWRKEY      4
#define MODEM_POWER_ON    23
#define MODEM_TX          27
#define MODEM_RX          26

HardwareSerial SerialGSM(1);

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


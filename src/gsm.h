#pragma once

#include <Arduino.h>

void setupGSM();
void loopGSM();
bool sendSms(const String &phoneNumber, const String &text);



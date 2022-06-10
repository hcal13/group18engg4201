#include "AnalogRead.h"

AnalogRead::AnalogRead(const int pin) {
  pinMode(this->pin = pin, INPUT);
}

int AnalogRead::read() {
  return analogRead(this->pin);
}

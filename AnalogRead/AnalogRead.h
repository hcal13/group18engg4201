#ifndef ANALOGREAD_H
#define ANALOGREAD_H
#include "Sensor.h"

class AnalogRead final : public Sensor {

  private:
    int pin;

  public:
    AnalogRead(int pin);
    int read() override;
};

#endif

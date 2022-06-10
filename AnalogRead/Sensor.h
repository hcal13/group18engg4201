#ifndef SENSOR_H
#define SENSOR_H

#if defined(ARDUINO) && (ARDUINO >= 100)
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

class Sensor {

  public:
    virtual ~Sensor() {};
    virtual int read() = 0;
};

#endif

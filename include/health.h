#ifndef HEALTH_H
#define HEALTH_H
#include <Arduino.h>
class HealthMonitor {
public:
    void begin();
    void update();
private:
    void printPin(const char *name, uint8_t pin, const char *role);
    unsigned long lastReport = 0;
};
extern HealthMonitor health;
#endif

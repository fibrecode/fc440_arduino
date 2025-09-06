#include "Service.h"

Service::Service(unsigned long intervalMs) : interval(intervalMs)
{
    
}

void Service::setup() {
    // default: do nothing
}

void Service::run() {
    if (timer >= interval) {
        run();
        timer = 0;
    }
}

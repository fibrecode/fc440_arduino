#ifndef SERVICE_H
#define SERVICE_H

#include <elapsedMillis.h>

class Service
{
    protected:
        elapsedMillis timer;
        unsigned long interval;

    public:
        Service(unsigned long intervalMs);
        virtual ~Service() {}

        virtual void setup();   // optional
        virtual void run() = 0; // must override
};

#endif
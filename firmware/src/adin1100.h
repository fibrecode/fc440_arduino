#if !defined(ADIN1100_H_)
#define ADIN1100_H_

#include <Arduino.h>
#include "smi.h"
#include "leds.h"

#include "fc1020.h"



class ADIN1100_Control
{
    public:
        void begin(SmiBitbang * smiInstance, uint8_t phyId, BoardLed *led, bool debugEnable = false);
        void default_setup();
        void process();
        uint32_t get_phy_id();
        void set_phy_role_force_master(bool enable);
        bool get_phy_role_force_master();
        void set_phy_role_master_off(bool enable);
        bool get_phy_role_master_off();

        // access functions, private!
        uint16_t readExtendedReg(uint16_t registerAddress);
        void writeExtendedReg(uint16_t registerAddress, uint16_t data);
        uint16_t readDevReg(uint8_t mmd, uint16_t registerAddress);
        void writeDevReg(uint8_t mmd, uint16_t registerAddress, uint16_t data);
    private:
        SmiBitbang * _smi;
        BoardLed * _led;
        bool _debugEnable;
        uint8_t _phyId;
        bool phyForceMaster;
        bool phyAdvertiseSlave;

};


#endif


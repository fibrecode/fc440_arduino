#if !defined(SMI_H_)
#define SMI_H_

#include <Arduino.h>

class SmiBitbang
{

    public:
        SmiBitbang();
        void begin(int pin_mdc = -1, int pin_mdio = -1);

        uint16_t readCL22(uint8_t portAddress, uint8_t registerAddress);
        void writeCL22(uint8_t portAddress, uint8_t registerAddress, uint16_t registerData);

        uint16_t readCL45(uint8_t portAddress, uint8_t deviceAddress, uint16_t registerAddress);
        void writeCL45(uint8_t portAddress, uint8_t deviceAddress, uint16_t registerAddress, uint16_t registerData);

        uint16_t read(uint8_t phy_id, uint8_t reg, uint8_t isCL45 = 0);
        void write(uint8_t phy_id, uint8_t reg, uint16_t data, uint8_t isCL45 = 0, uint8_t isAddress = 0, uint8_t leaveBusActive = 0);

    private:
        int _pin_mdc = -1;
        int _pin_mdio = -1;

        void clock_pulse();

};





#endif

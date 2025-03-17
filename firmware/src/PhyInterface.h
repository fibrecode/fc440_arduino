#if !defined(INT_PHY_INTERFACE_H_)
#define INT_PHY_INTERFACE_H_

#include <Arduino.h>


// High-Level Properties (these are NOT 1:1 mapped to related bits!)



class PhyInterface
{
    public:
        PhyInterface();
        // private reg-access, leave public for easier test-code
        virtual uint16_t read_phy_reg(uint8_t phyAddr, uint8_t registerAddress, uint16_t* data) = 0;
        virtual void write_phy_reg(uint8_t phyAddr, uint8_t registerAddress, uint16_t data) = 0;

    private:
};


#endif


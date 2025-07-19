#if !defined(INT_LAN9303_H_)
#define INT_LAN9303_H_

#include <Arduino.h>
#include "PhyInterface.h"
#include "Wire.h"



// High-Level Properties (these are NOT 1:1 mapped to related bits!)
enum REG_LAN9303
{
    REG_HW_CFG = 0x074,
};

union HW_CFG
{
    struct __attribute__((packed)) 
    {
        uint32_t RES : 24;
        uint8_t AMDIX_EN_PORT_1 : 1;
        uint8_t AMDIX_EN_PORT_2 : 1;
        uint8_t DEVICE_READY : 1;
        uint8_t RES2 : 4;
    };
    uint32_t raw;
};



class Lan9303 :  public PhyInterface
{
    public:
        Lan9303(uint8_t i2cAddr, bool debugEnable);
        
        // Public Phy Interface
        void init(TwoWire* pWire);

        void start();

        //access to PYH MDIO register
        virtual uint16_t read_phy_reg(uint8_t phyAddr, uint8_t registerAddress, uint16_t* data);
        virtual void write_phy_reg(uint8_t phyAddr, uint8_t registerAddress, uint16_t data);

        //access to switch CSR register
        uint16_t read_switch_reg(uint16_t registerAddress, uint32_t* data);
        void write_switch_reg(uint16_t registerAddress, uint32_t data);

        int write(uint32_t address, uint32_t data);
        int read(uint32_t address, uint32_t * data);

        void port2Config();
        void cyclic();

        HW_CFG getHWConfig();

    private:
        

        bool _debugEnable;
        uint8_t _i2cAddress;
        TwoWire* _pI2c;

};


#endif


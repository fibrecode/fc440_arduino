#if !defined(INT_PHY_H_)
#define INT_PHY_H_

#include <Arduino.h>

#include "smi.h"
#include "leds.h"
#include "PhyInterface.h"


// High-Level Properties (these are NOT 1:1 mapped to related bits!)

enum PhyRegisters_T : uint8_t
{
    PHY_BASIC_CONTROL       = 0,
    PHY_BASIC_STATUS        = 1,
    PHY_ID_MSB              = 2,
    PHY_ID_LSB              = 3,
    PHY_AN_ADV              = 4,
    PHY_AN_LP_BASE_ABILITY  = 5,
    PHY_AN_EXP              = 6,
    PHY_MODE_CONTROL_STATUS         = 17,
    PHY_SPECIAL_MODES               = 18,
    PHY_SPECIAL_CONTROL_STAT_IND    = 27,
    PHY_INTERRUPT_SOURCE            = 29,
    PHY_INTERRUPT_MASK              = 30,
    PHY_SPECIAL_CONTROL_STATUS      = 31,

};

union BASIC_CONTROL
{
    struct __attribute__((packed))
    {

        uint16_t RES_1 : 7;
        uint16_t COLLISION_TEST : 1;
        uint16_t DUPLEX_MODE : 1; //8
        uint16_t RESTART_AUTO_NEG : 1;
        uint16_t RES_2 : 1;
        uint16_t POWER_DOWN : 1;
        uint16_t AUTO_NEGOTIATION : 1;
        uint16_t SPEED_SELECT_LSB : 1;
        uint16_t LOOPBACK : 1;
        uint16_t RESET : 1;
    };
    uint16_t raw;
};

union BASIC_STATUS
{
    struct __attribute__((packed)) 
    {
        uint16_t EXTENDED_CAP : 1;
        uint16_t JABBER_DETECT : 1;
        uint16_t LINK_STATUS : 1;
        uint16_t AUTO_NEG_ABILITY : 1; //8
        uint16_t REMOTE_FAULT : 1;
        uint16_t AUTO_NEG_COMPLETE : 1;
        uint16_t RES : 3;
        uint16_t _100BASE_T2_HALF :1;
        uint16_t _100BASE_T2_FULL :1;
        uint16_t _10BASE_T_HALF : 1;
        uint16_t _10BASE_T_FULL : 1;
        uint16_t _100BASE_X_HALF : 1;
        uint16_t _100BASE_X_FULL : 1;
        uint16_t _100BASE_T4 : 1;
    };
    uint16_t raw;
};

union AUTO_NEG_ADV_REG
{
    struct __attribute__((packed)) 
    {
        uint16_t SELECTOR_FIELD : 5;
        uint16_t _10BASE_T_HALF_DUPLEX : 1;
        uint16_t _10BASE_T_FULL_DUPLEX : 1;
        uint16_t _100BASE_X_HALF_DUPLEX : 1;
        uint16_t _100BASE_X_FULL_DUPLEX : 1;
        uint16_t RES : 1;
        uint16_t SYMMETRIC_PAUSE : 1;
        uint16_t ASYSMMETRIC_PAUSE : 1;
        uint16_t RES_2 : 1;  //12
        uint16_t REMOTE_FAULT : 1;
        uint16_t RES_3 : 2;
    };
    uint16_t raw;
};

union AUTO_NEG_LP_BASE_REG
{
    struct __attribute__((packed)) 
    {
        uint16_t SELECTOR_FIELD : 5;
        uint16_t _10BASE_T_HALF_DUPLEX : 1;
        uint16_t _10BASE_T_FULL_DUPLEX : 1;
        uint16_t _100BASE_X_HALF_DUPLEX : 1;
        uint16_t _100BASE_X_FULL_DUPLEX : 1;
        uint16_t _100BASE_T4 : 1;
        uint16_t SYMMETRIC_PAUSE : 1;
        uint16_t ASYSMMETRIC_PAUSE : 1;
        uint16_t RES_2 : 1;  //12
        uint16_t REMOTE_FAULT : 1;
        uint16_t ACK : 1;
        uint16_t NEXT_PAGE : 1;
    };
    uint16_t raw;
};

union AUTO_NEG_EXPANSION_REG
{
    struct __attribute__((packed)) 
    {
        uint16_t LP_AUTO_NET_ABLE : 1;
        uint16_t PAGE_RECEIVED : 1;
        uint16_t LOCAL_DEVICE_NEXT_PAGE_ABLE : 1;
        uint16_t LINK_PARTNER_NEXT_PAGE_ABLE : 1;
        uint16_t PARALLEL_FAULT : 1;
        uint16_t RES : 11;

    };
    uint16_t raw;
};

union SPECIAL_MODE_REGISTER
{
    struct __attribute__((packed)) 
    {
        uint16_t PHY_ADDR : 5;
        uint16_t PHY_MODE : 3;
        uint16_t RES : 8;
    };
    uint16_t raw;
};

union SPECIAL_CONTROL_REGISTER
{
    struct __attribute__((packed)) 
    {
        uint16_t RES3 : 4;
        uint16_t _10BASE_POLARITY_STATE :1;
        uint16_t RES2 : 5; //10
        uint16_t RX_PLL_LOCK_CTL : 1;
        uint16_t SQE_TEST_DISABLE : 1;
        uint16_t RES : 1;
        uint16_t AUTO_MDIX_STATE : 1;
        uint16_t AUTO_MDIX_EN : 1;
        uint16_t AUTO_MDIX_CTL : 1;
    };
    uint16_t raw;
};


class Internal_Phy
{
    public:
        void begin(PhyInterface * phyInterface, uint8_t phyId, bool debugEnable = false);
        void default_setup();
        void process();
        void dump_regs();
        void dump_properties();

        void restart_link(uint8_t phyRole);
        void set_phy_role_auto(bool enable);
        void set_phy_role_req(uint8_t phyRole);

        uint32_t get_phy_id();
        bool enable_config(bool enable);
        uint8_t get_clock_mode();
        uint8_t set_clock_mode(uint8_t clockMode);
        uint8_t get_mii_mode();
        uint8_t set_mii_mode(uint8_t miiMode);
        uint8_t set_phy_role(uint8_t phyRole);
        uint8_t get_phy_role();
        uint8_t set_power_mode(uint8_t powwerMode);
        uint8_t get_power_mode();
        uint8_t set_phy_mode(uint8_t phyMode);  // Auto, Host-Controlled, ....
        uint8_t get_phy_mode();
        bool get_link_control();
        bool set_link_control(bool enable);
        bool get_link_status();
        uint8_t get_sqi_value();
        bool check_polarity(); // true: no crossed-polarity detected; false: polarity crossed

        // private reg-access, leave public for easier test-code
        uint16_t read_reg(uint8_t registerAddress);
        void write_reg(uint8_t registerAddress, uint16_t data);

    private:

        void setAutoMdix(void);
        void printBasicControl(uint16_t raw);
        void printBasicStatus(uint16_t raw);
        void printAutoNegAdvertisindStatus(uint16_t raw);
        void printAutoNegLpBase(uint16_t raw);
        void printAutoNegExpansion(uint16_t raw);
        void printSpecialMode(uint16_t raw);
        void printSpecialControl(uint16_t raw);
        SmiBitbang * _smi;
        PhyInterface * _pPhyInterface;
        BoardLed * _led;
        bool _debugEnable;
        uint8_t _phyId;
        uint8_t _phyRoleAuto;
        uint8_t _phyRoleReq;
};


#endif


#if !defined(TJA1101_H_)
#define TJA1101_H_

#include <Arduino.h>

#include "smi.h"
#include "leds.h"



// PHY register addresses
#define TJA110X_REG_BCR                 0x00        // Basic control register
#define TJA110X_REG_BSR                 0x01        // Basic status register
#define TJA110X_REG_ID1                 0x02        // PHY identification register 1
#define TJA110X_REG_ID2                 0x03        // PHY identification register 2

#define TJA110X_REG_EXT_STATUS          0x0F        // Extended status register
#define TJA110X_REG_EXT_CONTROL         0x11        // Extended control register
#define TJA110X_REG_CONFIG1             0x12        // Configuration register 1
#define TJA110X_REG_CONFIG2             0x13        // Configuration register 2
#define TJA110X_REG_ERROR_CNT           0x14        // Symbol error counter register
#define TJA110X_REG_INT_SOURCE          0x15        // Interrupt source register
#define TJA110X_REG_INT_ENABLE          0x16        // Interrupt enable register
#define TJA110X_REG_COM_STATUS          0x17        // Communication status register
#define TJA110X_REG_GEN_STATUS          0x18        // General status register
#define TJA110X_REG_EXTERNAL_STATUS     0x19        // External status register
#define TJA110X_REG_LINKFAIL_CNT        0x1A        // Link-fail counter register
#define TJA110X_REG_COMMON_CONFIG       0x1B        // Common Configuration Register
#define TJA110X_REG_CONFIG3             0x1C        // Configuration register 3

// BCR register bits
#define TJA110X_REG_BCR_RESET               0x8000          // software reset control
#define TJA110X_REG_BCR_SPEED_SEL_LSB       0x2000          // speed select (LSB)
#define TJA110X_REG_BCR_SPEED_SEL_MSB       0x0040          // speed select (MSB)

// BSR register bits
#define TJA110X_REG_BSR_LINK_STATUS         0x0004          // link status (0 - link is down, 1 - link is up)



// EXT CONTROL register bits
#define TJA110X_REG_EXT_CONTROL_LINK_CONTROL        (0x1u << 15)
#define TJA110X_REG_EXT_CONTROL_POWER_MODE_OFFSET   11
#define TJA110X_REG_EXT_CONTROL_POWER_MODE_MASK     (0xFu << TJA110X_REG_EXT_CONTROL_POWER_MODE_OFFSET)
#define TJA110X_REG_EXT_CONTROL_POWER_MODE_NORMAL   (0x3u << 11)
#define TJA110X_REG_EXT_CONTROL_CONFIG_EN           (0x1u << 2)

// COM STATUS register bits
#define TJA110X_REG_COM_STATUS_LINK_UP              (0x1u << 15)
#define TJA110X_REG_COM_SNR_OFFSET                  5
#define TJA110X_REG_COM_SNR_MASK                    (0x7u << TJA110X_REG_COM_SNR_OFFSET)

// CONFIG1 register bits
#define TJA110X_REG_CONFIG1_MASTER_SLAVE            (0x1u << 15)
#define TJA110X_REG_CONFIG1_FWDPHYLOC               (0x1u << 14)    // forward wake-up event
#define TJA110X_REG_CONFIG1_MII_MODE_OFFSET         8
#define TJA110X_REG_CONFIG1_MII_MODE_MASK           (0x3u << TJA110X_REG_CONFIG1_MII_MODE_OFFSET)

// Common Config Register bits

#define TJA110X_REG_COMMON_CONFIG_AUTO_OFFSET       15
#define TJA110X_REG_COMMON_CONFIG_AUTO_MASK         (0x1u << TJA110X_REG_COMMON_CONFIG_AUTO_OFFSET)

// External status register
#define TJA110X_REG_EXTERNAL_STATUS_POLARITY_OFFSET 6
#define TJA110X_REG_EXTERNAL_STATUS_POLARITY_MASK   (0x1u << TJA110X_REG_EXTERNAL_STATUS_POLARITY_OFFSET)


#define RESET_TIMEOUT               100000          // us
#define PHY_INIT_TIME               (10 * 1000)     // us
#define PHY_ENABLE_TIME             ( 5 * 1000)     // us


#define TJA1100_MRA4_ID             0x0180DC41      // MRA-4

#define TJA1101_MRA1_ID             0x0180DD01      // MRA-1 (TJA1101AHN)
#define TJA1101_MRA2_ID             0x0180DD02      // MRA-2 (TJA1101BHN)

#define TJA1102_MRA1_ID             0x0180DC81      // MRA-1 TJA1102
#define TJA1102_MRA2_ID             0x0180DC82      // MRA-2 TJA1102A
#define TJA1102S_MRA1_ID            0x0180DC91

// High-Level Properties (these are NOT 1:1 mapped to related bits!)

// Clock-Modes
#define CLK_MODE_XTAL           0
#define CLK_MODE_XTAL_CLK_OUT   1
#define CLK_MODE_EXT25          2
#define CLK_MODE_EXT50          3

// MII-MODE
#define MII_MODE_MII            0
#define MII_MODE_RMII_IN50      1
#define MII_MODE_RMII_OUT50     2
#define MII_MODE_REV_MII        3

// PHY-Roles (100BASE-T1 doesn#T support AUTONEG, refer to process() supporting auto-handler in SW)

#define PHY_ROLE_SLAVE          0
#define PHY_ROLE_MASTER         1
#define PHY_ROLE_FORCE_MASTER   2
#define PHY_ROLE_ADVANCE_SLAVE  16
#define PHY_ROLE_ADVANCE_MASTER 17
#define PHY_ROLE_INVALID        99

// PHY-Mode

#define PHY_MODE_AUTO               1
#define PHY_MODE_MANAGED            2
#define PHY_MODE_INVALID            99


// PHY-Modes (int. State-Machine, depends on Autonomous/Managed strapping)

#define POWER_MODE_STANDBY          1
#define POWER_MODE_SLEEP            2
#define POWER_MODE_RESET            3
#define POWER_MODE_NORMAL           4
#define POWER_MODE_SLIENT           5
#define POWER_MODE_UNKNOWN          99

class TJA1101_Control
{
    public:
        void begin(SmiBitbang * smiInstance, uint8_t phyId, BoardLed *led, bool debugEnable = false);
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
        SmiBitbang * _smi;
        BoardLed * _led;
        bool _debugEnable;
        uint8_t _phyId;
        uint8_t _phyRoleAuto;
        uint8_t _phyRoleReq;
};


#endif


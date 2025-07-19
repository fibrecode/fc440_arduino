#if !defined(DP83TD510E_H_)
#define DP83TD510E_H_

#include <Arduino.h>
#include "smi.h"
#include "leds.h"

#include "fc1020.h"

#define DP83TD510E_REG_MII_REG_0            0x000
#define DP83TD510E_REG_MII_REG_2            0x002
#define DP83TD510E_REG_MII_REG_3            0x003
#define DP83TD510E_REG_PHY_STS              0x010
#define DP83TD510E_REG_GEN_CFG              0x011
#define DP83TD510E_REG_INTERRUPT_REG_1      0x012
#define DP83TD510E_REG_INTERRUPT_REG2       0x013
#define DP83TD510E_REG_RX_ERR_CNT           0x015
#define DP83TD510E_REG_BISCR                0x016
#define DP83TD510E_REG_MAC_CFG_1            0x017
#define DP83TD510E_REG_MAC_CFG_2            0x018
#define DP83TD510E_REG_SOR_PHYAD            0x019
#define DP83TD510E_REG_TDR_CFG              0x01E
#define DP83TD510E_REG_PRBS_CFG1            0x119
#define DP83TD510E_REG_PRBS_CFG2            0x11A
#define DP83TD510E_REG_PRBS_CFG3            0x11B
#define DP83TD510E_REG_PRBS_STATUS_1        0x11C
#define DP83TD510E_REG_PRBS_STATUS_2        0x11D
#define DP83TD510E_REG_PRBS_STATUS_3        0x11E
#define DP83TD510E_REG_PRBS_STATUS_4        0x11F
#define DP83TD510E_REG_PRBS_STATUS_5        0x120
#define DP83TD510E_REG_PRBS_STATUS_6        0x121
#define DP83TD510E_REG_PRBS_STATUS_7        0x122
#define DP83TD510E_REG_PRBS_CFG4            0x123
#define DP83TD510E_REG_PRBS_CFG5            0x124
#define DP83TD510E_REG_PRBS_CFG6            0x125
#define DP83TD510E_REG_PRBS_CFG7            0x126
#define DP83TD510E_REG_PRBS_CFG8            0x127
#define DP83TD510E_REG_PRBS_CFG9            0x128
#define DP83TD510E_REG_PRBS_CFG10           0x129
#define DP83TD510E_REG_CRC_STATUS           0x12A
#define DP83TD510E_REG_PKT_STAT_1           0x12B
#define DP83TD510E_REG_PKT_STAT_2           0x12C
#define DP83TD510E_REG_PKT_STAT_3           0x12D
#define DP83TD510E_REG_PKT_STAT_4           0x12E
#define DP83TD510E_REG_PKT_STAT_5           0x12F
#define DP83TD510E_REG_PKT_STAT_6           0x130
#define DP83TD510E_REG_AN_CONTROL           0x200
#define DP83TD510E_REG_AN_STATUS            0x201
#define DP83TD510E_REG_AN_ADV_1             0x202
#define DP83TD510E_REG_AN_ADV_2             0x203
#define DP83TD510E_REG_AN_ADV_3             0x204
#define DP83TD510E_REG_AN_LP_ADV_1          0x205
#define DP83TD510E_REG_AN_LP_ADV_2          0x206
#define DP83TD510E_REG_AN_LP_ADV_3          0x207
#define DP83TD510E_REG_AN_NP_ADV_1          0x208
#define DP83TD510E_REG_AN_NP_ADV_2          0x209
#define DP83TD510E_REG_AN_NP_ADV_3          0x20A
#define DP83TD510E_REG_AN_LP_NP_ADV_1       0x20B
#define DP83TD510E_REG_AN_LP_NP_ADV_2       0x20C
#define DP83TD510E_REG_AN_LP_NP_ADV_3       0x20D
#define DP83TD510E_REG_AN_CTRL_10BT1        0x20E
#define DP83TD510E_REG_AN_STATUS_10BT1      0x20F
#define DP83TD510E_REG_TDR_CFG1             0x300
#define DP83TD510E_REG_TDR_CFG2             0x301
#define DP83TD510E_REG_TDR_CFG3             0x302
#define DP83TD510E_REG_FAULT_CFG1           0x303
#define DP83TD510E_REG_FAULT_CFG2           0x304
#define DP83TD510E_REG_FAULT_STAT1          0x305
#define DP83TD510E_REG_FAULT_STAT2          0x306
#define DP83TD510E_REG_FAULT_STAT3          0x307
#define DP83TD510E_REG_FAULT_STAT4          0x308
#define DP83TD510E_REG_FAULT_STAT5          0x309
#define DP83TD510E_REG_FAULT_STAT6          0x30A
#define DP83TD510E_REG_CHIP_SOR_0           0x420
#define DP83TD510E_REG_LEDS_CFG_1           0x460
#define DP83TD510E_REG_IO_MUX_CFG           0x461
#define DP83TD510E_REG_IO_MUX_GPIO_CTRL_1   0x462
#define DP83TD510E_REG_IO_MUX_GPIO_CTRL_2   0x463
#define DP83TD510E_REG_CHIP_SOR_1           0x467
#define DP83TD510E_REG_CHIP_SOR_2           0x468
#define DP83TD510E_REG_LEDS_CFG_2           0x469
#define DP83TD510E_REG_AN_STAT_1            0x60C
#define DP83TD510E_REG_ALCD_METRIC          0xA9D
#define DP83TD510E_REG_ALCD_STATUS          0xA9F
#define DP83TD510E_REG_SCAN_2               0xE01
#define DP83TD510E_REG_PMA_PMD_CTRL_1       0x1000
#define DP83TD510E_REG_PMA_PMD_CTRL_2       0x1007
#define DP83TD510E_REG_PMA_PMD_EXT_ABILITY2 0x100B
#define DP83TD510E_REG_PMA_PMD_EXT_ABILITY  0x1012
#define DP83TD510E_REG_PMA_PMD_CTRL         0x1834
#define DP83TD510E_REG_PMA_CTRL             0x18F6
#define DP83TD510E_REG_PMA_STATUS           0x18F7
#define DP83TD510E_REG_TEST_MODE_CTRL       0x18F8
#define DP83TD510E_REG_PCS_CTRL             0x3000
#define DP83TD510E_REG_PCS_CTRL_2           0x38E6
#define DP83TD510E_REG_PCS_STATUS           0x38E7




class DP83TD510E_Control
{
    public:
        void begin(SmiBitbang * smiInstance, uint8_t phyId, BoardLed *led, bool debugEnable = false);
        void default_setup();
        void process();
        uint16_t readExtendedReg(uint16_t registerAddress);
        void writeExtendedReg(uint16_t registerAddress, uint16_t data);
        void dump_regs();
        void dump_properties();
        // Phy-API
        void set_phy_role_force_master(bool enable);
        bool get_phy_role_force_master();
        void set_phy_role_master_off(bool enable);
        bool get_phy_role_master_off();
    
    private:
        SmiBitbang * _smi;
        BoardLed * _led;
        bool _debugEnable;
        uint8_t _phyId;

        bool phyForceMaster;
        bool phyAdvertiseSlave;

        void decodeRegisterAddress(uint16_t address, uint8_t * mmd, uint16_t * actualAddress);
};


#endif


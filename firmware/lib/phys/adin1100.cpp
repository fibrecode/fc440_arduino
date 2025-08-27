

#include "adin1100.h"

/*
AN_ADV_FORCE_MS AN_ADV_MST AN_LP_ADV_FORCE_MS AN_LP_ADV_MST Master/Slave Resolution
0 0 0 0 Master/slave Slave/master                   - F F
0 0 0 1 Slave Master
0 1 0 0 Master Slave                                - F T
0 1 0 1 Master/slave Slave/master
0 X 1 0 Master Slave
0 X 1 1 Slave Master
1 0 0 X Slave Master
1 1 0 X Master Slave                                - T F
1 0 1 0 Configuration fault Configuration fault
1 0 1 1 Slave Master
1 1 1 0 Master Slave                                - T T
1 1 1 1 Configuration fault Configuration fault
*/

// Force-Master: FM
// Advertise Master: AM

/*
    FM  AM
    F   F   0 0 0 0     = 0
    F   T   0 1 0 1     = 5  
    T   F   1 1 1 0     = 8
    T   T   1 1 1 0     = 14
*/


void ADIN1100_Control::begin(SmiBitbang * smiInstance, uint8_t phyId, BoardLed *led, bool debugEnable)
{
    this->_smi = smiInstance;
    this->_debugEnable = debugEnable;
    this->_phyId = phyId;
    this->_led = led;
}

void ADIN1100_Control::default_setup()
{
    uint16_t temp = 0;

    this->_led->setLed(BOARD_LED_COLOR_RED, BOARD_LED_MODE_BLINK_FAST);
// '
//     Serial.println("ADIN1100 Software Reset");
//     this->_smi->write(this->_phyId, 0x0, 0x8000, 0, 0);
//     delay(100);'


    temp = 0x1234;
    if(this->_debugEnable)
        Serial.printf("ADIN1100 Setup MAC_CFG_1: %04X\r\n", temp);
    

    this->_led->setLed(BOARD_LED_COLOR_RED, BOARD_LED_MODE_OFF);
}

void ADIN1100_Control::process()
{
    uint16_t phyId1, phyId2;
    uint16_t pmaLinkStat, pmaStat;
    uint16_t regANAdvAbilityM;
    uint16_t regANAdvAbilityL;

    if (this->phyForceMaster)
    {
        // set bit AN_ADV_ABILITY_M in AN_ADV_ABILITY_M
        regANAdvAbilityM = this->_smi->readCL45(this->_phyId, 0x7, 0x203);
        regANAdvAbilityM |= (1<<4);
        this->_smi->writeCL45(this->_phyId, 0x7, 0x203, regANAdvAbilityM);

        // set bit AN_ADV_FORCE_MS in AN_ADV_ABILITY_L
        regANAdvAbilityL = this->_smi->readCL45(this->_phyId, 0x7, 0x202);
        regANAdvAbilityL |= (1<<12);
        this->_smi->writeCL45(this->_phyId, 0x7, 0x202, regANAdvAbilityL);
    }
    else
    {
        // clear bit AN_ADV_ABILITY_M in AN_ADV_ABILITY_M
        regANAdvAbilityM = this->_smi->readCL45(this->_phyId, 0x7, 0x203);
        regANAdvAbilityM &= ~(1<<4);
        this->_smi->writeCL45(this->_phyId, 0x7, 0x203, regANAdvAbilityM);

        // clear bit AN_ADV_FORCE_MS in AN_ADV_ABILITY_L
        regANAdvAbilityL = this->_smi->readCL45(this->_phyId, 0x7, 0x202);
        regANAdvAbilityL &= ~(1<<12);
        this->_smi->writeCL45(this->_phyId, 0x7, 0x202, regANAdvAbilityL);
    }

    if (this->phyAdvertiseSlave)
    {
        // clear bit AN_ADV_ABILITY_M in AN_ADV_ABILITY_M
        regANAdvAbilityM = this->_smi->readCL45(this->_phyId, 0x7, 0x203);
        regANAdvAbilityM &= ~(1<<4);
        this->_smi->writeCL45(this->_phyId, 0x7, 0x203, regANAdvAbilityM);

        // clear bit AN_ADV_FORCE_MS in AN_ADV_ABILITY_L
        regANAdvAbilityL = this->_smi->readCL45(this->_phyId, 0x7, 0x202);
        regANAdvAbilityL &= ~(1<<12);
        this->_smi->writeCL45(this->_phyId, 0x7, 0x202, regANAdvAbilityL);
    }

    /*
    regB10L_PMA_LINK_STAT = 99074 # 01.8302
	regB10L_PMA_STAT = 67831 # 01.08F7
    */

    pmaLinkStat = this->_smi->readCL45(this->_phyId, 0x1, 0x8302);    // regB10L_PMA_LINK_STAT
    pmaStat = this->_smi->readCL45(this->_phyId, 0x1, 0x08F7);          // regB10L_PMA_STAT
    //  phyId1 = this->_smi->read(this->_phyId, 0x2, 0);
    //  phyId2 = this->_smi->read(this->_phyId, 0x3, 0);



    if ((pmaLinkStat & 1) == 1)
    {
        this->_led->setLed(BOARD_LED_COLOR_GREEN, BOARD_LED_MODE_ON);
    }
    else
    {    
        this->_led->setLed2(BOARD_LED_COLOR_ORANGE, BOARD_LED_COLOR_RED, BOARD_LED_MODE_FLASHING_BI);
    }

    if(this->_debugEnable){
        Serial.print("---------------------------------------------------------------\r\n");
        Serial.println("ADI process()");
        Serial.printf("ADIN1100-PhyId: %d %04X.%04X\r\n", this->_phyId, phyId1, phyId2);
        Serial.printf("ADIN1100-PMA LinkStat: %04X\r\n", pmaLinkStat);
        Serial.printf("ADIN1100-PMA Stat: %04X\r\n", pmaStat);
        Serial.print("---------------------------------------------------------------\r\n");

    }
}

uint32_t ADIN1100_Control::get_phy_id()
{
    uint32_t phyId;
    uint16_t phyId1, phyId2;

    phyId1 = this->_smi->readCL45(this->_phyId, 0x1E, 2);    
    phyId2 = this->_smi->readCL45(this->_phyId, 0x1E, 3);


    phyId = (phyId1 << 16) | phyId2;

    return phyId;
}

void ADIN1100_Control::set_phy_role_force_master(bool enable)
{
    this->phyForceMaster = enable;
}


bool ADIN1100_Control::get_phy_role_force_master()
{
    return this->phyForceMaster;
}

void ADIN1100_Control::set_phy_role_master_off(bool enable)
{
    this->phyAdvertiseSlave = enable;
}

bool ADIN1100_Control::get_phy_role_master_off()
{
    return this->phyAdvertiseSlave;
}




uint16_t ADIN1100_Control::readExtendedReg(uint16_t registerAddress)
{
    uint8_t mmd = 0;
    uint16_t addr = 0;
    uint16_t temp = 0;
    
    temp = this->_smi->readCL45(this->_phyId, 0x1E, registerAddress);
        

    if(this->_debugEnable)
        Serial.printf("ADIN1100-readCL45 ID:%02X REG:%04X - ADDR:%04X RESULT:%04X\r\n", this->_phyId, registerAddress, temp);

    return temp;
}

void ADIN1100_Control::writeExtendedReg(uint16_t registerAddress, uint16_t data)
{
    uint8_t mmd = 0;
    uint16_t addr = 0;

    this->_smi->write(this->_phyId, 0x000D, mmd);
    this->_smi->write(this->_phyId, 0x000E, addr);
    this->_smi->write(this->_phyId, 0x000D, 0x4000 | mmd);
    this->_smi->write(this->_phyId, 0x000E, data);

    if(this->_debugEnable)
        Serial.printf("ADIN1100-writeExtendedReg ID:%02X REG:%04X - MMD:%02X ADDR:%04X DATA:%04X\r\n", this->_phyId, registerAddress, mmd, addr, data);
}

uint16_t ADIN1100_Control::readDevReg(uint8_t mmd, uint16_t registerAddress)
{
    uint16_t temp = 0;
    
    temp = this->_smi->readCL45(this->_phyId, mmd, registerAddress);
        

    if(this->_debugEnable)
        Serial.printf("ADIN1100-readCL45 ID:%02X MMD: %02X REG:%04X RESULT:%04X\r\n", this->_phyId, mmd, registerAddress, temp);

    return temp;


}

void ADIN1100_Control::writeDevReg(uint8_t mmd, uint16_t registerAddress, uint16_t data)
{   
    this->_smi->writeCL45(this->_phyId, mmd, registerAddress, data);
        

    if(this->_debugEnable)
        Serial.printf("ADIN1100-writeCL45 ID:%02X MMD: %02X REG:%04X RESULT:%04X\r\n", this->_phyId, mmd, registerAddress, data);

}

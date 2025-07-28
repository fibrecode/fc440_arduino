
#include "dp83td510e.h"


void DP83TD510E_Control::begin(SmiBitbang * smiInstance, uint8_t phyId, BoardLed *led, bool debugEnable)
{
    this->_smi = smiInstance;
    this->_debugEnable = debugEnable;
    this->_phyId = phyId;
    this->_led = led;
}

void DP83TD510E_Control::default_setup()
{
    uint16_t temp = 0;

    this->_led->setLed(BOARD_LED_COLOR_RED, BOARD_LED_MODE_BLINK_FAST);

    temp = 0x4001;
    if(this->_debugEnable)
        Serial.printf("DP83TD510E Setup MAC_CFG_1: %04X\r\n", temp);
    this->_smi->writeCL22(this->_phyId, DP83TD510E_REG_MAC_CFG_1, temp);
    //this->writeExtendedReg(DP83TD510E_REG_MAC_CFG_1, temp);

    // MAC-CFG-2
    
    this->writeExtendedReg(DP83TD510E_REG_MAC_CFG_2, 0x0003);


    //Enable PMA 2.4V
    this->readExtendedReg(DP83TD510E_REG_PMA_CTRL);
    this->writeExtendedReg(DP83TD510E_REG_PMA_CTRL, 0x1000);
    this->readExtendedReg(DP83TD510E_REG_PMA_CTRL);

    //Change AN to Request 2.4V from Link Partner
    this->readExtendedReg(DP83TD510E_REG_AN_CTRL_10BT1);
    this->writeExtendedReg(DP83TD510E_REG_AN_CTRL_10BT1, 0xB000);
    this->readExtendedReg(DP83TD510E_REG_AN_CTRL_10BT1);

    //soft reset via bit15 in reg00
    this->writeExtendedReg(DP83TD510E_REG_MII_REG_0, 0x8000);
    sleep_ms(50);

    this->_led->setLed(BOARD_LED_COLOR_RED, BOARD_LED_MODE_OFF);
}

void DP83TD510E_Control::process()
{
    uint16_t temp = 0;
    uint16_t regANAdv1;
    uint16_t regPMAPMD;

    if (this->phyForceMaster)
    {
        // set bit AN_ADV_1 Register
        regANAdv1 = this->readExtendedReg(DP83TD510E_REG_AN_ADV_1);
        regANAdv1 |= (1<<12);
        this->writeExtendedReg(DP83TD510E_REG_AN_ADV_1, regANAdv1);
        // set bit AN_ADV_FORCE_MS in AN_ADV_ABILITY_L
        regPMAPMD = this->readExtendedReg(DP83TD510E_REG_PMA_PMD_CTRL);
        regPMAPMD |= (1<<14);
        this->writeExtendedReg(DP83TD510E_REG_PMA_PMD_CTRL, regPMAPMD);
    }
    else
    {
        // clear bit AN_ADV_1 Register
        regANAdv1 = this->readExtendedReg(DP83TD510E_REG_AN_ADV_1);
        regANAdv1 &= ~(1<<12);
        this->writeExtendedReg(DP83TD510E_REG_AN_ADV_1, regANAdv1);

        regPMAPMD = this->readExtendedReg(DP83TD510E_REG_PMA_PMD_CTRL);
        regPMAPMD &= ~(1<<14);
        this->writeExtendedReg(DP83TD510E_REG_PMA_PMD_CTRL, regPMAPMD);
    }

    if (this->phyAdvertiseSlave)
    {
        // clear bit AN_ADV_1 Register
        regANAdv1 = this->readExtendedReg(DP83TD510E_REG_AN_ADV_1);
        regANAdv1 &= ~(1<<12);
        this->writeExtendedReg(DP83TD510E_REG_AN_ADV_1, regANAdv1);

        regPMAPMD = this->readExtendedReg(DP83TD510E_REG_PMA_PMD_CTRL);
        regPMAPMD &= ~(1<<14);
        this->writeExtendedReg(DP83TD510E_REG_PMA_PMD_CTRL, regPMAPMD);
    }

    temp = this->readExtendedReg(DP83TD510E_REG_AN_STATUS);
    if(temp & 0x0004)
    {  //Link is UP!
        temp = this->readExtendedReg(DP83TD510E_REG_PMA_CTRL);
        if(temp & 0x1000)   // verify bit 12, LVL 0: 1.0V, 1: 2.4V
        {
            this->_led->setLed(BOARD_LED_COLOR_ORANGE, BOARD_LED_MODE_ON);  // Orange: 2.4V (default)
        }
        else
        {
            this->_led->setLed(BOARD_LED_COLOR_GREEN, BOARD_LED_MODE_ON);   // Green: 1.0V
        }
    }
    else if(temp & 0x0010)
    { //Autoneg Remote Fault!
        this->_led->setLed(BOARD_LED_COLOR_RED, BOARD_LED_MODE_ON);
    }
    else
    {
        this->_led->setLed(BOARD_LED_COLOR_ORANGE, BOARD_LED_MODE_FLASHING);
    }

    if(this->_debugEnable)
    {
        this->dump_regs();
        this->dump_properties();  
    }
}

uint16_t DP83TD510E_Control::readExtendedReg(uint16_t registerAddress)
{
    uint8_t mmd = 0;
    uint16_t addr = 0;
    uint16_t temp = 0;
    this->decodeRegisterAddress(registerAddress, &mmd, &addr);

    if(registerAddress <= 0x1F)
    {
        temp = this->_smi->readCL22(this->_phyId, registerAddress);
    }else
    {
        this->_smi->write(this->_phyId, 0x000D, mmd);
        this->_smi->write(this->_phyId, 0x000E, addr);
        this->_smi->write(this->_phyId, 0x000D, 0x4000 | mmd);
        temp = this->_smi->read(this->_phyId, 0x000E);
    }

/*
    if(this->_debugEnable)
        Serial.printf("DP83TD510E-readExtendedReg ID:%02X REG:%04X - MMD:%02X ADDR:%04X RESULT:%04X\r\n", this->_phyId, registerAddress, mmd, addr, temp);
*/
    return temp;
}

void DP83TD510E_Control::writeExtendedReg(uint16_t registerAddress, uint16_t data)
{
    uint8_t mmd = 0;
    uint16_t addr = 0;
    uint16_t temp = 0;

    if(registerAddress <= 0x1F)
    {
        this->_smi->write(this->_phyId, registerAddress, data);

        if(this->_debugEnable)
            Serial.printf("DP83TD510E-writeReg ID:%02X REG:%04X - DATA:%04X\r\n", this->_phyId, registerAddress, data);
    }
    else
    {
        this->decodeRegisterAddress(registerAddress, &mmd, &addr);    

        this->_smi->write(this->_phyId, 0x000D, mmd);
        this->_smi->write(this->_phyId, 0x000E, addr);
        this->_smi->write(this->_phyId, 0x000D, 0x4000 | mmd);
        this->_smi->write(this->_phyId, 0x000E, data);
    }

    if(this->_debugEnable)
        Serial.printf("DP83TD510E-writeExtendedReg ID:%02X REG:%04X - MMD:%02X ADDR:%04X DATA:%04X\r\n", this->_phyId, registerAddress, mmd, addr, data);
}


void DP83TD510E_Control::decodeRegisterAddress(uint16_t address, uint8_t * mmd, uint16_t * actualAddress)
{
    uint16_t addr = address;
    uint8_t devAddress = 0x1F;
    if((address >= 0x3000) && (address <= 0x38E7)){
        devAddress = 0x03;
        addr -= 0x3000;
    }else if((address >= 0x200) && (address <= 0x20F)){
        devAddress = 0x07;
    }else if((address >= 0x1000) && (address <= 0x18F8)){
        devAddress = 0x01;
        addr -= 0x1000;
    }
    *mmd = devAddress;
    *actualAddress = addr;
}

void DP83TD510E_Control::dump_regs()
{
    uint8_t j;
    uint16_t regs[32];
    uint32_t t1, t2;

    
    t1 = time_us_32();

    for (j = 0; j < 32; j++)
    {
        regs[j] = this->readExtendedReg(j);
    }
    
    t2 = time_us_32();

    Serial.printf("DP83TD510E: Time:%d (%d) us\r\n", t2, t2-t1);
    for (j = 0; j < 32; j++)
    {
        Serial.printf("[%02d]=0x%04X\r\n", j, regs[j]);
    }

    Serial.printf("DP83TD510E --- END DumpRegs\r\n");
}

void DP83TD510E_Control::dump_properties()
{

}

void DP83TD510E_Control::set_phy_role_force_master(bool enable)
{
    this->phyForceMaster = enable;
}


bool DP83TD510E_Control::get_phy_role_force_master()
{
    return this->phyForceMaster;
}

void DP83TD510E_Control::set_phy_role_master_off(bool enable)
{
    this->phyAdvertiseSlave = enable;
}

bool DP83TD510E_Control::get_phy_role_master_off()
{
    return this->phyAdvertiseSlave;
}





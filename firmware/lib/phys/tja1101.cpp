

#include "tja1101.h"
#include "fc1020.h"


void TJA1101_Control::begin(SmiBitbang * smiInstance, uint8_t phyId, BoardLed *led, bool debugEnable)
{
    this->_smi = smiInstance;
    this->_debugEnable = debugEnable;
    this->_phyId = phyId;
    this->_led = led;
}

void TJA1101_Control::default_setup()
{
    uint16_t temp = 0;
    uint16_t phyStatus = -1;

    if(this->_debugEnable)
    {
        phyStatus = this->read_reg(TJA110X_REG_BSR);
        Serial.printf("TJA1101 Setup: %04X\r\n", phyStatus);
    }

    this->enable_config(true);
    sleep_ms(1);

    this->set_mii_mode(MII_MODE_MII);
    sleep_ms(1);

    this->set_phy_mode(PHY_MODE_MANAGED);
    sleep_ms(1);

    this->set_power_mode(POWER_MODE_STANDBY);
    sleep_ms(1);
    this->set_power_mode(POWER_MODE_NORMAL);

    this->_phyRoleReq = 0xFF;
    this->_phyRoleAuto = 0xFF;

    this->_led->setLed(BOARD_LED_COLOR_RED, BOARD_LED_MODE_OFF);

}

void TJA1101_Control::process()
{
    uint16_t regComStatus = 0;
    uint16_t regConfig1 = 0;
    uint16_t regConfig2 = 0;
    uint8_t phyRole;

    regComStatus = this->read_reg(TJA110X_REG_COM_STATUS);
    regConfig1 = this->read_reg(TJA110X_REG_CONFIG1);
    regConfig2 = this->read_reg(TJA110X_REG_CONFIG2);

    // Phy-Role Handler, toggle until linkControl = up

    if (this->_phyRoleAuto)
    {
        if (this->get_link_status() == false)
        {
            phyRole = this->get_phy_role();
            if (phyRole == PHY_ROLE_MASTER)
            {
                this->restart_link(PHY_ROLE_SLAVE);

            }
            else
            {
                this->restart_link(PHY_ROLE_MASTER);
            }
        }

    }
    else
    {
        phyRole = this->get_phy_role();
        if (this->_phyRoleReq != phyRole)
        {
            this->restart_link(this->_phyRoleReq);
        }
    }

    if(this->get_link_status())
    {
        this->_led->setLed(BOARD_LED_COLOR_GREEN, BOARD_LED_MODE_ON);
    }
    else
    {    
        this->_led->setLed2(BOARD_LED_COLOR_RED, BOARD_LED_COLOR_ORANGE, BOARD_LED_MODE_FLASHING_BI);
    }


    if(this->_debugEnable)
    {
        //this->dump_regs();
        this->dump_properties();  
    }
}

uint32_t TJA1101_Control::get_phy_id()
{
    uint32_t phyId;

    phyId = (this->read_reg(TJA110X_REG_ID1) << 16) | this->read_reg(TJA110X_REG_ID2);

    return phyId;
}

void TJA1101_Control::dump_properties()
{
    uint8_t j;

    Serial.printf("TJA1101: %08X\r\n", this->get_phy_id());
    Serial.println("-------------------");
    Serial.printf("PhyRole: %d\r\n", this->get_phy_role());
    Serial.printf("MII-Mode: %d\r\n", this->get_mii_mode());
    Serial.printf("Power-Mode: %d\r\n", this->get_power_mode());
    Serial.printf("Link-Status: %d\r\n", this->get_link_status());
    Serial.printf("SQI: %d\r\n", this->get_sqi_value());
    Serial.printf("Polarity: %d\r\n", this->check_polarity());

    Serial.println("Phy Private:");
    Serial.println("-------------------");
    Serial.printf("PhyRole-Auto: %d\r\n", this->_phyRoleAuto);
    Serial.printf("PhyRole-Req: %d\r\n", this->_phyRoleReq);



}


void TJA1101_Control::dump_regs()
{
    uint8_t j;
    uint16_t regs[18];
    uint32_t t1, t2;

    
    t1 = time_us_32();

    for (j = 0; j < 18; j++)
    {
        if (j < 4)
            regs[j] = this->read_reg(j);        // 0-3
        else
            regs[j] = this->read_reg(j+11);     // 15-28
    }
    
    t2 = time_us_32();

    Serial.printf("TJA1101: Time: %d (%d) us\r\n", t2, t2-t1);
    for (j = 0; j < 18; j++)
    {
        if (j < 4)
            Serial.printf("[%02d]=0x%04X\r\n", j, regs[j]);
        else
            Serial.printf("[%02d]=0x%04X\r\n", j+11, regs[j]);
    }
}

void TJA1101_Control::restart_link(uint8_t phyRole)
{
    uint8_t currentPhyRole;

    currentPhyRole = this->get_phy_role();

    if (phyRole != currentPhyRole)
    {
        this->set_link_control(false);
        this->set_phy_role(phyRole);
        this->set_link_control(true);
    }  
}


void TJA1101_Control::set_phy_role_auto(bool enable)
{
    this->_phyRoleAuto = enable;

}

 void TJA1101_Control::set_phy_role_req(uint8_t phyRole)
 {
    this->_phyRoleReq = phyRole;
 }


bool TJA1101_Control::enable_config(bool enable)
{

    uint16_t regVal;

    regVal = this->read_reg(TJA110X_REG_EXT_CONTROL);

    regVal &= ~(TJA110X_REG_EXT_CONTROL_CONFIG_EN | TJA110X_REG_EXT_CONTROL_POWER_MODE_MASK);
    if ( enable ) {
        regVal |= TJA110X_REG_EXT_CONTROL_CONFIG_EN;
    }

    this->write_reg(TJA110X_REG_EXT_CONTROL, regVal);

    regVal = this->read_reg(TJA110X_REG_EXT_CONTROL);


    return ((regVal & TJA110X_REG_EXT_CONTROL_CONFIG_EN) > 0);
    
}

uint8_t TJA1101_Control::set_phy_role(uint8_t phyRole)
{
    uint16_t regVal;

    regVal = this->read_reg(TJA110X_REG_CONFIG1);

    if (phyRole == PHY_ROLE_MASTER)
        regVal |= TJA110X_REG_CONFIG1_MASTER_SLAVE;
    else if (phyRole == PHY_ROLE_SLAVE)
        regVal &= ~TJA110X_REG_CONFIG1_MASTER_SLAVE;
    else
        return PHY_ROLE_INVALID;

    this->write_reg(TJA110X_REG_CONFIG1, regVal);


    return this->get_phy_role();

}

uint8_t TJA1101_Control::get_phy_role()
{
    // TJA1101 only supports PHY_ROLE_SLAVE or PHY_ROLE_MASTER

    uint16_t regVal;

    regVal = this->read_reg(TJA110X_REG_CONFIG1);

    if ((regVal & TJA110X_REG_CONFIG1_MASTER_SLAVE) > 0)
        return PHY_ROLE_MASTER;
    else
        return PHY_ROLE_SLAVE;
}

uint8_t TJA1101_Control::get_mii_mode()
{
    uint16_t regVal;
    uint8_t miiMode;

    regVal = this->read_reg(TJA110X_REG_CONFIG1);

    miiMode = (regVal & TJA110X_REG_CONFIG1_MII_MODE_MASK) >> TJA110X_REG_CONFIG1_MII_MODE_OFFSET;

    switch(miiMode)
    {
        case 0:
            return MII_MODE_MII;
        case 1:
            return MII_MODE_RMII_IN50;
        case 2:
            return MII_MODE_RMII_OUT50;
        case 3:
            return MII_MODE_REV_MII;
    }

    return 0xFF;
}

uint8_t TJA1101_Control::set_mii_mode(uint8_t miiMode)
{
    uint16_t regVal;

    regVal = this->read_reg(TJA110X_REG_CONFIG1);

    regVal &= ~TJA110X_REG_CONFIG1_MII_MODE_MASK;       // clear all MII-Mode bits 

    switch(miiMode)
    {
        case MII_MODE_MII:
            break;
        case MII_MODE_RMII_IN50:
            regVal |= (1u << TJA110X_REG_CONFIG1_MII_MODE_OFFSET);
            break;
        case MII_MODE_RMII_OUT50:
            regVal |= (2u << TJA110X_REG_CONFIG1_MII_MODE_OFFSET);
            break;
        case MII_MODE_REV_MII:
            regVal |= (3u << TJA110X_REG_CONFIG1_MII_MODE_OFFSET);
            break;
    }
    this->write_reg(TJA110X_REG_CONFIG1, regVal);
    
    return get_mii_mode();
}

uint8_t TJA1101_Control::get_clock_mode()
{
    uint16_t regVal;

    regVal = this->read_reg(TJA110X_REG_CONFIG1);


    return (0);
}

uint8_t TJA1101_Control::set_clock_mode(uint8_t clockMode)
{
    uint16_t regVal;

    regVal = this->read_reg(TJA110X_REG_CONFIG1);

    switch(clockMode)
    {
        case 0: // MII
            regVal = (regVal & ~(3<<8));
            this->write_reg(TJA110X_REG_CONFIG1, regVal);

            break;

        default:
            break;
    }

    return (this->get_clock_mode());

}

uint8_t TJA1101_Control::get_power_mode()
{
    uint16_t regVal;
    uint8_t pMode;

    regVal = this->read_reg(TJA110X_REG_EXT_CONTROL);

    pMode = (regVal & TJA110X_REG_EXT_CONTROL_POWER_MODE_MASK) >> TJA110X_REG_EXT_CONTROL_POWER_MODE_OFFSET;

    switch(pMode)
    {
        case 12:
            return POWER_MODE_STANDBY;
        case 3:
            return POWER_MODE_NORMAL;
        case 9:
            return POWER_MODE_SLIENT;
        case 10:
            return POWER_MODE_SLEEP;
        default:
            return POWER_MODE_UNKNOWN;
    }

}

uint8_t TJA1101_Control::set_power_mode(uint8_t powerMode)
{
    uint16_t regVal;

    regVal = this->read_reg(TJA110X_REG_EXT_CONTROL);

    regVal &= ~TJA110X_REG_EXT_CONTROL_POWER_MODE_MASK;

    switch(powerMode)
    {
        case POWER_MODE_STANDBY:
            regVal |= (1 << TJA110X_REG_EXT_CONTROL_POWER_MODE_OFFSET);
            
            break;
            

            break;
        case POWER_MODE_NORMAL:
            regVal |= (3 << TJA110X_REG_EXT_CONTROL_POWER_MODE_OFFSET);
            this->write_reg(TJA110X_REG_EXT_CONTROL, regVal);
            break;

        default:
            break;
    }
    this->write_reg(TJA110X_REG_EXT_CONTROL, regVal);   // Update register Power-Mode

    return (this->get_power_mode());

}
/*
    Control Phy-Mode: Autonomous, Managed
*/
uint8_t TJA1101_Control::set_phy_mode(uint8_t phyMode)
{
    uint16_t regVal;

    regVal = this->read_reg(TJA110X_REG_COMMON_CONFIG);

    switch(phyMode)
    {
        case PHY_MODE_AUTO:
            regVal |= TJA110X_REG_COMMON_CONFIG_AUTO_MASK;
            break;
        case PHY_MODE_MANAGED:
            regVal &= ~TJA110X_REG_COMMON_CONFIG_AUTO_MASK;
            break;
    }

    this->write_reg(TJA110X_REG_COMMON_CONFIG, regVal);

    return this->get_phy_mode();
}

uint8_t TJA1101_Control::get_phy_mode()
{
    uint16_t regVal;
    uint8_t phyModeBit;

    regVal = this->read_reg(TJA110X_REG_COMMON_CONFIG);

    phyModeBit = (regVal & TJA110X_REG_COMMON_CONFIG_AUTO_MASK) >> TJA110X_REG_COMMON_CONFIG_AUTO_OFFSET;

    switch(phyModeBit)
    {
        case 0:
            return PHY_MODE_MANAGED;
        case 1:
            return PHY_MODE_AUTO;
        default:
            return PHY_MODE_INVALID;
    }

}

bool TJA1101_Control::get_link_control()
{
    uint16_t regVal;

    regVal = this->read_reg(TJA110X_REG_EXT_CONTROL);

    if ((regVal & TJA110X_REG_COM_STATUS_LINK_UP) > 0)
        return true;

    return false;


}

bool TJA1101_Control::set_link_control(bool enable)
{
    uint16_t regVal;

    regVal = this->read_reg(TJA110X_REG_EXT_CONTROL);

    if (enable)
        regVal |= TJA110X_REG_COM_STATUS_LINK_UP;
    else
        regVal &= ~TJA110X_REG_COM_STATUS_LINK_UP;

    this->write_reg(TJA110X_REG_EXT_CONTROL, regVal);

    return get_link_control();
}


bool TJA1101_Control::get_link_status()
{
    uint16_t regVal;

    regVal = this->read_reg(TJA110X_REG_COM_STATUS);

    if ((regVal & TJA110X_REG_COM_STATUS_LINK_UP) > 0)
        return true;

    return false;

}

uint8_t TJA1101_Control::get_sqi_value()
{
    uint16_t regVal;

    regVal = this->read_reg(TJA110X_REG_COM_STATUS);

    return (regVal & TJA110X_REG_COM_SNR_MASK) >> TJA110X_REG_COM_SNR_OFFSET;
        

}

bool TJA1101_Control:: check_polarity()
{
    uint16_t regVal;

    regVal = this->read_reg(TJA110X_REG_EXTERNAL_STATUS);

    return ((regVal & TJA110X_REG_EXTERNAL_STATUS_POLARITY_MASK) == 0);     // if 0, return True (means no cable crossing detected)

}

// ---------------------------------------------------------------------------------------------------------------------------
// private functions to access registers via SMI. Still active for simpler test, but recommended is using properties/functions
// ---------------------------------------------------------------------------------------------------------------------------

uint16_t TJA1101_Control::read_reg(uint8_t registerAddress)
{
    uint8_t mmd = 0;
    uint16_t addr = 0;
    uint16_t temp = 0;
    
    temp = this->_smi->read(this->_phyId, registerAddress);
    

    //if(this->_debugEnable)
    //    Serial.printf("TJA1101-read_reg ID:%02X REG:%04X - RESULT:%04X\r\n", this->_phyId, registerAddress, temp);

    return temp;
}

void TJA1101_Control::write_reg(uint8_t registerAddress, uint16_t data)
{

    this->_smi->write(this->_phyId, registerAddress, data);
    

    if(this->_debugEnable)
        Serial.printf("TJA1101-write_reg ID:%02X REG:%04X - DATA:%04X\r\n", this->_phyId, registerAddress, data);
}

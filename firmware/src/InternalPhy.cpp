

#include "InternalPhy.h"
#include "fc1020.h"


void Internal_Phy::begin(PhyInterface * smiInstance, uint8_t phyId, bool debugEnable)
{
    this->_pPhyInterface = smiInstance;
    this->_debugEnable = debugEnable;
    this->_phyId = phyId;
}

void Internal_Phy::default_setup()
{
    uint16_t temp = 0;
    uint16_t phyStatus = -1;

    BASIC_CONTROL bControl;
    bControl.AUTO_NEGOTIATION = 1;
    bControl.DUPLEX_MODE = 1;
    bControl.SPEED_SELECT_LSB = 1;

    write_reg(PHY_BASIC_CONTROL, bControl.raw);

    if(this->_debugEnable)
    {
        // phyStatus = this->read_reg(TJA110X_REG_BSR);
        Serial.printf("IntPhy Setup: %04X\r\n", phyStatus);
    }

    setAutoMdix();

}

void Internal_Phy::setAutoMdix(void)
{   
    SPECIAL_CONTROL_REGISTER temp;
    temp.raw = read_reg(PHY_SPECIAL_CONTROL_STAT_IND);
    temp.AUTO_MDIX_CTL = 1;
    temp.AUTO_MDIX_EN = 1;
    temp.AUTO_MDIX_STATE = 0;
    write_reg(PHY_SPECIAL_CONTROL_STAT_IND, temp.raw);
}

void Internal_Phy::printBasicControl(uint16_t raw)
{
    BASIC_CONTROL reg;
    reg.raw = raw;
    Serial.printf(
        "=====> IntPhy BasicControl: \r\n"
        "   Reset %u \r\n"
        "   Loopback: %u\r\n"
        "   SpeedSelect: %u\r\n"
        "   Full Duplex: %u\r\n"
        "   Auto-Neg: %u\r\n"
        "   PowerDown: %u\r\n"
        "<=====\n\r",
        reg.RESET,
        reg.LOOPBACK,
        reg.SPEED_SELECT_LSB,
        reg.DUPLEX_MODE,
        reg.AUTO_NEGOTIATION,
        reg.POWER_DOWN );

}

void Internal_Phy::printBasicStatus(uint16_t raw)
{
    BASIC_STATUS reg;
    reg.raw = raw;
    Serial.printf(
        "=====> printBasicStatus:\r\n"
        "   EXTENDED_CAP %u \r\n"
        "   JABBER_DETECT: %u\r\n"
        "   LINK_STATUS: %u\r\n"
        "   AUTO_NEG_ABILITY: %u\r\n"
        "   REMOTE_FAULT: %u\r\n"
        "   AUTO_NEG_COMPLETE: %u\r\n"
        "   _100BASE_T2_HALF: %u\r\n"
        "   _100BASE_T2_FULL: %u\r\n"
        "   _10BASE_T_HALF: %u\r\n"
        "   _10BASE_T_FULL: %u\r\n"
        "   _100BASE_X_HALF: %u\r\n"
        "   _100BASE_X_FULL: %u\r\n"
        "   _100BASE_T4: %u\r\n"
        "<=====\n\r",
        reg.EXTENDED_CAP,
        reg.JABBER_DETECT,
        reg.LINK_STATUS,
        reg.AUTO_NEG_ABILITY,
        reg.REMOTE_FAULT,
        reg.AUTO_NEG_COMPLETE,
        reg._100BASE_T2_HALF,
        reg._100BASE_T2_FULL,
        reg._10BASE_T_HALF,
        reg._10BASE_T_FULL ,
        reg._100BASE_X_HALF,
        reg._100BASE_X_FULL,
        reg._100BASE_T4);

}

void Internal_Phy::printAutoNegAdvertisindStatus(uint16_t raw)
{
    AUTO_NEG_ADV_REG reg;
    reg.raw = raw;
    Serial.printf(
        "=====>  printAutoNegAdvertisindStatus: \r\n"
        "   SELECTOR_FIELD 0x%X \r\n"
        "   _10BASE_T_HALF_DUPLEX: %u\r\n"
        "   _10BASE_T_FULL_DUPLEX: %u\r\n"
        "   _100BASE_X_HALF_DUPLEX: %u\r\n"
        "   _100BASE_X_FULL_DUPLEX: %u\r\n"
        "   SYMMETRIC_PAUSE: %u\r\n"
        "   ASYSMMETRIC_PAUSE: %u\r\n"
        "   REMOTE_FAULT: %u\r\n"
        "<=====\n\r",
        reg.SELECTOR_FIELD,
        reg._10BASE_T_HALF_DUPLEX,
        reg._10BASE_T_FULL_DUPLEX,
        reg._100BASE_X_HALF_DUPLEX,
        reg._100BASE_X_FULL_DUPLEX,
        reg.SYMMETRIC_PAUSE,
        reg.ASYSMMETRIC_PAUSE,
        reg.REMOTE_FAULT);
}

void Internal_Phy::printAutoNegLpBase(uint16_t raw)
{
    AUTO_NEG_LP_BASE_REG reg;
    reg.raw = raw;
    Serial.printf(
        "=====> printAutoNegLpBase: \r\n"
        "   SELECTOR_FIELD 0x%X \r\n"
        "   _10BASE_T_HALF_DUPLEX: %u\r\n"
        "   _10BASE_T_FULL_DUPLEX: %u\r\n"
        "   _100BASE_X_HALF_DUPLEX: %u\r\n"
        "   _100BASE_X_FULL_DUPLEX: %u\r\n"
        "   _100BASE_T4: %u\r\n"
        "   SYMMETRIC_PAUSE: %u\r\n"
        "   ASYSMMETRIC_PAUSE: %u\r\n "
        "   REMOTE_FAULT: %u\r\n"
        "   ACK: %u\r\n"
        "   NEXT_PAGE: %u\n\r"
        "<=====\n\r",
        reg.SELECTOR_FIELD,
        reg._10BASE_T_HALF_DUPLEX,
        reg._10BASE_T_FULL_DUPLEX,
        reg._100BASE_X_HALF_DUPLEX,
        reg._100BASE_X_FULL_DUPLEX,
        reg._100BASE_T4,
        reg.SYMMETRIC_PAUSE,
        reg.ASYSMMETRIC_PAUSE,
        reg.REMOTE_FAULT,
        reg.ACK ,
        reg.NEXT_PAGE);
}

void Internal_Phy::printAutoNegExpansion(uint16_t raw)
{
    AUTO_NEG_EXPANSION_REG reg;
    reg.raw = raw;
    Serial.printf(
        "=====> printAutoNegExpansion: \r\n"
        "   LP_AUTO_NET_ABLE %u \r\n"
        "   PAGE_RECEIVED: %u\r\n"
        "   LOCAL_DEVICE_NEXT_PAGE_ABLE: %u\r\n"
        "   LINK_PARTNER_NEXT_PAGE_ABLE: %u\r\n"
        "   PARALLEL_FAULT: %u\r\n "
        "<=====\n\r",
        reg.LP_AUTO_NET_ABLE,
        reg.PAGE_RECEIVED,
        reg.LOCAL_DEVICE_NEXT_PAGE_ABLE,
        reg.LINK_PARTNER_NEXT_PAGE_ABLE,
        reg.PARALLEL_FAULT);
}

void Internal_Phy::printSpecialMode(uint16_t raw)
{
    SPECIAL_MODE_REGISTER reg;
    reg.raw = raw;
    Serial.printf(
        "=====> printSpecialMode: \r\n"
        "   PHY_ADDR %u \r\n"
        "   PHY_MODE: %u\r\n"
        "<=====\n\r",
        reg.PHY_ADDR,
        reg.PHY_MODE);
}

void Internal_Phy::printSpecialControl(uint16_t raw)
{
    SPECIAL_CONTROL_REGISTER reg;
    reg.raw = raw;
    Serial.printf(
        "=====> printSpecialMode: \r\n"
        "   _10BASE_POLARITY_STATE %u \r\n"
        "   RX_PLL_LOCK_CTL: %u\r\n"
        "   SQE_TEST_DISABLE: %u\r\n"
        "   AUTO_MDIX_STATE: %u\r\n"
        "   AUTO_MDIX_EN: %u\r\n"
        "   AUTO_MDIX_CTL: %u\r\n"
        "<=====\n\r",
        reg._10BASE_POLARITY_STATE,
        reg.RX_PLL_LOCK_CTL,
        reg.SQE_TEST_DISABLE,
        reg.AUTO_MDIX_STATE,
        reg.AUTO_MDIX_EN,
        reg.AUTO_MDIX_CTL);
}




void Internal_Phy::process()
{
    uint16_t regComStatus = 0;
    uint16_t regConfig1 = 0;
    uint16_t regConfig2 = 0;
    uint8_t phyRole;

    //Debuging
    uint8_t registerMap[] = {0,1,2,3,4,5,6,17,18,27,29,30,31};

    for (uint8_t i = 0; i< sizeof(registerMap); i++)
    {
      uint16_t temp=0xAFFE;
      uint8_t regAddr = registerMap[i];
      temp = read_reg(regAddr);
      switch (regAddr)
      {
        case 0:
            printBasicControl(temp);
            break;
        case 1:
            printBasicStatus(temp);
            break;
        case 4:
            printAutoNegAdvertisindStatus(temp);
            break;
        case 5:
            printAutoNegLpBase(temp);
            break;
        case 6:
            printAutoNegExpansion(temp);
            break;
        case 18:
            printSpecialMode(temp);
            break;
        case 27:
            printSpecialControl(temp);
            break;
        
        default:
            Serial.printf("IntPhy CHECK register[%i]: 0x%x\r\n", regAddr, temp);
            break;
      }
      
    }

    //End of Debugging

    // regComStatus = this->read_reg(TJA110X_REG_COM_STATUS);
    // regConfig1 = this->read_reg(TJA110X_REG_CONFIG1);
    // regConfig2 = this->read_reg(TJA110X_REG_CONFIG2);

    // Phy-Role Handler, toggle until linkControl = up

    // if (this->_phyRoleAuto)
    // {
    //     if (this->get_link_status() == false)
    //     {
    //         phyRole = this->get_phy_role();
    //         // if (phyRole == PHY_ROLE_MASTER)
    //         // {
    //         //     this->restart_link(PHY_ROLE_SLAVE);

    //         // }
    //         // else
    //         // {
    //         //     this->restart_link(PHY_ROLE_MASTER);
    //         // }
    //     }

    // }
    // else
    // {
    //     phyRole = this->get_phy_role();
    //     if (this->_phyRoleReq != phyRole)
    //     {
    //         this->restart_link(this->_phyRoleReq);
    //     }
    // }

    // if(this->get_link_status())
    // {
    //     this->_led->setLed(BOARD_LED_COLOR_ORANGE, BOARD_LED_MODE_ON);
    // }
    // else
    // {    
    //     this->_led->setLed(BOARD_LED_COLOR_ORANGE, BOARD_LED_MODE_FLASHING);
    // }


    if(this->_debugEnable)
    {
        Serial.println("-------------------");
        Serial.printf("IntPhy: Some Debugging to the Phy\r\n");
        //this->dump_regs();
        this->dump_properties(); 
        Serial.println("-------------------");
    }
}

uint32_t Internal_Phy::get_phy_id()
{
    uint32_t phyId;

    // phyId = (this->read_reg(TJA110X_REG_ID1) << 16) | this->read_reg(TJA110X_REG_ID2);

    return phyId;
}

void Internal_Phy::dump_properties()
{
    uint8_t j;

    // uint16_t control = this->read_reg(PHY_BASIC_CONTROL);
    // uint16_t status = this->read_reg(PHY_BASIC_STATUS);
    // uint16_t id_msb = this->read_reg(2);
    // uint16_t id_lsb = this->read_reg(3);

     Serial.printf("IntPhy: %08X\r\n", 0xDEADBEEF);
     Serial.println("-------------------");
    //  Serial.printf("Control: 0x%X\r\n", control);
    //  Serial.printf("Status: 0x%X\r\n", status);
    //  Serial.printf("id_msb: 0x%X\r\n", id_msb);
    //  Serial.printf("id_lsb: 0x%X\r\n", id_lsb);
    // Serial.printf("Power-Mode: %d\r\n", this->get_power_mode());
    // Serial.printf("Link-Status: %d\r\n", this->get_link_status());
    // Serial.printf("SQI: %d\r\n", this->get_sqi_value());
    // Serial.printf("Polarity: %d\r\n", this->check_polarity());

    // Serial.println("Phy Private:");
    Serial.println("-------------------");
    // Serial.printf("PhyRole-Auto: %d\r\n", this->_phyRoleAuto);
    // Serial.printf("PhyRole-Req: %d\r\n", this->_phyRoleReq);



}


void Internal_Phy::dump_regs()
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

void Internal_Phy::restart_link(uint8_t phyRole)
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


void Internal_Phy::set_phy_role_auto(bool enable)
{
    this->_phyRoleAuto = enable;

}

 void Internal_Phy::set_phy_role_req(uint8_t phyRole)
 {
    this->_phyRoleReq = phyRole;
 }


bool Internal_Phy::enable_config(bool enable)
{

    uint16_t regVal;

    //regVal = this->read_reg(PHY_BASIC_CONTROL);

    // regVal &= ~(TJA110X_REG_EXT_CONTROL_CONFIG_EN | TJA110X_REG_EXT_CONTROL_POWER_MODE_MASK);
    // if ( enable ) {
    //     regVal |= TJA110X_REG_EXT_CONTROL_CONFIG_EN;
    // }

    // this->write_reg(TJA110X_REG_EXT_CONTROL, regVal);

    // regVal = this->read_reg(TJA110X_REG_EXT_CONTROL);


    // return ((regVal & TJA110X_REG_EXT_CONTROL_CONFIG_EN) > 0);
    return false;
}

uint8_t Internal_Phy::set_phy_role(uint8_t phyRole)
{
    uint16_t regVal;

    // regVal = this->read_reg(TJA110X_REG_CONFIG1);

    // if (phyRole == PHY_ROLE_MASTER)
    //     regVal |= TJA110X_REG_CONFIG1_MASTER_SLAVE;
    // else if (phyRole == PHY_ROLE_SLAVE)
    //     regVal &= ~TJA110X_REG_CONFIG1_MASTER_SLAVE;
    // else
    //     return PHY_ROLE_INVALID;

    // this->write_reg(TJA110X_REG_CONFIG1, regVal);


    // return this->get_phy_role();
    return false;

}

uint8_t Internal_Phy::get_phy_role()
{
    // TJA1101 only supports PHY_ROLE_SLAVE or PHY_ROLE_MASTER

    uint16_t regVal;

    // regVal = this->read_reg(TJA110X_REG_CONFIG1);

    // if ((regVal & TJA110X_REG_CONFIG1_MASTER_SLAVE) > 0)
    //     return PHY_ROLE_MASTER;
    // else
    //     return PHY_ROLE_SLAVE;
    return 1;
}

uint8_t Internal_Phy::get_mii_mode()
{
    // uint16_t regVal;
    // uint8_t miiMode;

    // regVal = this->read_reg(TJA110X_REG_CONFIG1);

    // miiMode = (regVal & TJA110X_REG_CONFIG1_MII_MODE_MASK) >> TJA110X_REG_CONFIG1_MII_MODE_OFFSET;

    // switch(miiMode)
    // {
    //     case 0:
    //         return MII_MODE_MII;
    //     case 1:
    //         return MII_MODE_RMII_IN50;
    //     case 2:
    //         return MII_MODE_RMII_OUT50;
    //     case 3:
    //         return MII_MODE_REV_MII;
    // }

    return 0xFF;
}

uint8_t Internal_Phy::set_mii_mode(uint8_t miiMode)
{
    uint16_t regVal;

    // regVal = this->read_reg(TJA110X_REG_CONFIG1);

    // regVal &= ~TJA110X_REG_CONFIG1_MII_MODE_MASK;       // clear all MII-Mode bits 

    // switch(miiMode)
    // {
    //     case MII_MODE_MII:
    //         break;
    //     case MII_MODE_RMII_IN50:
    //         regVal |= (1u << TJA110X_REG_CONFIG1_MII_MODE_OFFSET);
    //         break;
    //     case MII_MODE_RMII_OUT50:
    //         regVal |= (2u << TJA110X_REG_CONFIG1_MII_MODE_OFFSET);
    //         break;
    //     case MII_MODE_REV_MII:
    //         regVal |= (3u << TJA110X_REG_CONFIG1_MII_MODE_OFFSET);
    //         break;
    // }
    // this->write_reg(TJA110X_REG_CONFIG1, regVal);
    
    return get_mii_mode();
}

uint8_t Internal_Phy::get_clock_mode()
{
    uint16_t regVal;

    // regVal = this->read_reg(TJA110X_REG_CONFIG1);


    return (0);
}

uint8_t Internal_Phy::set_clock_mode(uint8_t clockMode)
{
    uint16_t regVal;

    // regVal = this->read_reg(TJA110X_REG_CONFIG1);

    switch(clockMode)
    {
        case 0: // MII
            regVal = (regVal & ~(3<<8));
            // this->write_reg(TJA110X_REG_CONFIG1, regVal);

            break;

        default:
            break;
    }

    return (this->get_clock_mode());

}

uint8_t Internal_Phy::get_power_mode()
{
    uint16_t regVal;
    uint8_t pMode;

    // regVal = this->read_reg(TJA110X_REG_EXT_CONTROL);

    // pMode = (regVal & TJA110X_REG_EXT_CONTROL_POWER_MODE_MASK) >> TJA110X_REG_EXT_CONTROL_POWER_MODE_OFFSET;

    // switch(pMode)
    // {
    //     case 12:
    //         return POWER_MODE_STANDBY;
    //     case 3:
    //         return POWER_MODE_NORMAL;
    //     case 9:
    //         return POWER_MODE_SLIENT;
    //     case 10:
    //         return POWER_MODE_SLEEP;
    //     default:
    //         return POWER_MODE_UNKNOWN;
    // }
    return 0;
}

uint8_t Internal_Phy::set_power_mode(uint8_t powerMode)
{
    // uint16_t regVal;

    // regVal = this->read_reg(TJA110X_REG_EXT_CONTROL);

    // regVal &= ~TJA110X_REG_EXT_CONTROL_POWER_MODE_MASK;

    // switch(powerMode)
    // {
    //     case POWER_MODE_STANDBY:
    //         regVal |= (1 << TJA110X_REG_EXT_CONTROL_POWER_MODE_OFFSET);
            
    //         break;
            

    //         break;
    //     case POWER_MODE_NORMAL:
    //         regVal |= (3 << TJA110X_REG_EXT_CONTROL_POWER_MODE_OFFSET);
    //         this->write_reg(TJA110X_REG_EXT_CONTROL, regVal);
    //         break;

    //     default:
    //         break;
    // }
    // this->write_reg(TJA110X_REG_EXT_CONTROL, regVal);   // Update register Power-Mode

    // return (this->get_power_mode());
    return 0;
}
/*
    Control Phy-Mode: Autonomous, Managed
*/
uint8_t Internal_Phy::set_phy_mode(uint8_t phyMode)
{
    uint16_t regVal;

    // regVal = this->read_reg(TJA110X_REG_COMMON_CONFIG);

    // switch(phyMode)
    // {
    //     case PHY_MODE_AUTO:
    //         regVal |= TJA110X_REG_COMMON_CONFIG_AUTO_MASK;
    //         break;
    //     case PHY_MODE_MANAGED:
    //         regVal &= ~TJA110X_REG_COMMON_CONFIG_AUTO_MASK;
    //         break;
    // }

    // this->write_reg(TJA110X_REG_COMMON_CONFIG, regVal);

    // return this->get_phy_mode();
    return 0;
}

uint8_t Internal_Phy::get_phy_mode()
{
    // uint16_t regVal;
    // uint8_t phyModeBit;

    // regVal = this->read_reg(TJA110X_REG_COMMON_CONFIG);

    // phyModeBit = (regVal & TJA110X_REG_COMMON_CONFIG_AUTO_MASK) >> TJA110X_REG_COMMON_CONFIG_AUTO_OFFSET;

    // switch(phyModeBit)
    // {
    //     case 0:
    //         return PHY_MODE_MANAGED;
    //     case 1:
    //         return PHY_MODE_AUTO;
    //     default:
    //         return PHY_MODE_INVALID;
    // }
    return 0;
}

bool Internal_Phy::get_link_control()
{
    // uint16_t regVal;

    // regVal = this->read_reg(TJA110X_REG_EXT_CONTROL);

    // if ((regVal & TJA110X_REG_COM_STATUS_LINK_UP) > 0)
    //     return true;

    return false;


}

bool Internal_Phy::set_link_control(bool enable)
{
    uint16_t regVal;

    // regVal = this->read_reg(TJA110X_REG_EXT_CONTROL);

    // if (enable)
    //     regVal |= TJA110X_REG_COM_STATUS_LINK_UP;
    // else
    //     regVal &= ~TJA110X_REG_COM_STATUS_LINK_UP;

    // this->write_reg(TJA110X_REG_EXT_CONTROL, regVal);

    return get_link_control();
}


bool Internal_Phy::get_link_status()
{
    uint16_t regVal;

    // regVal = this->read_reg(TJA110X_REG_COM_STATUS);

    // if ((regVal & TJA110X_REG_COM_STATUS_LINK_UP) > 0)
    //     return true;

    return false;

}

uint8_t Internal_Phy::get_sqi_value()
{
    // uint16_t regVal;

    // regVal = this->read_reg(TJA110X_REG_COM_STATUS);

    // return (regVal & TJA110X_REG_COM_SNR_MASK) >> TJA110X_REG_COM_SNR_OFFSET;
    return 0;

}

bool Internal_Phy:: check_polarity()
{
    // uint16_t regVal;

    // regVal = this->read_reg(TJA110X_REG_EXTERNAL_STATUS);

    // return ((regVal & TJA110X_REG_EXTERNAL_STATUS_POLARITY_MASK) == 0);     // if 0, return True (means no cable crossing detected)
    return false;

}

// ---------------------------------------------------------------------------------------------------------------------------
// private functions to access registers via SMI. Still active for simpler test, but recommended is using properties/functions
// ---------------------------------------------------------------------------------------------------------------------------

uint16_t Internal_Phy::read_reg(uint8_t registerAddress)
{
    uint8_t mmd = 0;
    uint16_t addr = 0;
    uint16_t temp = 0;
    
    _pPhyInterface->read_phy_reg(this->_phyId, registerAddress,&temp);
    

    if(this->_debugEnable)
        Serial.printf("IntPhy-read_reg ID:%02X REG:%04X - RESULT:%04X\r\n", this->_phyId, registerAddress, temp);

    return temp;
}

void Internal_Phy::write_reg(uint8_t registerAddress, uint16_t data)
{

    _pPhyInterface->write_phy_reg(this->_phyId, registerAddress, data);
    

    if(this->_debugEnable)
        Serial.printf("IntPhy-write_reg ID:%02X REG:%04X - DATA:%04X\r\n", this->_phyId, registerAddress, data);
}

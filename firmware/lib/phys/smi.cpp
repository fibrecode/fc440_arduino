

#include "smi.h"


SmiBitbang::SmiBitbang()
{

}

void SmiBitbang::begin(int pin_mdc, int pin_mdio)
{
    this->_pin_mdc = pin_mdc;
    this->_pin_mdio = pin_mdio;

    pinMode(this->_pin_mdc, OUTPUT_12MA);
    pinMode(this->_pin_mdio, INPUT);
}

uint16_t SmiBitbang::readCL22(uint8_t portAddress, uint8_t registerAddress)
{
    //Read and return the data
    return this->read(portAddress, registerAddress, 0);
}

void SmiBitbang::writeCL22(uint8_t portAddress, uint8_t registerAddress, uint16_t registerValue)
{
    this->write(portAddress, registerValue, 0, 0, 0);
}
uint16_t SmiBitbang::readCL45(uint8_t portAddress, uint8_t deviceAddress, uint16_t registerAddress)
{
    //Send Address Message
    this->write(portAddress, deviceAddress, registerAddress, 1, 1, 0);
    //Read and return the data
    return this->read(portAddress, deviceAddress, 1);
}

void SmiBitbang::writeCL45(uint8_t portAddress, uint8_t deviceAddress, uint16_t registerAddress, uint16_t registerValue)
{
    //Send Address Message
    this->write(portAddress, deviceAddress, registerAddress, 1, 1, 0);

    this->write(portAddress, deviceAddress, registerValue, 1, 0, 0);
}
    

uint16_t SmiBitbang::read(uint8_t phy_id, uint8_t reg, uint8_t isCL45)
{
    uint8_t byte = 0;
    uint16_t word, data;

    //We need to send first, so MDIO is OUTPUT
    pinMode(this->_pin_mdio, OUTPUT_12MA);
    sleep_us(1);
    // //Send Preamble
    digitalWrite(this->_pin_mdio, 1);
    for(byte = 0; byte < 32; byte++)
    {
        this->clock_pulse();
    }

    if(isCL45 == 0){
        // STAT CODE / MESSAGE TYPE
        digitalWrite(this->_pin_mdio, 0);
        this->clock_pulse();
        digitalWrite(this->_pin_mdio, 1);
        this->clock_pulse();

        // OP-CODE / READ
        digitalWrite(this->_pin_mdio, 1);
        this->clock_pulse();
        digitalWrite(this->_pin_mdio, 0);
        this->clock_pulse();
    }else{
        // STAT CODE / MESSAGE TYPE in Clause 45 this is 00
        digitalWrite(this->_pin_mdio, 0);
        this->clock_pulse();
        digitalWrite(this->_pin_mdio, 0);
        this->clock_pulse();

        // OP-CODE / READ in Clause 45 this is 11
        digitalWrite(this->_pin_mdio, 1);
        this->clock_pulse();
        digitalWrite(this->_pin_mdio, 1);
        this->clock_pulse();
    }

    //Write PHY Address (5 Bit)
    for(byte=0x10; byte != 0;){
        digitalWrite(this->_pin_mdio, (byte & phy_id) > 0 ? 1 : 0);
        this->clock_pulse();
        byte = byte >> 1;
    }

    //Write REG Address (5 Bit)
    for(byte=0x10; byte != 0;){
        digitalWrite(this->_pin_mdio, (byte & reg) > 0 ? 1 : 0);
        this->clock_pulse();
        byte = byte >> 1;
    }

    //TA -> Turn Around Time (switching to Read, 2 Bit)
    pinMode(this->_pin_mdio, INPUT);
    this->clock_pulse();
    this->clock_pulse();

    //Read 16 Bit Data
    data = 0;
    for(word=0x8000;word != 0;){
        if(digitalRead(this->_pin_mdio))
            data |= word;

        this->clock_pulse();
        word = word >> 1;
    }

    this->clock_pulse();

    return data;
}


void SmiBitbang::write(uint8_t phy_id, uint8_t reg, uint16_t data, uint8_t isCL45, uint8_t isAddress, uint8_t leaveBusActive)
{
    uint8_t byte = 0;
    uint16_t word;

    //We need to send first, so MDIO is OUTPUT
    pinMode(this->_pin_mdio, OUTPUT_12MA);
    sleep_us(1);
    //Send Preamble
    digitalWrite(this->_pin_mdio, 1);
    for(byte = 0; byte < 32; byte++)
    {
        this->clock_pulse();
    }

    if(isCL45 == 0){
        // STAT CODE / MESSAGE TYPE
        digitalWrite(this->_pin_mdio, 0);
        this->clock_pulse();
        digitalWrite(this->_pin_mdio, 1);
        this->clock_pulse();

        // OP-CODE / READ
        digitalWrite(this->_pin_mdio, 0);
        this->clock_pulse();
        digitalWrite(this->_pin_mdio, 1);
        this->clock_pulse();
    }else{
        // STAT CODE / MESSAGE TYPE in Clause 45 this is 00
        digitalWrite(this->_pin_mdio, 0);
        this->clock_pulse();
        digitalWrite(this->_pin_mdio, 0);
        this->clock_pulse();

        // OP-CODE / READ in Clause 45 this can be Address or Write in "Write Mode"
        digitalWrite(this->_pin_mdio, 0);
        this->clock_pulse();
        digitalWrite(this->_pin_mdio, isAddress == 0 ? 1 : 0);
        this->clock_pulse();
    }

    //Write PHY Address (5 Bit)
    for(byte=0x10; byte != 0;){
        digitalWrite(this->_pin_mdio, (byte & phy_id) > 0 ? 1 : 0);
        this->clock_pulse();
        byte = byte >> 1;
    }

    //Write REG Address (5 Bit)
    for(byte=0x10; byte != 0;){
        digitalWrite(this->_pin_mdio, (byte & reg) > 0 ? 1 : 0);
        this->clock_pulse();
        byte = byte >> 1;
    }

    //TA -> Turn Around Time (switching stay Write, 2 Bit)
    digitalWrite(this->_pin_mdio, 1);
    this->clock_pulse();
    digitalWrite(this->_pin_mdio, 0);
    this->clock_pulse();

    //Write 16 Bit Data    
    for(word=0x8000;word != 0;){
        digitalWrite(this->_pin_mdio, (word & data) > 0 ? 1 : 0);
        this->clock_pulse();
        word = word >> 1;
    }

    if(leaveBusActive == 0){

    digitalWrite(this->_pin_mdio, 1);
    this->clock_pulse();
    
    //go back to  input / idle
    pinMode(this->_pin_mdio, INPUT);
    }
}

void SmiBitbang::clock_pulse()
{
    digitalWrite(this->_pin_mdc, 0);
    sleep_us(2);
    //asm("nop");
    digitalWrite(this->_pin_mdc, 1);
    sleep_us(2);
    //asm("nop");
}

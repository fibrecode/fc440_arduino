

#include "Lan9303.h"
#include "fc1020.h"
#include "string"


#include "lan9303_registers.h"


Lan9303::Lan9303(uint8_t i2cAddr, bool debugEnable) 
{
    _i2cAddress = i2cAddr;
    _debugEnable = debugEnable;
}

void Lan9303::start()
{
  //Disable special VLAN tagging mode
  write_switch_reg(LAN9303_SWE_INGRSS_PORT_TYP, 0);

  //Revert to default configuration
  write_switch_reg(LAN9303_BM_EGRSS_PORT_TYPE, 0);

  //Disable port mirroring
  write_switch_reg(LAN9303_SWE_PORT_MIRROR, 0);
  
  //Configure port state
  write_switch_reg(LAN9303_SWE_PORT_STATE,
    LAN9303_SWE_PORT_STATE_PORT0_FORWARDING |
    LAN9303_SWE_PORT_STATE_PORT1_FORWARDING |
    LAN9303_SWE_PORT_STATE_PORT2_FORWARDING);

  uint32_t data;
  read_switch_reg(LAN9303_SWE_PORT_STATE, &data);

  Serial.printf("LAN SWE:PORT STATE : 0x%X",data);
}


void Lan9303::init(TwoWire* pWire)
{
    _pI2c = pWire;
}



void Lan9303::port2Config()
{

}

struct DebugRegister
{
    uint32_t addr;
    std::string text;
};

DebugRegister registersMap[] =
  {
    {LAN9303_MAC_TX_CFG_0,"LAN9303_MAC_TX_CFG_0"},
    {LAN9303_MAC_RX_CFG_0,"LAN9303_MAC_RX_CFG_0"},
    {LAN9303_MAC_TX_CFG_1,"LAN9303_MAC_TX_CFG_1"},
    {LAN9303_MAC_RX_CFG_1,"LAN9303_MAC_RX_CFG_1"},
    {LAN9303_MAC_TX_CFG_2,"LAN9303_MAC_TX_CFG_2"},
    {LAN9303_MAC_RX_CFG_2,"LAN9303_MAC_RX_CFG_2"},
    {LAN9303_MAC_RX_GOODPKTLEN_CNT_0,"LAN9303_MAC_RX_GOODPKTLEN_CNT_0"},
    {LAN9303_MAC_RX_GOODPKTLEN_CNT_1,"LAN9303_MAC_RX_GOODPKTLEN_CNT_1"},
    {LAN9303_MAC_RX_GOODPKTLEN_CNT_2,"LAN9303_MAC_RX_GOODPKTLEN_CNT_2"},
    {LAN9303_MAC_TX_PKTLEN_CNT_0,"LAN9303_MAC_TX_PKTLEN_CNT_0"},
    {LAN9303_MAC_TX_PKTLEN_CNT_1,"LAN9303_MAC_TX_PKTLEN_CNT_1"},
    {LAN9303_MAC_TX_PKTLEN_CNT_2,"LAN9303_MAC_TX_PKTLEN_CNT_2"},
    {LAN9303_MAC_TX_DEFER_CNT_0,"LAN9303_MAC_TX_DEFER_CNT_0"},
    {LAN9303_MAC_TX_DEFER_CNT_1,"LAN9303_MAC_TX_DEFER_CNT_1"},
    {LAN9303_MAC_TX_DEFER_CNT_2,"LAN9303_MAC_TX_DEFER_CNT_2"},
    {LAN9303_MAC_TX_TOTALCOL_CNT_0,"LAN9303_MAC_TX_TOTALCOL_CNT_0"},
    {LAN9303_MAC_TX_TOTALCOL_CNT_1,"LAN9303_MAC_TX_TOTALCOL_CNT_1"},
    {LAN9303_MAC_TX_TOTALCOL_CNT_2,"LAN9303_MAC_TX_TOTALCOL_CNT_2"},
    
    
    
    {LAN9303_MAC_RX_PKTOK_CNT_0,"LAN9303_MAC_RX_PKTOK_CNT_0"},
    {LAN9303_MAC_RX_PKTOK_CNT_1,"LAN9303_MAC_RX_PKTOK_CNT_1"},
    {LAN9303_MAC_RX_PKTOK_CNT_2,"LAN9303_MAC_RX_PKTOK_CNT_2"},
    {LAN9303_MAC_TX_PKTOK_CNT_0,"LAN9303_MAC_TX_PKTOK_CNT_0"},
    {LAN9303_MAC_TX_PKTOK_CNT_1,"LAN9303_MAC_TX_PKTOK_CNT_1"},
    {LAN9303_MAC_TX_PKTOK_CNT_2,"LAN9303_MAC_TX_PKTOK_CNT_2"},
  };

const uint16_t registersMapSize = sizeof(registersMap) / sizeof(registersMap[0]);

void Lan9303::cyclic()
{
  //print debug registers
  Serial.printf("SWitch printing %i registers\r\n", registersMapSize);
  for (uint8_t i = 0; i< registersMapSize; i++)
    {
      uint32_t data=0xAFFE;
      uint32_t regAddr = registersMap[i].addr;
      read_switch_reg(regAddr, &data);
      Serial.printf("Reg %s: 0x%x\r\n", registersMap[i].text, data);
    }

    //read 
    uint32_t macAddr=0xAFFE;
    read(LAN9303_SWITCH_MAC_ADDRH, &macAddr);
    Serial.printf("MAC H: 0x%x\r\n", macAddr);
    read(LAN9303_SWITCH_MAC_ADDRL, &macAddr);
    Serial.printf("MAC L: 0x%x\r\n", macAddr);

}

HW_CFG Lan9303::getHWConfig()
{
    HW_CFG retVal;
    read(REG_HW_CFG, &retVal.raw);
    return retVal;
}

uint16_t Lan9303::read_phy_reg(uint8_t phyAddr, uint8_t registerAddress, uint16_t* data)
{
    uint32_t temp = 0;
    temp = ((phyAddr & 0x1F) << 11) | ((registerAddress & 0x1F) << 6);
    write(0x0A8, temp);
    
    temp = 0x1;
    while(temp & 0x01){
      read(0x0A8, &temp);
      //Serial.print("I2C->MDIM busy\n\r");
    }
    
    read(0x0A4, &temp);
  
    *data = (uint16_t)(temp & 0xFFFF);
    return 0;
}

void Lan9303::write_phy_reg(uint8_t phyAddr,uint8_t registerAddress, uint16_t data)
{
    uint32_t temp = 0;
    temp = ((phyAddr & 0x1F) << 11) | ((registerAddress & 0x1F) << 6) | (1<<1);

    while(temp & 0x01)
    {
        //Serial.print("I2C->MDIM init wr busy\n\r");
        read(0x0A8, &temp);
    }
    write(0x0A4, data); //first data 
    write(0x0A8, temp); //then cmd
    
    temp = 0x1;
    while(temp & 0x01)
    {
        //Serial.print("I2C->MDIM wr busy\n\r");
        read(0x0A8, &temp);
    }
  
  return;
}



uint16_t Lan9303::read_switch_reg(uint16_t address, uint32_t* data)
{
  uint32_t value;
  
  //Set up a read operation
  value = LAN9303_SWITCH_CSR_CMD_BUSY | LAN9303_SWITCH_CSR_CMD_READ |
     LAN9303_SWITCH_CSR_CMD_BE;

  //Set register address
  value |= address & LAN9303_SWITCH_CSR_CMD_ADDR;

  //To perform a read of an individual switch fabric register, the read cycle
  //must be initiated by performing a single write to the SWITCH_CSR_CMD
  //register
  write(LAN9303_SWITCH_CSR_CMD, value);

  //Valid data is available for reading when the CSR_BUSY bit is cleared
  do
  {
     //Read SWITCH_CSR_CMD register
     read(LAN9303_SWITCH_CSR_CMD,&value);

     //Poll CSR_BUSY bit
  } while((value & LAN9303_SWITCH_CSR_CMD_BUSY) != 0);

  //Read data from the SWITCH_CSR_DATA register
  read(LAN9303_SWITCH_CSR_DATA, data);
  return 0;
}

void Lan9303::write_switch_reg(uint16_t registerAddress, uint32_t data)
{
  uint32_t value;
  
  //To perform a write to an individual switch fabric register, the desired
  //data must first be written into the SWITCH_CSR_DATA register
  write(LAN9303_SWITCH_CSR_DATA, data);

  //Set up a write operation
  value = LAN9303_SWITCH_CSR_CMD_BUSY | LAN9303_SWITCH_CSR_CMD_WRITE |
     LAN9303_SWITCH_CSR_CMD_BE;

  //Set register address
  value |= registerAddress & LAN9303_SWITCH_CSR_CMD_ADDR;

  //The write cycle is initiated by performing a single write to the
  //SWITCH_CSR_CMD register
  write(LAN9303_SWITCH_CSR_CMD, value);

  //The completion of the write cycle is indicated by the clearing of the
  //CSR_BUSY bit
  do
  {
     //Read SWITCH_CSR_CMD register
     read(LAN9303_SWITCH_CSR_CMD, &value);

     //Poll CSR_BUSY bit
  } while((value & LAN9303_SWITCH_CSR_CMD_BUSY) != 0);

}


int Lan9303::write(uint32_t address, uint32_t data)
{
  uint8_t addr = (uint8_t)(address >> 2);
  uint8_t buffer[5];

  buffer[0] = addr;
  buffer[1] = (uint8_t)((data >> 24) & 0xFF);
  buffer[2] = (uint8_t)((data >> 16) & 0xFF);
  buffer[3] = (uint8_t)((data >> 8) & 0xFF);
  buffer[4] = (uint8_t)((data >> 0) & 0xFF);

  _pI2c->beginTransmission(_i2cAddress);
  _pI2c->write(buffer, 5);
  _pI2c->endTransmission();

  return 0;
}

int Lan9303::read(uint32_t address, uint32_t * data)
{
  uint8_t addr = (uint8_t)(address >> 2);
  uint8_t temp[4];
  size_t bytesRead = 0;
  int failCnt = 0;

  _pI2c->beginTransmission(_i2cAddress);
  _pI2c->write(addr);
  _pI2c->endTransmission();

  bytesRead = _pI2c->requestFrom(_i2cAddress, 4, true);

  while(!_pI2c->available()){
    failCnt++;
    Serial.println("wait for data ...");
    sleep_ms(100);
    if(failCnt > 5)
    {
      *data = 0;
      return -1;
    }
  }
  temp[0] = _pI2c->read();
  temp[1] = _pI2c->read();
  temp[2] = _pI2c->read();
  temp[3] = _pI2c->read();

  Serial.printf("wait for raw data ...0x%X, 0x%X, 0x%X, 0x%X\n\r",temp[0],temp[1],temp[2],temp[3]);

  *data = (temp[0] << 24) | (temp[1] << 16) | (temp[2] << 8) | temp[3];
  return 0;
}
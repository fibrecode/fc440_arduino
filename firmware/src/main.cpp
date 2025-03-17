#include <Arduino.h>
#include <Wire.h>

#include "elapsedMillis.h"
#include "fc1020.h"
#include "dp83td510e.h"
#include "adin1100.h"
#include "tja1101.h"
#include "InternalPhy.h"
#include "Lan9303.h"
#include "smi.h"
#include "leds.h"

#define SWITCH_ADDRESS  0x0A

#define PHY_ADDRESS_INTERNAL  3
#define PHY_ADDRESS_100M_T1   2
#define PHY_ADDRESS_10M_TI    8
#define PHY_ADDRESS_10M_ADI   0

#define PHY_MOVE_ADD 2
//Forward declarations
void dipswitch_setup();
void led_setup();
int lan9303_read(uint32_t address, uint32_t * data);
int lan9303_write(uint32_t address, uint32_t data);
int lan9303_read_phy(uint16_t phyAddress, uint16_t phyRegister, uint16_t * data);
int lan9303_write_phy(uint16_t phyAddress, uint16_t phyRegister, uint16_t data);
void i2c_scan();
void print_bits32(uint32_t value);
void print_bits32_ex(uint32_t value);
uint16_t dp83td510e_readCL45(uint8_t portAddress, uint16_t registerAddress);


SmiBitbang smi;
DP83TD510E_Control dp83td;
ADIN1100_Control adin1100;
TJA1101_Control tja1101;
Internal_Phy internalPhy;
elapsedMillis debugTimer;
elapsedMillis phyPollTimer;
BoardLed LED1;
BoardLed LED2;
Lan9303 lanSwitch(SWITCH_ADDRESS, true);

uint8_t lastDip[4] = {0xFF, 0xFF, 0xFF, 0xFF};

void setup() {
  sleep_ms(2200);
  
  uint32_t temp = 0;
  pinMode(PIN_SMI_MDC, INPUT);
  pinMode(PIN_SMI_MDIO, INPUT);
  led_setup();
  dipswitch_setup();

 
  smi.begin(PIN_SMI_MDC, PIN_SMI_MDIO);

  Wire.setSDA(PIN_I2C_SDA);
  Wire.setSCL(PIN_I2C_SCL);
  Wire.begin();

  lanSwitch.init(&Wire);

  //dp83td.begin(&smi, PHY_ADDRESS_10M_TI, &LED1, true);
  adin1100.begin(&smi, PHY_ADDRESS_10M_ADI, &LED1, true);

  tja1101.begin(&smi, PHY_ADDRESS_100M_T1, &LED2, true);

  internalPhy.begin(&lanSwitch, PHY_ADDRESS_INTERNAL, true);
  //internalPhy.begin(&smi, 5, true);

  Serial.begin();
  Serial.print("Booting ");
  Serial.println(APPLICATION_NAME);

  LED1.setLed(BOARD_LED_COLOR_ORANGE, BOARD_LED_MODE_BLINK_SLOW);
  LED2.setLed(BOARD_LED_COLOR_ORANGE, BOARD_LED_MODE_FLASHING);

  

  Serial.println("ready.");
  
  sleep_ms(2200);  // -------------------------------------------------Extend here to debug stRTUP USING serial port.

  #define EEPROM_DATA 0x1B8
  Serial.println("Wait for the Switch zu timeout the EEPROM");
  sleep_ms(500);
  lanSwitch.read(0x1EC, &temp);
  Serial.print("Port1 CONFIG: ");
  print_bits32(temp);
  Serial.println("");


  sleep_ms(5000);
  uint32_t temp32;
  // lanSwitch.read(EEPROM_DATA, &temp32);
  // Serial.printf("PHY reading Reset CTL: 0x%X \n\r",temp32);
  // temp32 |= 0xAA;
  // lanSwitch.write(EEPROM_DATA, temp32);
  // lanSwitch.read(EEPROM_DATA, &temp32);
  // Serial.printf("PHY reading Reset CTL: 0x%X \n\r",temp32);
  // lanSwitch.read(EEPROM_DATA, &temp32);
  // Serial.printf("PHY reading Reset CTL: 0x%X \n\r",temp32);
  // lanSwitch.read(EEPROM_DATA, &temp32);
  // Serial.printf("PHY reading Reset CTL: 0x%X \n\r",temp32);
  // sleep_ms(1000);

  uint16_t regAdd;
  lanSwitch.read(0x1F8, &temp32);
  Serial.printf("PHY reading Reset CTL: 0x%X \n\r",temp32);
  //switch Port 2 Phy to address 5
  // Serial.print("PHY writing CONFIG: to INT_PHY \n\r");
  // lanSwitch.read_phy_reg(PHY_ADDRESS_INTERNAL, 0, &regAdd);
  // regAdd |= (1 << 15);
  // lanSwitch.write_phy_reg(PHY_ADDRESS_INTERNAL, 0, regAdd);
  // sleep_ms(1000);
  

  // lanSwitch.write_phy_reg(PHY_ADDRESS_INTERNAL, 27, 0xC000);
  // #define PHY_ID_H 2
  // lanSwitch.read_phy_reg(PHY_ADDRESS_INTERNAL, 0, &regAdd);
  // Serial.printf("PHY CONFIG read: 0x%X \n\r",regAdd);

  //regAdd |= (1 << 9);
  //lanSwitch.write_phy_reg(PHY_ADDRESS_INTERNAL, 0, regAdd);
  //sleep_ms(1000);
  //lanSwitch.read_phy_reg(PHY_ADDRESS_INTERNAL, 0, &regAdd);
  //Serial.printf("PHY CONFIG read: 0x%X \n\r",regAdd);




  //lan9303_write_phy(2, 18, (0x7 << 5) | 0x3);

  Serial.println("Go.");

  //dp83td.default_setup();
  adin1100.default_setup();
  tja1101.default_setup();
  internalPhy.default_setup();

  lanSwitch.start();
}

void loop() {
  uint32_t temp = 0;
  uint16_t phytemp = 0;
  uint8_t j, dip;


  if(phyPollTimer > 500)
  {
    for (j=0;j<4;j++)
    {
      dip = digitalRead(PIN_DIPSW1+j);

      // Serial.printf("DIPSW: %d %04X\r\n", j, dip); 

      if (dip != lastDip[j])    // change detected
      {
        Serial.printf("DIPSW Changed: %d %04X (last:%04X)\r\n", j, dip, lastDip[j]); 
        switch(j)
        {
          case 0: // 100T1 Auto-Mode Master-Slave
              tja1101.set_phy_role_auto(dip==0);    // Low-Active: 0=ON
            break;
          case 1: // Master/Slave 100T1 (has no effect if Auto-Mode is active)
            if (dip == 0)
              tja1101.set_phy_role_req(PHY_ROLE_MASTER);
            else
              tja1101.set_phy_role_req(PHY_ROLE_SLAVE);
            break;
          case 2: // Force Master
            adin1100.set_phy_role_force_master(dip==0);
            break;
          case 3: // Advertise Master=0; get slave on local
            adin1100.set_phy_role_master_off(dip==0);

          default:
            break;
        }

      }
      lastDip[j] = dip;

    }


    //dp83td.process();
    adin1100.process();

    tja1101.process();

    internalPhy.process();

    lanSwitch.cyclic();

    HW_CFG status = lanSwitch.getHWConfig();
    Serial.printf("lan9303 device Ready: 0x%x\r\n", status.DEVICE_READY);
   

    

    // lan9303_read_phy(3,3, &temp);
    // Serial.printf("IntPhy CHECK addres id: 0x%x\r\n", temp);

    // uint32_t temp32;
    // lan9303_read(0x50, &temp32);
    // Serial.printf("lan9303_read 50: 0x%x\r\n", temp32);

    phyPollTimer = 0;
  }

  if(debugTimer > 2000)
  {
    // for(j = 0; j<8; j++)
    // {
    //   phytemp = smi.read(j, 1, 0);
    //   Serial.printf("PhyAd: %i - %04X\r\n", j, phytemp);
    // }
    // lan9303_read(0x050, &temp);
    // Serial.print("ID_REV: ");  
    // Serial.printf("0x%08X\r\n", temp);

    // lan9303_read(0x064, &temp);
    // Serial.print("BYTE_TEST: ");  
    // Serial.printf("0x%08X\r\n", temp);

    // lan9303_read(0x074, &temp);
    // Serial.print("HW_CFG: ");  
    // Serial.printf("0x%08X\r\n", temp); 

      //Serial.println("---------------------------------------------------------------\r\n");
    // for(int i = 0; i<16;i++){    
      //  lan9303_read_phy(i, 0, &mi_control);
      //  lan9303_read_phy(i, 2, &mi_id0);
      //  lan9303_read_phy(i, 3, &mi_id1);
    //   Serial.printf("PHY%02i - MI_CONTROL: %04X  MI_PHYID0: %04X  MI_PHYID1: %04X\r\n", i, mi_control, mi_id0, mi_id1);    
    // }
    // mi_control = smi.read(PHY_ADDRESS_10M_TI, 0);
    // mi_id0 = smi.read(PHY_ADDRESS_10M_TI, 2);
    // mi_id1 = smi.read(PHY_ADDRESS_10M_TI, 3);
    // mi_an_control = dp83td.readExtendedReg(DP83TD510E_REG_AN_CONTROL);
    // mi_an_status = dp83td.readExtendedReg(DP83TD510E_REG_AN_STATUS);

      // Serial.printf("MI_CONTROL: %04X  MI_PHYID0: %04X  MI_PHYID1: %04X  AN_CONTROL: %04X  AN_STATUS: %04X\r\n", mi_control, mi_id0, mi_id1, mi_an_control, mi_an_status);

      // phytemp = dp83td.readExtendedReg(DP83TD510E_REG_PHY_STS);
      // Serial.print("10B-T1L LINK: ");
      // Serial.println((phytemp & 0x1) > 0 ? "1" : "0");

      // phytemp = tja1101.read_reg(TJA110X_REG_BSR);
      // Serial.print("100B-T1 LINK: ");
      // Serial.println((phytemp & 0x1) > 0 ? "1" : "0");

      // for (j=0;j<4;j++)
      // {
      //   Serial.printf("DIPSW: %d %04X\r\n", j, digitalRead(PIN_DIPSW1)); 
      // }

      // Serial.println("---------------------------------------------------------------\r\n");
      // Serial.println("");
      //Serial.printf("HF my stuff");
      debugTimer = 0;
  }

  LED1.process();
  LED2.process();

}


void led_setup()
{
  pinMode(PIN_LED1_RED, OUTPUT_12MA);
  pinMode(PIN_LED1_GREEN, OUTPUT_12MA);
  pinMode(PIN_LED2_RED, OUTPUT_12MA);
  pinMode(PIN_LED2_GREEN, OUTPUT_12MA);

  digitalWrite(PIN_LED1_RED, LOW);
  digitalWrite(PIN_LED1_GREEN, LOW);
  digitalWrite(PIN_LED2_RED, LOW);
  digitalWrite(PIN_LED2_GREEN, LOW);

  LED1.begin(PIN_LED1_RED, PIN_LED1_GREEN);
  LED2.begin(PIN_LED2_RED, PIN_LED2_GREEN);
}

void dipswitch_setup()
{
  pinMode(PIN_DIPSW1, INPUT_PULLUP);
  pinMode(PIN_DIPSW2, INPUT_PULLUP);
  pinMode(PIN_DIPSW3, INPUT_PULLUP);
  pinMode(PIN_DIPSW4, INPUT_PULLUP);
}

void print_bits32(uint32_t value)
{
  for(int i = 31; i>=0;i--)
  {
    if((i+1)%4 == 0) {
      Serial.print(" ");
    }
    Serial.print((value & (1<<i)) > 0 ? "1" : "0");    
  }
}

void print_bits32_ex(uint32_t value)
{
  for(int i = 31; i>=0;i--)
  {
    if(((i+1)%4 == 0) && (i != 31)) {
      Serial.print("| ");
    }
    Serial.printf("%02i ", i);    
  }
  Serial.println("");

  for(int i = 31; i>=0;i--)
  {
    if(((i+1)%4 == 0) && (i != 31)) {
      Serial.print("| ");
    }    
    Serial.print(" ");
    Serial.print((value & (1<<i)) > 0 ? "1" : "0");    
    Serial.print(" ");    
  }
  Serial.println("");
}



uint16_t dp83td510e_readCL45(uint8_t portAddress, uint16_t registerAddress)
{
  //get the right DEVAD / MMD Address for the given register
  uint16_t addr = registerAddress;
  uint8_t devAddress = 0x1F;
  if((registerAddress >= 0x3000) && (registerAddress <= 0x38E7)){
    devAddress = 0x03;
    addr -= 0x3000;
  }else if((registerAddress >= 0x200) && (registerAddress <= 0x20F)){
    devAddress = 0x07;
  }else if((registerAddress >= 0x1000) && (registerAddress <= 0x18F8)){
    devAddress = 0x01;
    addr -= 0x1000;
  }
  smi.write(portAddress, 0x000D, devAddress);
  smi.write(portAddress, 0x000E, addr);
  smi.write(portAddress, 0x000D, 0x4000 | devAddress);
  return smi.read(portAddress, 0x000E);

  //return smi.readCL45(portAddress, devAddress, registerAddress);
}

void i2c_scan()
{
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}
 
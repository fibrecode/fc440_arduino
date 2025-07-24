#include <Arduino.h>
#include <Wire.h>

#include "elapsedMillis.h"
#include "fc1020.h"
#include "dp83td510e.h"
#include "adin1100.h"
#include "tja1101.h"
#include "Lan9303InternalPhy.h"
#include "Lan9303.h"
#include "smi.h"
#include "leds.h"

#define SWITCH_ADDRESS  0x0A

#define PHY_ADDRESS_INTERNAL  3
#define PHY_ADDRESS_100M_T1   2
#define PHY_ADDRESS_10M_TI    8
#define PHY_ADDRESS_10M_ADI   1

#define PHY_MOVE_ADD 2

// Static configure 10BASE-T1L
// TI for DP83TD510E
// undef TI for ADI ADIN1100

#define _TI
//Forward declarations
void dipswitch_setup();
void led_setup();

void i2c_scan();
void print_bits32(uint32_t value);
void print_bits32_ex(uint32_t value);


SmiBitbang smi;
DP83TD510E_Control dp83td;
ADIN1100_Control adin1100;
TJA1101_Control tja1101;
Lan9303InternalPhy internalPhy;
elapsedMillis debugTimer;
elapsedMillis phyPollTimer;
BoardLed LED1;
BoardLed LED2;
Lan9303 lanSwitch(SWITCH_ADDRESS, true);

uint8_t lastDip[4] = {0xFF, 0xFF, 0xFF, 0xFF};

void setup()
{
  //sleep_ms(2200);
  
  uint32_t temp = 0;
  pinMode(PIN_SMI_MDC, INPUT);
  pinMode(PIN_SMI_MDIO, INPUT);
  led_setup();
  dipswitch_setup();

 
  smi.begin(PIN_SMI_MDC, PIN_SMI_MDIO);

  Wire.setSDA(PIN_I2C_SDA);
  Wire.setSCL(PIN_I2C_SCL);
  Wire.setClock(50000);
  Wire.begin();

  lanSwitch.init(&Wire);

  #ifdef _TI
    dp83td.begin(&smi, PHY_ADDRESS_10M_TI, &LED1, true);
  #else
    adin1100.begin(&smi, PHY_ADDRESS_10M_ADI, &LED1, true);
  #endif

  tja1101.begin(&smi, PHY_ADDRESS_100M_T1, &LED2, true);

  
  internalPhy.begin(&lanSwitch, PHY_ADDRESS_INTERNAL, true);

  Serial.begin();
  Serial.print("Booting ");
  Serial.println(APPLICATION_NAME);

  LED1.setLed(BOARD_LED_COLOR_ORANGE, BOARD_LED_MODE_BLINK_SLOW);
  LED2.setLed(BOARD_LED_COLOR_ORANGE, BOARD_LED_MODE_FLASHING);

  Serial.println("ready.");
  
  //sleep_ms(2200);  // -------------------------------------------------Extend here to debug startup using serial port.

  #define EEPROM_DATA 0x1B8
  Serial.println("Wait for the Switch / timeout the EEPROM");
  sleep_ms(100);
  lanSwitch.read(0x1EC, &temp);
  Serial.print("Port1 CONFIG: ");
  print_bits32(temp);
  Serial.println("");


  //sleep_ms(5000);
  uint32_t temp32;
   lanSwitch.read(0x50, &temp32);
   Serial.printf("reading switch id: 0x%X \n\r",temp32);

  uint16_t regAdd;
  lanSwitch.read(0x1F8, &temp32);
  Serial.printf("PHY reading Reset CTL: 0x%X \n\r",temp32);


  Serial.println("Go.");
  #ifdef _TI
    dp83td.default_setup();
  #else
    adin1100.default_setup();
  #endif

  tja1101.default_setup();
  internalPhy.default_setup();

  lanSwitch.start();
}

void loop() {
  uint32_t temp = 0;
  uint16_t phytemp[8] = {0};
  uint8_t j, dip;
  uint8_t phyAddr;


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
            #ifdef _TI
              dp83td.set_phy_role_force_master(dip==0);
            #else
              adin1100.set_phy_role_force_master(dip==0);
            #endif
            
            break;
          case 3: // Advertise Master=0; get slave on local
            #ifdef _TI
              dp83td.set_phy_role_master_off(dip==0);
            #else
              adin1100.set_phy_role_master_off(dip==0);
            #endif
            
          default:
            break;
        }

      }
      lastDip[j] = dip;

    }

    Serial.printf("phy process loop\n\r");
    #ifdef _TI
      dp83td.process();
    #else
      adin1100.process();
    #endif

    tja1101.process();

    internalPhy.process();

    lanSwitch.cyclic();

    HW_CFG status = lanSwitch.getHWConfig();
    Serial.printf("lan9303 device Ready: 0x%x\r\n", status.DEVICE_READY);

    phyPollTimer = 0;
  }

  if(debugTimer > 2000)
  {
    // for(phyAddr = 0; phyAddr<9; phyAddr++)
    // {
    //   phytemp[1] = smi.read(phyAddr, 1, 0);
    //   phytemp[2] = smi.read(phyAddr, 2, 0);
    //   phytemp[3] = smi.read(phyAddr, 3, 0);
    //   Serial.printf("PhyAd: %i - %04X.%04X.%04X\r\n", phyAddr, phytemp[1], phytemp[2], phytemp[3]);
    // }
    
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
 
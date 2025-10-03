# FibreCode FC1020 Module
Small and compact switch module using:
* RP2040 - control switch and Ethernet-Phys
* Microchip LAN9303Mi - 3-port switch incl. 100BASE-TX and 2x MII-Ports
* NXP TJA1101BHN for 100BASE-T1 (SPE-Port IEEE802.3bw)
* Analog ADIN1100 for 10BASE-T1L (SPE-Port IEEE802.3cg)

# FC440 Arduino
Firmware-project based on Arduino using VSCode and PlatformIO for RP2040 controller.
Initial firmware includes full running environment to use all ports.
* LAN9303Mi configured via I2C
* TJA1101BHN initialized and includes Auto-Detect for Master/Slave
* ADIN100 initialized on Autoneg
* LED-control to signal Link-Up
* DIP-switches to enable user-configs
*   DIP.0: Configure TJA1101 for Autoneg/Manual
*   DIP.1: Configure TJA1101 as Master/Slave (DIP.0 = Off)
*   DIP.2: Configure ADIN1100 Autoneg
*   DIP.3: Configure ADIN1100 Manual (Force Master/Slave) 

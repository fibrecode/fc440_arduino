#pragma once

/**
 * @file lan9303_registers.h
 * @brief LAN9303 register offsets and bit values used by the firmware.
 *
 * The constants in this header are derived from the LAN9303M-00002309A
 * datasheet. Only the subset required by the current firmware is listed here.
 */

/* -------------------------------------------------------------------------- */
/*  System register offsets (byte addressing, host interface view)           */
/* -------------------------------------------------------------------------- */

#define LAN9303_SWITCH_CSR_DATA  0x01ACu
#define LAN9303_SWITCH_CSR_CMD   0x01B0u
#define LAN9303_SWITCH_MAC_ADDRH 0x01F0u
#define LAN9303_SWITCH_MAC_ADDRL 0x01F4u

/* -------------------------------------------------------------------------- */
/*  Switch CSR command register (SWITCH_CSR_CMD @ 0x01B0)                    */
/* -------------------------------------------------------------------------- */

#define LAN9303_SWITCH_CSR_CMD_BUSY  0x80000000u
#define LAN9303_SWITCH_CSR_CMD_READ  0x40000000u
#define LAN9303_SWITCH_CSR_CMD_WRITE 0x00000000u
#define LAN9303_SWITCH_CSR_CMD_BE    0x000F0000u
#define LAN9303_SWITCH_CSR_CMD_ADDR  0x0000FFFFu

/* -------------------------------------------------------------------------- */
/*  Switch engine register offsets (accessed indirectly via CSR interface)   */
/* -------------------------------------------------------------------------- */

#define LAN9303_SWE_PORT_STATE      0x1843u
#define LAN9303_SWE_PORT_MIRROR     0x1846u
#define LAN9303_SWE_INGRSS_PORT_TYP 0x1847u

/* -------------------------------------------------------------------------- */
/*  Buffer manager register offsets                                          */
/* -------------------------------------------------------------------------- */

#define LAN9303_BM_EGRSS_PORT_TYPE 0x1C0Cu

/* -------------------------------------------------------------------------- */
/*  SWE_PORT_STATE bit fields                                                */
/* -------------------------------------------------------------------------- */

#define LAN9303_SWE_PORT_STATE_PORT0_MASK (0x3u << 0)
#define LAN9303_SWE_PORT_STATE_PORT1_MASK (0x3u << 2)
#define LAN9303_SWE_PORT_STATE_PORT2_MASK (0x3u << 4)

#define LAN9303_SWE_PORT_STATE_FORWARDING        0x0u
#define LAN9303_SWE_PORT_STATE_LISTENING_BLOCKED 0x1u
#define LAN9303_SWE_PORT_STATE_LEARNING          0x2u
#define LAN9303_SWE_PORT_STATE_DISABLED          0x3u

#define LAN9303_SWE_PORT_STATE_PORT0_FORWARDING \
  (LAN9303_SWE_PORT_STATE_FORWARDING << 0)
#define LAN9303_SWE_PORT_STATE_PORT1_FORWARDING \
  (LAN9303_SWE_PORT_STATE_FORWARDING << 2)
#define LAN9303_SWE_PORT_STATE_PORT2_FORWARDING \
  (LAN9303_SWE_PORT_STATE_FORWARDING << 4)

/* -------------------------------------------------------------------------- */
/*  SWE_PORT_MIRROR bit fields                                               */
/* -------------------------------------------------------------------------- */

#define LAN9303_SWE_PORT_MIRROR_RX_MIRRORING_FILT_EN (1u << 8)
#define LAN9303_SWE_PORT_MIRROR_SNIFFER_PORT_MASK    (0x7u << 5)
#define LAN9303_SWE_PORT_MIRROR_SNIFFER_PORT_0       (1u << 5)
#define LAN9303_SWE_PORT_MIRROR_SNIFFER_PORT_1       (1u << 6)
#define LAN9303_SWE_PORT_MIRROR_SNIFFER_PORT_2       (1u << 7)
#define LAN9303_SWE_PORT_MIRROR_MIRRORED_PORT_MASK   (0x7u << 2)
#define LAN9303_SWE_PORT_MIRROR_MIRRORED_PORT_0      (1u << 2)
#define LAN9303_SWE_PORT_MIRROR_MIRRORED_PORT_1      (1u << 3)
#define LAN9303_SWE_PORT_MIRROR_MIRRORED_PORT_2      (1u << 4)
#define LAN9303_SWE_PORT_MIRROR_RX_MIRRORING_EN      (1u << 1)
#define LAN9303_SWE_PORT_MIRROR_TX_MIRRORING_EN      (1u << 0)

/* -------------------------------------------------------------------------- */
/*  SWE_INGRSS_PORT_TYP bit fields                                           */
/* -------------------------------------------------------------------------- */

#define LAN9303_SWE_INGRSS_PORT_TYP_PORT0_MASK (0x3u << 0)
#define LAN9303_SWE_INGRSS_PORT_TYP_PORT1_MASK (0x3u << 2)
#define LAN9303_SWE_INGRSS_PORT_TYP_PORT2_MASK (0x3u << 4)
#define LAN9303_SWE_INGRSS_PORT_TYP_PORT0_EN   (0x3u << 0)
#define LAN9303_SWE_INGRSS_PORT_TYP_PORT1_EN   (0x3u << 2)
#define LAN9303_SWE_INGRSS_PORT_TYP_PORT2_EN   (0x3u << 4)

/* -------------------------------------------------------------------------- */
/*  MAC register offsets per port (indirect switch CSR space)                */
/* -------------------------------------------------------------------------- */

#define LAN9303_MAC_RX_CFG_0 0x0401u
#define LAN9303_MAC_RX_CFG_1 0x0801u
#define LAN9303_MAC_RX_CFG_2 0x0C01u

#define LAN9303_MAC_TX_CFG_0 0x0440u
#define LAN9303_MAC_TX_CFG_1 0x0840u
#define LAN9303_MAC_TX_CFG_2 0x0C40u

#define LAN9303_MAC_RX_GOODPKTLEN_CNT_0 0x0421u
#define LAN9303_MAC_RX_GOODPKTLEN_CNT_1 0x0821u
#define LAN9303_MAC_RX_GOODPKTLEN_CNT_2 0x0C21u

#define LAN9303_MAC_RX_PKTOK_CNT_0 0x0418u
#define LAN9303_MAC_RX_PKTOK_CNT_1 0x0818u
#define LAN9303_MAC_RX_PKTOK_CNT_2 0x0C18u

#define LAN9303_MAC_TX_DEFER_CNT_0 0x0451u
#define LAN9303_MAC_TX_DEFER_CNT_1 0x0851u
#define LAN9303_MAC_TX_DEFER_CNT_2 0x0C51u

#define LAN9303_MAC_TX_PKTLEN_CNT_0 0x045Cu
#define LAN9303_MAC_TX_PKTLEN_CNT_1 0x085Cu
#define LAN9303_MAC_TX_PKTLEN_CNT_2 0x0C5Cu

#define LAN9303_MAC_TX_PKTOK_CNT_0 0x0453u
#define LAN9303_MAC_TX_PKTOK_CNT_1 0x0853u
#define LAN9303_MAC_TX_PKTOK_CNT_2 0x0C53u

#define LAN9303_MAC_TX_TOTALCOL_CNT_0 0x0463u
#define LAN9303_MAC_TX_TOTALCOL_CNT_1 0x0863u
#define LAN9303_MAC_TX_TOTALCOL_CNT_2 0x0C63u

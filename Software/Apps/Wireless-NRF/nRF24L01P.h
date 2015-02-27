/* 
 * File:   nRF24L01p.h
 * Author: Alistair
 *
 * taken from the nRF24L01P datasheet. Only works for nRF24L01+ model.
 * Created on 25 February 2015, 16:34
 */

#ifndef NRF24L01P_H
#define	NRF24L01P_H

#ifdef	__cplusplus
extern "C" {
#endif
    //##############################COMMANDS##########################################
    /* NRF Commands page 51 of data sheet */
    /* R and W Registers have a mask of 0b 000A AAAA*/
#define R_REGISTER 0x00
#define W_REGISTER 0x20
#define RW_MASK  0x1F
#define R_RX_PAYLOAD 0x61
#define W_TX_PAYLOAD 0xA0
#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2
#define REUSE_TX_PL 0xE3
    /* these bits in the FEATURE register ..... have to be set*/
#define R_RX_PL_WID 0x60
    /* last three bits can be used  000 to 101 for additional payload*/
#define W_ACK_PAYLOAD 0xA8
#define W_TX_PAYLOAD_NOACK 0xB0

#define NOP 0xFF




    //##############################END OF COMMANDS#################################

    //#############################REGISTERS AND BIT MAPPING########################

#define CONFIG 0x00
#define MASK_RX_DR 6
#define MASK_TX_DS 5
#define MASK_MAX_RT 4
#define EN_CRC 3
#define CRCO 2
#define PWR_UP 1
#define PRIM_RX 0

#define EN_AA 0x01
#define ENAA_P5 5
#define ENAA_P4 4
#define ENAA_P3 3
#define ENAA_P2 2
#define ENAA_P1 1
#define ENAA_P0 0

#define EN_RXADDR 0x02
#define ERX_P5 5
#define ERX_P4 4
#define ERX_P3 3
#define ERX_P2 2
#define ERX_P1 1
#define ERX_P0 0

#define SETUP_AW 0x03
    /*AW word size is bits 1:0*/
#define AW 0

#define SETUP_RETR 0x04
    /*ARD 7:0*/
#define ARD 4
    /*ARC 3:0*/
#define ARC 0

#define RF_CH 0x05
    /*RF_CH 6:0*/
#define RF_CH_BIT 0

#define RF_SETUP 0x06
#define CONT_WAVE 7
#define RF_DR_LOW 5
#define PLL_LOCK 4
#define RF_DR_HIGH 3
    /*RF_PWR 2:1*/
#define RF_PWR_LOW 1
    #define RF_PWR_HIGH 2

#define STATUS 0x07
#define RX_DR 6
#define TX_DS 5
#define MAX_RT 4
    /*RX_P_NO 3:1*/
#define RX_P_NO 1
#define TX_FULL 0

#define OBSERVE_TX 0x08
    /*PLOS_CNT 7:4*/
#define PLOS_CNT 4
    /*ARC_CNT 3:0*/
#define ARC_CNT 0

#define RPD 0x09
#define RPD_BIT 0

#define RX_ADDR_P0 0x0A

#define RX_ADDR_P1 0x0B

#define RX_ADDR_P2 0x0C

#define RX_ADDR_P3 0x0D

#define RX_ADDR_P4 0x0E

#define RX_ADDR_P5 0x0F

#define TX_ADDR 0x10

#define RX_PW_P0 0x11
    /*RX_PW_P0 5:0*/
#define RX_PW_P0_BIT 0

#define RX_PW_P1 0x12
    /*RX_PW_P1 5:0*/
#define RX_PW_P1_BIT 0

#define RX_PW_P2 0x13
    /*RX_PW_P2 5:0*/
#define RX_PW_P2_BIT 0

#define RX_PW_P3 0x14
    /*RX_PW_P3 5:0*/
#define RX_PW_P3_BIT 0

#define RX_PW_P4 0x15
    /*RX_PW_P4 5:0*/
#define RX_PW_P4_BIT 0

#define RX_PW_P5 0x16
    /*RX_PW_P5 5:0*/
#define RX_PW_P5_BIT 0

#define FIFO_STATUS 0x17
#define TX_REUSE 6
#define TX_FIFO_FULL 5
#define TX_EMPTY 4
#define RX_FULL 1
#define RX_EMPTY 0

    #define DYNPD	    0x1C
#define DPL_P5 5
#define DPL_P4 4
#define DPL_P3 3
#define DPL_P2 2
#define DPL_P1 1
#define DPL_P0 0

    #define FEATURE	    0x1D
#define EN_DPL 2
#define EN_ACK_PAY 1
#define EN_DYN_ACK 0











#ifdef	__cplusplus
}
#endif

#endif	/* NRF24L01P_H */


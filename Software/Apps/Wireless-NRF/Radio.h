/* 
 * File:   Radio.h
 * Author: alistair
 * Handles the actual communication to the nRF24L01+
 * Created on 25 February 2015, 21:53
 */

#ifndef RADIO_H
#define	RADIO_H
#include "SPI.h"
#include <stdint.h>
#include "Timing.h"
#include "nRF24L01P.h"

typedef enum {
    RF24_1MBPS = 0, RF24_2MBPS, RF24_250KBPS
} rf24_datarate_e;

typedef enum {
    RF24_CRC_DISABLED = 0, RF24_CRC_8, RF24_CRC_16
} rf24_crclength_e;

typedef enum {
    RF24_PA_MIN = 0, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX, RF24_PA_ERROR
} rf24_pa_dbm_e;

static const uint8_t child_pipe[] = {
    RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5
};
static const uint8_t child_payload_size[] = {
    RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5
};
static const uint8_t child_pipe_enable[] = {
    ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5
};

static const char * rf24_datarate_e_str_0  = "1MBPS";
static const char * rf24_datarate_e_str_1  = "2MBPS";
static const char * rf24_datarate_e_str_2  = "250KBPS";
static const char * const rf24_datarate_e_str_P[]  = {
  rf24_datarate_e_str_0,
  rf24_datarate_e_str_1,
  rf24_datarate_e_str_2,
};
static const char * rf24_model_e_str_0  = "nRF24L01";
static const char * rf24_model_e_str_1  = "nRF24L01+";
static const char * const rf24_model_e_str_P[]  = {
  rf24_model_e_str_0,
  rf24_model_e_str_1,
};
static const char * rf24_crclength_e_str_0  = "Disabled";
static const char * rf24_crclength_e_str_1  = "8 bits";
static const char * rf24_crclength_e_str_2  = "16 bits" ;
static const char * const rf24_crclength_e_str_P[]  = {
  rf24_crclength_e_str_0,
  rf24_crclength_e_str_1,
  rf24_crclength_e_str_2,
};
static const char * rf24_pa_dbm_e_str_0  = "PA_MIN";
static const char * rf24_pa_dbm_e_str_1  = "PA_LOW";
static const char * rf24_pa_dbm_e_str_2  = "LA_MED";
static const char * rf24_pa_dbm_e_str_3  = "PA_HIGH";
static const char * const rf24_pa_dbm_e_str_P[]  = { 
  rf24_pa_dbm_e_str_0,
  rf24_pa_dbm_e_str_1,
  rf24_pa_dbm_e_str_2,
  rf24_pa_dbm_e_str_3,
};

class Radio {
public:
    Radio(char* spidevpath, uint8_t _cepin, uint8_t _cspin, bool debug = false);
    virtual ~Radio();
    void begin(void);
    void setChannel(uint8_t channel);
    bool setDataRate(rf24_datarate_e speed);
    void setPALevel(rf24_pa_dbm_e level);
    void setCRCLength(rf24_crclength_e length);
    void enableDynamicPayloads(void);
    void setRetries(uint8_t delay, uint8_t count);
    void openWritingPipe(uint64_t address);
    void openReadingPipe(uint8_t number, uint64_t address);
    void startListening(void);
    void stopListening(void);
    void printDetails(void);
    rf24_datarate_e getDataRate( void );
    bool isPVariant(void) ;
    rf24_crclength_e getCRCLength(void);
    rf24_pa_dbm_e getPALevel( void );
     bool write( const void* buf, uint8_t len );
     void startWrite( const void* buf, uint8_t len );
     bool available(void);
     bool available(uint8_t* pipe_num);
     uint8_t getDynamicPayloadSize(void);
     bool read( void* buf, uint8_t len );
private:
    uint8_t read_register(uint8_t reg);
    uint8_t flush_tx(void);
    uint8_t flush_rx(void);
    void toggle_features(void);
    void ce(int level);
    void csn(int level);
    uint8_t write_register(uint8_t reg, uint8_t value);
    void print_status(uint8_t status);
    uint8_t get_status(void);
    void print_byte_register(const char* name, uint8_t reg, uint8_t qty = 1);
    void print_address_register(const char* name, uint8_t reg, uint8_t qty = 1);
    uint8_t write_register(uint8_t reg, const uint8_t* buf, uint8_t len);
    uint8_t read_register(uint8_t reg, uint8_t* buf, uint8_t len);
    void whatHappened(bool& tx_ok,bool& tx_fail,bool& rx_ready);
    uint8_t read_payload(void* buf, uint8_t len);
    void powerDown(void);
    SPI* spidev;
    Timing* timing;
    bool debug;
    uint8_t ce_pin; /**< "Chip Enable" pin, activates the RX or TX role */
    uint8_t csn_pin; /**< SPI Chip select */
    bool wide_band; /* 2Mbs data rate in use? */
    bool p_variant; /* False for RF24L01 and true for RF24L01P */
    uint8_t payload_size; /**< Fixed size of payloads */
    bool ack_payload_available; /**< Whether there is an ack payload waiting */
    bool dynamic_payloads_enabled; /**< Whether dynamic payloads are enabled. */
    uint8_t ack_payload_length; /**< Dynamic size of pending ack payload. */
    uint64_t pipe0_reading_address; /**< Last address set on pipe 0 for reading. */
uint8_t write_payload(const void* buf, uint8_t len);
};

#endif	/* RADIO_H */


/* 
 * File:   Radio.cpp
 * Author: alistair
 * modifed version of maniacbugs code ( reduced features and adapted for linux, and made only work with nRF24L01+)
 * https://github.com/maniacbug/RF24
 * Created on 25 February 2015, 21:53
 */

#include "Radio.h"
#include "SPI.h"
#include "nRF24L01P.h"
#include "GPIO.h"
#include "Timing.h"
#include <algorithm>
#include <string.h>

#define _BV(x) (1<<(x))

Radio::Radio(char* spidevpath, uint8_t _cepin, uint8_t _cspin, bool debug) {
    this->spidev = new SPI(spidevpath, 8, 16000000);
    this->ce_pin = _cepin;
    this->csn_pin = _cspin;
    this->wide_band = true;
    this->p_variant = false;
    this->payload_size = 32;
    this->ack_payload_available = false;
    this->dynamic_payloads_enabled = false;
    this->pipe0_reading_address = 0;
    this->debug = debug;

}

Radio::~Radio() {
}

/****************************************************************************/

void Radio::begin(void) {
    // just to simulate arduino milis()
    timing = new Timing();
    // Initialize pins
    GPIO::open(csn_pin, GPIO_DDR_OUT);
    GPIO::open(ce_pin, GPIO_DDR_OUT);

    ce(GPIO_VAL_LOW);
    csn(GPIO_VAL_HIGH);

    // Must allow the radio time to settle else configuration bits will not necessarily stick.
    // This is actually only required following power up but some settling time also appears to
    // be required after resets too. For full coverage, we'll always assume the worst.
    // Enabling 16b CRC is by far the most obvious case if the wrong timing is used - or skipped.
    // Technically we require 4.5ms + 14us as a worst case. We'll just call it 5ms for good measure.
    // WARNING: Delay is based on P-variant whereby non-P *may* require different timing.
    timing->__msleep(5);

    // Set 1500uS (minimum for 32B payload in ESB@250KBPS) timeouts, to make testing a little easier
    // WARNING: If this is ever lowered, either 250KBS mode with AA is broken or maximum packet
    // sizes must never be used. See documentation for a more complete explanation.
    write_register(SETUP_RETR, (0b0100 << ARD) | (0b1111 << ARC));

    // Restore our default PA level
    setPALevel(RF24_PA_MAX);

    // Determine if this is a p or non-p RF24 module and then
    // reset our data rate back to default value. This works
    // because a non-P variant won't allow the data rate to
    // be set to 250Kbps.
    if (setDataRate(RF24_250KBPS)) {
        p_variant = true;
    }

    // Then set the data rate to the slowest (and most reliable) speed supported by all
    // hardware.
    setDataRate(RF24_1MBPS);

    // Initialize CRC and request 2-byte (16bit) CRC
    setCRCLength(RF24_CRC_16);

    // Disable dynamic payloads, to match dynamic_payloads_enabled setting
    write_register(DYNPD, 0);

    // Reset current status
    // Notice reset and flush is the last thing we do
    write_register(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT));

    // Set up default configuration.  Callers can always change it later.
    // This channel should be universally safe and not bleed over into adjacent
    // spectrum.
    setChannel(76);

    // Flush buffers
    flush_rx();
    flush_tx();
}

void Radio::csn(int level) {
    GPIO::write(csn_pin, level);
}

void Radio::ce(int level) {
    GPIO::write(ce_pin, level);
}

uint8_t Radio::write_register(uint8_t reg, uint8_t value) {
    uint8_t status;

    if (debug)
        printf("write_register(%02x,%02x)\r\n", reg, value);

    csn(GPIO_VAL_LOW);
    status = spidev->transfer(W_REGISTER | (RW_MASK & reg));
    spidev->transfer(value);
    csn(GPIO_VAL_HIGH);

    return status;
}

uint8_t Radio::flush_rx(void) {
    uint8_t status;

    csn(GPIO_VAL_LOW);
    status = spidev->transfer(FLUSH_RX);
    csn(GPIO_VAL_HIGH);

    return status;
}

/****************************************************************************/

uint8_t Radio::flush_tx(void) {
    uint8_t status;

    csn(GPIO_VAL_LOW);
    status = spidev->transfer(FLUSH_TX);
    csn(GPIO_VAL_HIGH);

    return status;
}

void Radio::setChannel(uint8_t channel) {
    // TODO: This method could take advantage of the 'wide_band' calculation
    // done in setChannel() to require certain channel spacing.

    const uint8_t max_channel = 127;
    write_register(RF_CH, std::min(channel, max_channel));
}

bool Radio::setDataRate(rf24_datarate_e speed) {
    bool result = false;
    uint8_t setup = read_register(RF_SETUP);

    // HIGH and LOW '00' is 1Mbs - our default
    wide_band = false;
    setup &= ~(_BV(RF_DR_LOW) | _BV(RF_DR_HIGH));
    if (speed == RF24_250KBPS) {
        // Must set the RF_DR_LOW to 1; RF_DR_HIGH (used to be RF_DR) is already 0
        // Making it '10'.
        wide_band = false;
        setup |= _BV(RF_DR_LOW);
    } else {
        // Set 2Mbs, RF_DR (RF_DR_HIGH) is set 1
        // Making it '01'
        if (speed == RF24_2MBPS) {
            wide_band = true;
            setup |= _BV(RF_DR_HIGH);
        } else {
            // 1Mbs
            wide_band = false;
        }
    }
    write_register(RF_SETUP, setup);

    // Verify our result
    if (read_register(RF_SETUP) == setup) {
        result = true;
    } else {
        wide_band = false;
    }

    return result;
}

void Radio::setPALevel(rf24_pa_dbm_e level) {
    uint8_t setup = read_register(RF_SETUP);
    setup &= ~(_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH));

    // switch uses RAM (evil!)
    if (level == RF24_PA_MAX) {
        setup |= (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH));
    } else if (level == RF24_PA_HIGH) {
        setup |= _BV(RF_PWR_HIGH);
    } else if (level == RF24_PA_LOW) {
        setup |= _BV(RF_PWR_LOW);
    } else if (level == RF24_PA_MIN) {
        // nothing
    } else if (level == RF24_PA_ERROR) {
        // On error, go to maximum PA
        setup |= (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH));
    }

    write_register(RF_SETUP, setup);
}

void Radio::setCRCLength(rf24_crclength_e length) {
    uint8_t config = read_register(CONFIG) & ~(_BV(CRCO) | _BV(EN_CRC));

    // switch uses RAM (evil!)
    if (length == RF24_CRC_DISABLED) {
        // Do nothing, we turned it off above. 
    } else if (length == RF24_CRC_8) {
        config |= _BV(EN_CRC);
    } else {
        config |= _BV(EN_CRC);
        config |= _BV(CRCO);
    }
    write_register(CONFIG, config);
}

uint8_t Radio::read_register(uint8_t reg) {
    csn(GPIO_VAL_LOW);
    spidev->transfer(R_REGISTER | (RW_MASK & reg));
    uint8_t result = spidev->transfer(0xff);

    csn(GPIO_VAL_HIGH);
    return result;
}

void Radio::enableDynamicPayloads(void) {
    // Enable dynamic payload throughout the system
    write_register(FEATURE, read_register(FEATURE) | _BV(EN_DPL));

    // If it didn't work, the features are not enabled
    if (!read_register(FEATURE)) {
        // So enable them and try again
        printf("ERROR SETTING DYNAMIC PAYLOAD!!!!!!!!!!");
        write_register(FEATURE, read_register(FEATURE) | _BV(EN_DPL));

    }
  if (debug)
    printf("FEATURE=%i\r\n", read_register(FEATURE));

    // Enable dynamic payload on all pipes
    //
    // Not sure the use case of only having dynamic payload on certain
    // pipes, so the library does not support it.
    write_register(DYNPD, read_register(DYNPD) | _BV(DPL_P5) | _BV(DPL_P4) | _BV(DPL_P3) | _BV(DPL_P2) | _BV(DPL_P1) | _BV(DPL_P0));

    dynamic_payloads_enabled = true;
}

void Radio::setRetries(uint8_t delay, uint8_t count) {
    write_register(SETUP_RETR, (delay & 0xf) << ARD | (count & 0xf) << ARC);
}

void Radio::openWritingPipe(uint64_t value) {
    // Note that AVR 8-bit uC's store this LSB first, and the NRF24L01(+)
    // expects it LSB first too, so we're good.

    write_register(RX_ADDR_P0, reinterpret_cast<uint8_t*> (&value), 5);
    write_register(TX_ADDR, reinterpret_cast<uint8_t*> (&value), 5);

    const uint8_t max_payload_size = 32;
    write_register(RX_PW_P0, std::min(payload_size, max_payload_size));
}

void Radio::openReadingPipe(uint8_t child, uint64_t address) {
    // If this is pipe 0, cache the address.  This is needed because
    // openWritingPipe() will overwrite the pipe 0 address, so
    // startListening() will have to restore it.
    if (child == 0)
        pipe0_reading_address = address;

    if (child <= 6) {
        // For pipes 2-5, only write the LSB
        if (child < 2)
            write_register(child_pipe[child], reinterpret_cast<const uint8_t*> (&address), 5);
        else
            write_register(child_pipe[child], reinterpret_cast<const uint8_t*> (&address), 1);

        write_register(child_payload_size[child], payload_size);

        // Note it would be more efficient to set all of the bits for all open
        // pipes at once.  However, I thought it would make the calling code
        // more simple to do it this way.
        write_register(EN_RXADDR, read_register(EN_RXADDR) | _BV(child_pipe_enable[child]));
    }
}

void Radio::startListening(void) {
    write_register(CONFIG, read_register(CONFIG) | _BV(PWR_UP) | _BV(PRIM_RX));
    write_register(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT));

    // Restore the pipe0 adddress, if exists
    if (pipe0_reading_address)
        write_register(RX_ADDR_P0, reinterpret_cast<const uint8_t*> (&pipe0_reading_address), 5);

    // Flush buffers
    flush_rx();
    flush_tx();

    // Go!
    ce(GPIO_VAL_HIGH);

    // wait for the radio to come up (130us actually only needed)
    timing->__usleep(130);
}

void Radio::printDetails(void) {
    print_status(get_status());

    print_address_register("RX_ADDR_P0-1", RX_ADDR_P0, 2);
    print_byte_register("RX_ADDR_P2-5", RX_ADDR_P2, 4);
    print_address_register("TX_ADDR", TX_ADDR);

    print_byte_register("RX_PW_P0-6", RX_PW_P0, 6);
    print_byte_register("EN_AA", EN_AA);
    print_byte_register("EN_RXADDR", EN_RXADDR);
    print_byte_register("RF_CH", RF_CH);
    print_byte_register("RF_SETUP", RF_SETUP);
    print_byte_register("CONFIG", CONFIG);
    print_byte_register("DYNPD/FEATURE", DYNPD, 2);

    printf("Data Rate\t = %s\n", rf24_datarate_e_str_P[getDataRate()]);
    printf("Model\t\t = %s\n", rf24_model_e_str_P[isPVariant()]);
    printf("CRC Length\t = %s\n", rf24_crclength_e_str_P[getCRCLength()]);
    printf("PA Power\t = %s\n", rf24_pa_dbm_e_str_P[getPALevel()]);
}

void Radio::print_status(uint8_t status) {
    printf("STATUS\t\t = 0x%02x RX_DR=%x TX_DS=%x MAX_RT=%x RX_P_NO=%x TX_FULL=%x\r\n",
            status,
            (status & _BV(RX_DR)) ? 1 : 0,
            (status & _BV(TX_DS)) ? 1 : 0,
            (status & _BV(MAX_RT)) ? 1 : 0,
            ((status >> RX_P_NO) & 0b111),
            (status & _BV(TX_FULL)) ? 1 : 0
            );
}

uint8_t Radio::get_status(void) {
    uint8_t status;

    csn(GPIO_VAL_LOW);
    status = spidev->transfer(NOP);
    csn(GPIO_VAL_HIGH);

    return status;
}

void Radio::print_byte_register(const char* name, uint8_t reg, uint8_t qty) {
    char extra_tab = strlen(name) < 8 ? '\t' : 0;
    printf("%s\t%c =", name, extra_tab);
    while (qty--)
        printf(" 0x%02x", read_register(reg++));
    printf("\r\n");
}

void Radio::print_address_register(const char* name, uint8_t reg, uint8_t qty) {
    char extra_tab = strlen(name) < 8 ? '\t' : 0;
    printf("%s\t%c =", name, extra_tab);

    while (qty--) {
        uint8_t buffer[5];
        read_register(reg++, buffer, sizeof buffer);

        printf(" 0x");
        uint8_t* bufptr = buffer + sizeof buffer;
        while (--bufptr >= buffer)
            printf("%02x", *bufptr);

    }

    printf("\r\n");
}

rf24_datarate_e Radio::getDataRate(void) {
    rf24_datarate_e result;
    uint8_t dr = read_register(RF_SETUP) & (_BV(RF_DR_LOW) | _BV(RF_DR_HIGH));

    // switch uses RAM (evil!)
    // Order matters in our case below
    if (dr == _BV(RF_DR_LOW)) {
        // '10' = 250KBPS
        result = RF24_250KBPS;
    } else if (dr == _BV(RF_DR_HIGH)) {
        // '01' = 2MBPS
        result = RF24_2MBPS;
    } else {
        // '00' = 1MBPS
        result = RF24_1MBPS;
    }
    return result;
}

bool Radio::isPVariant(void) {
    return p_variant;
}

rf24_crclength_e Radio::getCRCLength(void) {
    rf24_crclength_e result = RF24_CRC_DISABLED;
    uint8_t config = read_register(CONFIG) & (_BV(CRCO) | _BV(EN_CRC));

    if (config & _BV(EN_CRC)) {
        if (config & _BV(CRCO))
            result = RF24_CRC_16;
        else
            result = RF24_CRC_8;
    }

    return result;
}

rf24_pa_dbm_e Radio::getPALevel(void) {
    rf24_pa_dbm_e result = RF24_PA_ERROR;
    uint8_t power = read_register(RF_SETUP) & (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH));

    // switch uses RAM (evil!)
    if (power == (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH))) {
        result = RF24_PA_MAX;
    } else if (power == _BV(RF_PWR_HIGH)) {
        result = RF24_PA_HIGH;
    } else if (power == _BV(RF_PWR_LOW)) {
        result = RF24_PA_LOW;
    } else {
        result = RF24_PA_MIN;
    }

    return result;
}

uint8_t Radio::write_register(uint8_t reg, const uint8_t* buf, uint8_t len) {
    uint8_t status;

    csn(GPIO_VAL_LOW);
    status = spidev->transfer(W_REGISTER | (RW_MASK & reg));
    while (len--)
        spidev->transfer(*buf++);

    csn(GPIO_VAL_HIGH);

    return status;
}

uint8_t Radio::read_register(uint8_t reg, uint8_t* buf, uint8_t len) {
    uint8_t status;

    csn(GPIO_VAL_LOW);
    status = spidev->transfer(R_REGISTER | (RW_MASK & reg));
    while (len--)
        *buf++ = spidev->transfer(0xff);

    csn(GPIO_VAL_HIGH);

    return status;
}

void Radio::stopListening(void) {
    ce(GPIO_VAL_LOW);
    flush_tx();
    flush_rx();
}

bool Radio::write(const void* buf, uint8_t len) {
    bool result = false;

    // Begin the write
    startWrite(buf, len);

    // ------------
    // At this point we could return from a non-blocking write, and then call
    // the rest after an interrupt

    // Instead, we are going to block here until we get TX_DS (transmission completed and ack'd)
    // or MAX_RT (maximum retries, transmission failed).  Also, we'll timeout in case the radio
    // is flaky and we get neither.

    // IN the end, the send should be blocking.  It comes back in 60ms worst case, or much faster
    // if I tighted up the retry logic.  (Default settings will be 1500us.
    // Monitor the send
    uint8_t observe_tx;
    uint8_t status;
    uint32_t sent_at = timing->__millis();
    const uint32_t timeout = 500; //ms to wait for timeout
    do {
        status = read_register(OBSERVE_TX, &observe_tx, 1);
        if (debug)
            printf("%.X ", observe_tx);
    } while (!(status & (_BV(TX_DS) | _BV(MAX_RT))) && (timing->__millis() - sent_at < timeout));

    // The part above is what you could recreate with your own interrupt handler,
    // and then call this when you got an interrupt
    // ------------

    // Call this when you get an interrupt
    // The status tells us three things
    // * The send was successful (TX_DS)
    // * The send failed, too many retries (MAX_RT)
    // * There is an ack packet waiting (RX_DR)
    bool tx_ok, tx_fail;
    whatHappened(tx_ok, tx_fail, ack_payload_available);

    //printf("%u%u%u\r\n",tx_ok,tx_fail,ack_payload_available);

    result = tx_ok;
    if (debug)
        printf(result ? "...OK." : "...Failed");

    // Handle the ack packet
    if (ack_payload_available) {
        ack_payload_length = getDynamicPayloadSize();
        if (debug) {
            printf("[AckPacket]/");
            printf("\n%d", ack_payload_length);
        }
    }

    // Yay, we are done.

    // Power down
    powerDown();

    // Flush buffers (Is this a relic of past experimentation, and not needed anymore??)
    flush_tx();

    return result;
}

void Radio::startWrite(const void* buf, uint8_t len) {
    // Transmitter power-up
    write_register(CONFIG, (read_register(CONFIG) | _BV(PWR_UP)) & ~_BV(PRIM_RX));
    timing->__usleep(150);

    // Send the payload
    write_payload(buf, len);

    // Allons!
    ce(GPIO_VAL_HIGH);
    timing->__usleep(15);
    ce(GPIO_VAL_LOW);
}

uint8_t Radio::write_payload(const void* buf, uint8_t len) {
    uint8_t status;

    const uint8_t* current = reinterpret_cast<const uint8_t*> (buf);

    uint8_t data_len = std::min(len, payload_size);
    uint8_t blank_len = dynamic_payloads_enabled ? 0 : payload_size - data_len;

    if (debug)
    {
    printf("[Writing %u bytes %u blanks]",data_len,blank_len);
    }
    csn(GPIO_VAL_LOW);
    status = spidev->transfer(W_TX_PAYLOAD);
    while (data_len--)
        spidev->transfer(*current++);
    while (blank_len--)
        spidev->transfer(0);
    csn(GPIO_VAL_HIGH);

    return status;
}

void Radio::whatHappened(bool& tx_ok, bool& tx_fail, bool& rx_ready) {
    // Read the status & reset the status in one easy call
    // Or is that such a good idea?
    uint8_t status = write_register(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT));

    // Report to the user what happened
    tx_ok = status & _BV(TX_DS);
    tx_fail = status & _BV(MAX_RT);
    rx_ready = status & _BV(RX_DR);
}

uint8_t Radio::getDynamicPayloadSize(void) {
    uint8_t result = 0;

    csn(GPIO_VAL_LOW);
    spidev->transfer(R_RX_PL_WID);
    result = spidev->transfer(0xff);
    csn(GPIO_VAL_HIGH);

    return result;
}

void Radio::powerDown(void) {
    write_register(CONFIG, read_register(CONFIG) & ~_BV(PWR_UP));
}

bool Radio::available(void) {
    return available(NULL);
}

bool Radio::available(uint8_t* pipe_num) {
    uint8_t status = get_status();

    // Too noisy, enable if you really want lots o data!!
    //IF_SERIAL_DEBUG(print_status(status));

    bool result = (status & _BV(RX_DR));

    if (result) {
        // If the caller wants the pipe number, include that
        if (pipe_num)
            *pipe_num = (status >> RX_P_NO) & 0b111;

        // Clear the status bit

        // ??? Should this REALLY be cleared now?  Or wait until we
        // actually READ the payload?

        write_register(STATUS, _BV(RX_DR));

        // Handle ack payload receipt
        if (status & _BV(TX_DS)) {
            write_register(STATUS, _BV(TX_DS));
        }
    }

    return result;
}

bool Radio::read(void* buf, uint8_t len) {
    // Fetch the payload
    read_payload(buf, len);

    // was this the last of the data available?
    return read_register(FIFO_STATUS) & _BV(RX_EMPTY);
}

uint8_t Radio::read_payload(void* buf, uint8_t len) {
    uint8_t status;
    uint8_t* current = reinterpret_cast<uint8_t*> (buf);

    uint8_t data_len = std::min(len, payload_size);
    uint8_t blank_len = dynamic_payloads_enabled ? 0 : payload_size - data_len;

    //printf("[Reading %u bytes %u blanks]",data_len,blank_len);

    csn(GPIO_VAL_LOW);
    status = spidev->transfer(R_RX_PAYLOAD);
    while (data_len--)
        *current++ = spidev->transfer(0xff);
    while (blank_len--)
        spidev->transfer(0xff);
    csn(GPIO_VAL_HIGH);

    return status;
}

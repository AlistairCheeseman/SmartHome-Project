/*
 * PowerOutletRevA.h
 *
 * Created: 16/03/2015 09:10:49
 *  Author: Ali
 */ 


#ifndef POWEROUTLETREVA_H_
#define POWEROUTLETREVA_H_

//check the global compile time variables have been defined.
#ifndef HIGH
# warning "HIGH LEVEL NOT DEFINED"
#endif
#ifndef LOW
# warning "LOW LEVEL NOT DEFINED"
#endif
#ifndef CSN
#warning "CSN PIN NOT DEFINED"
#endif
#ifndef CE
#warning "CSN PIN NOT DEFINED"
#endif
#ifndef F_CPU
#warning "CPU Speed NOT DEFINED"
#endif


//INDIVIDUAL DEVICE ID
#define MAC_SUFF "AEAEAE"
#define MAC_SUFF_HEX 0xAEAEAE


#define STATUS_LED_1 PORTD5 //OUT1_LED
#define STATUS_LED_2 PORTD6 //OUT2_LED

#define STATUS_LED_1_DDR DDRD //OUT1_LED
#define STATUS_LED_2_DDR DDRD //OUT2_LED

#define STATUS_LED_1_PORT PORTD //OUT1_LED PORT - this will be connection status???
#define STATUS_LED_2_PORT PORTD //OUT2_LED PORT - this will be state change pending????

/* PIN DEFINITIONS */
//MQTT ID Mappings (ASCII VALUE FOR NUMBER)
#define ID1 "1" //0x31 //1 maps to Power
#define ID2 "2" //0x32 //2 maps to Power Factor.

#define ID1_PIN  1 //ADC CHANNEL 1 (PIN 24)
#define ID2_PIN  0 //ADC CHANNEL 0 (PIN 23)

//for readability
#define VOLTAGE_CH ID1_PIN
#define CURRENT_CH ID2_PIN

#define TOPIC_STATUS_UPDATE "S"

/* PIN DEFINITIONS */



#endif /* POWEROUTLETREVA_H_ */
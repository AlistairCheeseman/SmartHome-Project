/*
 * HeatingControllerRevA.h
 *
 * Created: 04/04/2015 17:31:10
 *  Author: Ali
 */ 


#ifndef HEATINGCONTROLLERREVA_H_
#define HEATINGCONTROLLERREVA_H_


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
#define MAC_SUFF "898989"
#define MAC_SUFF_HEX 0x898989
//this should be assigned dynamically but for time being will be manual
#define THIS_LEVEL 1
#define THIS_DEV 3
#define THIS_MQTTCLIENTID 0x48

#define STATUS_LED_1 PORTD5 //OUT1_LED
#define STATUS_LED_2 PORTD6 //OUT2_LED

#define STATUS_LED_1_DDR DDRD //OUT1_LED
#define STATUS_LED_2_DDR DDRD //OUT2_LED

#define STATUS_LED_1_PORT PORTD //OUT1_LED PORT - this will be connection status???
#define STATUS_LED_2_PORT PORTD //OUT2_LED PORT - this will be state change pending????

/* PIN DEFINITIONS */
//MQTT ID Mappings (ASCII VALUE FOR NUMBER)
#define ID1 "3" // host water boost
#define ID2 "4" //0x32 // heating boost
#define ID3 "2" //0x33 //heating
#define ID4 "1" //0x34 // hot water


#define ID1_PIN PORTC5 //OUT1_BUTTON
#define ID2_PIN PORTC4 //OUT2_BUTTON
#define ID3_PIN PORTD3 //OUT1_RELAY (heating)
#define ID4_PIN PORTD2 //OUT2_RELAY (hotwater)



#define ID1_PORT PORTC
#define ID2_PORT PORTC
#define ID3_PORT PORTD
#define ID4_PORT PORTD

#define ID1_PINPORT PINC
#define ID2_PINPORT PINC

#define ID1_DDR DDRC
#define ID2_DDR DDRC
#define ID3_DDR DDRD
#define ID4_DDR DDRD
/* PIN DEFINITIONS */




#endif /* HEATINGCONTROLLERREVA_H_ */
/*
 * LightSwitchRevA.h
 *
 * Created: 08/04/2015 10:29:10
 *  Author: Ali
 */ 


#ifndef LIGHTSWITCHREVA_H_
#define LIGHTSWITCHREVA_H_

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

#define MAC_SUFF "7E7E7E"
#define MAC_SUFF_HEX 0x7E7E7E

//this should be assigned dynamically but for time being will be manual
#define THIS_LEVEL 1
#define THIS_DEV 5

//MQTT ID Mappings (ASCII VALUE FOR NUMBER)
#define ID1 "1" //0x31 //1 maps to button press
#define ID1_PIN  PORTC5 //OUT1_BUTTON
#define ID1_PORT PORTC
#define ID1_PINPORT PINC
#define ID1_DDR DDRC

#define STATUS_LED_1 PORTD5 //LED1
#define STATUS_LED_1_DDR DDRD //LED1
#define STATUS_LED_1_PORT PORTD //LED1 PORT













#endif /* LIGHTSWITCHREVA_H_ */
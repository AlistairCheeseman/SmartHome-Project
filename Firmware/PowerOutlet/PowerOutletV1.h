/*
 * PowerOutletV1.h
 *
 * Created: 27/02/2015 21:28:07
 *  Author: Ali
 */ 


#ifndef POWEROUTLETV1_H_
#define POWEROUTLETV1_H_



#define CSN PORTB2
#define CE PORTB1

#define F_CPU 8000000UL

#define HIGH 1
#define LOW 0
#define OUTPUT 1
#define INPUT 0


#define TOPIC_STATUS_UPDATE "S"
#define TOPIC_REQUEST "R"
#define TOPIC_STATUS_REQUEST "P"

//INDIVIDUAL DEVICE ID
#define MAC_SUFF "87865E"


#define STATUS_LED_1 PORTD5 //OUT1_LED
#define STATUS_LED_2 PORTD6 //OUT2_LED

/* PIN DEFINITIONS */
//MQTT ID Mappings (ASCII VALUE FOR NUMBER)
#define ID1 "1"
#define ID2 "2" //0x32
#define ID3 "3" //0x33
#define ID4 "4" //0x34


#define ID1_PIN  PORTC5 //OUT1_BUTTON
#define ID2_PIN  PORTC4 //OUT2_BUTTON
#define ID3_PIN PORTD3 //OUT1_RELAY
#define ID4_PIN PORTD2 //OUT2_RELAY



#define ID1_PORT PORTC
#define ID2_PORT PORTC
#define ID3_PORT PORTD
#define ID4_PORT PORTD

#define ID1_DDR DDRD
#define ID2_DDR DDRD
#define ID3_DDR DDRD
#define ID4_DDR DDRD
/* PIN DEFINITIONS */
 



#endif /* POWEROUTLETV1_H_ */
/*
* PowerOutlet.cpp
*
* Created: 27/02/2015 21:25:47
*  Author: Ali
*/



#include <stdio.h>
#include <math.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "Timing.h"
#include "PowerOutletV1.h"
#define BAUD 9600
#include <util/setbaud.h>

#include "MQTTSN.h"
#include "SensorNet.h"

int USART0SendByte (char c, FILE *stream);
int USART0ReceiveByte(FILE *stream);
void USARTinit(void);
void setup(void);

FILE * usart0_str;
SensorNet network;
MQTTSN app(network,(uint8_t) 0x28);

//NOTE:: app.tick() must be called to handle any replies, and in turn update any status!
int main(void)
{
	setup();
	_delay_ms(1000);
	while(1)
	{
		printf("CONNECTING\n");
		while (app.currentState != STATE_ACTIVE)
		{
				_delay_ms(2000); //dont spam the network with reconnections.
				app.connect();
				app.tick();
		}
	printf("CONNECTED\n");
		//if we get disconnected stop doing work and wait until reconnected.
		while(app.currentState != STATE_DISCONNECTED)
		{
					app.tick(); // process the radio no matter if connected to a MQTT server or not. this is to help the network layer communications.
			printf("main loop\n");
			//	if (network.pendingpacket == true) //? handle here or in the MQTTSN class?
			//network.sendpacket((const void*)0x28596E72, 4);
			//	comms.tick(); // this polls the MQTT layer which itself polls the network layer and in turn the radio.
			//	_delay_ms(100); // wait a bit to ensure we don't over do the tick.
			printf("test publish\n");
			unsigned char data[1];
			data[0] =  (unsigned char)0x01;
			app.publish((unsigned char*)"d/868789/R",data, 0x0A, 0x01);
			printf("done\n");
		}
		printf("DISCONNECTED\n");
	}
}
void setup(void)
{
	//setup serial
	USARTinit();
	//start timers for milli function.
	Timing::init();
	//start up mqtt layer, this will also initialize the radio, and the network layer running on the network.
	network.setup(); //ensure network is setup before any MQTT work is done.
	//	comms.setcallback(messageReceived);
}
int USART0SendByte (char c, FILE *stream)
{
	if (c == '\n')
	USART0SendByte('\r', stream);
	
	// Wait for the transmit buffer to be empty
	while ( !( UCSR0A & (1<<UDRE0)) );
	
	// Put our character into the transmit buffer
	UDR0 = c;
	
	return 0;
}
int USART0ReceiveByte(FILE *stream)
{
	uint8_t u8Data;
	// Wait for byte to be received
	while(!(UCSR0A&(1<<RXC0))){};
	u8Data=UDR0;
	//echo input data
	USART0SendByte(u8Data,stream);
	// Return received data
	return u8Data;
}
void USARTinit(void)
{
	//set baud rate.
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	
	#if USE_2X
	UCSR0A |= (1<<U2X0);
	#else
	UCSR0A &= ~(1<<U2X0);
	#endif
	
	//enable RX & TX
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);

	//set frame format, 8 data, 1 stop
	//1 stop bit
	UCSR0C &= ~(1<<USBS0);
	//8 data
	UCSR0C |=  (1<<UCSZ00)|(1<<UCSZ01);
	
	usart0_str = fdevopen(USART0SendByte, USART0ReceiveByte);
	stdin=stdout=usart0_str;
	

}

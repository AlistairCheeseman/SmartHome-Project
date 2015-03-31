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


#include "PowerOutletV1.h"
#define BAUD 9600
#include <util/setbaud.h>
#include "Timing.h"
#include "MQTTSN.h"
#include "SensorNet.h"
#include "RF24.h"

FILE * usart0_str;
RF24 radio;
SensorNet network(radio);
MQTTSN app(network,(uint8_t) 0x28, (uint32_t) MAC_SUFF_HEX);

int USART0SendByte (char c, FILE *stream);
int USART0ReceiveByte(FILE *stream);
void USARTinit(void);
void setup(void);
void connectedLed(uint8_t level);
void pendingLed(uint8_t level);


//for quik publishes we will store the id for the request topic. it stops the need to look the topic up every time before a publish.
//topic to publish a Request to
uint16_t pub1id = 0;
uint16_t pub2id = 0;
//topic we will get SR from
uint16_t sub3id = 0;
uint16_t sub4id = 0;
//where we send the updated status.
uint16_t pubS3id = 0;
uint16_t pubS4id = 0;

void callback(uint16_t topicId, uint8_t *payload,unsigned int payloadLen) {
	//update the new state to the server.
	unsigned char changestate[3] ={(unsigned char) 0x31};
	printf("\n");
	//todo: lookup topic id and find what topic it corresponds to.
	printf("Received topic update on ID: %d\n", topicId);
	printf("Payload :%c \n", payload[0]);
	printf("\n");
	if (topicId == sub3id)
	{
		printf("RECEIVED SR FOR OUT ID3\n");
		
		switch (payload[0])
		{
			case 0x31:
			//turn on.
			ID3_PORT |= (1<<ID3_PIN);
			break;
			case 0x30:
			//turn off
			ID3_PORT &= ~(1<<ID3_PIN);
			break;
			case 0x43:
			//flip states.
			ID3_PORT ^= (1<<ID3_PIN);
			break;
		}
		
		

		if (!(ID3_PORT & (1<< ID3_PIN)))
		{
			changestate[0] = 0x30;
		}
		app.publish(pubS3id, changestate, 0x01);
		printf("updated status\n");
		//	app.publish((unsigned char*)"d/"MAC_SUFF"/"ID3"/"TOPIC_STATUS_UPDATE,changestate, 0x0C, 0x01);
	}
	else if(topicId == sub4id)
	{
		printf("RECEIVED SR FOR OUT ID4\n");
		switch (payload[0])
		{
			case 0x31:
			//turn on.
			ID4_PORT |= (1<<ID4_PIN);
			break;
			case 0x30:
			//turn off
			ID4_PORT &= ~(1<<ID4_PIN);
			break;
			case 0x43:
			//flip states.
			ID4_PORT ^= (1<<ID4_PIN);
			break;
		}
		
		//update the new state to the server.
		if (!(ID4_PORT & (1<< ID4_PIN)))
		{
			changestate[0] = 0x30;
		}
		app.publish(pubS4id, changestate, 0x01);
		printf("updated status\n");
		//app.publish((unsigned char*)"d/"MAC_SUFF"/"ID4"/"TOPIC_STATUS_UPDATE,changestate, 0x0C, 0x01);
		
	}
	
	
}

//NOTE:: app.tick() must be called to handle any replies, and in turn update any status!
int main(void)
{
	_delay_ms(2000); // wait two seconds for power supply to stabilize.THIS IS COMPULSORY OR CONFIG IN SETUP GOES MAD!
	setup();
	_delay_ms(1000); //wait for a bit.
	//0x43 = flip states.
	//0x31 = turn on.
	//0x30 = turn off.
	unsigned char changestate[3] ={(unsigned char) 0x43};
	//unsigned char changestate[3] ={(unsigned char) 0x31};
	bool id1button = false;
	bool id2button = false;
	app.connect(); //this is to speed up boot up times as we don't want the 2 second delay on clean boot.
	
	app.tick();
	while(1)
	{
		printf("CONNECTING\n");
		pendingLed(HIGH);
		while (app.currentState != STATE_ACTIVE)
		{
			//_delay_ms(2000); //dont spam the network with reconnections.
			app.connect();
			_delay_ms(2500); //wait for response.
			app.tick();
		}
		connectedLed(HIGH);
		printf("CONNECTED\n");
		unsigned char offstate[1] ={(unsigned char) 0x30};
		//need to test this.
		pub1id = app.gettopicid((unsigned char*)"d/"MAC_SUFF"/"ID1"/"TOPIC_REQUEST, 0x0C);
		_delay_ms(5);
		pub2id = app.gettopicid((unsigned char*)"d/"MAC_SUFF"/"ID2"/"TOPIC_REQUEST, 0x0C);
		_delay_ms(5);
		pubS3id = app.gettopicid((unsigned char*)"d/"MAC_SUFF"/"ID3"/"TOPIC_STATUS_UPDATE, 0x0C);
		_delay_ms(5);
		pubS4id = app.gettopicid((unsigned char*)"d/"MAC_SUFF"/"ID4"/"TOPIC_STATUS_UPDATE, 0x0C);
		_delay_ms(5);
		
		
		app.publish(pubS3id, offstate, 0x01); //set  current state to off.
		app.publish(pubS4id, offstate, 0x01); //set  current state to off.
		
		//subscribe to the topics here, we only need to subscribe to the State requests for outputs as they are the only things that can make it change.
		
		sub3id = 	app.subscribe((unsigned char*)"d/"MAC_SUFF"/"ID3"/"TOPIC_STATUS_REQUEST, 0x0C);
		
		_delay_ms(5);
		app.tick(); //process the ack.
		sub4id = 	app.subscribe((unsigned char*)"d/"MAC_SUFF"/"ID4"/"TOPIC_STATUS_REQUEST, 0x0C);

		_delay_ms(5);
		app.tick(); //process the ack.
		
		
		
		
		
		printf("ID1 SR mapped to %d\n", pub1id);
		printf("ID2 SR mapped to %d\n", pub2id);
		printf("ID3 STATE mapped to %d\n", pubS3id);
		printf("ID4 STATE mapped to %d\n", pubS4id);
		printf("Subscribed to: %d\n", sub3id);
		printf("Subscribed to: %d\n", sub4id);
		printf("SUBSCRIBED\n");
		pendingLed(LOW);
		//if we get disconnected stop doing work and wait until reconnected.
		while(app.currentState != STATE_DISCONNECTED)
		{
			if ((app.currentState & STATE_WAIT_MASK) > 0)
			{ // we are waiting for something
				pendingLed(HIGH);
			}
			else
			{
				pendingLed(LOW);
				
			}
				_delay_ms(90); // wait a bit to ensure we don't over do the tick and to allow time to recieve.
			app.tick(); // process the radio no matter if connected to a MQTT server or not. this is to help the network layer communications.
			//printf("main loop\n");
		
			if (!(ID1_PINPORT & (1<<ID1_PIN))) // pullup so we want to check if it is low.
			{
				_delay_ms(1);
				//kinda debounce.
				if (!(ID1_PINPORT & (1<<ID1_PIN)))
				{
					//make sure the button hasnt just been held down for a long time.
					if (id1button == false)
					{
						//the button has been pushed., publish a Request.
						
						app.publish(pub1id,changestate, 0x01);
						id1button = true;
					}
				}
			}
			else
			{
				id1button = false;
			}
			if (!(ID2_PINPORT & (1<<ID2_PIN)))
			{
				_delay_ms(1);
				//kinda debounce.
				if (!(ID2_PINPORT & (1<<ID2_PIN)))
				{
					//make sure the button hasnt just been held down for a long time.
					if (id2button == false)
					{
						//the button has been pushed., publish a Request ( we need to know the state though, this will not enable cross mapping but with QOS2 we can have cross mapping, and publish a change value not absolute)
						app.publish(pub2id,changestate, 0x01);
						id2button = true;
					}
				}
			}
			else
			{
				id2button = false;
			}
			
		}
		printf("DISCONNECTED\n");
		connectedLed(LOW);
		pendingLed(LOW); // we don't have any pending packets anymore
		
	}
}
void setup(void)
{
	USARTinit(); //enable serial output;
	Timing::init();// start the timer - used for the millis function (rough time since powered on);
	network.setup(THIS_LEVEL, THIS_DEV); //ensure network is setup before any MQTT work is done.
	app.setCallback(callback); // give the MQTT layer the functuion we will use to process entries.
	//setup outputs
	ID3_DDR |= (1<<ID3_PIN);  //relay 1
	ID4_DDR |= (1<<ID4_PIN); //relay 2
	STATUS_LED_1_DDR |= (1<<STATUS_LED_1); //STATUS1_LED
	STATUS_LED_2_DDR |= (1<<STATUS_LED_2); //STATUS2_LED
	//setup input buttons
	ID1_DDR &= ~(1<<ID1_PIN);
	ID2_DDR &= ~(1<<ID2_PIN);
	//set pullups.
	ID1_PORT |= (1<<ID1_PIN);
	ID2_PORT |= (1<<ID2_PIN);
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
void connectedLed(uint8_t level)
{
	if (level == HIGH)
	{
		STATUS_LED_1_PORT |= (1<<STATUS_LED_1);
	}
	else
	{
		
		STATUS_LED_1_PORT &= ~(1<<STATUS_LED_1);
	}
	
}
void pendingLed(uint8_t level)
{
	if (level == HIGH)
	{
		STATUS_LED_2_PORT |= (1<<STATUS_LED_2);
	}
	else
	{
		
		STATUS_LED_2_PORT &= ~(1<<STATUS_LED_2);
	}
	
}

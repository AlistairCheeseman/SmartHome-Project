/*
* LightSwitch.cpp
*
* Created: 03/04/2015 16:33:20
*  Author: Ali
*/


#include <avr/io.h>
#include <string.h>


#include "LightSwitchRevA.h"
#define BAUD 9600
#include <util/setbaud.h>
#include "Timing.h"

#include "MQTTSN.h"
#include "SensorNet.h"


int USART0SendByte (char c, FILE *stream);
int USART0ReceiveByte(FILE *stream);
void USARTinit(void);
void statusLed(uint8_t level);
void setup(void);



FILE * usart0_str;
RF24 radio;
SensorNet network(radio, MAC_SUFF_HEX);
MQTTSN app(network,THIS_MQTTCLIENTID, MAC_SUFF_HEX);

uint16_t pub1id = 0; // topic to publish to on button press.

int main(void)
{
	_delay_ms(2000); // wait two seconds for power supply to stabilize.THIS IS COMPULSORY OR CONFIG IN SETUP GOES MAD!
	setup();
	_delay_ms(1000); //wait for a bit.
	bool id1button = false;
	unsigned char changestate[3] ={(unsigned char) 0x43};
	while(1)
	{
		// make sure we are still connected.
		while (!(app.currentState == STATE_ACTIVE) | (app.currentState == STATE_WAIT_PINGRESP))
		{
			app.connect();
			_delay_ms(2500); //wait for response.
			app.tick();
		}
		statusLed(HIGH); // show we are connected with a long flash.
		_delay_ms(1000);
		statusLed(LOW);
		printf("CONNECTED\n");
		//get the topic id of the publish topis
				pub1id = app.gettopicid((unsigned char*)"d/"MAC_SUFF"/"ID1"/"TOPIC_REQUEST, 0x0C);
				_delay_ms(5);
		
		printf("Button Press mapped to %d\n", pub1id);
		//having an inner loop ensures that we don't have to get the topic id every loop, only when we get disconnected.
		while(app.currentState != STATE_DISCONNECTED)
		{
			app.tick();// keep the network ticking over.
			//check if the button has been pressed.
			if (!(ID1_PINPORT & (1<<ID1_PIN))) // pullup so we want to check if it is low.
			{
				_delay_ms(1);
				//kinda debounce.
				if (!(ID1_PINPORT & (1<<ID1_PIN)))
				{
					//make sure the button hasn't just been held down for a long time.
					if (id1button == false)
					{
						statusLed(HIGH); // show that we are processing the request
						//the button has been pushed., publish a Request.
						
						app.publish(pub1id,changestate, 0x01); // we don't know what the state is that we are controlling, changestate is just a state change request, let the server sort it out to a value.
						statusLed(LOW);
						id1button = true;
					}
				}
			}
			else
			{
				id1button = false;
			}
			
			
		}
		printf("DISCONNECTED\n");
	}
}
void setup(void)
{
	USARTinit(); //enable serial output;
	Timing::init();// start the timer - used for the millis function (rough time since powered on);
	network.setup(THIS_LEVEL, THIS_DEV); //ensure network is setup before any MQTT work is done.
	//setup outputs
	STATUS_LED_1_DDR |= (1<<STATUS_LED_1); //STATUS1_LED
	//setup input buttons
	ID1_DDR &= ~(1<<ID1_PIN);
	//set pullups.
	ID1_PORT |= (1<<ID1_PIN);
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
void statusLed(uint8_t level)
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
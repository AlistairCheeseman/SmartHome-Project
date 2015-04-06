/*
* EnvironmentSensor.cpp
*
* Created: 29/03/2015 06:45:32
*  Author: Ali
*/


#include <avr/io.h>

#include "EnvironmentSensorRevA.h"
#define BAUD 9600
#include <util/setbaud.h>
#include "Timing.h"

#include "MQTTSN.h"
#include "SensorNet.h"



void adc_init();
void i2c_init();
uint16_t  adc_read(uint8_t ch);
int USART0SendByte (char c, FILE *stream);
int USART0ReceiveByte(FILE *stream);
void USARTinit(void);
void setup(void);
double readALS(void);
uint16_t getHumid(void);
uint16_t getTemp(void);

FILE * usart0_str;
RF24 radio;
SensorNet network(radio, MAC_SUFF_HEX);
MQTTSN app(network,(uint8_t) 0x28, MAC_SUFF_HEX);

int main(void)
{
	setup();
	
	while(1)
	{
		// sleep for a period of time
		_delay_ms(10000);
		double ALS = readALS();
		uint16_t humid = getHumid();// make sure the humidity is read first, the temperature is read by the chip when measuring humidity. so to reduce time a special command can be issued after the humidity for quick temp readouts.
		uint16_t temp = getTemp();
		char humidC[4];
		char ALSC[4];
		char tempC[4];
		ltoa(humid, humidC, 10);
		ltoa(ALS,ALSC, 10);
		ltoa(temp, tempC, 10);
		printf("Humidity: %s\n", humidC);
		printf("ALS:      %s\n", ALSC);
		printf("Temp:     %s\n", tempC);
		while (app.currentState != STATE_ACTIVE)
		{
			//_delay_ms(2000); //dont spam the network with reconnections.
			app.connect();
			_delay_ms(2500); //wait for response.
			app.tick();
		}
		uint8_t	pub1id = app.gettopicid((unsigned char*)"d/"MAC_SUFF"/"ID1"/"TOPIC_STATUS_UPDATE, 0x0C);
		uint8_t	pub2id = app.gettopicid((unsigned char*)"d/"MAC_SUFF"/"ID2"/"TOPIC_STATUS_UPDATE, 0x0C);
		uint8_t	pub3id = app.gettopicid((unsigned char*)"d/"MAC_SUFF"/"ID3"/"TOPIC_STATUS_UPDATE, 0x0C);
		
		app.publish(pub1id,(unsigned char*)humidC, strlen(humidC));
		app.publish(pub2id,(unsigned char*)tempC, strlen(tempC));
		app.publish(pub3id,(unsigned char*)ALSC, strlen(ALSC));
		// on wake get the readings from sensors: light level, humidity, temperature.
		// connect to the wireless Net and send sensor data.
		// go back to sleep.
	}
}
double readALS()
{
	//turn on the sensor.
	
	ALS_ENABLE_PORT &= ~(1<< ALS_ENABLE_PIN);
	//need a minimum of 0.2milliseconds for the sensor to activate fully.
	_delay_ms(200);
	uint16_t reading1 =  adc_read(ALS_CH);
	_delay_ms(200);
	uint16_t reading2 =  adc_read(ALS_CH);
	_delay_ms(200);
	uint16_t reading3 =  adc_read(ALS_CH);
	_delay_ms(200);
	printf("%d, %d, %d\n", reading1, reading2, reading3);
	// 0.25V -> 10 lux
	// 1.25V -> 100000 lux
	
	//turn off the sensor.
	ALS_ENABLE_PORT |= (1<< ALS_ENABLE_PIN);
	//need a minimum of 0.2milliseconds for the sensor to deactivate fully.
	_delay_ms(200);
	return (double)(((reading1+reading2+reading3)/3.0) + ADC_OFFSET) * ADC_LINEAR_SCALE;
	
}
void setup()
{
	USARTinit(); //enable serial output;
	Timing::init();// start the timer - used for the millis function (rough time since powered on);
	adc_init();
	i2c_init();
	network.setup(THIS_LEVEL, THIS_DEV); //ensure network is setup before any MQTT work is done.
	ALS_ENABLE_DDR |= (1<<ALS_ENABLE_PIN); //set als enable pin as output.
	
}
void adc_init()
{
	/* AREF = AVcc*/
	//	ADMUX |= (1<<REFS0);
	/*AREF = (ARef Pin Voltage)*/
	ADMUX &=~(1<<REFS0);
	ADMUX &=~(1<<REFS1);
	
	/*right adjust result */
	ADMUX &= ~(1<<ADLAR);


	/* Set Prescaler for ADC Speed */
	//111 = CLK / 128
	//ADCSRA |=(1<<ADPS2);
	//ADCSRA |=(1<<ADPS1);
	//ADCSRA |=(1<<ADPS0);
	//128 is too slow.
	//set to /2 max speed
	ADCSRA &=~(1<<ADPS2);
	ADCSRA &=~(1<<ADPS1);
	ADCSRA |=(1<<ADPS0);
	
	
	ADCSRA |= (1<<ADEN); //Enable the ADC now it has been configured
}
void i2c_init(){
	
}
// read adc value
uint16_t adc_read(uint8_t ch)
{
	// select the corresponding channel 0~7
	// ANDing with '7' will always keep the value
	// of 'ch' between 0 and 7
	ch &= 0b00000111; // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing

	// start single conversion
	// write '1' to ADSC
	ADCSRA |= (1<<ADSC);

	// wait for conversion to complete
	// ADSC becomes '0' again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));

	return (ADC);
}
uint16_t getHumid(void)
{
	return 0;
}
uint16_t getTemp(void)
{
	return 0;
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

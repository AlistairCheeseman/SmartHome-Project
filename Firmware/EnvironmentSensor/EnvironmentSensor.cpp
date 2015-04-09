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
#include <util/twi.h>
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
uint16_t readALS(void);
uint8_t getHumid(void);
uint8_t getTemp(void);

FILE * usart0_str;
RF24 radio;
SensorNet network(radio, MAC_SUFF_HEX);
MQTTSN app(network,THIS_MQTTCLIENTID, MAC_SUFF_HEX);




int main(void)
{
	setup();
	
	while(1)
	{
		// sleep for a period of time ( 2 mins)
		for (int c = 0; c<120;c++) // report every minute or so. ( 120 * 500msec) =
		{
			delay(500); // wait 1/2 second
			app.tick(); // tick the network to route any packets or pings.
		}
				for (int c = 0; c<120;c++) // report every minute or so. ( 120 * 500msec) =
				{
					delay(500); // wait 1/2 second
					app.tick(); // tick the network to route any packets or pings.
				}
		uint16_t ALS = readALS();
		uint8_t humid = getHumid();// make sure the humidity is read first, the temperature is read by the chip when measuring humidity. so to reduce time a special command can be issued after the humidity for quick temp readouts.
		uint8_t temp = getTemp();
		char humidC[4];
		char ALSC[6]; //10,000 max value
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
			delay(200); // wait 1/5 second
		app.publish(pub2id,(unsigned char*)tempC, strlen(tempC));
			delay(200); // wait 1/5 second
		app.publish(pub3id,(unsigned char*)ALSC, strlen(ALSC));
			delay(200); // wait 1/5 second
		// on wake get the readings from sensors: light level, humidity, temperature.
		// connect to the wireless Net and send sensor data.
		// go back to sleep.
	}
}
uint16_t readALS()
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
	uint16_t  averageRead = ((reading1+reading2+reading3)/3.0);
	//rather than use lookup table, have defined set areas of linearity. this is due to datasheet not being fully concise.
	if (averageRead <77)
	return 10; // return min value.
	else if (averageRead < 167)
	return (averageRead * 0.5988);
	else if (averageRead < 251)
	return (averageRead * 3.9840);
	else if (averageRead < 335)
	return (averageRead * 29.8507);
	else
	return 0x2710; // return max value 10,000
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
	//Si7006 max clock speed is 400KHz, <100KHz with minimal clock skew.
	// SCL speed = F_CPU / (16 + 2(TWBR)*prescaler))
	// running at 80KHz with a 8MHz clock, the best values are 42 bit rate and 1 prescaler.
	TWBR = 0x2a; // set bit rate to 42.
	TWSR &= ~((1<<TWPS1) | (1<<TWPS0)); // set prescaler to 1, 00
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
uint8_t getHumid(void)
{
	uint16_t rhCode = 0; // the received data from the sensor.
	//send start condition
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // clear interrupt flag ( to one), set start and enable conditions.
	// wait for twint flag set. this indicated that the START condition has been transmitted.
	while (!(TWCR & (1<<TWINT))) // ( wait for the interrupt flag ( set to zero)
	;
	// check the value of TWI status register. Mask prescaler bits. If status different from START go to ERROR.
	if ((TWSR & 0xF8) != TW_START)
	{
		//ERROR();
	}
	
	
	//load SLA_W into TWDR Register. Clear TWINT bit in TWCR to start transmission of address.
	TWDR = (0x40<<1)|TW_WRITE; // shift 7 bit address register and set write flag.
	TWCR = (1<<TWINT) |	(1<<TWEN); // clear interrupt flag, enable two wire. ( go send the data)
	
	//Wait for TWINT Flag set. This indicates that the SLA+W has been transmitted , and ACK/NACK has been received.
	while (!(TWCR & (1<<TWINT)))
	;
	
	//check the value of TWI status Register. Mask pre-scaler buts. If status different from MT_SLA_ACK goto ERROR
	if ((TWSR & 0xF8) != TW_MT_SLA_ACK)
	{
		//	ERROR();
	}
	
	
	
	//Load DATA into TWDR Register. Clear TWINT bit in TWCR to start transmission of data
	TWDR = 0xE5; // measure humidity, Hold Master Mode
	TWCR = (1<<TWINT) |	(1<<TWEN); //go
	
	// Wait for TWINT Flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
	while (!(TWCR & (1<<TWINT)))
	;
	
	//Check value of TWI Status Register. Mask prescaler bits. If status different from MT_DATA_ACK go to ERROR
	if ((TWSR & 0xF8) != TW_MT_DATA_ACK) // data txd with ack.
	{
		//ERROR();
	}
	// the address and command has been sent. Now send a repeated start and read, clock stretch and then get data.
	
	
	//send repeated start condition ( hardware state machine detects if it is a repeated start or a start.
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // clear interrupt flag ( to one), set start and enable conditions.
	// wait for twint flag set. this indicated that the START condition has been transmitted.
	while (!(TWCR & (1<<TWINT))) // ( wait for the interrupt flag ( set to zero)
	;
	// check the value of TWI status register. Mask prescaler bits. If status different from REPEATED START go to ERROR.
	if ((TWSR & 0xF8) != TW_REP_START)
	{
		//ERROR();
	}
	
	
	//load SLA_R into TWDR Register. Clear TWINT bit in TWCR to start transmission of address.
	TWDR = (0x40<<1)|TW_READ; // shift 7 bit address register and set read flag.
	TWCR = (1<<TWINT) |	(1<<TWEN); // clear interrupt flag, enable two wire. ( go send the data)
	
	//Wait for TWINT Flag set. This indicates that the SLA+W has been transmitted , and ACK/NACK has been received.
	while (!(TWCR & (1<<TWINT)))
	;
	
	//check the value of TWI status Register. Mask pre-scaler buts. If status different from Master Receiver SLA_ACK goto ERROR
	if ((TWSR & 0xF8) !=  TW_MR_SLA_ACK)
	{
		//	ERROR();
	}
	
	//now set it so that we send ack and enable TWI.
	TWCR = (1<<TWINT)|	(1<<TWEN)|(1<<TWEA);
	
	while (!(TWCR & (1<<TWINT))) // ( wait for the interrupt flag ( set to zero)
	;

	//check the value of TWI status Register. Mask pre-scaler buts. If status different from Master Receiver DATA rec ACK sent goto ERROR
	if ((TWSR & 0xF8) !=  TW_MR_DATA_ACK)
	{
		//	ERROR();
	}
	// data has been received and status good.
	rhCode = (TWDR<<8); //put msbyte in upper byte
	
	
	
	//for the last bit no ACK is sent.
	TWCR = (1<<TWINT)|	(1<<TWEN);;
	// ( wait for the interrupt flag ( set to zero)
	while (!(TWCR & (1<<TWINT)))
	;
	//check the value of TWI status Register. Mask pre-scaler buts. If status different from Master Receiver DATA rec ACK NOT sent goto ERROR
	if ((TWSR & 0xF8) !=  TW_MR_DATA_NACK)
	{
		//	ERROR();
	}
	rhCode |=(uint16_t) (TWDR); //put LSByte in lower byte
	
	
	//Transmit STOP condition
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	

	int res = (((rhCode * 125.0)/65536.0) - 6.0); // factor correction. to get percentage.
	if ((res >= 0) && (res <= 100))
	return (uint8_t) res;
	else
	return 0;
}
uint8_t getTemp(void)
{
	uint16_t tCode = 0; // the recieved code
	//send start condition
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // clear interrupt flag ( to one), set start and enable conditions.
	// wait for twint flag set. this indicated that the START condition has been transmitted.
	while (!(TWCR & (1<<TWINT))) // ( wait for the interrupt flag ( set to zero)
	;
	// check the value of TWI status register. Mask prescaler bits. If status different from START go to ERROR.
	if ((TWSR & 0xF8) != TW_START)
	{
		//ERROR();
	}
	
	
	//load SLA_W into TWDR Register. Clear TWINT bit in TWCR to start transmission of address.
	TWDR = (0x40<<1)|TW_WRITE; // shift 7 bit address register and set write flag.
	TWCR = (1<<TWINT) |	(1<<TWEN); // clear interrupt flag, enable two wire. ( go send the data)
	
	//Wait for TWINT Flag set. This indicates that the SLA+W has been transmitted , and ACK/NACK has been received.
	while (!(TWCR & (1<<TWINT)))
	;
	
	//check the value of TWI status Register. Mask pre-scaler buts. If status different from MT_SLA_ACK goto ERROR
	if ((TWSR & 0xF8) != TW_MT_SLA_ACK)
	{
		//	ERROR();
	}
	
	
	
	//Load DATA into TWDR Register. Clear TWINT bit in TWCR to start transmission of data
	TWDR = 0xE0; // measure humidity, Hold Master Mode
	TWCR = (1<<TWINT) |	(1<<TWEN); //go
	
	// Wait for TWINT Flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
	while (!(TWCR & (1<<TWINT)))
	;
	
	//Check value of TWI Status Register. Mask prescaler bits. If status different from MT_DATA_ACK go to ERROR
	if ((TWSR & 0xF8) != TW_MT_DATA_ACK) // data txd with ack.
	{
		//ERROR();
	}
	// the address and command has been sent. Now send a repeated start and read, clock stretch and then get data.
	
	
	//send repeated start condition ( hardware state machine detects if it is a repeated start or a start.
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // clear interrupt flag ( to one), set start and enable conditions.
	// wait for twint flag set. this indicated that the START condition has been transmitted.
	while (!(TWCR & (1<<TWINT))) // ( wait for the interrupt flag ( set to zero)
	;
	// check the value of TWI status register. Mask prescaler bits. If status different from REPEATED START go to ERROR.
	if ((TWSR & 0xF8) != TW_REP_START)
	{
		//ERROR();
	}
	
	
	//load SLA_R into TWDR Register. Clear TWINT bit in TWCR to start transmission of address.
	TWDR = (0x40<<1)|TW_READ; // shift 7 bit address register and set read flag.
	TWCR = (1<<TWINT) |	(1<<TWEN); // clear interrupt flag, enable two wire. ( go send the data)
	
	//Wait for TWINT Flag set. This indicates that the SLA+W has been transmitted , and ACK/NACK has been received.
	while (!(TWCR & (1<<TWINT)))
	;
	
	//check the value of TWI status Register. Mask pre-scaler buts. If status different from Master Receiver SLA_ACK goto ERROR
	if ((TWSR & 0xF8) !=  TW_MR_SLA_ACK)
	{
		//	ERROR();
	}
	
	//now set it so that we send ack and enable TWI.
	TWCR = (1<<TWINT)|	(1<<TWEN)|(1<<TWEA);
	
	while (!(TWCR & (1<<TWINT))) // ( wait for the interrupt flag ( set to zero)
	;

	//check the value of TWI status Register. Mask pre-scaler buts. If status different from Master Receiver DATA rec ACK sent goto ERROR
	if ((TWSR & 0xF8) !=  TW_MR_DATA_ACK)
	{
		//	ERROR();
	}
	// data has been received and status good.
	tCode = (TWDR<<8); //put msbyte in upper byte
	
	
	
	//for the last bit no ACK is sent.
	TWCR = (1<<TWINT)|	(1<<TWEN);;
	// ( wait for the interrupt flag ( set to zero)
	while (!(TWCR & (1<<TWINT)))
	;
	//check the value of TWI status Register. Mask pre-scaler buts. If status different from Master Receiver DATA rec ACK NOT sent goto ERROR
	if ((TWSR & 0xF8) !=  TW_MR_DATA_NACK)
	{
		//	ERROR();
	}
	tCode |=(uint16_t) (TWDR); //put LSByte in lower byte
	
	
	//Transmit STOP condition
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	
	
	
	
	
	
	int res = (((tCode * 175.72)/65536.0) - 46.85); // factor correction. to get temperature in celcius.
	if ((res >= 0) && (res <= 100))
	return (uint8_t) (res + 100);// 0 deg C == 100, 100C = 200, -100C = 0.
	else
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

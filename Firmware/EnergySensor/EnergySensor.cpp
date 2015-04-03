/*
* EnergySensor.cpp
*
* Created: 16/03/2015 09:09:38
*  Author: Ali
* Energy calculations inspired by https://github.com/openenergymonitor/EmonLib
*/


#include <avr/io.h>
#include <string.h>


#include "EnergySensorRevA.h"
#define BAUD 9600
#include <util/setbaud.h>
#include "Timing.h"

#include "MQTTSN.h"
#include "SensorNet.h"


void adc_init();
uint16_t  adc_read(uint8_t ch);
int USART0SendByte (char c, FILE *stream);
int USART0ReceiveByte(FILE *stream);
void USARTinit(void);
void connectedLed(uint8_t level);
void pendingLed(uint8_t level);
void setup(void);
void calcPwr(double &power, double &pf);



FILE * usart0_str;
RF24 radio;
SensorNet network(radio, MAC_SUFF_HEX);
MQTTSN app(network,(uint8_t) 0x27, MAC_SUFF_HEX);


int main(void)
{
	delay(2000); // wait two seconds for power supply to stabilize.THIS IS COMPULSORY OR CONFIG IN SETUP GOES MAD!
	setup();
	while(1)
	{
		for (int c = 0; c<120;c++) // report every minute or so. ( 120 * 500msec) =
		{
			delay(500); // wait one second
			app.tick(); // tick the network to route any packets or pings.
		}
		//take three readings.
		double pow1, pow2,pow3;
		double pf1, pf2, pf3;
		calcPwr(pow1, pf1);
		calcPwr(pow2, pf2);
		calcPwr(pow3, pf3);
		//average them.
		double averagePower =(pow1+pow2+pow3) /3;
		double averagePF = (pf1+pf2+pf3)/3;
		
		//strip to 1 sig figure.
		unsigned int temp =(unsigned int) (averagePower *10.0); // shift to the left by 1dp and by putting into int the extra values are lost.
		averagePower = temp /10.0; //shift back and put into the original value.
		
		//strip to  2 sig figure.
		temp =(unsigned int)(averagePF * 100.0); // shift to the left by 2dp and by putting into int the extra values are lost.
		averagePF = temp /100.0;
		
		
		//buffers to store text of values.
		char power [7];// maximum wattage (100A ( current meter max) * 300V (we definetley don't want any rms higher than that.)) 30000.0 (6+1 digits)
		char pf [4]; // 0.01 precision + 1
		//shift so no DP. ( saves space)
		int	avPFShift = averagePF * 100; // shift to left 2dp. will give 0.01 precision.
		int avPowerShift = averagePower * 10; // shift left 1dp. will give 0.1W precision
		
		//convert to strings
		itoa(avPowerShift,power, 10);
		itoa(avPFShift, pf, 10);


		// make sure we are still connected.
		while (!(app.currentState == STATE_ACTIVE) | (app.currentState == STATE_WAIT_PINGRESP))
		{
			connectedLed(LOW);
			//_delay_ms(2000); //dont spam the network with reconnections.
			app.connect();
			_delay_ms(2500); //wait for response.
			app.tick();
		}
		connectedLed(HIGH);
		char  payload [10];
		//format the final payload.
		sprintf((char*)payload,"%s,%s", power, pf);
		payload [10] = 0x00;
		// get the size of the payload
		uint8_t plen = (unsigned)strlen(payload);
		//get the id of the topic we are going to publish to.
		uint16_t topicid = app.gettopicid((unsigned char*)"d/"MAC_SUFF"/"ID1"/"TOPIC_STATUS_UPDATE, 0x0C);
		//send the message.
		app.publish(topicid,(unsigned char*)payload,plen);
	}
	
}
void calcPwr(double &power, double &pf)
{
	unsigned int timeout = 2000;
	//how many zero crossings to measure
	unsigned int crossingCount = 20;
	//initial offset for filter at 1024/2
	double filterV = 512;
	double filterA = 512;
	//the number of times zero has been crossed
	unsigned int currentCrossCount = 0;
	
	//the number of samples taken.
	unsigned int numberOfSamples = 0;
	
	
	bool lastVCross;
	bool checkVCross = false;
	bool go = false;
	int initialV;
	double summedV = 0, summedA=0, summedP = 0;
	unsigned long start = Timing::millis();
	
	while (go == false)
	{//check if voltage is around 0 mark.
		initialV = adc_read(VOLTAGE_CH);
		if ((initialV < (1024*0.55)) &&(initialV >(1024*0.45) ))
		{
			go = true;
		}//if not found in time just continue
		if ((Timing::millis()-start) > timeout)
		{
			go = true;
		}
	}
	start = Timing::millis();
	
	
	while ((currentCrossCount < crossingCount) && ((Timing::millis()-start)<timeout))
	{
		numberOfSamples++;
		//read the instantaneous voltage and current
		int instV = adc_read(VOLTAGE_CH);
		int instA = adc_read(CURRENT_CH);
		
		filterV = filterV + ((instV-filterV)/1024);
		double filteredV = instV - filterV;
		
		filterA = filterA + ((instA-filterA)/1024);
		double filteredA = instA - filterA;
		
		double sqV= filteredV * filteredV;
		double sqA= filteredA * filteredA;
		summedV += sqV;
		summedA += sqA;
		summedP += filterA * filterV;
		
		
		
		
		
		lastVCross = checkVCross;
		if (instV > initialV) checkVCross = true;
		else checkVCross = false;
		if (numberOfSamples==1) lastVCross = checkVCross;
		if (lastVCross != checkVCross) currentCrossCount++;
	}
	int Tout  = (sqrt(summedV / numberOfSamples))  * (3.30/1024) * 248.528; //was 230.412
	
	double rmsARaw =(sqrt(summedA / numberOfSamples)-2.9);

	if (rmsARaw < 0)
	rmsARaw =0;
	
	double AAvg = (rmsARaw * (3.30/1024.0)) * 90.9  ;
	power =AAvg * Tout;
	int raw = AAvg;


	printf("Current (RMS)         : %d\n", raw);
	power = AAvg * Tout;
	//	printf("Voltage (RMS)         : %dV\n", Tout);
	pf = 1;
}
void setup(void)
{
	USARTinit(); //enable serial output;
	Timing::init();// start the timer - used for the millis function (rough time since powered on);
	adc_init();
	network.setup(THIS_LEVEL, THIS_DEV); //ensure network is setup before any MQTT work is done.
	STATUS_LED_1_DDR |= (1<<STATUS_LED_1); //STATUS1_LED
	STATUS_LED_2_DDR |= (1<<STATUS_LED_2); //STATUS2_LED

	connectedLed(LOW);
	pendingLed(LOW);

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

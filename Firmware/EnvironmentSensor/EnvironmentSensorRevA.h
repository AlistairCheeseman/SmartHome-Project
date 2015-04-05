/*
 * EnvironmentSensorRevA.h
 *
 * Created: 29/03/2015 07:02:30
 *  Author: Ali
 */ 


#ifndef ENVIRONMENTSENSORREVA_H_
#define ENVIRONMENTSENSORREVA_H_

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

//Ambient Light Sensor Pins.
#define ALS_CH 0
#define ALS_ENABLE_PORT  PORTD
#define ALS_ENABLE_PIN	PORTD2
#define ALS_ENABLE_DDR  DDRD

//3.3/1024 = v/bit
//reading * v/bit = vout at pin

//for 27K burden, scale is log scale so have to use a look up table. for time being implementing a 1024 or so value LUT may have issues. so will keep this a const numberto be improved upon later
//0.25 / log10 = 0.25
//1.75 / log100000 = 0.35




#define ADC_LINEAR_SCALE  1
// the dc offset of the voltage read.
#define ADC_OFFSET 0



#define MAC_SUFF "6E6E6E"
#define MAC_SUFF_HEX 0x6E6E6E
//this should be assigned dynamically but for time being will be manual
#define THIS_LEVEL 1
#define THIS_DEV 4

//MQTT ID Mappings (ASCII VALUE FOR NUMBER)
#define ID1 "1" //0x31 //1 maps to Humidity
#define ID2 "2" //0x32 //2 maps to Temperature
#define ID3 "3" //0x32 //2 maps to Light Level
#endif /* ENVIRONMENTSENSORREVA_H_ */
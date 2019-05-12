#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "i2c_master.h"
#include "9960.h"
#define BAUD 9600
#define BAUDR (F_CPU/16/BAUD) - 1

void UART_initialization();					//	Function declarations
void Colors();								//
void sen9960_initialization();				//
int sending_char( char info, FILE * str);	//

uint16_t baud_rate = BAUDR;		// Int declaration
uint8_t red_low, red_high;		//
uint8_t blue_low, blue_high;	//
uint8_t green_low, green_high;	//
uint8_t config;					//

FILE UART_str = FDEV_SETUP_STREAM(sending_char, NULL , _FDEV_SETUP_WRITE);
char results[256];			// Array results

int main(void)
{
	uint16_t red = 0;		// Declare red
	uint16_t blue = 0;		// Declare blue
	uint16_t green = 0;		// Declare green
	
	i2c_init();				// i2C function call
	UART_initialization();	// uart initialization function call
	stdout = &UART_str;		// standard stream
	sen9960_initialization();  // initialize 9960 sensor function call
	
	_delay_ms(2000);	// Give time of 2s
	printf("AT\r\n");	// AT cmd	
	_delay_ms(3000);	// Give time of 3s
	printf("AT+CWMODE=1\r\n");	// AT cmd mode	
	_delay_ms(3000);	// Give time of 3s
	printf("AT+CWJAP=\"MOE_Wifi\",\"moo123456\"\r\n");	// AT cmd to connect to personal hotspot information	
	while (1)
	{
		_delay_ms(1500);				// Give time of 1.5s
		
		printf("AT+CIPMUX=0\r\n");		// AT cmd for CIPMUX
		
		_delay_ms(1500);				// Give time of 1.5s
		
		printf("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");  // AT cmd to communicate to thingspeak
		
		_delay_ms(1500);				// Give time of 1.5s
		
		Colors(&red, &blue, &green);	// Call colors function
		
		printf("AT+CIPSEND=100\r\n");	// AT cmd to send 100 chars
	
		printf("GET https://api.thingspeak.com/update?api_key=VBIZQ9TJL06VKSR8&field1=%05u\r\n", red); // Send red value to thingspeak

		
		_delay_ms(1500);				// Give time of 1.5s
		
		printf("AT+CIPMUX=0\r\n");		// AT cmd for CIPMUX
		
		_delay_ms(1500);				// Give time of 1.5s
		
		printf("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");  // AT cmd to communicate to thingspeak
		
		_delay_ms(1500);				// Give time of 1.5s
	
		Colors(&red, &blue, &green);	// Call colors function
		
		printf("AT+CIPSEND=100\r\n");	// AT cmd to send 100 chars
		
		printf("GET https://api.thingspeak.com/update?api_key=VBIZQ9TJL06VKSR8&field2=%05u\r\n", blue);	// Send blue value to thingspeak
		
	
		_delay_ms(1500);				// Give time of 1.5s
		
		printf("AT+CIPMUX=0\r\n");		// AT cmd for CIPMUX
		
		_delay_ms(1500);				// Give time of 1.5s
		
		printf("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");  // AT cmd to communicate to thingspeak
		
		_delay_ms(1500);				// Give time of 1.5s
		
		Colors(&red, &blue, &green);	// Call colors function
		
		printf("AT+CIPSEND=100\r\n");	// AT cmd to send 100 chars
		
		printf("GET https://api.thingspeak.com/update?api_key=VBIZQ9TJL06VKSR8&field3=%05u\r\n", green); // Send green value to thingspeak
		
		_delay_ms(1500);				// Give time of 1.5s
	}
}
void UART_initialization(void)
{
	UBRR0H = baud_rate >> 8;		// Setting the baud rate
	UBRR0L = baud_rate & 0xFF;		//
	
	UCSR0B = ( 1 << RXEN0) | ( 1 << TXEN0) | (1 << RXCIE0);	// Receive and transmit are enabled
	
	UCSR0C = (3 << UCSZ00);	// Set frame format: 8data, 1stop bit
}

int sending_char(char info, FILE *str)
{
	while ( !( UCSR0A & ( 1 << UDRE0)) );	// Wait until buffer empty
	
	UDR0 = info;	// Data placed into buffer

	return 0; 
}

void Colors(uint16_t *red, uint16_t *blue, uint16_t *green)	 // Function used to read the colors for sensor (red, blue and green)
{
	i2c_readReg(APDS_WRITE, APDS9960_RDATAL, &red_low, 1);
	
	i2c_readReg(APDS_WRITE, APDS9960_RDATAH, &red_high, 1);
	
	i2c_readReg(APDS_WRITE, APDS9960_GDATAL, &green_low, 1);
	
	i2c_readReg(APDS_WRITE, APDS9960_GDATAH, &green_high, 1);
	
	i2c_readReg(APDS_WRITE, APDS9960_BDATAL, &blue_low, 1);
	
	i2c_readReg(APDS_WRITE, APDS9960_BDATAH, &blue_high, 1);
	
	*red = red_high << 8 | red_low;
	
	*blue = blue_high << 8 | blue_low;
	
	*green = green_high << 8 | green_low;
	
}

void sen9960_initialization()	// Function used to initialize the Sensor only for the RGB, no gesture or proximity used
{	
	i2c_readReg(APDS_WRITE, APDS9960_ID, &config,1);
	
	if(config != APDS9960_ID_1)
	while(1)
	{
		config = 1 << 1 | 1 << 0 | 1 << 3 | 1 << 4;
	}
	
	i2c_writeReg(APDS_WRITE, APDS9960_ENABLE, &config, 1);
	config = DEFAULT_ATIME;
	
	i2c_writeReg(APDS_WRITE, APDS9960_ATIME, &config, 1);
	config = DEFAULT_WTIME;
	
	i2c_writeReg(APDS_WRITE, APDS9960_WTIME, &config, 1);
	config = DEFAULT_PROX_PPULSE;
	
	i2c_writeReg(APDS_WRITE, APDS9960_PPULSE, &config, 1);
	config = DEFAULT_POFFSET_UR;
	
	i2c_writeReg(APDS_WRITE, APDS9960_POFFSET_UR, &config, 1);
	config = DEFAULT_POFFSET_DL;
	
	i2c_writeReg(APDS_WRITE, APDS9960_POFFSET_DL, &config, 1);
	config = DEFAULT_CONFIG1;
	
	i2c_writeReg(APDS_WRITE, APDS9960_CONFIG1, &config, 1);
	config = DEFAULT_PERS;
	
	i2c_writeReg(APDS_WRITE, APDS9960_PERS, &config, 1);
	config = DEFAULT_CONFIG2;
	
	i2c_writeReg(APDS_WRITE, APDS9960_CONFIG2, &config, 1);
	config = DEFAULT_CONFIG3;
	
	i2c_writeReg(APDS_WRITE, APDS9960_CONFIG3, &config, 1);
}

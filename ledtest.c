#include <avr/io.h>
#include <stdlib.h>
#include "ledcontrol.h"
#include <util/delay.h>

#define F_CPU 8000000UL   //internal RC oscillator 8MHz
#include <util/delay.h>
#define nrCol  8  //nr. of columns
#define PERIOD 10 //in milliseconds

#define DUTY_CYCLE 32 //used in start_buzzer() funct.

//shift register output pins from PORTB
#define CLK   0 //PB0
#define S_IN  1 //PB1
#define LATCH 2 //PB2

#define _B_TIME 200


void initialize(void)
{
    /*
        Port A (sequence)

        Set all 8 pins as output pins
        One hot code will be stored in PORTA register
        Column driver
    */
    DDRA = 0xFF;
    PORTA = 0xF1;

    /*
        Port D (data)

        Set all 8 pins as output pins
        Column data will be stored in PORTD register
    */
    DDRD = 0x00;
    PORTD = 0x00;

    /*
        Port B (buttons)

        Set last 4 pins as input pins
        Button state will be read from PINB
    */
    DDRB &= ~0xF0;
    

    /*
        Port B (buzzer + reg_shift)

        Set first 4 pins as output pins
        PB0, PB1, PB2 - for the shift register
        PB3 - for the buzzer (PWM controlled)
    */
    DDRB |= 0x0F;

    /*
        FCPU / Prescaler = frequency at which counter increments by 1

        PWM - power width modulation setup
        Fast PWM (WGM)
        Non inverting (COM)
       
        Prescaler
        CS02 CS01 CS00
        001 - 1; 010 - 8; 011 - 64; 100 - 256; 101 - 1024
    */
    TCCR0 |= (1<<WGM01) | (1<<WGM00) | (1<<COM01) | (0<<CS02) | (1<<CS01) | (1<<CS00);
    TCCR0 &= ~((1<<COM01) | (1<<COM00)); //for now deactivate output to buzzer
}

uint8_t numbers[10]={0x81,0xF3,0x49,0x61,0x33,0x45,0x05,0xF1,0x01,0x41 };
int main(void)
{
	initialize();
	uint8_t i;
	for(i=0;i<10;i++)
	{
		PORTA=numbers[i];
		PORTA|=0x01;
		_delay_ms(300);
		
		
		
	}
	return 0;
}







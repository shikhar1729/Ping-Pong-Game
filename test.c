#include <avr/io.h>
#include <stdlib.h>
#include "ledcontrol.h"

#define F_CPU 8000000UL   //internal RC oscillator 8MHz
#include <util/delay.h>

#define nrCol  8  //nr. of columns
#define PERIOD 10 //in milliseconds

#define DUTY_CYCLE 32 //used in start_buzzer() funct.

#define _B_TIME 200


uint8_t column[nrCol];
uint8_t l[12]={0,0,1,2,4,8,16,32,64,128,0,0}; //line mapping
uint8_t sw,sw1,sw2,score1,score2;
uint8_t PSPEED=10, BSPEED=20;
uint8_t go;

struct ball
{
	int8_t x;
	int8_t y;
	
	int8_t old_x;
	int8_t old_y;

	
	int8_t speed_x;
	int8_t speed_y;
}ball;

//+++ basic functions
//---------------------------------------------------------------------

void initialize(void);

//this function takes <PERIOD> milliseconds to execute
void display(uint8_t *column);

//reads current state of a button: 1 if pressed, 0 otherwise
//index: values from 1->4 (4 buttons)
uint8_t readButton(uint8_t index);
//Player 1 (right side of the board): 1(up), 2(down)
//Player 2 (left side of the board): 3(up), 4(down)

/* Buzzer related functions */
void start_buzzer(); //continue annoying until stop_buzzer is called
void stop_buzzer();  //end torment
void beep_once(uint16_t d_ms); //d_ms - time in milliseconds

/* 7 segment display related functions */
void update_score(uint8_t score_p1, uint8_t score_p2); //use this directly

//---------------------------------------------------------------------

void init_matrix(uint8_t *column)
{
    uint8_t i;
    for (i = 0; i < nrCol; i++)
        column[i] = 0x00; //turn all LEDs on
        
}

void clear()
{
	uint8_t i;
	clear_led_matrix(0);
    for (i = 0; i < nrCol; i++)
        column[i] = 0;
        
}

void long_display()
{
	uint8_t i,j;
	for(i=0 ;i<50; i++)
	 display(column);
	 
}
void initBall()
{
	int8_t v[2]={-1,1};
	int8_t v2[3]={-1,0,1};

	ball.x=3+rand()%2;
	ball.y=3+rand()%2;
	ball.old_x=ball.x;
	ball.old_y=ball.y;
	ball.speed_x=v[rand()%2];
	ball.speed_y=0;

	column[0]=column[7]=0x18;


}

void move_ball()
{
		
	uint8_t i;
	ball.old_x=ball.x;
	ball.x=ball.x + ball.speed_x;
	
	ball.old_y=ball.y;
	ball.y=ball.y + ball.speed_y;


	//Wall Collision

	if(ball.y==7 && ball.speed_y==1)
		ball.speed_y=-1;

	if(ball.y==0 && ball.speed_y==-1)
		ball.speed_y=1;
		
	sw1=0; sw2=0;



	//Paddle Collision

// Collision with the right paddle
	if(ball.x==6)
	{
		if((l[2+ball.y]+l[2+ball.y-1])==column[7]) 
		{
			ball.speed_x=-1;
			if(ball.speed_y!=1)
				ball.speed_y++;
			sw2=1;
		}
		

		if ((l[2+ball.y+1]+l[2+ball.y])==column[7])
		{
			ball.speed_x=-1;
			if(ball.speed_y!=-1)
				ball.speed_y--;
			sw2=1;
		}
		

		if ( ((l[2+ball.y+1]+l[2+ball.y+2])==column[7]) && (ball.speed_y==1))
		{
			ball.speed_x=-1;
			ball.speed_y=-1;
			sw2=1;
			
		}

		if ( ((l[2+ball.y-1]+l[2+ball.y-2])==column[7]) && (ball.speed_y==-1))
		{
			ball.speed_x=-1;
			ball.speed_y=1;
			sw2=1;
			
		}

	}

// Left Wins
	if (sw2==0 && ball.x==6)
	{
		score1++;
		//beep_once(_B_TIME);
		go=1;
		update_score(score1, score2);


		column[7]=255;
		column[ball.x]=l[2+ball.y];
		for (sw=0;sw<100;sw++) display(column);
		initBall();	}

// Collision with the left paddle
	if(ball.x==1)
	{
		if((l[2+ball.y]+l[2+ball.y-1])==column[0])  
		{
			ball.speed_x=1;
			if(ball.speed_y!=1)
				ball.speed_y++;
			
			sw1=1;
		}
	

		if ((l[2+ball.y+1]+l[2+ball.y])==column[0]) 
		{
			ball.speed_x=1;
			if(ball.speed_y!=-1)
				ball.speed_y--;
			sw1=1;
		}
		

		if ( ((l[2+ball.y+1]+l[2+ball.y+2])==column[0]) && (ball.speed_y==1))
		{
			ball.speed_x=1;
			ball.speed_y=-1;
			sw1=1;
		}

		if ( ((l[2+ball.y-1]+l[2+ball.y-2])==column[0]) && (ball.speed_y==-1))
		{
			ball.speed_x=1;
			ball.speed_y=1;
			sw1=1;
		}

		
	}

// Right wins
	if (sw1==0 && ball.x==1)
	{
		score2++;
		//beep_once(_B_TIME);
		go=1;
		update_score(score1, score2);

		column[0]=255;
		column[ball.x]=l[2+ball.y];
		for (sw=0;sw<100;sw++) display(column);
		initBall();
	}
// conditions for the corners of the field

	if (ball.x==1 && ball.y==0 && ball.speed_y==-1)
		ball.speed_y=1;

	if (ball.x==6 && ball.y==0 && ball.speed_y==-1)
		ball.speed_y=1;

	if (ball.x==1 && ball.y==7 && ball.speed_y==1)
		ball.speed_y=-1;

	if (ball.x==6 && ball.y==7 && ball.speed_y==1)
		ball.speed_y=-1;




//Ball LED ON
	column[ball.x]=l[2+ball.y];


}

void game()
{


	uint8_t i;
	
	clear();


	column[0] = 0x18;
	column[7] = 0x18;
	sw1=0; sw2=0;
	score1=0; score2=0;	

	uint8_t btn1 = 0, btn2 = 0, btn3 = 0, btn4 = 0;
	uint8_t counter = 0;
	uint8_t counter2 = 0;
	uint8_t c1=column[0],c2=column[7];

	initBall();

	for(;;)
	{

///////////////////////////////
///////////Button Read/////////
		btn1 = readButton(1);
		btn2 = readButton(2);
		btn3 = readButton(3);
		btn4 = readButton(4);

        if(counter>PSPEED)
		{
			if (btn1)
			{
				if((column[7] << 1) <=192)
				c2=column[7] = column[7] << 1;
			}

			if (btn2)
			{
				if((column[7] >> 1) >= 3)
				c2=column[7] = column[7] >> 1;
			}

			if (btn3)
			{
				if((column[0] << 1) <=192)
				c1=column[0] = column[0] << 1;
			}

			if (btn4)
			{
				if((column[0] >> 1) >= 3)
				c1=column[0] = column[0] >> 1;
			}
			counter=0;
		}
		counter++;
////////////////////////////
////////////////////////////


////////////////////////
////////Score Update////
		if(score1>9 || score2>9)
		 { 
		   score1=0;
		   score2=0;
		   update_score(score1,score2);
		  }
////////////////////////
////////////////////////

		for(i=1; i<nrCol-1; i++)
			column[i]=0;

		//column[7]=c2; 
		//column[0]=c1;

///////////////////////
////////Ball movement//
		if(counter2>BSPEED)
		{
		 move_ball();
		 counter2=0;
		}
		counter2++;
//////////////////////
//////////////////////

		display(column);
				
	}
	
}

uint8_t numbers1[10]={0x81,0xF3,0x49,0x61,0x33,0x45,0x05,0xF1,0x01,0x41 };
uint8_t numbers2[10]={0x81,0xF3,0x49,0x61,0x33,0x45,0x05,0xF1,0x01,0x41 };

int main(void)
{
    initialize();
    init_led_matrix(1);
    update_score(0, 0);
	game(); 
    return 0;
}

void initialize(void)
{
    /*
        Port A (sequence)

        Set all 8 pins as output pins
        They will be driving the 7-segment display
    */
    DDRA = 0xFF;
    PORTA = 0x81;

    /*
        Port D (data)

        Set all 8 pins as input pins
        Switch presses will be detected through this
    */
    DDRD = 0x00;
    PORTD = 0x00;

    /*
        Port B 
        
        Set first 4 pins as output pins
        They act as enable lines for 7-segment displays
        and for enabling the buzzer
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

void display(uint8_t *column)
{
    uint8_t j;
	
	
	for(j = 0; j<8; j++)
	 	set_row_led_matrix(0, j, column[j]);
        
	

	if (go==1) {
		beep_once(_B_TIME);
		go=0;
		}
}

uint8_t readButton(uint8_t index)
{
    if (index < 1 || index > 4) return 0;

    return !(PIND & (0x10 << (index - 1)));
}

void start_buzzer()
{
    TCCR0 |= (1<<COM01);  //activate output to buzzer
    OCR0 = DUTY_CYCLE;  //apply signal to buzzer
    //mean voltage: DUTY_CYCLE / 256 * 5V
}

void stop_buzzer()
{
    TCCR0 &= ~((1<<COM01) | (1<<COM00));  //deactivate output to buzzer
    OCR0 = 0; //even so, mean voltage 20mV (buzzer would still sound because of this
              //if output wasn't deactivated)
}

void beep_once(uint16_t d_ms)
{
    start_buzzer();
    _delay_ms(100);
    stop_buzzer();
}


void update_score(uint8_t score_p1, uint8_t score_p2)
{
	PORTB&=0xFC;
	PORTB|=0x01;
    PORTA=numbers1[score_p1];
    _delay_ms(200);
	PORTB&=0xFC;
	PORTB|=0x02;
    PORTA=(numbers2[score_p2] | 0x01);
    
}

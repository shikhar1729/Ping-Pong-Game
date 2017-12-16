#include <avr/io.h>
#include <stdlib.h>
#include "ledcontrol.h"
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

int melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};
//Mario main theme tempo
int tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};


#define F_CPU 8000000UL   //internal RC oscillator 8MHz
#include <util/delay.h>

#define nrCol  8  //nr. of columns
#define PERIOD 10 //in milliseconds

#define DUTY_CYCLE 32 //used in start_buzzer() funct.

#define _B_TIME 200


uint8_t column[nrCol];
uint8_t l[12]={0,0,1,2,4,8,16,32,64,128,0,0}; //line mapping
uint8_t sw,sw1,sw2,score1,score2;
uint8_t PSPEED=10, BSPEED=30;
uint8_t go;

void delay_ms(uint16_t d)
{
  uint16_t i;
  for (i = 0; i < d/10; ++i)
  {
    _delay_ms(1);
  }
}

void delay_us(uint16_t d)
{
	uint16_t i; 
	for(i = 0; i < d/10; ++i)
		_delay_us(1);
}

void beep (long frequency, long time)
{ 
   
    long x;   
    long delay_amount = (long)(1000000/frequency/2);
    long num_cycles = frequency*time/1000;
 
    for (x=0;x<num_cycles;x++)   
    {    
        PORTB = 0x04;
        delay_us(delay_amount);
        PORTB = 0x00;
        delay_us(delay_amount);
    }    

}    

//music for win or loose

void march()
{    
	int size = sizeof(melody) / sizeof(int);
	int note;
    for (note = 0; note < size; note++) {
 
      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int note_duration = 1000 / tempo[note];
 
      beep(melody[note], note_duration);
 
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pause_bw = note_duration * 1.30;
      
      delay_us(10*pause_bw);
 
      // stop the tone playing:
      beep(0, note_duration);
 	}    
}

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
    march();
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
    _delay_ms(50);
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

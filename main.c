/* 
create: 09.4.2019
autorTimers
Exercise 2

Comment by Antti: read the instructions: 
	Connect a switch to P0.3. 
	Connect a servo motor signal line to  P0.4. 

*/

#include <m8c.h>        // part specific constants and macros
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules

BOOL Button_Pressed = 0;	 // flag to indicate if the button is pressed
int nextPosition = 4500;	 //	the next postition that the motor should reach
int currentPosition = 4500;	 // the position that ne motor is at the moment
int step = 0;				 // how often the button is pressed
BOOL forSlowMove = FALSE;	 // flag to start a smoothly movement of the motor

// a function for the output on the lcd to control the reaction of the motor
void lcdOutput(int x,int y, int position){
	char intToString[5];
	itoa(intToString,position,10);
	LCD_Position(y,x);
	LCD_PrString(intToString);
}

void main(void)
{
	LCD_Start();   			// Initialize LCD
	
	M8C_EnableGInt ; 		// Turn on Interrupts
	M8C_EnableIntMask(INT_MSK0, INT_MSK0_GPIO);	//Enable GPIO Inturupt mask
	PWM16_EnableInt();		//Enables the interrupt mode operation
	PWM16_Start ();			//Starts the PWM16 User Module
   	SleepTimer_Start();		//Starts the SleepTimer User Module
   	SleepTimer_SetInterval(SleepTimer_8_HZ);   // Set interrupt to a
  	SleepTimer_EnableInt();                    // 8 Hz rate

	//Long term LCD-Output
	LCD_Position(1,0);	
	LCD_PrCString("Next:");  // for next  Position
	LCD_Position(1,10);
	LCD_PrCString("c:");	//c for currentPosition
	
	while(1){

		if (Button_Pressed){ 		//the button was pressed
			SleepTimer_SyncWait(4, SleepTimer_WAIT_RELOAD);	// to debaunce
			step++;					// the button was pressed once more
			lcdOutput(0,0,step);
			nextPosition += 900;	//a new nextPosition will be generate
			
			if ( nextPosition > 5900){	// to prevent that the Position 
				nextPosition = 3100;	// is out of the reach for the motor
			} 
			
			lcdOutput(5,1,nextPosition);
			currentPosition++;			// start to change the position
			PWM16_WritePulseWidth(currentPosition);	// Set pulse width to generate the currentPosition
			Button_Pressed = FALSE;		 //clear your own soft flags#
		}
		
		if (forSlowMove){				 // start a smoothly movement
			while (currentPosition != nextPosition){
				if (nextPosition > currentPosition){	// in witch direction the motor should move
					currentPosition++;
				}
				if (nextPosition < currentPosition){	// in witch direction the motor should move
					currentPosition--;
				}
				PWM16_WritePulseWidth(currentPosition);	// Set pulse width to generate the currentPosition
				lcdOutput(12,1,currentPosition);
			}
		}
	}
}


//MyPWM code to start a smoothly movement
#pragma interrupt_handler MyPWM

void MyPWM(void ){
	forSlowMove = TRUE;
}

//MyISR code to generate a new next position
#pragma interrupt_handler MyISR

void MyISR(void ){
	Button_Pressed = TRUE;
}



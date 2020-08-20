/*	Author: Alex Thomas
 *  Partner(s) Name: 
 *	Lab Section: 021
 *	Assignment: Lab #11  Exercise #3
 *	Exercise Description: Combine LCD implementation and keypad code
 *
 *	LOT OF CODE FROM PRELAB MATERIAL
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include "timer.h"
//this has the timer setup we have used in previous labs

#include "bit.h"
//(Pin/Port char, pin #, possibly new val)

#include "keypad.h"
//Get keypad key

#include "scheduler.h"
//GCD and task struct

#include "io.h"
//LCD implementation

// Shared variables
unsigned tmpB = 0x00;

//GET KEY STATE MACHINE

//enumeration of states
enum getKey_States { GetKey };

int getKeySMTick(int state) {
	unsigned char tmp;

	switch(state) { // SM transitions
		case GetKey: state = GetKey; break;
		default: state = GetKey; break;
	}

	switch(state) { // SM actions
		case GetKey: 
			tmp = GetKeypadKey(); 
			tmpB = tmp; 
			break;
		default: break;
	}
	return state;
}


//DISPLAY KEY STATE MACHINE

//enumeration of states
enum displayKey_States { DisplayKey };

int displayKeySMTick(int state) {
	unsigned char tmp;
	switch(state) { //SM transitions
		case DisplayKey: state = DisplayKey; break;
		default: state = DisplayKey; break;
	}
	
	switch(state) { //SM actions
		case DisplayKey: 
			switch(tmpB) {
				case '\0': 
					break;
				case '1':  
				case '2':  
				case '3': 
				case '4': 
				case '5': 
				case '6': 
				case '7':  
				case '8': 
				case '9':
				case 'A':
				case 'B': 
				case 'C': 
				case 'D': 
				case 'E':
				case '0': 
				case '#':
					LCD_DisplayString(1, "");
				        LCD_Cursor(1);
					LCD_WriteData(tmpB);	
					break;

				default: LCD_DisplayString(1, "ERROR"); break;
			} 
			break;
		default:
			break;
	}
	return state;
}

//MAIN

int main() {
	DDRC = 0xF0; PORTC = 0x0F;
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	static task task1, task2;
	task *tasks[] = { &task1, &task2 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start  = -1;

	task1.state = start;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &getKeySMTick;

	task2.state = start;
	task2.period = 10;
	task2.elapsedTime = task2.period;
	task2.TickFct = &displayKeySMTick;

	unsigned long GCD = tasks[0] -> period;
	for (int i = 1; i < numTasks; i++) {
		GCD = findGCD(GCD, tasks[i] -> period);
	}

	TimerSet(GCD);
	TimerOn();
	
	LCD_init();

	unsigned short i; // scheduler for-loop iterator
	while(1) {
                //scheduler code
                for ( i = 0; i < numTasks; i++ ) {
                        //Task is ready to tick
                        if ( tasks[i]->elapsedTime == tasks[i]->period ) {
                                // Sitting next state for task
                                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                                // Reset the elapsed time for next tick
                                tasks[i]->elapsedTime = 0;
                        }
                        tasks[i]->elapsedTime += GCD;
                }
                while(!TimerFlag);
                TimerFlag = 0;
        }

	return 0; //Error program should not exit
}

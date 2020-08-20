/*	Author: Alex Thomas
 *  Partner(s) Name: 
 *	Lab Section: 021
 *	Assignment: Lab #11  Exercise #1
 *	Exercise Description: Modify the keypad code given to be a SM task
 *	Then utilize task scheduler format by modifying the keypad SM
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
				case '\0': PORTB = 0x1F; break;
				case '1':  PORTB = 0x01; break;
				case '2':  PORTB = 0x02; break;
				case '3':  PORTB = 0x03; break;
				case '4':  PORTB = 0x04; break;
				case '5':  PORTB = 0x05; break;
				case '6':  PORTB = 0x06; break;
				case '7':  PORTB = 0x07; break;
				case '8':  PORTB = 0x08; break;
				case '9':  PORTB = 0x09; break;
				case 'A':  PORTB = 0x0A; break;
				case 'B':  PORTB = 0x0B; break;
				case 'C':  PORTB = 0x0C; break;
				case 'D':  PORTB = 0x0D; break;
				case '*':  PORTB = 0x0E; break;
				case '0':  PORTB = 0x00; break;
				case '#':  PORTB = 0x0F; break;
				
				default:   PORTB = 0x1B; break; //Should never occur. Middle LED off.
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

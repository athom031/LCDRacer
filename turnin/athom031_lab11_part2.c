/*	Author: Alex Thomas
 *  Partner(s) Name: 
 *	Lab Section: 021
 *	Assignment: Lab #11  Exercise #2
 *
 *	Exercise Description: LCD* code from io.h/io.c with time delay
 *	output message: "CS120B is Legend... wait for it DARY!"
 *	String won't fit on display at once, so need to scroll through text
 *	
 *	LOT OF CODE FROM PRELAB MATERIAL
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.// Reset the elapsed time for next tick
*/

#include "timer.h"
//this has the timer setup we have used in previous labs

#include "bit.h"
//(Pin/Port char, pin #, possibly new val)

#include "keypad.h"
//Get keypad key

#include "scheduler.h"
//GCD and task struct

//shared variables
const unsigned char Stinson[53] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    			            'C', 'S', '1', '2', '0', 'B', ' ', 'i', 's', ' ', 'L', 'e', 'g', 'e', 'n', 'd', 
				    '.', '.', '.', ' ', 'w', 'a', 'i', 't', ' ', 'f', 'o', 'r', ' ', 'i', 't', ' ',
				    'D', 'A', 'R', 'Y', '!' };
const unsigned char sz = 16;
unsigned char indexStart = 0;
unsigned char tmpOutput[16];

//AssingString

//enumeration of states
enum assignString_States { AssignString };

int assignStringSMTick(int state)  {

	switch(state) { //SM actions
		case AssignString: state = AssignString; break;
		default: state = AssignString; break;
	}

	switch(state) { //SM transitions
		case AssignString:
			for (int i = 0; i < sz; i++) {
				if((i + indexStart) > 52) {
					tmpOutput[i] = ' ';
				}
				else {
					tmpOutput[i] = Stinson[i + indexStart]; // 0 to 52
				}
			}
			//tmpOutput = tmp;
			break;
		default:
			break;
	} 
	return state;	
}


//Print String

//enumeration of states
enum printString_States { PrintString };

int printStringSMTick(int state) {
	switch(state) { //SM transitions
		case PrintString: state = PrintString; break;
		default: state = PrintString; break;
	}

	switch(state) { //SM actions
		case PrintString:
			LCD_DisplayString(1, tmpOutput);
		default:
			break;
	}
	return state;
}

//Scroll String

//enumeration of states
enum scrollString_States { ScrollString };

int scrollStringSMTick(int state) {
	switch(state) { //SM transitions
		case ScrollString: state = ScrollString; break;
		default: state = ScrollString; break;
	}
	switch(state) { //SM actions
		case ScrollString:
			if(indexStart < 52) {
				indexStart++;
			}
			else {
				indexStart = 0;
			}
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

        static task task1, task2, task3;
        task *tasks[] = { &task1, &task2, &task3 };
        const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

        const char start  = -1;

        task1.state = start;
        task1.period = 100;
        task1.elapsedTime = task1.period;
        task1.TickFct = &assignStringSMTick;

        task2.state = start;
        task2.period = 200;
        task2.elapsedTime = task2.period;
        task2.TickFct = &printStringSMTick;

	task3.state = start;
	task3.period = 200;
	task3.elapsedTime = task3.period;
	task3.TickFct = &scrollStringSMTick;

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


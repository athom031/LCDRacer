/*	Author: Alex Thomas
 *  Partner(s) Name: 
 *	Lab Section: 021
 *	Assignment: Optional Final - LCD RACER
 *
 *	Exercise Description: LCD racing game help Smee (>) avoid the obstacles
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


// global variables
const unsigned char player = '>';
const unsigned char upperLevel[60]= {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
				     
				     ' ', ' ', '#', ' ', ' ', ' ', '#', ' ', ' ', '#', ' ', 
				     ' ', ' ', '#', '#', '#', ' ', ' ', ' ', ' ', '#', ' ', 
				     ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', '#', ' ', 
				     ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#', 

				     ' ', ' ', ' ', ' ', ' ', '#', ' ', ' '};
				  
const unsigned char lowerLevel[60] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	
				      '#', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', 
				      '#', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', 
				      ' ', '#', '#', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
				      ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', 
				      
				      ' ', ' ', '#', '#', ' ', ' ', ' ', '#'};	  
//const unsigned char UpperLevel
// Shared variables
unsigned char reset = 0x00;
unsigned char tmpA = 0x00;
unsigned char row = 0;
unsigned char column = 1;
unsigned char stateG = 0;
//unsigned char upperIt = 0;
unsigned char levelIt = 0;
unsigned char waitSpd = 12;
unsigned char waitSpdIt = 0;

unsigned char speedCount = 0;
unsigned char levelIndex = 0;

int score = 0;
int scoreTest = 0; 
unsigned char temp_array[6];

// INCREMENT SCORE

//enumeration of states
enum incrementScore_States {wait_score, IncrementScore };

int incrementScoreSMTick(int state) {
	switch(state) { // SM transitions
		case wait_score:
			if(stateG > 1) {
				state = wait_score;
			}
			else {
				state = IncrementScore;
			}
			break;
		case IncrementScore:
			if(stateG > 1) {
				state = wait_score;
			}
			else {
				state = IncrementScore; 
			}
			break;
		default:
			break;
	}

	switch(state) { // SM actions
		case wait_score:
			score = 0;
			break;
		case IncrementScore:
			score = 100;
			break;
		default:
			break;
	}	

	return state;
}

//CHECK PLAYER

//enumeration of states 
enum checkPlayer_States { wait_check, CheckPlayer };

int checkPlayerSMTick(int state) {
	switch(state) { //SM transitions
		case wait_check:
			if(stateG == 1) {
				state = CheckPlayer;
			} else { 
				state = wait_check;
			}
			break;
		case CheckPlayer:
			if(stateG == 1) {
				state = CheckPlayer;
			}
			else {
				state = wait_check;
			}
			break;
		default:
			state = wait_check;
			break;		
	}

	switch(state) { //SM actions
		case wait_check:
			break;
		case CheckPlayer:
			if (row == 0) {
				if(upperLevel[levelIt] == '#') { 
					stateG = 2;
				}
			}
			else {
				if(lowerLevel[levelIt] == '#') {
					stateG = 2;
				}
			}
			break;
		default:
			break;
	}

	return state;
}
//INCREMENT LEVEL

//enumeration of states
enum incrementLevels_States { wait_inc, increment_level };

int incrementLevelsSMTick(int state) {
	switch(state) { //SM transitions
		case wait_inc:
			levelIt = 0;
			waitSpd = 12;
			waitSpdIt = 0;
			speedCount = 0;
			if(stateG == 1) {
				state = increment_level;
			} else {
				state = wait_inc;
			}
			break;
		case increment_level:
			if(stateG == 1) {
				state = increment_level;
			} else {
				state = wait_inc;
			}
			break;
		default:
			state = wait_inc;
			break;
	}

	switch(state) { //SM actions
		case wait_inc:
			break;
		case increment_level:
			if(waitSpdIt < waitSpd) {
				waitSpdIt++;
			}
			else {
				if(levelIt < 60) {
					levelIt++;
				} else {
					levelIt = 0;
				}
				waitSpdIt = 0;
				if(speedCount < 4) { 
					speedCount++;
				}
				else {
					speedCount = 0;
					if(waitSpd > 3) {
						waitSpd--;
					}
				}
			}
			break;
		default:
			break;
	}
	return state;
}

//PRINT LEVEL
enum printLevel_States { wait_print, PrintLevel };

int printLevelSMTick(int state) {
	switch(state) { // SM transitions
		case wait_print:
			if(stateG == 1) {
				state = PrintLevel;
			}
			else {
				state = wait_print;
			}
			break;
		case PrintLevel:
			if(stateG == 1) {
				state = PrintLevel;
			}
			else { 
				state =  wait_print;
			}
			break;
		default:
			state = wait_print;
			break;
	}

	switch(state) { // SM actions
		case wait_print:
			break;
		case PrintLevel:
			for(int i = 0; i < 16; i++) {
				if(i + levelIt >= 60) {
					levelIndex = (i + levelIt) - 60;
				}
				else {
					levelIndex = i + levelIt;
				}
				
				LCD_Cursor(1 + i);
				LCD_WriteData(upperLevel[levelIndex]);
				LCD_Cursor(17 + i);
				LCD_WriteData(lowerLevel[levelIndex]);
			}
			break;	
		default:
			break;
	}

	return state;
}
//GAME STAGES

//enumeration of states
enum gameStages_States { Intro, Intro_W, Play, Crash, End, Rset_W };

int gameStagesSMTick(int state) {
	reset = (~PINA & 0x04);

	switch(state) { // SM transitions
		case Intro:
			stateG = 0;
			row = 0;
			if(reset) {
				state = Intro_W;
			}
			else {
				state = Intro;
			}
			break;
		case Intro_W:
			stateG = 0;
			if(reset) {
				state = Intro_W;
			}
			else {
				stateG = 1;
				LCD_ClearScreen();
				state = Play;
				//state = Crash;
			}
			break;
		case Play:
			if(reset) {
				stateG = 0;
				state = Rset_W;
			}
			if(stateG == 2) {
				state = Crash;
			}
			else {
				scoreTest++;
				state = Play;
			}
			break;
		case Crash:
			if(reset) {
				stateG = 0;
				state = Rset_W;
			}
			else {
				stateG = 4;
				state = End;
			}
			break;
		case End:
			if(reset) {
				stateG = 0;
				scoreTest = 0;
				state = Rset_W;
			}
			else {
				state = End;
			}
			break;
		case Rset_W:
			stateG = 0;
			if(reset) {
				state = Rset_W;
			}
			else {
				state = Intro;
			}
			break;
		default:
			state = Intro;
			break;
	}

	switch(state) { // SM actions
		case Intro:
			LCD_DisplayString(1, "----- LCD! -----Help Smee B Free");
			LCD_Cursor(0);
			break;
		case Intro_W:
		case Play:
			break;	
		case Crash:
			 for(int i = 0; i < 6; i++) {
                                LCD_DisplayString(1, "       oo              oo      ");
                                for(int i = 0; i < 10000; i++) {
					continue;
                                }
                                LCD_DisplayString(1, "       XX              XX      ");
                                for(int i = 0; i < 10000; i++) {
					continue;
                                }
                                LCD_DisplayString(1, "-_-_-_ 00 _-_-_--_-_-_ 00 _-_-_-");
                                for(int i = 0; i < 10000; i++) {
					continue;
                                }
                        }
			break;

		case End:
			//	unsigned char temp_array[6] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
			LCD_DisplayString(1, "-- GAME OVER. --  SCORE: ");
			LCD_DisplayString_NoClear(26, LCD_To_String(scoreTest, temp_array, 6));
		     	LCD_Cursor(0);	
			break;
		case Rset_W:
			break;
		default:
			break;

	}

	return state;
}



//POS PLAYER

//enumeration of states
enum posPlayer_States { WAIT_PRS, UP_MOV, DOWN_MOV };

int posPlayerSMTick(int state) {
	tmpA = (~PINA & 0x03);
	
	switch(state) { // SM transitions
		case WAIT_PRS:
			if(tmpA == 0x00 || tmpA == 0x03) {
				state = WAIT_PRS;
			}
			else if(tmpA == 0x01) {
				state = UP_MOV;
			}
			else {
				state = DOWN_MOV;
			}
			break;
		case UP_MOV:
			if(tmpA == 0x01) {
				state = UP_MOV;
			}
			else {
				state = WAIT_PRS;
			}
			break;
		case DOWN_MOV:
			if(tmpA == 0x02) {
				state = DOWN_MOV;
			}
			else {
				state = WAIT_PRS;
			}
			break;
		default:
			state = WAIT_PRS;
			break;
	}			

	switch(state) { // SM actions
		case WAIT_PRS:
			break;
		case UP_MOV:
			row = 0;
			break;
		case DOWN_MOV:
			row = 1;
			break;
		default:
			break;
	}
	return state;
}


// PRINT PLAYER

//enumeration of states

enum printPlayer_States { PrintPlayer };

int printPlayerSMTick(int state) {
	
	switch(state) { // SM transitions
		case PrintPlayer:
			state = PrintPlayer;
			break;
		default:
			state = PrintPlayer;
			break;
	}

	switch(state) { // SM actions
		case PrintPlayer:
			if(stateG == 0x01) {
				LCD_Cursor(1);
				LCD_WriteData(' ');
				LCD_Cursor(17);
				LCD_WriteData(' ');
				//LCD_ClearScreen();
				LCD_Cursor((row * 16) + column);
				LCD_WriteData(player);
				LCD_Cursor(0);
			}
			break;
		default:
			break;
	} 
	return state;
}

//MAIN

int main() {
	DDRA = 0x00; PORTA = 0xFF;
	//DDRC = 0xF0; PORTC = 0x0F;
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	static task task1, task2, task3, task4, task5, task6, task7;
	task *tasks[] = { &task1, &task2, &task3, &task4, &task5, &task6, &task7 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start  = -1;

	task1.state = start;
	task1.period = 10;
	task1.elapsedTime = task1.period;
	task1.TickFct = &posPlayerSMTick;

	task2.state = start;
	task2.period = 100;
	task2.elapsedTime = task2.period;
	task2.TickFct = &printPlayerSMTick;

	task3.state = start;
	task3.period = 150;
	task3.elapsedTime = task3.period;
	task3.TickFct = &gameStagesSMTick;

	task4.state = start;
	task4.period = 50;
	task4.elapsedTime = task4.period;
	task4.TickFct = &incrementLevelsSMTick;

	task5.state = start;
	task5.period =  150;
	task5.elapsedTime = task5.period;
	task5.TickFct = &printLevelSMTick;
	
	
	task6.state = start;
	task6.period = 10;
	task6.elapsedTime = task6.period;
	task6.TickFct = &checkPlayerSMTick;

	task7.state = start;
	task7.period = 500;
	task7.elapsedTime = task7.period;
	task7.TickFct = &incrementScoreSMTick;


	unsigned long GCD = tasks[0] -> period;
	for (int i = 1; i < numTasks; i++) {
		GCD = findGCD(GCD, tasks[i] -> period);
	}

	TimerSet(GCD);
	TimerOn();
	
	LCD_init();
	LCD_ClearScreen();
	LCD_Cursor(0);

	//LCD_Cursor(1);
	//LCD_WriteData('>');
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

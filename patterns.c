/*		This file contains all of the functions, structs,
			and variables necessary to create the functionality
			in main(). The functions in this list are written in
			the order in which they are called in main(). If a
			function has helper functions (several do), the helper
			functions are written directly above the function.
*/

#include <MK64F12.h>
#include <stdlib.h>
#include "utils_extern.h"
#include "utils.h"
#include "patterns.h"

struct LED {  //struct representing one LED action (turn on or off)
	int action;  //1 (turn LED on) or 0 (turn LED off)
	int num;  //1 (white), 2 (yellow), 3 (red), 4 (blue), or 5 (green)
	unsigned int delay;  //time in ms since previous button press or release
	struct LED *next; //next LED_action in linked list (for normal traversal)
	struct LED *prev;  //previous LED_action in linked list (for reverse traversal)
};

//		global variables
LED_action *head;  //start of LED list
int max_num= 45;  //maximum number of LED presses allowed
unsigned int current_time;  //elapsed time in ms since last LED action
int direction= 1;  //1 (normal) or 0 (reverse) for list traversal


/*
		Function that displays a sequence of LED flashes to
		the user signaling that the board is on and the
		program has started.
*/
void welcome(void) {
	int j;
	int wait= 0; //animation starts fast
	
	for (int i= 0; i<12; i++) {  //do sliding animation 10 times
		wait= wait + 50000;  //animation gets slower
		White_On();
		for(j=0; j<wait; j++);  //do nothing
		White_Off();
		Yellow_On();
		for(j=0; j<wait; j++);
		Yellow_Off();
		Red_On();
		for(j=0; j<wait; j++);
		Red_Off();
		Blue_On();
		for(j=0; j<wait; j++);
		Blue_Off();
		Green_On();
		for(j=0; j<wait; j++);
		Green_Off();
	}
	
	for (j= 0; j<2; j++) {  //finish with two flashes
		White_On();
		Yellow_On();
		Red_On();
		Blue_On();
		Green_On();
		delay();
		White_Off();
		Yellow_Off();
		Red_Off();
		Blue_Off();
		Green_Off();
		delay();
	}
}


/*
		Function that lets users choose which interactive
		mode they want to use and visually displays their 
		options. Returns 1 for freestyle mode and 0 for 
		repetition mode.
*/
int mode_select(void) {
	Yellow_On();  //yellow represents freestyle mode
	Blue_On();  //blue represents repetition mode
	
	int result= 2;  //return value
	
	while(result == 2) {  //polling
		if (PTC->PDIR & (1 << 2)) result= 1;  //choose freestyle mode
		if (PTB->PDIR & (1 << 9)) result= 0;  //choose repetition mode
	}
	
	if (result) {  //freestyle selected
		Blue_Off();  //display selection
		for (int j=0; j<6000000; j++);
		Yellow_Off();
	} else {  //repetition selected
		Yellow_Off();  //display selection
		for (int j=0; j<6000000; j++);
		Blue_Off();
		delay();
		delay();
	}
	
	return result;  //return result
}


/*
		Function for freestyle mode. Users can press any button
		and light up its corresponding LED.
*/
void freestyle(void) {
	while(1) {  //polling
		if (PTC->PDIR & (1 << 3)) White_On();  //press for white
		else White_Off();
		
		if (PTC->PDIR & (1 << 2)) Yellow_On();  //press for yellow
		else Yellow_Off();
		
		if (PTB->PDIR & (1 << 23)) Red_On();  //press for red
		else Red_Off();
		
		if (PTB->PDIR & (1 << 9)) Blue_On();  //press for blue
		else Blue_Off();
		
		if (PTB->PDIR & (1 << 18)) Green_On();  //press for green
		else Green_Off();
	}
}


/*
		Helper function that displays a countdown animation.
*/
void countdown(void) {
	White_On();
	Yellow_On();
	Red_On();
	Blue_On();
	Green_On();
	delay();
	delay();
	Green_Off();
	delay();
	delay();
	Blue_Off();
	delay();
	delay();
	Red_Off();
	delay();
	delay();
	Yellow_Off();
	delay();
	delay();
	White_Off();
}


/*
		Helper function that creates a circular doubly linked list 
		representing user's LED pattern input. This list is 
		created by appending LED_action structs to the end of 
		the list. Returns 1 if successful and 0 if unsuccessful.
*/
int append(int action, int num) {
	NVIC_DisableIRQ(PIT0_IRQn);  //keep time where it's at
	LED_action *new = (malloc(sizeof(LED_action))); //allocate LED_action space and return pointer
	if (new == NULL) return 0; //allocation failed
	
	new->delay= current_time*(SystemCoreClock/1000);  //number of cycles since last LED action
	
	new->action= action;  //on or off
	new->num= num;  //LED number
	
	if (head == NULL) {  //if list is empty
		head= new;  //only element in list
		new->next= new;  //element points to itself
		new->prev= new;  //in both directions
	}
	else {  //list not empty; new is the tail now
		new->next= head;
		new->prev= head->prev;
		head->prev->next= new;
		head->prev= new;
	}
	
	current_time= 0;  //reset time to measure next press
	NVIC_EnableIRQ(PIT0_IRQn);  //start incrementing again
	
	return 1;  //append successful
}


/*
		Helper function that enables a timer that will 
		help record what a user inputs in real-time in 
		pattern_input().
*/
void timer_enable(void) {
	SIM->SCGC6 = SIM_SCGC6_PIT_MASK; // enable clock to PIT module
	PIT->MCR = (0 << 1); // enable clock to PIT timers
	NVIC_EnableIRQ(PIT0_IRQn); //enable PIT0 interrupts
	PIT->CHANNEL[0].LDVAL = SystemCoreClock/1000;  //interrupt once every ms
	PIT->CHANNEL[0].TFLG = 0x1; // Write 1 to this flag to clear it

	current_time= 0;  //current time starts at zero
	
	PIT->CHANNEL[0].TCTRL |= 0x3; //enable interrupts and start current time
}


/*
		Function for storing user inputs in a linked list and
		converting them into the desired LED sequence.
*/
void pattern_input(void) {
	countdown();  //animation tells user when to start inputting
	int press_num= 0;  //number of buttons pressed
	unsigned int milli= SystemCoreClock/1000;  //millisecond delay value
	unsigned int i;  //for loop variable
	
	int w_prev= 0;  //values to monitor button presses
	int w_cur= 0;	  //buttons named based on LED they control
	int y_prev= 0;	//0 or low means button wasn't pressed
	int y_cur= 0;		//1 or high means button was pressed
	int r_prev= 0;	//each loop cycle, check current value and store previous
	int r_cur= 0;		//compare these values to detect button press/release
	int b_prev= 0;	//initially no buttons have been pressed
	int b_cur= 0;
	int g_prev= 0;
	int g_cur= 0;
	
	timer_enable();  //measure elapsed time between button presses
	
	while(1) {  //polling
		w_prev= w_cur;  //current press becomes previous
		y_prev= y_cur;
		r_prev= r_cur;
		b_prev= b_cur;
		g_prev= g_cur;
		
		w_cur= (PTC->PDIR & (1 << 3)) ? 1 : 0;  //check if button is pressed
		y_cur= (PTC->PDIR & (1 << 2)) ? 1 : 0;  //update current press
		r_cur= (PTB->PDIR & (1 << 23)) ? 1 : 0;  //1 means button is pressed
		b_cur= (PTB->PDIR & (1 << 9)) ? 1 : 0;  //0 means button isn't pressed
		g_cur= (PTB->PDIR & (1 << 18)) ? 1 : 0;

		if (!w_prev && w_cur && press_num < max_num) {  //white pressed and list not full
			for (i=0; i<milli; i++);  //debounce
			if (append(1,1)) White_On();  //if append is successful, white on
			else return;  //this should never happen
		}
		else if (w_prev && !w_cur && press_num < max_num) {  //white released and list not full
			for (i=0; i<milli; i++);  //debounce
			if (append(0,1)) {
				White_Off();
				press_num++;  //count button press
			}
			else return;
		}
		
		if (!y_prev && y_cur && press_num < max_num) {  //yellow pressed and list not full
			for (i=0; i<milli; i++);  //debounce
			if (append(1,2)) Yellow_On();  //if append is successful, yellow on
			else return;  //this should never happen
		}
		else if (y_prev && !y_cur && press_num < max_num) {  //yellow released and list not full
			for (i=0; i<milli; i++);  //debounce
			if (append(0,2)) {
				Yellow_Off();
				press_num++;  //count button press
			}
			else return;
		}
		
		if (!r_prev && r_cur && press_num < max_num) {  //red pressed and list not full
			for (i=0; i<milli; i++);  //debounce
			if (append(1,3)) Red_On();  //if append is successful, red on
			else return;  //this should never happen
		}
		else if (r_prev && !r_cur && press_num < max_num) {  //red released and list not full
			for (i=0; i<milli; i++);  //debounce
			if (append(0,3)) {
				Red_Off();
				press_num++;  //count button press
			}
			else return;
		}
		
		if (!b_prev && b_cur && press_num < max_num) {  //blue pressed and list not full
			for (i=0; i<milli; i++);  //debounce
			if (append(1,4)) Blue_On();  //if append is successful, blue on
			else return;  //this should never happen
		}
		else if (b_prev && !b_cur && press_num < max_num) {  //blue released and list not full
			for (i=0; i<milli; i++);  //debounce
			if (append(0,4)) {
				Blue_Off();
				press_num++;  //count button press
			}
			else return;
		}
		
		if (!g_prev && g_cur && press_num < max_num) {  //green pressed and list not full
			for (i=0; i<milli; i++);  //debounce
			if (append(1,5)) Green_On();  //if append is successful, green on
			else return;  //this should never happen
		}
		else if (g_prev && !g_cur && press_num < max_num) {  //green released and list not full
			for (i=0; i<milli; i++);  //debounce
			if (append(0,5)) {
				Green_Off();
				press_num++;  //count button press
			}
			else return;
		}

		//exit loop by pressing max number of buttons or pressing non-LED button
		if ((press_num == max_num) || (PTC->PDIR & (1 << 12))) {
			for (i=0; i<milli; i++);  //debounce
			NVIC_DisableIRQ(PIT0_IRQn);  //timer no longer needed
			return;
		}
	}
}


/*
		Function that marks transition from pattern input
		stage to pattern display stage. LEDs corresponding 
		to buttons which can be used to modify the LED pattern 
		are lit up.
*/
void modify(void) {
	White_On();
	Green_On();
	
	int red_on= 0;  //red initially off
	int green_on= 0;  //green initially off
	int blink= 0;  //used to decide when to blink LEDs
	int start_prev= 1;  //variables to monitor start button
	int start_cur= 1;
	
	while(1) {  //polling and flashing red and blue LEDs
		start_prev= start_cur;
		start_cur= (PTC->PDIR & (1 << 12)) ? 1 : 0;  //check if button is pressed
		blink++;  //keep incrementing blink
		
		if (blink%100000 == 0) {  //blink fast
			if (red_on) {  //if red is on, turn it off
				Red_Off();
				red_on= 0;
			} else {  //if red is off, turn it on
				Red_On();
				red_on= 1;
			}
		}
		if (blink == 1000000) {  //blink slow
			if (green_on) {  //if green is on, turn it off
				Green_Off();
				green_on= 0;
			} else {  //if green is off, turn it on
				Green_On();
				green_on= 1;
			}
			blink= 0;  //reset blink
		}
		if (!start_prev && start_cur) {  //press non-LED button to start displaying pattern
			White_Off();  //turn LEDs off
			Red_Off();
			Green_Off();
			return;  //return to main
		}
	}
}


/*
		Helper function that enables interrupts for buttons 
		corresponding to the white, red, and green LEDs.
*/
void interrupt_enable(void) {
	PORTC->PCR[3] |= (1 <<  16 | 1 << 19);  //interrupt on rising edge
	PORTB->PCR[23] |= (1 <<  16 | 1 << 19);
	PORTB->PCR[18] |= (1 <<  16 | 1 << 19);
	
	NVIC_EnableIRQ(PORTC_IRQn);  //enable port C interrupts
	NVIC_EnableIRQ(PORTB_IRQn);  //enable port B interrupts
}


/*
		Function that displays the user's pattern repeatedly.
		It does so by iterating through the linked list and
		processing the action each element represents.
*/
void display(void) {
	interrupt_enable();  //enable button interrupts
	int i;  //loop variable
	LED_action *elem= head;  //start at head
	
	while (1) {  //infinitely loop through LED sequence
		if (elem->num == 1) {  //white LED
			(elem->action) ? White_On() : White_Off();  //choose action
		}
		else if (elem->num == 2) {  //yellow LED
			(elem->action) ? Yellow_On() : Yellow_Off();  //choose action
		}
		else if (elem->num == 3) {  //red LED
			(elem->action) ? Red_On() : Red_Off();  //choose action
		}
		else if (elem->num == 4) {  //blue LED
			(elem->action) ? Blue_On() : Blue_Off();  //choose action
		}
		else if (elem->num == 5) {  //green LED
			(elem->action) ? Green_On() : Green_Off();
		}
		
		if (direction) {  //traverse normally
			elem= elem->next;  //get next LED action in list
			unsigned int num_loops= (elem->delay)/4;  //compute delay
			for (i= 0; i< num_loops; i++);  //delay
		} else {  //traverse in reverse- must get next element and the delay in opposite order
			unsigned int num_loops= (elem->delay)/4;  //get delay in number of cycles
			elem= elem->prev;  //get prev element in list
			for (i= 0; i< num_loops; i++);  //delay
		}
	}
}


/* 
     PIT0 Interrupt Handler for incrementing current time by 1 ms.
*/
void PIT0_IRQHandler(void) {
	NVIC_ClearPendingIRQ(PIT0_IRQn); // Clear PIT0 interrupts
	PIT->CHANNEL[0].TFLG = 0x1; // Write 1 to this flag to clear it
	PIT->CHANNEL[0].TCTRL &= 0x2; // Disable timer
	current_time++;  //increment time
	PIT->CHANNEL[0].TCTRL |= (1 << 0); //restart countdown
}


/* 
		PORTB Interrupt Handler for changing speed of LED pattern
		as it is displayed.
*/
void PORTB_IRQHandler(void) {
	NVIC_DisableIRQ(PORTB_IRQn); //no interrupts here
	NVIC_ClearPendingIRQ(PORTB_IRQn); // Clear port B interrupts
	unsigned int milli= SystemCoreClock/1000;  //millisecond delay value
	for (int i=0; i<milli; i++);  //debounce
	
	int speed;  //used to determine whether to speed up or slow down
	if (PORTB->PCR[23] & (1 << 24)) {  //if red pressed, speed up
		speed= 1;
		PORTB->PCR[23] |= (1 << 24);  //clear interrupt flag
	}
	else if (PORTB->PCR[18] & (1 << 24)) {  //if green pressed, slow down
		speed= 0;
		PORTB->PCR[18] |= (1 << 24);  //clear interrupt flag
	}
	
	LED_action *tmp= head;
	int done=0;  //indicates whether all elements have been modified
	while (!done) {  //elements left to modify
		tmp->delay= (speed) ? (tmp->delay)*.75 : (tmp->delay)*1.25;  //modify speed
		if (tmp->next == head) done= 1;  //all elements have been modified
		else tmp= tmp->next;  //otherwise move to next element
	}

	NVIC_EnableIRQ(PORTB_IRQn); //enable interrupts
}


/* 
		PORTC Interrupt Handler for reversing direction
		of LED pattern that user created.
*/
void PORTC_IRQHandler(void) {
	NVIC_DisableIRQ(PORTC_IRQn); //no interrupts here
	NVIC_ClearPendingIRQ(PORTC_IRQn); // Clear port C interrupts
	PORTC->PCR[3] |= (1 << 24);  //clear interrupt flag
	unsigned int milli= SystemCoreClock/1000;  //millisecond delay value
	for (int i=0; i<milli; i++);  //debounce
	
	LED_action *tmp= head;
	int done=0;  //indicates whether all elements have been modified
	while (!done) {  //elements left to modify
		tmp->action= !(tmp->action);  //reversing traversal reverses actions
		if (tmp->next == head) done= 1;  //all elements have been modified
		else tmp= tmp->next;  //otherwise move to next element
	}
	
	direction= !direction;  //change traversal direction

	NVIC_EnableIRQ(PORTC_IRQn); //enable interrupts
}

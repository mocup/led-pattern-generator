/*		This file contains main(). It is written in
			such a way to show the general flow of the 
			main stages of my program. The small details
			and helper functions have been abstracted away
			here.
*/

#include <MK64F12.h>
#include "utils.h"
#include "utils_extern.h"
#include "patterns.h"


int main (void)
{
	LED_Initialize();  //initialize board LEDs
	LED_ExInit();  //initialize external LEDs
	Button_Init();  //initialize buttons
	
	welcome();  //display welcome animation
	
	if (mode_select()) freestyle();  //select freestyle mode- stay here until reset
	else {  //select repetition mode
		pattern_input();  //user inputs their pattern
		modify();  //display which buttons to press to modify the input
		display();  //display pattern repeatedly and allow modifications- stay here until reset
	}

	while(1);
	
	return 0;
}

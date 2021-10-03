#ifndef __PATTERNS_H__
#define __PATTERNS_H__

#include <stdlib.h>
#include <MK64F12.h>

struct LED;
typedef struct LED LED_action;

void welcome(void);
int mode_select(void);
void freestyle(void);
void pattern_input(void);
void modify(void);
void display(void);

#endif

/*************************************
* Lab 1 Exercise 3
* Name: Nicholas Jimmy Alden & Nixon Widjaja
* Student No: A0244833A & A0236430N
* Lab Group: B13 & B06
*************************************/

#include "functions.h"

// Initialize the func_list array here!
int (*func_list[5])(int) = {&add_one, &add_two, &multiply_five, &square, &cube}/* Fill this up */;

// You can also use this function to help you with
// the initialization. This will be called in ex3.c.
void update_functions() {}

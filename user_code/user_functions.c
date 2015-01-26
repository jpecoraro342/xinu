/*  

User Developed Code
Everything in here is part of my work/practice etc. 

*/

#include <xinu.h>
#include <stdio.h>

void callUserFunctions() {
	//This is like the main
	printHello();
}

void printHello() {
	kprintf("Hello World\n");
}
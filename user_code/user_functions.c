/*  

User Developed Code
Everything in here is part of my work/practice etc. 

*/

#include <xinu.h>
#include <stdio.h>

void callUserFunctions() {
	//This is like the main
	int i = 0;
	for (i = 0; i < 10; i ++) {
		resume(create(printHello, 4096, 50, "hello", 1, i));
	}
}

void printHello(int n) {
	kprintf("Hello World %d\n", n);
}
/*  

User Developed Code
Everything in here is part of my work/practice etc. 

*/

#include <xinu.h>
#include <stdio.h>

#define BUFFER_SIZE 100

int buffer[BUFFER_SIZE];
int head = 0;
int tail = 0;

int counter = 0;

sid32 consumed;
sid32 produced;

void consume();
void produce();

void callUserFunctions() {
	//This is like the main

	consumed = semcreate(1);
	produced = semcreate(0);

	int i = 0;
	
	intmask mask = disable();
	resume(create(produce, 4096, 50, "produce", 0, 0));
	resume(create(consume, 4096, 50, "consume", 0, 0));
	restore(mask);
	
}

void consume() {
	while (TRUE) {
		wait(produced);

		kprintf("Consuming %d - %d\n", buffer[head]/BUFFER_SIZE, buffer[head]%BUFFER_SIZE);
		head = ((head+1)%BUFFER_SIZE);
		sleepms(100);
		signal(consumed);
	}
}

void produce() {
	while (TRUE) {
		wait(consumed);

		buffer[tail] = counter;
		kprintf("Producing %d - %d\n", buffer[tail]/BUFFER_SIZE, buffer[tail]%BUFFER_SIZE);
		counter++;
		tail = ((tail+1)%BUFFER_SIZE);
		sleepms(100);
		signal(produced);
	}
}
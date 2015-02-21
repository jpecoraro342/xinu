/*  
	Producer Consumer Problem
	
	There are a few things that can go wrong when sharing a circular buffer between two processes.

	1. The producer produces quicker than the consumer can consumer
		In this case, the circular buffer will fill, and the tail will overtake the ehad if nothing stops it 

	2. The consumer occurs to quickly
		In this case, the head can overtake the tail, and the consumer will consume old data

	3. Consumer can resume before the producer is finished
		Same problem as consumer being too fast, except this doesn't have to do with speed as much as an inoppurtune timing

	4. Producer can resume before the consumer is finished
		Again, same as producer being too quick


	Solution:
		I took the solution of syncrhonizing my producer and consumer. Producer produces once, followed by the consumer consuming once
		Alternatives include locking the head/tail when it reaches the tail/head

*/

#include <xinu.h>
#include <stdio.h>

#define BUFFER_SIZE 100

//for display purposes
#define SLEEPTIME_MS 100

int buffer[BUFFER_SIZE];
int head = 0;
int tail = 0;

int counter = 0;

sid32 consumed;
sid32 produced;

void consume();
void produce();

int main(int argc, char **argv)
{
	resume(create(callUserFunctions, 4096, 50, "Start Consumer Producer", 0, 0));

	while (1);

	return OK;
}

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
		head = ((head+1)%BUFFER_SIZE); //increment head, 0 if size of buffer
		sleepms(SLEEPTIME_MS);

		signal(consumed);
	}
}

void produce() {
	while (TRUE) {
		wait(consumed);
		buffer[tail] = counter;

		kprintf("Producing %d - %d\n", buffer[tail]/BUFFER_SIZE, buffer[tail]%BUFFER_SIZE);
		counter++;

		tail = ((tail+1)%BUFFER_SIZE); //increment tail, 0 if size of buffer
		sleepms(SLEEPTIME_MS);

		signal(produced);
	}
}
/*  

User Developed Code
Everything in here is part of my work/practice etc. 

*/

#include <xinu.h>
#include <stdio.h>

#define NUM_PHILOSOPHERS 5

#define LEFT (n+NUM_PHILOSOPHERS-1)%NUM_PHILOSOPHERS
#define RIGHT (n+1)%NUM_PHILOSOPHERS

#define THINK_TIME ((n+1)%3+1)*617
#define EAT_TIME ((n+1)%3+1)*603

#define THINKING 0
#define HUNGRY 1
#define EATING 2

void be_a_philosopher(int n);
void attempt_fork_pickup(int n);
void pickup_forks(int n);
void put_down_forks(int n);
void think(int n);
void eat(int n);

void print_eaten_data();

sid32 waiter;
sid32 printing;
int philosopher_state[NUM_PHILOSOPHERS];
sid32 forks[NUM_PHILOSOPHERS];
int times_eaten[NUM_PHILOSOPHERS];

void callUserFunctions() {
	//This is like the main
	waiter = semcreate(1);
	printing = semcreate(1);

	int i = 0;
	for (i =0; i < NUM_PHILOSOPHERS; i++) {
		times_eaten[i] = 0;
	}

	i = 0;
	for (i =0; i < NUM_PHILOSOPHERS; i++) {
		forks[i] = semcreate(1);
	}

	for (i = 0; i < NUM_PHILOSOPHERS; i++) {
		resume(create(be_a_philosopher, 4096, 50, "philosopher", 1, i));
	}

	resume(create(print_eaten_data, 4096, 50, "print data", 0));
}

void be_a_philosopher(int n) {
	kprintf(" philosopher %d is alive and well\n", n);

	while (TRUE) {
		think(n);
		pickup_forks(n);
		eat(n);
		put_down_forks(n);
	}
}

void pickup_forks(int n) {
	//kprintf("philosopher %d is waiting for the waiter to pick up forks\n", n);
	wait(waiter);

	if (philosopher_state[n] != HUNGRY) {
		philosopher_state[n] = HUNGRY;
		kprintf(" philosopher %d is hungry\n", n);
	}
	attempt_fork_pickup(n);

	signal(waiter);
}

void put_down_forks(int n) {
	//kprintf("philosopher %d is waiting for the waiter to put down forks\n", n);
	wait(waiter);

	philosopher_state[n] = THINKING;
	//kprintf("philosopher %d is putting down forks %d and %d\n", n, LEFT, RIGHT);
	signal(forks[n]);
	signal(forks[RIGHT]);
	kprintf(" philosopher %d put down forks %d and %d\n", n, LEFT, RIGHT);
	attempt_fork_pickup(LEFT);
	attempt_fork_pickup(RIGHT);
	signal(waiter);
}

void attempt_fork_pickup(int n) {
	if (philosopher_state[n] == HUNGRY && philosopher_state[LEFT] != EATING && philosopher_state[RIGHT] != EATING) {
		//kprintf("philosopher %d is taking forks %d and %d\n", n, LEFT, RIGHT);
		wait(forks[n]);
		wait(forks[RIGHT]);
		kprintf(" philosopher %d took forks %d and %d\n", n, n, RIGHT);
		philosopher_state[n] = EATING;
	}
}

void think(int n) {
	if (philosopher_state[n] == THINKING) {
		kprintf(" philosopher %d is thinking\n", n);
		sleepms(THINK_TIME); //thinks for think time
		//kprintf("philosopher %d is done thinking\n", n);
	}
}

void eat(int n) {
	if (philosopher_state[n] == EATING) {
		kprintf(" philosopher %d is eating\n", n);
		times_eaten[n]++;
		sleepms(EAT_TIME); //eats for eat time
		//kprintf("philosopher %d is done eating\n", n);
	}
}

void print_eaten_data() {
	while (TRUE) {
		sleepms(10000);
		wait(waiter);

		kprintf("\n\nSystem Summary: \n\n   Philosopher  |   Times Eaten\n");
		int i = 0;
		for (i = 0; i < NUM_PHILOSOPHERS; i++) {
			kprintf("\t%d\t|\t%d\n", i, times_eaten[i]);
		}
		kprintf("\n\n");
		signal(waiter);
	}
}


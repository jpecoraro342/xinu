/*  

Dining Philosopher Problem

Solution:
	I provided a solution using the idea of a "waiter"

	Philosophers must pick up two forks at a time, and when a philosopher is done eating,
	he "notifies" the philosophers next to him, so they may have a chance to pick up the forks.

	The states of each philosopher are kept to know whether or not the ones next to you are eating

	My code should run as is, and prints a summary of who has eaten approximately every 10 seconds

	The print statements in the code are what is actually signifying when a philosopher is taking an action

*/

#include <xinu.h>
#include <stdio.h>

#define NUM_PHILOSOPHERS 5

//Describes the left and right index for each philosopher
#define LEFT (n+NUM_PHILOSOPHERS-1)%NUM_PHILOSOPHERS
#define RIGHT (n+1)%NUM_PHILOSOPHERS

//Think and Eat time, could be anything, wanted each philosopher to be diffrent
#define THINK_TIME ((n+1)%3+1)*617 
#define EAT_TIME ((n+1)%3+1)*603

//Philosopher states
#define THINKING 0
#define HUNGRY 1
#define EATING 2

//function defines
void be_a_philosopher(int n);
void attempt_fork_pickup(int n);
void pickup_forks(int n);
void put_down_forks(int n);
void think(int n);
void eat(int n);

//This is what prints the eaten summary
void print_eaten_data();

sid32 waiter; //The "Waiter" semaphore
sid32 printing; //used briefly to keep printing synchronized

//Array of philosopher states
int philosopher_state[NUM_PHILOSOPHERS];

//Array of fork semaphores (not necessary, but ensures only two forks are taken at a time)
//Note: theoretically, if my program broke and someone did try and take a fork being used, I'm pretty sure my solution would stop working
//BUT, this never happens
sid32 forks[NUM_PHILOSOPHERS];

//Just keeping track of some data
int times_eaten[NUM_PHILOSOPHERS];

int main(int argc, char **argv)
{
	resume(create(callUserFunctions, 4096, 50, "Start Problem", 0, 0));

	while (1);

	return OK;
}

void callUserFunctions() {
	//Initialize semaphores
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

	//Start the philosopher processes
	for (i = 0; i < NUM_PHILOSOPHERS; i++) {
		resume(create(be_a_philosopher, 4096, 50, "philosopher", 1, i));
	}

	resume(create(print_eaten_data, 4096, 50, "print data", 0));
}

//This is what philosophers do
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
	//Call the waiter over
	wait(waiter);

	//if they are currently thinking, they should switch state to hungry
	if (philosopher_state[n] == THINKING) {
		philosopher_state[n] = HUNGRY;
		kprintf(" philosopher %d is hungry\n", n);
	}

	attempt_fork_pickup(n); /

	//release the waiter
	signal(waiter);
}

void put_down_forks(int n) {
	//call the waiter
	wait(waiter);

	//If the philosopher was eating, put down the forks, tell your neighbors you put them down, and start thinking
	if (philosopher_state[n] == EATING) {
		philosopher_state[n] = THINKING;
		
		//releases your forks
		signal(forks[n]);
		signal(forks[RIGHT]);

		kprintf(" philosopher %d put down forks %d and %d\n", n, n, RIGHT);

		//Notifying neighbors
		attempt_fork_pickup(LEFT);
		attempt_fork_pickup(RIGHT);
	}

	//release the waiter
	signal(waiter);
}

//If neither neighbors are eating, start eating
void attempt_fork_pickup(int n) {
	if (philosopher_state[n] == HUNGRY && philosopher_state[LEFT] != EATING && philosopher_state[RIGHT] != EATING) {
		//holds your forks
		wait(forks[n]);
		wait(forks[RIGHT]);

		kprintf(" philosopher %d took forks %d and %d\n", n, n, RIGHT);
		philosopher_state[n] = EATING;
	}
}

//Think for a little bit
void think(int n) {
	if (philosopher_state[n] == THINKING) {
		kprintf(" philosopher %d is thinking\n", n);
		sleepms(THINK_TIME); //thinks for think time
	}
	else {
		//I'm pretty sure this is unneccessary, don't remember why it is there
		sleepms(THINK_TIME); //thinks for think time/ 10
	}
}

//Eat some grub
void eat(int n) {
	if (philosopher_state[n] == EATING) {
		kprintf(" philosopher %d is eating\n", n);
		times_eaten[n]++;
		sleepms(EAT_TIME); //eats for eat time
	}
}

//Pretty print how many times the philosophers have eaten
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


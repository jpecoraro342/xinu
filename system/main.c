/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>

#define MSG_BUFFER_SIZE 20
#define PROC_MSGS_LOC pid * MSG_BUFFER_SIZE 

umsg32 message_queue[NPROC * MSG_BUFFER_SIZE]; /* The message queue for each process */

int head[NPROC]; /* list of head pointers for each process */
int tail[NPROC]; /* list of tail pointers for each process */

/* Message Queue Functions */
int enqueue_msg(umsg32 msg, pid32 pid);
umsg32 dequeue_msg(pid32 pid);
umsg32 peek_msg_queue(pid32 pid);
int msg_queue_empty(pid32 pid);
int msg_queue_full(pid32 pid);

void test();

void print_proc_msg_info(pid32 pid);

int main(int argc, char **argv) {
	int i;
	for (i = 0; i < NPROC; i++) {
		head[i] = MSG_BUFFER_SIZE - 1;
	}

	resume(create(test, 4096, 50, "Start Teting", 0, 0));

	while (1);
	return OK;
}

/* Testing Process */

void test() {
	intmask mask = disable();
	pid32 testpid = 0;

	kprintf("head: %d tail: %d\n", head[testpid], tail[testpid]);
	sleepms(500);

	while (!msg_queue_full(testpid)) {
		enqueue_msg(5, testpid);
	}

	kprintf("head: %d tail: %d\n", head[testpid], tail[testpid]);
	sleepms(500);

	while (!msg_queue_empty(testpid)) {
		umsg32 msg = dequeue_msg(testpid);
		kprintf("item: %d msg: %d\n", head[testpid], msg);
		sleepms(300);
	}

	kprintf("head: %d tail: %d\n", head[testpid], tail[testpid]);
	sleepms(500);

	resume(mask);
}

/* Message Passing Functions */

/* 
* Sending message (msg) to process (pid). In case a message or a number of
* messages are already waiting on pid to receive them, the new msg will be queued.
* It will return OK on success or SYSERR on error.
*/
syscall sendMsg (pid32 pid, umsg32 msg) {

}

/*
* Receiving a message from any sender. It causes the calling process to wait for a message
* to be sent (by any process). When a message is sent, it is received and returned.
*/
umsg32 receiveMsg (void) {

}

/*
* Sending a group (msg_count) of messages (msgs) to process (pid). It will return
* the number of msgs successfully sent or SYSERR on error
*/
uint32 sendMsgs (pid32 pid, umsg32* msgs, uint32 msg_count) {

}

/*
* Recieving a group (msg_count) of messages (msgs). It causes the calling processs
* to wait for msg_count messages to be sent. When all messages are in the queue, they
* are then all together immediately received.
*/
syscall receiveMsgs (umsg32* msgs, uint32 msg_count) {

}

/*
* Sending the message (msg) to the number (pid_count) of processes (pids). It will return 
* the number of pids successfully sent to or SYSERR on error.
*/
uint32 sendnMsg (uint32 pid_count, pid32* pids, umsg32 msg) {

}

/* Circular Buffer Functions */

/* Checks to see if the message queue for pid is empty */
int msg_queue_empty(pid32 pid) {
	if (head[pid] == (tail[pid] - 1) % MSG_BUFFER_SIZE) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/* Checks to see if the message queue is full */
int msg_queue_full(pid32 pid) {
	if (head[pid] == tail[pid]) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/* Adds a message to the queue for the process pid */
int enqueue_msg(umsg32 msg, pid32 pid) {
	if (!msg_queue_full(pid)) {
		int currtail = tail[pid];
		message_queue[PROC_MSGS_LOC + currtail] = msg;

		tail[pid] = currtail + 1;
		return TRUE;
	}

	return FALSE;
}

/* Peeks at the message on the front of the msg queue for pid */
umsg32 peek_msg_queue(pid32 pid) {
	if (!msg_queue_empty(pid)) {
		int currhead = head[pid];
		return message_queue[PROC_MSGS_LOC + ((currhead + 1) % MSG_BUFFER_SIZE)];
	}

	return NULL;
}

/* Removes a message from the front of the queue for the process pid */
umsg32 dequeue_msg(pid32 pid) {
	umsg32 msg = NULL;

	if (!msg_queue_empty(pid)) {
		msg = peek_msg_queue(pid);
		head[pid] = (head[pid] + 1) % MSG_BUFFER_SIZE;
	}

	return msg;
}

/* Printing/Debugging */


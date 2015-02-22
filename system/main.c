/*  main.c  - main */

/*
* Lab 2 Checklist
*
* sendMSg:
* 	√ Send Msg to Non-Full Process Queue (Test Case 1)
*	? Sending Msg to Full Process Queue returns SYSERR
*	? Sending Msg to bad pid returns SYSERR
*
* receiveMsg:
* 	√ Receives message when a message is queued (Test Case 1)
*	X Waits to receive message when none are queued (Test Case 2)
*
* sendMsgs:
* 	? Sends multiple messages to a single process
*	? Returns number of messages sent
*	? Returns SYSERR on badpid
* 
* receiveMsgs:
* 	? Receives number of messages if queued
*	? Waits for number of messages to be queued before receiving
*
* sendnMsg:
* 	√ Sends messages to all pids
*	? Returns number successfully sent
*	? Returns SYSERR on badpid
*	? Does not send any messages if badpid encoutnered
*
*/

#include <xinu.h>
#include <stdio.h>

#define MSG_BUFFER_SIZE 20
#define PROC_MSGS_LOC pid * MSG_BUFFER_SIZE 

umsg32 message_queue[NPROC * MSG_BUFFER_SIZE]; /* The message queue for each process */

int head[NPROC]; /* list of head pointers for each process */
int tail[NPROC]; /* list of tail pointers for each process */

sid32 print_mutex; /* Mutex Used for Printing */

/* Message Queue Functions */
int enqueue_msg(umsg32 msg, pid32 pid);
umsg32 dequeue_msg(pid32 pid);
umsg32 peek_msg_queue(pid32 pid);
int msg_queue_empty(pid32 pid);
int msg_queue_full(pid32 pid);

/* Printing Functions */
void print_send_msg(pid32 pid, umsg32 msg);
void print_receive_msg(umsg32 msg);

/* Function for testing */
void test();

int main(int argc, char **argv) {
	print_mutex = semcreate(1);

	int i;
	for (i = 0; i < NPROC; i++) {
		head[i] = MSG_BUFFER_SIZE - 1;
	}

	test();
	//resume(create(test, 4096, 50, "Start Testing", 0, 0));

	while (1);
	return OK;
}

/* Testing Process */

void test() {
	
	/* Test Case 1
	 *
	 * proc1 and proc2 send msg 1 and 2 to proc3
	 * proc3 receives msg at front of queue
	 *
	 * Expected Output:
	 * "Sent Msg 1"
	 * "Sent Msg 2"
	 * "Receive Msg 1"
	 * 
	 */

	umsg32 msg1 = 1;
	umsg32 msg2 = 2;
	
	// Receive Process
	pid32 proc3 = create(receiveMsg, 4096, 50, "Receive", 0, 0);

	// Send Processes
	pid32 proc1 = create(sendMsg, 4096, 50, "Send message to proc3", 2, proc3, msg1);
	pid32 proc2 = create(sendMsg, 4096, 50, "Send message to proc3", 2, proc3, msg2);

	resume(proc1);
	resume(proc2);

	resume(proc3);

	/* End Test Case 1 */

	/* Test Case 2
	 *
	 * proc4 sends msg3 to proc5
	 * proc5 waits to receive message until sent by proc4
	 *
	 * Expected Output:
	 * "Sent Msg 3"
	 * "Receive Msg 3"
	 * 
	 */

	umsg32 msg3 = 1;
	
	// Receive Process
	pid32 proc5 = create(receiveMsg, 4096, 50, "Receive", 0, 0);

	// Send Processes
	pid32 proc4 = create(sendMsg, 4096, 50, "Send message to proc3", 2, proc5, msg3);

	resume(proc5);
	resume(proc4);

	/* End Test Case 2 */

	//umsg32 msg_list_1[5] = { 1, 2, 3, 4, 5 };
	//umsg32 msg_list_1_rec[5];

	/* receive 5 messages */
	//pid32 proc1 = create(receiveMsgs, 4096, 50, "Receive 5 Messages", 2, &msg_list_1_rec, 5);

	/* proc6 will send msg_list_1 to proc1 */
	//pid32 proc6 = create(sendMsgs, 4096, 50, "Send 5 to proc1", 3, proc1, &msg_list_1, 5);

	/*
	//sender list processes
	int i;
	for (i = 0; i < 5; i++) {
		sender_list[i] = create(receiveMsg, 4096, 50, "Receive", 0, 0);;
	}

	pid32 sender_list[5];

	pid32 proc11 = create(sendnMsg, 4096, 50, "Send 5 messages", 3, 5, &sender_list, msg22);

	for (i = 0; i < 5; i++) {
		resume(sender_list[i]);
	}
	*/
}

/* Message Passing Functions */

/* 
* Sending message (msg) to process (pid). In case a message or a number of
* messages are already waiting on pid to receive them, the new msg will be queued.
* It will return OK on success or SYSERR on error.
*/
syscall sendMsg (pid32 pid, umsg32 msg) {
	intmask	mask = disable();

	if (isbadpid(pid)) {
		wait(print_mutex);
		kprintf("Could not sent message to Process: %d - Bad PID\n", pid);
		signal(print_mutex);

		restore(mask);
		return SYSERR;
	}

	//TODO: Change this if we need to be able to handle this
	if (msg_queue_full(pid)) {
		wait(print_mutex);
		kprintf("Could not sent message to Process: %d - Message Queue Full\n", pid);
		signal(print_mutex);

		restore(mask);
		return SYSERR;
	}

	enqueue_msg(msg, pid);

	print_send_msg(pid, msg);
	wait(print_mutex);
	kprintf("Process: %d successfully sent 1 message\n", currpid);
	signal(print_mutex);

	restore(mask);
	return OK;
}

/*
* Receiving a message from any sender. It causes the calling process to wait for a message
* to be sent (by any process). When a message is sent, it is received and returned.
*/
umsg32 receiveMsg (void) {
	umsg32 msg;
	if (!msg_queue_empty(currpid)) {
		msg = dequeue_msg(currpid); 
	}
	else {
		//Reschedule? Wait? Do something?

		wait(print_mutex);
		kprintf("Could not receive message on process: %d - Message Queue Empty\n", currpid);
		signal(print_mutex);
	}

	print_receive_msg(msg);
	return msg;	
}

/*
* Sending a group (msg_count) of messages (msgs) to process (pid). It will return
* the number of msgs successfully sent or SYSERR on error
*/
uint32 sendMsgs (pid32 pid, umsg32* msgs, uint32 msg_count) {
	intmask	mask = disable();

	if(isbadpid(pid)) {
		wait(print_mutex);
		kprintf("Could not sent messages to Process: %d - Bad PID\n", pid);
		signal(print_mutex);

		restore(mask);
		return SYSERR;
	}

	uint32 successfull_messages = 0;
	uint32 i;
	for (i = 0; i < msg_count; i++) {
		if (enqueue_msg(msgs[i], pid)) {
			print_send_msg(pid, msgs[i]);
			successfull_messages++;
		}
		else {
			break;
		}
	}

	wait(print_mutex);
	kprintf("Process: %d Successfully Sent %d Messages\n", currpid, successfull_messages);
	signal(print_mutex);

	restore(mask);
	return i;
}

/*
* Recieving a group (msg_count) of messages (msgs). It causes the calling processs
* to wait for msg_count messages to be sent. When all messages are in the queue, they
* are then all together immediately received.
*/
syscall receiveMsgs (umsg32* msgs, uint32 msg_count) {
	//Currently it does not receive all at once
	int i;
	for (i = 0; i < msg_count; i++) {
		msgs[i] = receive();
	}

	wait(print_mutex);
	kprintf("Process: %d Successfully Recieved %d Messages\n", currpid, msg_count);
	signal(print_mutex);	

	return OK;
}

/*
* Sending the message (msg) to the number (pid_count) of processes (pids). It will return 
* the number of pids successfully sent to or SYSERR on error.
*/
uint32 sendnMsg (uint32 pid_count, pid32* pids, umsg32 msg) {
	intmask	mask = disable();

	uint32 successfull_messages = 0;
	uint32 i;
	for (i = 0; i < pid_count; i++) {
		pid32 pid = pids[i];
		if (isbadpid(pid)) {
			continue;
		}

		if (enqueue_msg(msg, pid)) {
			print_send_msg(pid, msg);
			successfull_messages++;
		}
	}

	wait(print_mutex);
	kprintf("Process: %d Successfully Sent %d Messages\n", currpid, i);
	signal(print_mutex);

	restore(mask);
	return i;
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

/* Prints: Sent Message: %d to PID: %d */
void print_send_msg(pid32 pid, umsg32 msg) {
	wait(print_mutex);
	kprintf("Sent Message: %d to PID: %d\n", msg, pid);
	signal(print_mutex);
}

/* Prints: Received Message: %d */
void print_receive_msg(umsg32 msg) {
	wait(print_mutex);
	kprintf("Current Process: %d Received Message: %d\n", currpid, msg);
	signal(print_mutex);
}

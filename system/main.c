/*  main.c  - main */

/*
* Lab 2 Checklist
*
* √ works
* X doesn't work
* ? not tested
* - works but will need to be retested after change
*
* sendMSg:
* 	√ Send Msg to Non-Full Process Queue (Test Case 1)
*	√ Sending Msg to bad pid returns SYSERR (Test Case 3)
*	? Sending Msg to Full Process Queue returns SYSERR
*
* receiveMsg:
* 	√ Receives message when a message is queued (Test Case 1)
*	√ Waits to receive message when none are queued (Test Case 2)
*
* sendMsgs:
* 	√ Sends multiple messages to a single process (Test Case 4)
*	√ Returns number of messages sent (Test Case 4, )
*	? Returns SYSERR on badpid
*	? Returns SYSERR on no messages sent
* 
* receiveMsgs:
* 	√ Receives number of messages if queued (Test Case 4)
*	√ Messages saved in array (Test Case 4)
*	√ Waits for number of messages to be queued before receiving all at once (Test Case 5)
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

#define MSG_BUFFER_SIZE 11
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
int curr_buffer_size(pid32 pid);

/* Printing Functions */
void print_send_msg(pid32 pid, umsg32 msg);
void print_receive_msg(umsg32 msg);

/* Function for testing */
void test();
void empty(); //Creates process that does nothing

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
	int i;
	
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

	// umsg32 msg1 = 1;
	// umsg32 msg2 = 2;
	
	// // Receive Process
	// pid32 proc3 = create(receiveMsg, 4096, 50, "Receive", 0, 0);

	// // Send Processes
	// pid32 proc1 = create(sendMsg, 4096, 50, "Send message to proc3", 2, proc3, msg1);
	// pid32 proc2 = create(sendMsg, 4096, 50, "Send message to proc3", 2, proc3, msg2);

	// resume(proc1);
	// resume(proc2);

	// resume(proc3);

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

	umsg32 msg3 = 3;
	
	// Receive Process
	pid32 proc5 = create(receiveMsg, 4096, 50, "Receive", 0, 0);

	// Send Processes
	pid32 proc4 = create(sendMsg, 4096, 50, "Send message to proc3", 2, proc5, msg3);

	resume(proc5);
	resume(proc4);

	/* End Test Case 2 */

	/* Test Case 3
	 *
	 * proc4 sends msg3 to badpid
	 *
	 * Expected Output:
	 * Bad PID
	 * 
	 */

	// umsg32 msg4 = 4;
	
	// // Send Processes
	// pid32 proc6 = create(sendMsg, 4096, 50, "Send message to badpid", 2, 121, msg4);

	// resume(proc6);

	/* End Test Case 3 */

	/* Test Case 4
	 *
	 * proc7 sends msgs 5 - 10 to proc8
	 * proc8 receives msg at front of queue
	 *
	 * Expected Output:
	 * "Sent Msg 5" ...
	 * "Sent Msg 9"
	 * "5 Messages Sent"
	 * "Receive Msg 5" ...
	 * "Receive Msg 9"
	 * "Msg 5" ...
	 * "Msg 9"
	 *  
	 */

	// umsg32 msg_list_1[5] = { 5, 6, 7, 8, 9 };
	// umsg32 msg_list_1_rec[5];

	// //Receives 5 messages
	// pid32 proc8 = create(receiveMsgs, 4096, 50, "Receive 5 Messages", 2, &msg_list_1_rec, 5);

	// 
	// pid32 proc7 = create(sendMsgs, 4096, 50, "Send 5 to proc8", 3, proc8, &msg_list_1, 5);

	// resume(proc7);
	// resume(proc8);

	// wait(print_mutex);
	// for (i = 0; i < 5; i++) {
	// 	kprintf("Msg %d\n", msg_list_1_rec[i]);
	// }
	// signal(print_mutex);

	/* End Test Case 4 */

	/* Test Case 5
	 *
	 * proc9, 10 sends msgs 10-14, 15-19 to proc10
	 * proc10 will need to wait to receive all 10 messages
	 *
	 * Expected Output:
	 * "Sent Msg 10" ...
	 * "Sent Msg 14"
	 * "5 Messages Sent"
	 * "Sent Msg 15" ...
	 * "Sent Msg 19"
	 * "5 Messages Sent"
	 * "Receive Msg 10" ...
	 * "Receive Msg 19"
	 *  
	 */

	// umsg32 msg_list_2[5] = { 10, 11, 12, 13, 14 };
	// umsg32 msg_list_3[5] = { 15, 16, 17, 18, 19 };
	// umsg32 msg_list_23_rec[10];

	// //Receiving Process
	// pid32 proc11 = create(receiveMsgs, 4096, 50, "Receive 10 Messages", 2, &msg_list_23_rec, 10);

	// pid32 proc9 = create(sendMsgs, 4096, 50, "Send 5 to proc11", 3, proc11, &msg_list_2, 5);
	// pid32 proc10 = create(sendMsgs, 4096, 50, "Send 5 to proc11", 3, proc11, &msg_list_3, 5);

	// resume(proc11);
	// resume(proc9);
	// resume(proc10);

	/* End Test Case 5 */


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
		kprintf("Could not send message %d to Process: %d - Bad PID\n", msg, pid);
		signal(print_mutex);

		restore(mask);
		return SYSERR;
	}

	//TODO: Change this if we need to be able to handle this
	if (msg_queue_full(pid)) {
		wait(print_mutex);
		kprintf("Could not send message %d to Process: %d - Message Queue Full\n", msg, pid);
		signal(print_mutex);

		restore(mask);
		return SYSERR;
	}

	enqueue_msg(msg, pid);

	print_send_msg(pid, msg);
	wait(print_mutex);
	kprintf("Process: %d successfully sent 1 message\n", currpid);
	signal(print_mutex);

	//Restore Process if Waiting
	struct	procent *prptr = &proctab[pid];
	if (prptr->prstate == PR_RECV) {
		ready(pid, RESCHED_YES);
	} else if (prptr->prstate == PR_RECTIM) {
		unsleep(pid);
		ready(pid, RESCHED_YES);
	}

	restore(mask);
	return OK;
}

/*
* Receiving a message from any sender. It causes the calling process to wait for a message
* to be sent (by any process). When a message is sent, it is received and returned.
*/
umsg32 receiveMsg (void) {
	umsg32 msg;

	if (msg_queue_empty(currpid)) {
		wait(print_mutex);
		kprintf("No message to receive, Reschedule Process: %d\n", currpid);
		signal(print_mutex);

		struct	procent *prptr = &proctab[currpid];
		prptr->prstate = PR_RECV;
		resched();
	}
	
	msg = dequeue_msg(currpid);

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

	if (msg_count > MSG_BUFFER_SIZE) {
		wait(print_mutex);
		kprintf("Could not sent messages to Process: %d - Overflow Error\n", pid);
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

	if (successfull_messages == 0) {
		wait(print_mutex);
		kprintf("Process: %d did not successfully send any messages\n", currpid);
		signal(print_mutex);

		restore(mask);
		return SYSERR;
	}

	wait(print_mutex);
	kprintf("Process: %d Successfully Sent %d Messages\n", currpid, successfull_messages);
	signal(print_mutex);

	//Restore Process if Waiting
	struct	procent *prptr = &proctab[pid];
	if (prptr->prstate == PR_RECV) {
		ready(pid, RESCHED_YES);
	} else if (prptr->prstate == PR_RECTIM) {
		unsleep(pid);
		ready(pid, RESCHED_YES);
	}

	restore(mask);
	return i;
}

/*
* Recieving a group (msg_count) of messages (msgs). It causes the calling processs
* to wait for msg_count messages to be sent. When all messages are in the queue, they
* are then all together immediately received.
*/
syscall receiveMsgs (umsg32* msgs, uint32 msg_count) {
	if (msg_count > MSG_BUFFER_SIZE) {
		wait(print_mutex);
		kprintf("Could not Receive Messages - Overflow Error\n");
		signal(print_mutex);

		return SYSERR;
	}

	//Wait until buffer has msg_count messages in it
	while (curr_buffer_size(currpid) < msg_count) {
		wait(print_mutex);
		kprintf("Head: %d Tail: %d CURR_BUFFER_SIZE: %d\n", head[currpid], tail[currpid], curr_buffer_size(currpid));
		signal(print_mutex);
		wait(print_mutex);
		kprintf("Not enough messages, rescheduling process: %d\n", currpid);
		signal(print_mutex);

		struct	procent *prptr = &proctab[currpid];
		prptr->prstate = PR_RECV;
		resched();
	}

	wait(print_mutex);
	kprintf("Head: %d Tail: %d CURR_BUFFER_SIZE: %d\n", head[currpid], tail[currpid], curr_buffer_size(currpid));
	signal(print_mutex);

	int i;
	for (i = 0; i < msg_count; i++) {
		//NOTE: If we have waited for all the messages to be in the queue 
		//this if statement should be true every time
		if (!msg_queue_empty(currpid)) {
			msgs[i] = dequeue_msg(currpid); 
			print_receive_msg(msgs[i]);
		}
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
	if ((head[pid] + 1) % MSG_BUFFER_SIZE == tail[pid]) {
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

int curr_buffer_size(pid32 pid) {
	if (head[pid] == tail[pid]) {
		return MSG_BUFFER_SIZE - 1;
	}
	return (MSG_BUFFER_SIZE-head[pid]+tail[pid] - 1) % (MSG_BUFFER_SIZE - 1);
}

/* Printing/Debugging */

/* Creates an empty process to be used as a dummy (for testing) */
void empty() {
	while(1);
}

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

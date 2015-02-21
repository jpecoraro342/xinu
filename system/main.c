/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	uint32 retval;

	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	recvclr();
	while (TRUE) {
		retval = receive();
		kprintf("\n\n\rMain process recreating shell\n\n\r");
		resume(create(shell, 4096, 1, "shell", 1, CONSOLE));
	}
	while (1);

	return OK;
}

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

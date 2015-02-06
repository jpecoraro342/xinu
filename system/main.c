/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>

#include <user_functions.h>

int main(int argc, char **argv)
{
	
	callUserFunctions();

	while (1);

	return OK;
}

/********************
* Lab 2, task 6 - Integer overflow fixed
* Author: Ryan Morehart
*
* This is a program that "creates" a user.
* The root user has an ID of 0, normal users
* are much higher numbered.
*******************/
#include <stdio.h>
#include <limits.h>

int main(int argc, char **argv)
{
	unsigned int currUserID = 0;
	unsigned int newUserID = 0;
		
	// We would now figure out the current highest numbered user. 
	// Simulated, they have a very large ID number. An attacker might
	// force this by creating a bunch of pointless users
	currUserID = UINT_MAX;
	printf("Last user was ID %u\n", currUserID);
	
	// The next user ID is then...
	newUserID = currUserID + 1;
	
	// FIX for task 5
	if(newUserID < currUserID)
	{
		printf("Maximum number of users exceeded, unable to create new user\n");
		return 1;
	}
	
	printf("New user has ID %u\n", newUserID);
	
	// Their privilege level?
	if(newUserID == 0)
		printf("New user created with root privileges\n");
	else
		printf("New user created with normal privileges\n");
		
	return 0;
}


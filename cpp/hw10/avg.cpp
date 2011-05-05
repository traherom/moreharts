// Author: Ryan Morehart
// Date: Mon Oct 29 09:01:01 EDT 2007
// Purpose: HW10 p5.4 - Computes stats on 4 scores

#include <iostream>
#include <cmath>
using namespace std;

const int NUM_SCORES = 4;

void getNums(double nums[NUM_SCORES]);
void computeStats(double nums[NUM_SCORES], double &avg, double &dev);
double total(double nums[NUM_SCORES]);

int main()
{
	char runAgain;
	double avg;
	double dev;

	double nums[NUM_SCORES];

	do
	{
		getNums(nums);
		computeStats(nums, avg, dev);
		cout << "Avg: " << avg << " Std Dev: " << dev << "\n";
		
		// Run again?
		cout << "Run again? (y/n) ";
		cin >> runAgain;
	}
	while(runAgain == 'y' || runAgain == 'Y');

	return 0;
}

void getNums(double nums[NUM_SCORES])
{
	// Get scores from user
	cout << "Enter " << NUM_SCORES << " scores:\n";
	for(int i = 0; i < NUM_SCORES; i++)
	{
		cout << i + 1 << ": ";
		cin >> nums[i];
	}
}

void computeStats(double nums[NUM_SCORES], double &avg, double &dev)
{
	// Find average
	avg = total(nums) / NUM_SCORES;

	// Find standard deviation
	dev = sqrt(avg);
}

// Totals all items in an array
double total(double nums[NUM_SCORES])
{
	double total = 0;
	for(int i = 0; i < NUM_SCORES; i++)
	{
		total += nums[i];
	}

	return total;
}


// Author: Ryan Morehart
// Date: Fri Sep 7 09:13:12 EDT 2007
// Purpose:  

#include <iostream>
using namespace std;

int main()
{
	double total = 0;
	double score = 0;
	int const scoreCount = 3;

	// Get scores and total as we go
	for(int i = 0; i < scoreCount; i++)
	{
		cout << "Score " << i + 1 << ": ";
		cin >> score;
		total += score;
	}

	// Display decimal average
	cout << "Average: " << total / scoreCount << "\n";

	return 0;
}


// Author: Ryan Morehart
// Date: Mon Oct 29 09:01:01 EDT 2007
// Purpose: HW10 5.5 - Computes coins needed to match
//		a certain cent amount

#include <iostream>
using namespace std;

void computeCoin(int coinValue, int &num, int &amountLeft);
int getMoneyAmount();
void runThroughCoinTypes(int money);

int main()
{
	char runAgain;
	
	do
	{
		runThroughCoinTypes(getMoneyAmount());
	
		cout << "Run again? (y/n) ";
		cin >> runAgain;
	}
	while(runAgain == 'y' || runAgain == 'Y');
	
	return 0;
}

// Gets maximum number of a coin that can go int the amount left
void computeCoin(int coinValue, int &num, int &amountLeft)
{
	num = amountLeft / coinValue;
	amountLeft -= num * coinValue;
}

int getMoneyAmount()
{
	int money;
	
	cout << "How much money (1-99)? ";
	cin >> money;
	
	// Make sure it's a valid amount
	while(money <= 0 || money >= 100)
	{
		cout << "The money amount must be between 1 and 99 cents.\n";
		cout << "How much money? ";
		cin >> money;
	}
	
	return money;
}

void runThroughCoinTypes(int money)
{
	int num;
	
	// Get the max of each type possible by starting with largest coin
	computeCoin(25, num, money);
	cout << num << " quarters\n";
	
	computeCoin(10, num, money);
	cout << num << " dimes\n";
	
	computeCoin(1, num, money);
	cout << num << " pennies\n";
}


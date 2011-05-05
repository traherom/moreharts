// Author:  Keith Shomper
// Date:    9/22/03
// Putpose: To demonstrate the use of expression in C++

#include <iostream>
#include <string>

using namespace std;

int main() {
	enum   colors {red, orange, yellow, green, blue, indigo, violet};
	int    i = 6, j = 9, k = 12;
	bool   p = true, q = false;
	colors sky = blue, grass = green;
	string s;  // to allow the program to wait for input;

	// demonstrate various boolean expressions using the variables declared above
	cout << "The statement (i < j) is true (i.e., has the value " << (i < j) 
		<< " since (6 < 9)." << endl;
	getline(cin, s, '\n');

	cout << "True or false, the sky is blue and the grass is red:  " 
		<< (sky == blue && grass == red) << ".  It's false (i.e., 0)," << endl 
		<<  "because grass is not red." << endl;
	getline(cin, s, '\n');

	cout << "Note that you can mix object types as long are they are composed according to" << endl
		<< "the rules for boolean expressions:  " << ((p || (k/3 >= --i)) && sky != red) << endl 
		<< endl;

			cout << "Note that you can mix object types as long are they are composed according to" << endl
		<< "the rules for boolean expressions:  " << (k/3+1 >= --i) << endl 
		<< endl;
	return 0;
}


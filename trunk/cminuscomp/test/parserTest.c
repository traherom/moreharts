/**********************************************************
* Tests out tons of things that the parser should support *
**********************************************************/
int aGlobal; // Global variable
int aGlobalArray[ 6 ]; // Global array

/*
* Function returning int taking two parameters
*/
int pow(int num, int pow)
{
	int temp; // local variable declaration
	temp = 1; // Set variable to simple number

	while(pow >= 1) // iteration and relop
	{
		temp = temp * num; // set variable to multiplication
		pow = pow - 1; // set variable to subtraction
	}

	return temp; // Return an expression
}

/*
* No return, no parameter function
*/
void main(void)
{
	int inner; // local variable
	int innerArray[3]; // local array
	int first;
	int second;
	int sum;

	inner = 2; // assignment to variable
	innerArray[1] = 20; // assignment to array

	if(inner == 1) // selection statement w/o else
		out(innerArray[0]); // function call passed an array ref

	if(inner < 3) // selection statement w/ else
		out(innerArray[1]);
	else // else
	{ // compound to else
		inner = 4;
		out(innerArray[2]);
	}

	if(inner > 3) // >
		if(innerArray[1] != 0) // nested selection
		{ // compound to if
			out(inner); // function call with just variable
		}
		else // else belonging to inner if
			out(3); // function call with number
	else // else belonging to outter if
		out(3 * 5); // function call with expression

	inner = pow(2, 16); // function call w/ 2 args, assigned to variable

	out(pow(2, 8)); // function call being passed return from function

	while(innerArray[2] <= innerArray[1]) // iteration statement
	{
		innerArray[0] = 2;
		innerArray[2] = innerArray[2] + innerArray[1]; // array addition

		inner = 3 / 5 * 2;

		first = 3;
		second = 5;
		sum = first + second;

		if(inner == sum)
		{
			sum = sum / 2;
		}
		else if(inner > sum)
		{
			sum = sum / 2;
		}
		else
		{
			sum = sum * 2;
		}
	}

	if(innerArray[0] >= 5) // >=
		return; // return w/o expression

	inner = 3 + 4 * 12 - 4 / (2 + 5 * 3) + sum - (pow(3, 5) + 2) * pow(5, 3); // complex math
}

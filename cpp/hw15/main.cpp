// Author: Ryan Morehart
// Date: Wed Dec 5 21:48:37 EST 2007
// Purpose: Decrypt a text file using a function provided by Dr. Shimper

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
using namespace std;

#include "crypt_func.h"

int main()
{
	// Get file name
	string fname;
	cout << "File to decrypt: ";
	cin >> fname;

	// Open file
	ifstream encrypted;
	encrypted.open(fname.c_str());
	if(encrypted.fail())
	{
		cout << "Unable to open encrypted file.\n";
		return 1;
	}

	// Figure out name of file to write to
	string oname;
	oname = fname.substr(0, fname.find('.'));
	oname += "_pic.txt";

	// Open file to write decpyted version
	ofstream decrypted;
	decrypted.open(oname.c_str());
	if(decrypted.fail())
	{
		cout << "Unable to open '" << oname << "' for writing.\n";
		return 1;
	}

	cout << "Writing to output file '" << oname << "'.\n";

	// Step through each number in the encrypted file and output
	// decoded version to decrypted file
	int charCount = 0;
	while(!encrypted.eof())
	{
		charCount++;

		// Get next in file
		string sym;
		encrypted >> sym;
		
		// Decrypt
		int num = atoi(sym.c_str());
		string dec;
		dec = decrypt(num);
	
		// Output
		decrypted << dec;
	}
	
	cout << "There are " << charCount << " characters in the image.\n";

	// Close files
	encrypted.close();
	decrypted.close();

	return 0;
}


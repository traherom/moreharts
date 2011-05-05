// This is a comment, that is, a note to the person (you) reading the program.
// Comments begin as soon as you see the double slash, i.e., "//"

                     // By the way, comments can begin anywhere on a line

// We use comments to tell us (1) what the program is about, (2) who wrote it,
// and (3) when they wrote it.

// We also use comments to explain parts of the code which we think the reader
// may not understand (like you see on the right hand side of the lines below).

// Program #1:  HelloWorld - Demonstrates the basic parts of a C++ program
// Author:      Dr. Keith A. Shomper
// Date:        8 Jan 04

#include <iostream>        // already written code for input and output (I/O)

using namespace std;       // tells compiler to use standard names for things

int main()                 // the beginning of our program is always called "main"
{                          // an opening brace starts the program
   cout << "Hello Ryan"; // write the letters "Hello World!" to the screen
   cout << endl;           // write a new line, "endl" stands for end-of-the-line
   return 0;               // stop the program and return to operating system (O/S)
}                          // a closing brace finished the program

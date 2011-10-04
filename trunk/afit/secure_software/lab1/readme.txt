Assignment: Lab 1
Author: Lt Ryan Morehart

Compiling: gcc lab1.c -o lab1
Running: ./lab1

###############################
# Sample runs:
# Successful login:
traherom@ramlap:~/src/moreharts/afit/secure_software/lab1$ ./lab1 
Password (up to 30 chars): bah
Match

# Failed login:
traherom@ramlap:~/src/moreharts/afit/secure_software/lab1$ ./lab1 
Password (up to 30 chars): aoeuoaeuao
DENIED!

# Seg fault:
traherom@ramlap:~/src/moreharts/afit/secure_software/lab1$ ./lab1 
Password (up to 30 chars): eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
DENIED!
Segmentation fault

###############################
# Additional notes:
Fixing the hole here could be done by adding a width specifier to the scanf.
Also, I might use strncmp instead, just because I can.


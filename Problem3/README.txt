To run and compile the code for problem 2:
gcc -o search assignment1_p3.c to compile,
./search <number to search> <number of children> to run the code.
Also, you can use shuf -i option to feed random numeric sequences:
shuf -i LO-HI | ./search <number to search> <number of children>

Note: I'm writing this to prevent any confusion when you don't see 
status of a child. A child prints the result of a succes or a failure 
when it finishes its chunk, but if a child finds the number, all 
other processes are to be killed.

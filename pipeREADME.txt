the pipe.c file is just a program that takes a string in and sedns it to the other process then takes in a string in that process and sends it back to the main one 
so steps are
parent: 
       1. takes in string from user
       2. send string to the child using pipe1
       3. waits to receive a string back from the child rom pipe2
       4. prints out the message received from the child 
       5. loop
child: 
       1. waits to receive a string from the parent
       2. prints out the string form the parent when the message is received in the pipe1
       3. gets input from the user to send back to the parent
       4. sends the string back to the parent in pipe2
       5. loops

the important lines for getting the piping to work in the main function because this couldnt really work well in a seperate function are as followes: 
                         for initializing: 9,11,12,14
                         for parent side handling: 19,20,23,24,34,37,44,45
                         for child side handling: 52,53,55,56,60,69,72,73
NOTE: those lines are only for the pipe specific code so it leaves out taking in user input  

you need to have these includes 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

and there is not special flags tht need to be added to the compile command so the standard gcc filename.c -o filename will work just fine

as mentioned in one of the comments the while loops dont work perfectly but they are there to show that they can work at the same time and isnt just a one at a time type deal

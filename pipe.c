#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stddef.h>
//WEIRD BUG THING: some inputs just dont want to work in any capacity in the function "sdf" for example and other random mashings of the keyboard but not all for some reason just dont even start the function at all and just get stuck in an infinite wait for some reason and i have no idea why
//this takes in the sentence from the user and checks all of the words on the dictionary to see if the words can be okayed 
int pB(char* sentence){
        //the dictionary file that is being read from
        FILE *wordBank;
        wordBank = fopen("dictionary.txt", "r");
        //word is the array that is used to store a single word from the sentence that is passed
        char word[20];
        //check is the word from the dictionary that we are currently checking the word variable on to see if they are equal
        char check[20];
        int stringL = strlen(sentence);
        //used as our check for if the whole string is valid
        int valid = 0;
        //location of a space if there is one
        int spacefound = 0;
        //location in the sentence where the word is locatated
        int wordlocation = 0;
        memset(word, 0, sizeof(word));
        //loop to run through the entire string. it will parse and check to make sure the word is valid
        for(int i = 0; i < stringL-1; i++){
               //checks to see if you have come to the end of a word to begin the checking process
        	if(sentence[i] == ' '){
        	//printf("word check");
        	spacefound = 1;
        	   //ok this line 90% works. it gets the next string in the file but because when i set the length to something long to get everything it made the strcmps not work right when using two variables. i could check the variable check that held "hello" against the string "hello" and it work but if word was used instead of the literal "hello" it would break and this went both ways. checking the variable word on the literal string "hello" worked fine. with this a word such as "helo" will be accepted because it only takes in the size of the word from the file so the word "helots" will only take in "helo" and pass it. this so far is the only major issue and its only on certain cases where the word has to be misspelled to be the start of another word. 
        	   fgets(check, strlen(word)+1, wordBank);
        	   //runs through the file and checks word with check to see if the word is in the dictionary 
        	   while(strcmp(check,"zulus") != 0){
        	       if(strcmp(word,check) == 0){
        	       valid = 1;
        	       //restarts back to the begining of the file for reading
        	       rewind(wordBank);
        	       break;
        	       }
        	       fgets(check, strlen(word)+1, wordBank);
        	   }
        	   //if you get through the whole dictionary without finding the word you can return that there is an error
        	   if(valid == 0){
        	   return 0;
        	   }
        	   //resetting variables for next time in the word check while
        	   rewind(wordBank);
        	   valid = 0;
        	   memset(word, 0, sizeof(word));
        	   wordlocation = 0;
        	}
        	//builds the word variable
        	else{
		   word[wordlocation] = sentence[i];
		   wordlocation = wordlocation +1;
        	}
        	
        }
        //lines 61 through 69 are the same code as in the pervious while however this is here for single words and the last word in a sentence
        rewind(wordBank);
        fgets(check, strlen(word)+1, wordBank);
        while(strcmp(check,"zulus") != 0){
             if(strcmp(word,check) == 0){
             fclose(wordBank);
             return 1;
             }
          fgets(check, strlen(word)+1, wordBank);
        }

        fclose(wordBank);
        return 0;
}

int main(){
   //the fd arrays are just how the write and read descripters are stored into and the difference of arrays makes different pipes
   printf("BEFORE PIPE\n");
   int fd[2];
   pipe(fd);//child to parent communication
   int fd2[2];
   pipe(fd2);//parent to child communication
   //locations of fd[0] and fd2[0] are reads and fd[1] and fd2[1] are writes
   printf("BEFORE FORK\n");
   int pid = fork();
   
   //parent process
   if(pid != 0){
      //close the unneeded pipe sides so because you are not reading in pipe fd2 you close the read end and because you are not writing on the fd you close the write side. 
      close(fd2[0]);
      close(fd[1]);
      //the length of 250 is just filler change it how you need
      //this just helps to keep things seperated which is why there are two 
      char messageParent[1000];
      char recieveParent[1000];
      //this while loops works a little too good right now. but you can send back and forth messages with the two pipes 
      while(1){
      //just to make sure there is nothing the stdin buffer and you get a completely new message
      fflush(stdin);
      //the two lines of code that ask for the input and then gets the input from the stdin
      printf("Enter a Sentence: ");
      memset(messageParent, 0, sizeof(messageParent));
      fgets(messageParent, sizeof(messageParent), stdin);
      fflush(stdin);
      //pA(messageParent);
      
      //parent to child writing
      printf("before parent send\n");
      write(fd2[1], &messageParent, sizeof(messageParent));
      printf("After parent send\n");
      //waiting for the read in the pipe that handles child -> parent communication 
      read(fd[0], &recieveParent, sizeof(recieveParent));
      
      //printing out the message from the child
      printf("Received from child: %s\n" , recieveParent);
      
      }
      //close the sides of the pipe that actually get used the first one being the the write for the parent -> child communication and the second being the read for the child -> parent communication  
      close(fd2[1]);
      close(fd[0]);
   }
   
   //child process
   //a lot of the comments in this are the same to the parent as they have very similar function but in different order so i will leave out the stuff that is the same like why i have two char arrays
   else{
      //closing the unneeded pipe ends which are the parent -> child communication and the second being the read for the child -> parent communication  
      close(fd2[1]);
      close(fd[0]);
      //strings for recieving and sending on the child side
      char messageChild[1000];
      char success[20] = "SENTENCE GOOD\n";
      char failed[20] = "ERROR IN SENTENCE\n";
      int check;
      //this while loops works a little too good right now. but you can send back and forth messages with the two pipes 
      while(1){
      memset(messageChild, 0, sizeof(messageChild));
      //read in the message from the parent
      read(fd2[0],&messageChild, sizeof(messageChild));
      //print out the message from the parent and then ask for input from the child
      printf("The Message from parent: %s\n", messageChild);
      //send then message to parent process
      check = pB(messageChild);
      printf("SEND CHILD: %d\n", check);
      //printf("after pB call\n");
      if(check == 0){
         write(fd[1], &failed, 19);
      }
      else{
         write(fd[1], &success, 15);
      }
      
      }
      //close the sides of the pipe that actually get used the first one being the write for the child -> parent communication and the second being the read for the parent -> child communication
      close(fd[1]);
      close(fd2[0]);
   }

return 0;
}



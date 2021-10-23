/*

There is a warning in compilation which i am unsure about 
because i use the same char** as landen does in his main 
but i get "warning: assignment to ‘char **’ from incompatible pointer type ‘char *’"
even with testing in landen's main the use of char** works fine

other than that hopefully i can get together with landen to figure out 
the multiple use issue and then apply the finishing touches and we 
should be good to go completely

*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "client.h"
#include "clientThread.h"
#include "retrieveFile.h"

int main(){
   int fd[2];
   int fd2[2];
   char* words;
   char tempsend[1000];
   char recieveParent[1000];
   char fileName[50];
   int askagain;
   pipe(fd2);//parent to child communication
   pipe(fd);//child to parent communication
   int menu = 0;
   //locations of fd[0] and fd2[0] are reads and fd[1] and fd2[1] are writes
   int pid = fork();
   if(pid != 0){
      //close the unneeded pipe sides so because you are not reading in pipe fd2 you close the read end and because you are not writing on the fd you close the write side. 
      close(fd2[0]);
      close(fd[1]);
      while(menu != 3){
      printf("1. add to file\n2. get file\n3. exit\n");
      scanf("%d", &menu);
      if(menu == 1){
      askagain = 0;
        while(askagain == 0){
         printf("before write parent\n");
         //memset(words, 0, strlen(words));
         words = pA();
         if(words[0] == '\n'){
         strcpy(words, &words[1]);
         }
         //strcpy(words, &words[1]);
         printf("Before parent send %s\n", words);
         write(fd2[1], words, strlen(words));
         
         //waiting for the read in the pipe that handles child -> parent communication 
         memset(recieveParent, 0, strlen(recieveParent));
         read(fd[0], recieveParent, sizeof(recieveParent));
         //strcpy(tempsend, words);
         printf("words: %s\n", words);
         printf("%s after read parent\n", recieveParent);
         if(strcmp(recieveParent, "-1") == 0){
         sendSentence(words);
         askagain = 1;
         }
         else{
         printf("Please fix these words: %s", recieveParent);
         }
         }
         
      }
      else if(menu == 2){
         printf("Input the file name that you wish to get: ");
         scanf("%s", fileName);
         retrieveFile(fileName);
      }
      else{
         //this write will help to exit the program cleanly 
         write(fd2[1], "-1", 2);
         //break;
      }
      }
      //close the sides of the pipe that actually get used the first one being the the write for the parent -> child communication and the second being the read for the child -> parent communication  
      close(fd2[1]);
      close(fd[0]);
   }
   else{
      close(fd2[1]);
      close(fd[0]);
      char message[1000];
      char** badwords;
      char fullBW[1000];
      int i = 0;
      //printf("before while child\n");
      while(1){
         //printf("in while child\n");
         memset(message, 0, sizeof(message));
         //printf("PRE READ CHILD\n");
         read(fd2[0], message, sizeof(message));
         //printf("POST 1 READ CHILD\n");
         printf("Message from Parent: %s\n", message);
         if(strcmp(message,"-1") == 0 || strlen(message) == 0){
            //printf("EXIT");
            exit(0);
         }
         else{
            badwords = pB(message);
            //only sending the first word that is in the array until we get pB figured out
            printf("FIRST WORD FROM pB:%s", badwords[0]);
            if(strlen(badwords[0]) != 0){
               printf("after BAD write\n");
               write(fd[1], badwords[0], strlen(badwords[0]));
            }
            else{
               printf("in good\n");
               write(fd[1], "-1", 2);
            }
         }
      }
      
   }
return 0;
}

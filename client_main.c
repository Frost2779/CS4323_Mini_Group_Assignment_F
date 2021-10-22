/*

The only issue now is that sometimes the data gets messed up being passed to and from Landen's functions.
other than that the code works together.
i will update it as updated code comes through.
the code connects to all of Michaels and Landen's but once the issue with transition of the data gets fixed it
should all come together.
ill check again for updates periodically throughout the day so i can finalize this code

*/

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "clientThread.h"
#include "encryption.h"
#include "linkedList.h"
#include "socketConnection.h"
#include "util.h"
#include <ctype.h>
#include <stddef.h>
#include "landenthornton.c"
#include "michael_somdecerff.c"
//#include "RODEN_Serv.c"

#define SOCKET_EXIT "|exit|"
#define ERROR_MSG "ERROR"
#define RETRIEVE_FILE_COMMAND "|retrieve_file|"
#define SAVE_FILE_COMMAND "|save_file|"
int main(){
   int fd[2];
   int fd2[2];
   char* words;
   char recieveParent[1000];
   char fileName[50];
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
         printf("before write parent\n");
         words = pA();
         write(fd2[1], &words, sizeof(words));
         printf("After parent send\n");
         //waiting for the read in the pipe that handles child -> parent communication 
         read(fd[0], &recieveParent, sizeof(recieveParent));
         printf("%s after read parent\n", recieveParent);
         sendSentence(words);
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
      char* badwords;
      //printf("before while child\n");
      while(1){
         //printf("in while child\n");
         memset(message, 0, sizeof(message));
         //printf("PRE READ CHILD\n");
         read(fd2[0], &message, sizeof(message));
         //printf("POST 1 READ CHILD\n");
         //printf("%s\n", message);
         if(strcmp(message,"-1") == 0){
            printf("EXIT");
            exit(0);
         }
         else{
            badwords = pB(message);
            //strcpy(message, badwords);
            printf("MESSAGE FROM pB:%s", badwords);
            if(sizeof(badwords) != 0){
               printf("after BAD write\n");
               write(fd[1], &badwords, sizeof(badwords));
            }
            else{
               //printf("in good\n")
               write(fd[1], "All Words are Good", 18);
            }
         }
      }
      
   }
return 0;
}

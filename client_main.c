/*

For some reason the code stops after line 89 and goes back to the parent function
it is not from landen's pA because not using that still gets the issue
will work through it tomorrow
compiles and runs with all of the code and connects on Michaels side of the code
at least on the file retrieve side because i cannot currently get passed line 89
as mentioned. i will comment it out tomorrow when i get the issues fixed up with
line 89 and with the prints that have been in there to debug it doesnt even get 
to the conditionals so this code can work kinda as a test but its current progress
for the night and will fix it up tomorrow maybe

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
   char sentence[20] = "hello world";
   char* recieveParent;
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
         //sentence = pA();
         printf("before write parent\n");
         write(fd2[1], sentence, sizeof(sentence));
         printf("before read parent\n");
         read(fd[0], recieveParent, sizeof(recieveParent));
         printf("after read parent\n");
      }
      else if(menu == 2){
         printf("Input the file name that you wish to get: ");
         scanf("%s", fileName);
         retrieveFile(fileName);
      }
      else{
         //this write will help to exit the program cleanly 
         write(fd2[1], "-1", 2);
         break;
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
      printf("before while child\n");
      while(1){
      printf("in while child\n");
      memset(message, 0, sizeof(message));
      read(fd2[0], message, sizeof(message));
      printf("after read child %s\n", message);
      if(strcmp(message,"-1") == 0){
      printf("EXIT");
      break;
      }
      else{
      printf("IN ELSE");
      badwords = pB(message);
      printf("MESSAGE FROM pB:%s", badwords);
      if(sizeof(badwords) != 0){
      printf("after BAD write\n");
      write(fd[1], badwords, sizeof(badwords));
      printf("after write child bad\n");
      }
      else{
      printf("after GOOD write\n");
      write(fd[1], "All Words are Good", 18);
      printf("after write child good\n");
      }
      }
      printf("WHERE???????????????????");
      }
      
   }
return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

/*
int main(){
   //the fd arrays are just how the write and read descripters are stored into and the difference of arrays makes different pipes
   int fd[2];
   pipe(fd);//child to parent communication
   int fd2[2];
   pipe(fd2);//parent to child communication
   //locations of fd[0] and fd2[0] are reads and fd[1] and fd2[1] are writes
   int pid = fork();
   
   //parent process
   if(pid != 0){
      //close the unneeded pipe sides so because you are not reading in pipe fd2 you close the read end and because you are not writing on the fd you close the write side. 
      close(fd2[0]);
      close(fd[1]);
      //the length of 250 is just filler change it how you need
      //this just helps to keep things seperated which is why there are two 
      char messageParent[250];
      char recieveParent[250];
      //this while loops works a little too good right now. but you can send back and forth messages with the two pipes 
      while(strcmp(messageParent, "exit") != 0 && strcmp(recieveParent, "exit") != 0){
      //just to make sure there is nothing the stdin buffer and you get a completely new message
      fflush(stdin);
      //the two lines of code that ask for the input and then gets the input from the stdin
      printf("Type the message: ");
      fgets(messageParent, sizeof(messageParent), stdin);
      
      //parent to child writing
      write(fd2[1], &messageParent, sizeof(messageParent));
      
      //waiting for the read in the pipe that handles child -> parent communication 
      read(fd[0], &recieveParent, sizeof(recieveParent));
      
      //printing out the message from the child
      printf("Received from child: %s " , recieveParent);
      
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
      char messageChild[250];
      char sendChild[250];
      //this while loops works a little too good right now. but you can send back and forth messages with the two pipes 
      while(strcmp(messageChild, "exit") != 0 && strcmp(sendChild, "exit") != 0){
      //read in the message from the parent
      read(fd2[0],&messageChild, sizeof(messageChild));
      //print out the message from the parent and then ask for input from the child
      printf("The Message from parent: %s\n", messageChild);
      printf("Type the message to send back: ");
      //make sure the stdin is cleared
      fflush(stdin);
      //get the input from the fresh stdin
      fgets(sendChild, sizeof(sendChild), stdin);
      //send then message to parent process
      write(fd[1], &sendChild, sizeof(sendChild));
      }
      //close the sides of the pipe that actually get used the first one being the write for the child -> parent communication and the second being the read for the parent -> child communication
      close(fd[1]);
      close(fd2[0]);
   }

return 0;
}
*/
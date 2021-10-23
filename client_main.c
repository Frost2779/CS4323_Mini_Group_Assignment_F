
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include "client.h"
#include "clientThread.h"
#include "retrieveFile.h"

int main() {
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
    if (pid != 0) {
        // Create client thread
        createClientThread();

        //close the unneeded pipe sides so because you are not reading in pipe fd2 you close the read end and because you are not writing on the fd you close the write side.
        close(fd2[0]);
        close(fd[1]);
        //loop for the menu
        while (menu != 3) {
            //print the menu
            printf("1. add to file\n2. get file\n3. exit\n");
            scanf("%d", &menu);
            //if else statements for menu handling
            if (menu == 1) {
                askagain = 0;
                //askagain and this while handle if you need to ask for the sentence to be fixed
                while (askagain == 0) {
                    words = pA();
                    //sometimes pA() places a \n in front so this just makes sure that we can get rid of it
                    if (words[0] == '\n') {
                        strcpy(words, &words[1]);
                    }
                    //send the data to the child for checking
                    write(fd2[1], words, strlen(words));

                    //waiting for the read in the pipe that handles child -> parent communication
                    memset(recieveParent, 0, strlen(recieveParent));
                    read(fd[0], recieveParent, sizeof(recieveParent));
                    //checks to make sure if the sentence is good
                    if (strcmp(recieveParent, "-1") == 0) {
                        sendSentence(words);
                        askagain = 1;
                        //to help prevent mixing of output
                        sleep(1);
                    }
                        //prints the words that dont work and asks for a new sentence
                    else {
                        printf("Words that need to be fixed:%s\n", recieveParent);
                    }
                }

            }
                //handles file retrieve
            else if (menu == 2) {
                printf("Input the file name that you wish to get: ");
                scanf("%s", fileName);
                retrieveFile(fileName);
            }
                //exit
            else {
                //this write will help to exit the program cleanly
                write(fd2[1], "-1", 2);
            }
        }
        //close the sides of the pipe that actually get used the first one being the the write for the parent -> child communication and the second being the read for the child -> parent communication
        close(fd2[1]);
        close(fd[0]);
    } else {
        //closes the unneeded side of the pipe
        close(fd2[1]);
        close(fd[0]);
        //strings for checking and sending
        char message[1000];
        char badwords[1020] = " ";
        char fullBW[1000] = " ";
        char tempsentence[1000] = " ";
        //the placement for tempsentence
        int i = 0;
        //counts the amount of sentences
        int sentcount;
        //will keep looping until the user selects the exit option
        while (1) {
            strcpy(fullBW, " ");
            strcpy(tempsentence, " ");
            strcpy(badwords, " ");
            sentcount = 0;
            memset(message, 0, sizeof(message));
            read(fd2[0], message, sizeof(message));
            //handles a clean exit
            if (strcmp(message, "-1") == 0 || strlen(message) == 0) {
                exit(0);
            }
                //works on the checking of the words
            else {
                //handles the sentence by sentence parsing and checking
                for (int x = 0; x < strlen(message); x++) {
                    //finds the end of sentences
                    if (message[x] == '.') {
                        pB(tempsentence, badwords);
                        strcpy(fullBW, badwords);
                        i = 0;
                        sentcount++;
                        memset(tempsentence, 0, strlen(tempsentence));
                    }
                        //makes the words
                    else {
                        tempsentence[i] = tolower(message[x]);
                        i++;

                    }
                }
                //checks to see if we need to send a error in sentence message or a string good message
                if (strlen(badwords) > sentcount) {
                    write(fd[1], badwords, strlen(badwords));
                } else {
                    write(fd[1], "-1", 2);
                }
            }
        }

    }
    return 0;
}

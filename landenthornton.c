#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

//This keeps throwing a segmentation fault, currently trying to get that figured out.


char* pA(){

        char sentence[1000];
        printf("Enter a sentence: ");
        char get;
        int i = 0;
        while(get != '\n'){
                //printf("%s\n", sentence);
                scanf("%c", &get);
                //printf("%c\n", get);
                sentence[i] = get;
                i++;
        }
        sentence[i] = ' ';

        //printf("1. Send data to server\n2. Recieve file from server\n3. Terminate\n");
        return sentence;
}

char* pB(char* sentence){

        printf("Not yet");

        FILE *wordBank;
        wordBank = fopen("dictionary.txt", "r");

        //char* sentence;
        printf("Not yet");
        char word[1000];
        char next = fgetc(wordBank);
        char wrongWords[1000];

        printf("Not yet");
        int stringL = strlen(sentence);
        printf("%d", stringL);

        int valid = 0;

        for(int i = 0; i < stringL; i++){
                if(strcmp(&sentence[i], " ") == 0){
                        strcpy(sentence, &sentence[i + 1]);
                        i = 0;

                        while(next != EOF){
                                printf("%c\n", next);
								 printf("%c\n", next);
                                char check[1000];
                                while(strcmp(&next, "\n") != 0){
                                        check[i] = next;
                                        next = fgetc(wordBank);
                                }

                                if(strcmp(word, check) == 0){
                                        valid = 1;
                                }
                                next = fgetc(wordBank);
                        }

                        if(valid != 1){
                                strcat(wrongWords, word);
                                strcat(wrongWords, " ");
                        }
                        valid = 0;
                }
                else{
                        char toAdd = tolower((unsigned char) sentence[i]);
                        strcat(word, &toAdd);
                }
        }

        fclose(wordBank);
        char* wrongWordsR;
        strcpy(wrongWordsR, wrongWords);
        return wrongWordsR;
}

int main(){

        char* sentence = pA();
        printf("Not yet");
        char* wrongWords = pB(sentence);
        printf("%s", wrongWords);

        return 0;
}

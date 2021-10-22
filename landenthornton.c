#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "client.h"

char* pA(){

    char* sentence = malloc(sizeof(char) * 1000);
    printf("Enter a sentence: ");
    char get;
    int i = 0;
	scanf("%c", &get);
    do{
        sentence[i] = get;
        i++;
		scanf("%c", &get);
    }while(get != '\n');

    //printf("1. Send data to server\n2. Recieve file from server\n3. Terminate\n");
    return sentence;
		
}

char* pB(char* sentence){

    FILE *wordBank;
    wordBank = fopen("dictionary.txt", "r");

    //char* sentence;
        
    char word[1000];
    char next = fgetc(wordBank);
    char wrongWords[1000];

    int stringL = strlen(sentence);

    int valid = 0;

    for(int i = 0; i < stringL; i++){
		printf("iter %d\n", i);
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

/*
int main(){

    char* sentence = pA();
	printf("Not yet");
    char* wrongWords = pB(sentence);
    printf("%s", wrongWords);

    return 0;
	
}
*/
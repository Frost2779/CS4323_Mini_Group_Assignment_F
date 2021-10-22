#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

char* pA(){
	//Allocates memory for the message.
    char* message = malloc(sizeof(char) * 1000);
    printf("Enter a message: ");
    char get;
    int i = 0;
    scanf("%c", &get);
	
	//Accepts the message that the user entered.
    do{
        message[i] = get;
        i++;
        scanf("%c", &get);
    }while(get != '\n');
	
	//Prints the menu.
    printf("1. Send data to server\n2. Recieve file from server\n3. Terminate\n");
    return message;
}

char** pB(char* message){
	
	//Opens the file containing the dictionary.
    FILE *wordBank;
    wordBank = fopen("dictionary.txt", "r");
	
	//Creates an array to hold the wrong words.
    char** wrongWords = malloc(sizeof(char*) * 100);
    for(int i = 0; i < 100; i++){
        wrongWords[i] = malloc(sizeof(char) * 50);
    }
	//Creates an array to hold all of the sentences in the message.
    char** sentences = malloc(sizeof(char*) * 20);
    for(int i = 0; i < 20; i++){
        sentences[i] = malloc(sizeof(char) * 1000);
    }
    int stringL = strlen(message);
    int lastPeriod = 0;
    int numSentences = 0;
	//Stores each sentence of the message separately in the "sentences" array.
    for(int i = 0; i < stringL; i++){
        if(message[i] == '.' || message[i] == '!' || message[i] =='?'){
            int h = 0;
            for(int j = lastPeriod; j <= i; j++){
                sentences[numSentences][h] = message[j];
                h++;
            }
            lastPeriod = i + 2;
            numSentences++;
        }
    }
	
	//Creates an array to hold all the words in the dictionary.
	char** dictionary = malloc(sizeof(char*) * 58110);
    for(int i = 0; i < 58110; i++){
        dictionary[i] = malloc(sizeof(char) * 50);
    }
	
	//Stores all the words in the dictionary in the "dictionary" array.
    char next = fgetc(wordBank);
    int h = 0;
    int wordNum = 0;
    while(next != EOF){
        if(next != '\n'){
            dictionary[wordNum][h] = next;
            h++;
        }
        else{
            wordNum++;
            h = 0;
        }
        next = fgetc(wordBank);
    }
	
	//Scans each sentence for invalid words.
    int numWrongWords = 0;
    for(int i = 0; i < numSentences; i++){
		//Creates an array to hold each word in the current sentence.
        char** words = malloc(sizeof(char*) * 100);
        for(int j = 0; j < 100; j++){
            words[j] = malloc(sizeof(char) * 50);
        }
        int numWords = 0;
        int lastSpace = 0;
        int sentenceL = strlen(sentences[i]);
		//Converts all the words in the sentence to lowercase and stores them in the "words" array.
        for(int j = 0; j < sentenceL; j++){
            if(sentences[i][j] == ' ' || sentences[i][j] == '.' || sentences[i][j] == '!' || sentences[i][j] == '?' || sentences[i][j] == ';' || sentences[i][j] == ',' || sentences[i][j] == ':'){
                int h = 0;
                for(int k = lastSpace; k < j; k++){
                    words[numWords][h] = tolower(sentences[i][k]);
                    h++;
                }
                lastSpace = j + 1;
                words[numWords][h] = '\0';
                numWords++;
            }
        }

		//Checks to see if all the words in the sentence are in the dictionary. If they are not, they are stored in the "wrongWords" array.
        for(int j = 0; j < numWords; j++){
            int valid = 0;
            for(int k = 0; k < 58110; k++){
                if(strcmp(words[j], dictionary[k]) == 0){
                    valid = 1;
                }
            }
			if(valid != 1){
                wrongWords[numWrongWords] = words[j];
				numWrongWords++;
            }

        }
		
		//Frees all of the strings in the "words" array that are not also stored in the "wrongWords" array.
        for(int j = 0; j < 100; j++){
            int dontFree = 0;
            for(int k = 0; k < 100; k++){
                if(strcmp(words[j], wrongWords[k]) == 0 && strcmp(words[j], "\0") != 0){
                    dontFree = 1;
                }
            }
            if(dontFree == 0){
                free(words[j]);
            }
        }
        free(words);
    }
	//Frees the memory from the "dictionary" array.
    for(int i = 0; i < 58110; i++){
        free(dictionary[i]);
    }
    free(dictionary);
	
	//Frees the memory from the "sentences" array.
    for(int i = 0; i < 20; i++){
        free(sentences[i]);
    }
    free(sentences);
    fclose(wordBank);
	//Returns the "wrongWords" array.
    return wrongWords;
}

//This main function is just for testing purposes.
int main(){

    char* message = pA();
        
    char** wrongWords = pB(message);

    for(int i = 0; i < 2; i++){
        printf("%s\n", wrongWords[i]);
    }

    free(message);

    for(int i = 0; i < 100; i++){
        free(wrongWords[i]);
    }
    free(wrongWords);

    return 0;
}
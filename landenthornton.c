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

    return message;
}

int pB(char* sentence, char* badwords){
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
        int templen;
        memset(word, 0, sizeof(word));
        //loop to run through the entire string. it will parse and check to make sure the word is valid
        for(int i = 0; i < stringL; i++){
               //checks to see if you have come to the end of a word to begin the checking process
        	if(sentence[i] == ' '){
        	spacefound = 1;
        	   //gets the next word in the wordbank for checking
        	   fscanf(wordBank, "%s", check);
        	   //runs through the file and checks word with check to see if the word is in the dictionary 
        	   while(strcmp(check,"zulus") != 0){
        	       if(strcmp(word,check) == 0){
        	       valid = 1;
        	       //restarts back to the begining of the file for reading
        	       rewind(wordBank);
        	       break;
        	       }
        	       fscanf(wordBank, "%s", check);
        	   }
        	   //if you get through the whole dictionary without finding the word you can return that there is an error
        	   if(valid == 0){
        	   strcat(badwords, word);
        	   strcat(badwords, " ");
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
        //printf("here");
        fscanf(wordBank, "%s", check);
        while(strcmp(check,"zulus") != 0){
             if(strcmp(word,check) == 0){
             fclose(wordBank);
             return 1;
             }
          fscanf(wordBank, "%s", check);
        }
        strcat(badwords, word);
        strcat(badwords, " ");
        fclose(wordBank);
        return 0;
}

//This main function is just for testing purposes.
/*int main(){
char* message;
char** wrongWords;
  while(1){
    message = pA();
        
    wrongWords = pB(message);

    for(int i = 0; i < 2; i++){
        printf("%s\n", wrongWords[i]);
    }
}
    free(message);

    for(int i = 0; i < 100; i++){
        free(wrongWords[i]);
    }
    free(wrongWords);

    return 0;
}*/

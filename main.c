#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef struct WordNode_struct {
    char* myWord;
    struct WordNode_struct* next; 
} WordNode;

typedef struct LadderNode_struct {
    WordNode* topWord;
    struct LadderNode_struct* next; 
} LadderNode;

int countWordsOfLength(char* filename, int wordSize) { 
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }
    int count = 0;
    char word[200];

    while (fscanf(file, "%s", word) == 1) {
        if (strlen(word) == wordSize) {
            count++;
        }
    }

    fclose(file);

    return count;
}

bool buildWordArray(char* filename, char** words, int numWords, int wordSize) { 
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return false;
    }

    int index = 0;
    char word[200];
    int count = 0;
    while (fscanf(file, "%s", word) == 1) {
        if (strlen(word) == wordSize) {
            strcpy(words[index], word);
            index++;
            count++;
            // if (index == numWords) {
            //     break;
            // }
        }
    }
    fclose(file);

    return (count == numWords);
}

int findWord(char** words, char* aWord, int loInd, int hiInd) { 
    while (loInd <= hiInd) {
        int midpoint = loInd + (hiInd - loInd) / 2; //caculate midpoint for the binary search
        int cmpRes = strcmp(aWord, words[midpoint]);

        if (cmpRes == 0) {
            return midpoint; // index of found word
        } else if (cmpRes < 0) {
            hiInd = midpoint - 1;
        } else {
            loInd = midpoint + 1;
        }
    }
    return -1; // only if word is not found
}

void freeWords(char** words, int numWords) {
   for (int i = 0; i < numWords; i++) {
       free(words[i]);
   }
   free(words);
}

void insertWordAtFront(WordNode** ladder, char* newWord) {
    WordNode* newNode = (WordNode*)malloc(sizeof(WordNode));
    newNode->myWord = newWord;
    newNode->next = *ladder;
    *ladder = newNode;
}

int getLadderHeight(WordNode* ladder) {
    int height = 0;
    while(ladder != NULL) {
        height++;
        ladder = ladder->next;
    }
    return height; // modify this line
}

WordNode* copyLadder(WordNode* ladder) {
    WordNode* copy = NULL;
    WordNode* current = ladder;
    WordNode* ctail = NULL;

    while (current != NULL) {
        WordNode* newN = (WordNode*)malloc(sizeof(WordNode));
        newN->myWord = current->myWord;
        newN->next = NULL;

        if (copy == NULL) {
            copy = newN;
            ctail = newN;
        } else {
            ctail->next = newN;
            ctail = newN;
        }
        current = current->next;
    }
    return copy; //modify this
}

void freeLadder(WordNode* ladder) {
    while (ladder != NULL) {
        WordNode* temp = ladder;
        ladder = ladder->next;
        //free(temp->myWord);
        free(temp);
    }  
}

void insertLadderAtBack(LadderNode** list, WordNode* newLadder) {
    LadderNode* newNode = (LadderNode*)malloc(sizeof(LadderNode));
    newNode->topWord = newLadder;
    newNode->next = NULL;

    if (*list == NULL) {
        *list = newNode;
    } else {
        LadderNode* current = *list;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

WordNode* popLadderFromFront(LadderNode** list) {
    if (*list == NULL) {
        return NULL;
    }
    LadderNode* it = *list;
    *list = (*list)->next;

    WordNode* topWord = it->topWord;
    free(it);

    return topWord; // returns pointer to the word
}

void freeLadderList(LadderNode* myList) {
    while (myList != NULL) {
        LadderNode* current = myList;
        myList = myList->next;

        freeLadder(current->topWord);
        free(current);
    }
}

//          run algorithm to find the shortest word ladder
//          from <startWord> to <finalWord> in the <words>
//          word array, where each word is <wordSize> long 
//          and there are <numWords> total words;
//          <usedWord> also has size <numWords>, such that
//          usedWord[i] is only true if words[i] has 
//          previously be entered into a ladder, and should
//          therefore not be added to any other ladders; 
//          the algorithm creates partial word ladders, 
//          which are [WordNode] linked lists, and stores 
//          them in a [LadderNode] linked list. 
//              return a pointer to the shortest ladder;
//              return NULL if no ladder is possible;
//                  before return, free all heap-allocated 
//                  memory that is created here that is not 
//                  the returned ladder
WordNode* findShortestWordLadder(   char** words, 
                                    bool* usedWord, 
                                    int numWords, 
                                    int wordSize, 
                                    char* startWord, 
                                    char* finalWord ) {
    LadderNode* myList = NULL;
    WordNode* myLadder = NULL;
    insertWordAtFront(&myLadder, startWord);
    insertLadderAtBack(&myList, myLadder);
    int startWordIndex = findWord(words, startWord, 0, numWords - 1);
    usedWord[startWordIndex] = true;

    while (myList != NULL) {
        myLadder = popLadderFromFront(&myList); // gets the starting word

        // nested loop to go through each possible word by changing each index of start word and
        // changing to each letter of the alphabet.
        for (int i = 0; i < wordSize; i++) {
           char temp[wordSize+1];
           //char* temp = (char*)malloc((wordSize+1) * sizeof(char)); // Allocate memory for temp
           strcpy(temp, myLadder->myWord);

           for (char alpha = 'a'; alpha <= 'z'; alpha++) { // changing temp index to letters of alphabet works 10/26
                temp[i] = alpha;
                int tempInDict = findWord(words, temp, 0, numWords - 1); // returns index position of the current temp word iteration
                if (tempInDict > -1 && !usedWord[tempInDict]) { // word must exist in dictionary and word cannot have already been used
                   // if (strcmp(temp,finalWord) == 0) { // neighbor word is the finalWord
                   if (strcmp(words[tempInDict], finalWord) == 0) {
                        insertWordAtFront(&myLadder, finalWord);
                        printf("found word: %s\n", temp);
                        freeLadderList(myList);
                        return myLadder;
                    } else {
                        WordNode* anotherLadder = copyLadder(myLadder);
                        //insertWordAtFront(&anotherLadder, temp);
                        insertWordAtFront(&anotherLadder, words[tempInDict]);
                        insertLadderAtBack(&myList, anotherLadder);
                        usedWord[tempInDict] = true;
                    } 
                }
           }
        }
        freeLadder(myLadder);
    }
    freeLadderList(myList);
    // no ladder found
    return NULL;
}

// interactive user-input to set a word; 
//  ensures sure the word is in the dictionary word array
void setWord(char** words, int numWords, int wordSize, char* aWord) {
    bool valid = false;
    printf("  Enter a %d-letter word: ", wordSize);
    int count = 0;
    while (!valid) {
        scanf("%s",aWord);
        count++;
        valid = (strlen(aWord) == wordSize);
        if (valid) {
            int wordInd = findWord(words, aWord, 0, numWords-1);
            if (wordInd < 0) {
                valid = false;
                printf("    Entered word %s is not in the dictionary.\n",aWord);
                printf("  Enter a %d-letter word: ", wordSize); 
            }
        } else {
            printf("    Entered word %s is not a valid %d-letter word.\n",aWord,wordSize);
            printf("  Enter a %d-letter word: ", wordSize);
        }
        if (!valid && count >= 5) { //too many tries, picking random word
            printf("\n");
            printf("  Picking a random word for you...\n");
            strcpy(aWord,words[rand()%numWords]);
            printf("  Your word is: %s\n",aWord);
            valid = true;
        }
    }
}

// helpful debugging function to print a single Ladder
void printLadder(WordNode* ladder) {
    WordNode* currNode = ladder;
    while (currNode != NULL) {
        printf("\t\t\t%s\n",currNode->myWord);
        currNode = currNode->next;
    }
}

// helpful debugging function to print the entire list of Ladders
void printList(LadderNode* list) {
    printf("\n");
    printf("Printing the full list of ladders:\n");
    LadderNode* currList = list;
    while (currList != NULL) {
        printf("  Printing a ladder:\n");
        printLadder(currList->topWord);
        currList = currList->next;
    }
    printf("\n");
}


int main() {
    srand((int)time(0));
    
    printf("\nWelcome to the CS 211 Word Ladder Generator!\n\n");

    // set word length using interactive user-input
    int wordSize;
    printf("Enter the word size for the ladder: ");
    scanf("%d",&wordSize);
    printf("\n");

    printf("This program will make the shortest possible\n"); 
    printf("word ladder between two %d-letter words.\n\n",wordSize);
    
    // interactive user-input sets the dictionary file;
    //  check that file exists; if not, user enters another file
    //  if file exists, count #words of desired length [wordSize]
    char dict[100];
    printf("Enter filename for dictionary: ");
    scanf("%s", dict);
    printf("\n");
    int numWords = countWordsOfLength(dict,wordSize);
    while (numWords < 0) {
        printf("  Dictionary %s not found...\n",dict);
        printf("Enter filename for dictionary: ");
        scanf("%s", dict);
        printf("\n");
        numWords = countWordsOfLength(dict,wordSize);
    }
    
    // allocate heap memory for the word array; only words with desired length
    char** words = (char**)malloc(numWords*sizeof(char*));
    for (int i = 0; i < numWords; ++i) {
        words[i] = (char*)malloc((wordSize+1)*sizeof(char));
    }

    // end program if file does not have at least two words of desired length
    if (numWords < 2) {
        printf("  Dictionary %s contains insufficient %d-letter words...\n",dict,wordSize);
        printf("Terminating program...\n");
        return -1;
    }
    
    // [usedWord] bool array has same size as word array [words];
    //  all elements initialized to [false];
    //  later, usedWord[i] will be set to [true] whenever 
    //      words[i] is added to ANY partial word ladder;
    //      before adding words[i] to another word ladder,
    //      check for previous usage with usedWord[i] 
    bool* usedWord = (bool*)malloc(numWords*sizeof(bool));
    for (int i = 0; i < numWords; ++i) {
        usedWord[i] = false;
    }
    
    // build word array (only words with desired length) from dictionary file
    printf("Building array of %d-letter words... ", wordSize); 
    bool status = buildWordArray(dict,words,numWords,wordSize);
    if (!status) {
        printf("  ERROR in building word array.\n");
        printf("  File not found or incorrect number of %d-letter words.\n",wordSize);
        printf("Terminating program...\n");
        return -1;
    }
    printf("Done!\n"); 

    // set the two ends of the word ladder using interactive user-input
    //  make sure start and final words are in the word array, 
    //  have the correct length (implicit by checking word array), AND
    //  that the two words are not the same
    char startWord[30];
    char finalWord[30];
    printf("Setting the start %d-letter word... \n", wordSize);
    setWord(words, numWords, wordSize, startWord);
    printf("\n");
    printf("Setting the final %d-letter word... \n", wordSize);
    setWord(words, numWords, wordSize, finalWord);
    while (strcmp(finalWord,startWord) == 0) {
        printf("  The final word cannot be the same as the start word (%s).\n",startWord);
        printf("Setting the final %d-letter word... \n", wordSize);
        setWord(words, numWords, wordSize, finalWord);
    }
    printf("\n");

    // run the algorithm to find the shortest word ladder
    WordNode* myLadder = findShortestWordLadder(words, usedWord, numWords, wordSize, startWord, finalWord);

    // display word ladder and its height if one was found
    if (myLadder == NULL) {
        printf("There is no possible word ladder from %s to %s\n",startWord,finalWord);    
    } else {
        printf("Shortest Word Ladder found!\n");
        printLadder(myLadder);
    }
    printf("Word Ladder height = %d\n",getLadderHeight(myLadder));
    
    // free the heap-allocated memory for the shortest ladder
    freeLadder(myLadder);
    // free the heap-allocated memory for the words array
    freeWords(words,numWords);
    free(usedWord);
    
    return 0;
}

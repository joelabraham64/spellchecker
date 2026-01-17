#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256

typedef struct node {
    char *value;
    struct node *next;
} Node;

typedef struct openHashTable {
    int size;
    Node **table;
} OpenHashTable;

unsigned long hash(char *word, int size) {
    unsigned long total = 0;
    int len = strlen(word);
    for (int i = 0; i < len; i++) {
        total = total * 101 + word[i];
    }
    return total % size;
}

OpenHashTable *initializeHashTable(int size) {
    OpenHashTable *hashTable = malloc(sizeof(OpenHashTable));
    if (!hashTable) {
        fprintf(stderr, "Failed to allocate memory for hash table\n");
        exit(EXIT_FAILURE);
    }
    hashTable->size = size;
    hashTable->table = calloc(size, sizeof(Node*));
    if (!hashTable->table) {
        fprintf(stderr, "Failed to allocate memory for hash table nodes\n");
        exit(EXIT_FAILURE);
    }
    return hashTable;
}

Node *initializeNode(char *value, Node *next) {
    Node *newNode = malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Failed to allocate memory for node\n");
        exit(EXIT_FAILURE);
    }
    newNode->value = strdup(value);
    newNode->next = next;
    return newNode;
}

void insert(OpenHashTable *hashTable, char *value) {
    int position = hash(value, hashTable->size);
    Node *ptr = hashTable->table[position];
    while (ptr != NULL) {
        if (strcmp(ptr->value, value) == 0)
            return; // Already exists
        ptr = ptr->next;
    }
    hashTable->table[position] = initializeNode(value, hashTable->table[position]);
}

void loadDictionary(FILE *fp, OpenHashTable *myHash) {
    char wrd[BUFSIZE];
    while (fscanf(fp, "%1023s", wrd) != EOF) {  // Read words safely
        insert(myHash, wrd);
    }
}

int find(OpenHashTable *hashTable, const char *word) {
    unsigned int position = hash((char *) word, hashTable->size); // Cast to (char *) if needed
    Node *ptr = hashTable->table[position];
    while (ptr != NULL) {
        if (strcmp(ptr->value, word) == 0)
            return 1;
        ptr = ptr->next;
    }
    return 0;
}


int main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <dictionaryFilePath> <inputFilePath> <add|ignore>\n", argv[0]);
        return EXIT_FAILURE;
    }
	char *dictionaryFilePath = argv[1]; //this keeps the path to the dictionary file file
	char *inputFilePath = argv[2]; //this keeps the path to the input text file
	char *check = argv[3]; // this keeps the flag to whether we should insert mistyped words into dictionary or ignore
	int numOfWords=0; //this variable will tell us how much memory to allocate

	int insertToDictionary;
	if(strcmp(check,"add")==0)
		insertToDictionary = 1;
	else
		insertToDictionary = 0;
    
	////////////////////////////////////////////////////////////////////
	//read dictionary file
    FILE *fp = fopen(dictionaryFilePath, "r");
    char *line = NULL; //variable to be used for line counting
    size_t lineBuffSize = 0; //variable to be used for line counting
    size_t lineSize; //variable to be used for line counting

    //check if the file is accessible, just to make sure...
    if(fp == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        return -1;
    }

    //First, let's count number of words in the dictionary.
    //This will help us know how much memory to allocate for our hash table
    while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1)
        numOfWords++;

    //Printing line count for debugging purposes.
    //You can remove this part from your submission.
    //printf("%d\n",numOfWords);
    
    //HINT: You can initialize your hash table here, since you know the size of the dictionary
    OpenHashTable *myHash = initializeHashTable(numOfWords);

    //rewind file pointer to the beginning of the file, to be able to read it line by line.
    fseek(fp, 0, SEEK_SET);

    char wrd[BUFSIZE];
    for (int i = 0; i < numOfWords; i++)
    {
        fscanf(fp, "%s \n", wrd);
        //You can print the words for Debug purposes, just to make sure you are loading the dictionary as intended
        //printf("%d: %s\n",i,wrd);
        insert(myHash,wrd);
        //HINT: here is a good place to insert the words into your hash table
    }
    fclose(fp);
    
	////////////////////////////////////////////////////////////////////
	//read the input text file word by word
    fp = fopen(inputFilePath, "r");
	
	//check if the file is accessible, just to make sure...
	if(fp == NULL)
	{
		fprintf(stderr, "Error opening file\n");
		return -1;
	}

    //HINT: You can use a flag to indicate if there is a misspelled word or not, which is initially set to 1
	int noTypo=1;

	//read a line from the input file
	while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1)
	{
		char *word;
        //These are the delimiters you are expected to check for. Nothing else is needed here.
		const char delimiter[]= " ,.:;!\n";

		//split the buffer by delimiters to read a single word
		word = strtok(line,delimiter); 
		
		//read the line word by word
		while(word!=NULL)
		{
            // You can print the words of the inpit file for Debug purposes, just to make sure you are loading the input text as intended
			//printf("%s\n",word);
            
        if (!find(myHash, word)) {
            printf("Misspelled word: %s\n", word);
            noTypo = 0;

            printf("Suggestions: ");
            char temp[BUFSIZE];
            int foundSuggestions = 0;
            int length = strlen(word);
            
            // Check swapped adjacent letters
            for (int i = 0; i < length - 1; i++) {
                strcpy(temp, word);
                char swap = temp[i];
                temp[i] = temp[i + 1];
                temp[i + 1] = swap;
                if (find(myHash, temp)) {
                    if (foundSuggestions);
                    printf("%s ", temp);
                    foundSuggestions = 1;
                }
            }

            // Check missing first letter
            strcpy(temp, word + 1); // Skip the first character
            if (find(myHash, temp)) {
                if (foundSuggestions);
                printf("%s ", temp);
                foundSuggestions = 1;
            }

            // Check missing last letter
            if (length > 1) {
                strcpy(temp, word);
                temp[length - 1] = '\0'; // Remove the last character
                if (find(myHash, temp)) {
                    if (foundSuggestions);
                    printf("%s ", temp);
                    foundSuggestions = 1;
                }
            }

            // Check adding a letter at the beginning or the end
            for (char letter = 'a'; letter <= 'z'; ++letter) {
                // Adding a letter at the beginning
                temp[0] = letter;
                strcpy(temp + 1, word);
                if (find(myHash, temp)) {
                    if (foundSuggestions);
                    printf("%s ", temp);
                    foundSuggestions = 1;
                }

                // Adding a letter at the end
                strcpy(temp, word);
                temp[length] = letter;
                temp[length + 1] = '\0';
                if (find(myHash, temp)) {
                    if (foundSuggestions);
                    printf("%s ", temp);
                    foundSuggestions = 1;
                }
            }


                if (insertToDictionary == 1) {
                insert(myHash, word);
                }  
            printf("\n");
        }

        word = strtok(NULL, delimiter);
    }
}
fclose(fp);

if (noTypo == 1)
    printf("No typo!\n");

    // DON'T FORGET to free the memory that you allocated
        for (int i = 0; i < myHash->size; i++) {
        Node *node = myHash->table[i];
        while (node != NULL) {
            Node *toDelete = node;
            node = node->next;
            free(toDelete->value);
            free(toDelete);
        }
    }
    free(myHash->table);
    free(myHash);

    return 0;
}
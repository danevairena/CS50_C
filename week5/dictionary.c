/* In speller.c, we’ve put together a program that’s designed to spell-check a file after loading a dictionary of words from disk into memory. 
That dictionary, meanwhile, is implemented in a file called dictionary.c. (It could just be implemented in speller.c, but as programs get more complex, 
it’s often convenient to break them into multiple files.) The prototypes for the functions therein, meanwhile, are defined not in dictionary.c 
itself but in dictionary.h instead. That way, both speller.c and dictionary.c can #include the file. Unfortunately, we didn’t quite get around to 
implementing the loading part. Or the checking part. Both (and a bit more) we leave to you!

Alright, the challenge now before you is to implement, in order, load, hash, size, check, and unload as efficiently as possible using a hash table
- You may alter dictionary.c (and, in fact, must in order to complete the implementations of load, hash, size, check, and unload), but you may not 
alter the declarations (i.e., prototypes) of load, hash, size, check, or unload. You may, though, add new functions and (local or global) variables to dictionary.c.
- You may change the value of N in dictionary.c, so that your hash table can have more buckets.
- You may alter dictionary.h, but you may not alter the declarations of load, hash, size, check, or unload.
- Your implementation of check must be case-insensitive. In other words, if foo is in dictionary, then check should return true given any capitalization thereof; 
none of foo, foO, fOo, fOO, fOO, Foo, FoO, FOo, and FOO should be considered misspelled.
- Capitalization aside, your implementation of check should only return true for words actually in dictionary. Beware hard-coding common words 
(e.g., the), lest we pass your implementation a dictionary without those same words. Moreover, the only possessives allowed are those actually in dictionary. 
In other words, even if foo is in dictionary, check should return false given foo's if foo's is not also in dictionary.
- You may assume that any dictionary passed to your program will be structured exactly like ours, alphabetically sorted from top to bottom with one word per 
line, each of which ends with \n. You may also assume that dictionary will contain at least one word, that no word will be longer than LENGTH 
(a constant defined in dictionary.h) characters, that no word will appear more than once, that each word will contain only lowercase alphabetical characters 
and possibly apostrophes, and that no word will start with an apostrophe.
- You may assume that check will only be passed words that contain (uppercase or lowercase) alphabetical characters and possibly apostrophes.
- Your spell checker may only take text and, optionally, dictionary as input. Although you might be inclined (particularly if among those more comfortable) 
to “pre-process” our default dictionary in order to derive an “ideal hash function” for it, you may not save the output of any such pre-processing to disk 
in order to load it back into memory on subsequent runs of your spell checker in order to gain an advantage.
- Your spell checker must not leak any memory. Be sure to check for leaks with valgrind. */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>

#include "dictionary.h"

// make count_words a global variable so you can use it in more than one place
unsigned int count_words = 0;

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// TODO: Choose number of buckets in hash table
// Number of buckets = max word length
// ex: cat has hash_index 2, because has 3 letters (array is zero indexed)
const unsigned int N = LENGTH;

// Hash table - an array of linked lists
// * points to the first address
node *table[N];

// Hashes word to a number
unsigned int hash(const char *word)
{
    int index = 0;
    // TODO: Improve this hash function
    // count the num of letters of the word
    for (int i = 0; word[i] != '\0'; i++)
    {
        index = i;
    }

    //return % N to be sure not going outside the boundaries of the hash table array
    return index % N;
}


// Loads dictionary into memory, returning true if successful, else false
// *dictionary is pointer to the memory address of the input file
bool load(const char *dictionary)
{
    // TODO
    // Open the dictionary file
    FILE *open_dict = fopen(dictionary, "r");

    //check if not opened
    if (open_dict == NULL)
    {
        printf("Could not open the dictionary file.\n");
        return false;
    }

    //continue if opened correctly
    else
    {
        //create buffer for reading
        // length + 1 because max word lenght + 1 for end of string sign '\0'
        char buffer[LENGTH + 1];

        //initialize hash index
        int hash_index = 0;

        //read from file while not reached EOF that means end of file
        while (fscanf(open_dict, "%s", buffer) != EOF)
        {
            // Create a new node  by allocating memory size of the node
            node *n = malloc(sizeof(node));

            // check if memory is allocated
            if (n == NULL)
            {
                return false;
                break;
            }
            else
            {
                // copy the word into the word part of the node
                strcpy(n->word, buffer);

                // set address to next node to NULL
                n->next = NULL;

                // get the hash index number
                hash_index = hash(buffer);

                // case 1 - the first entry
                // if nothing is there equal the first entry to the new node
                if (table[hash_index] == NULL)
                {
                    table[hash_index] = n;
                }

                // case 2 - an entry already exists
                // inserts at the front of the linked list each time
                else
                {
                    // set the new node's next address to the first element of the index position
                    n->next = table[hash_index];

                    // set the head of the linked list to the previously inserted node
                    table[hash_index] = n;
                }

                // count words added to the dictionary
                count_words++;
            }
        }

        //close opened dictionary file
        fclose(open_dict);
        return true;
    }
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    // TODO
    //return sum of words counted while reading the dictionary file
    return count_words;
}

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    // TODO
    // find word's index position
    int word_index = hash(word);

    // initialize cursor to first node in the linked list
    // note cursor->word is current word and cursor->next is a pointer to the next node address
    node *cursor = table[word_index];

    // while loop to iterate until NULL (the last word added in the chain)
    while (cursor != NULL)
    {
        //strcasecmp - compare strings case insenitively
        // if result = 0 (same strings) the word is found
        if (strcasecmp(word, cursor->word) == 0)
        {
            return true;
        }
        // set the cursor to the next node in the linked list
        cursor = cursor->next;
    }

    //if word is not found
    return false;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    // TODO
    // loop along the hash table
    for (int i = 0; i < N; i++)
    {
        // set temp and cursor pointers to first node's addres
        node *temp = table[i];
        node *cursor = table[i];

        // while loop until there are no nodes to be freed
        while (cursor != NULL)
        {
            // set temporary cursor to next node's address
            temp = temp->next; // i+1

            //free cursor
            free(cursor);

            //redirect cursor to the next node's address, stored in temp
            cursor = temp;
        }
    }
    return true;
}

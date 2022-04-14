/*
 Student Name:
 Date:

=======================
ECE 2035 Project 2-1:
=======================
This file provides definition for the structs and functions declared in the
header file. It also contains helper functions that are not accessible from
outside of the file.

FOR FULL CREDIT, BE SURE TO TRY MULTIPLE TEST CASES and DOCUMENT YOUR CODE.

===================================
Naming conventions in this file:
===================================
1. All struct names use camel case where the first letter is capitalized.
  e.g. "HashTable", or "HashTableEntry"

2. Variable names with a preceding underscore "_" will not be called directly.
  e.g. "_HashTable", "_HashTableEntry"

  Recall that in C, we have to type "struct" together with the name of the struct
  in order to initialize a new variable. To avoid this, in hash_table.h
  we use typedef to provide new "nicknames" for "struct _HashTable" and
  "struct _HashTableEntry". As a result, we can create new struct variables
  by just using:
    - "HashTable myNewTable;"
     or
    - "HashTableEntry myNewHashTableEntry;"

  The preceding underscore "_" simply provides a distinction between the names
  of the actual struct defition and the "nicknames" that we use to initialize
  new structs.
  [See Hidden Definitions section for more information.]

3. Functions, their local variables and arguments are named with camel case, where
  the first letter is lower-case.
  e.g. "createHashTable" is a function. One of its arguments is "numBuckets".
       It also has a local variable called "newTable".

4. The name of a struct member is divided by using underscores "_". This serves
  as a distinction between function local variables and struct members.
  e.g. "num_buckets" is a member of "HashTable".

*/

/****************************************************************************
* Include the Public Interface
*
* By including the public interface at the top of the file, the compiler can
* enforce that the function declarations in the the header are not in
* conflict with the definitions in the file. This is not a guarantee of
* correctness, but it is better than nothing!
***************************************************************************/
#include "hash_table.h"


/****************************************************************************
* Include other private dependencies
*
* These other modules are used in the implementation of the hash table module,
* but are not required by users of the hash table.
***************************************************************************/
#include <stdlib.h>   // For malloc and free
#include <stdio.h>    // For printf


/****************************************************************************
* Hidden Definitions
*
* These definitions are not available outside of this file. However, because
* the are forward declared in hash_table.h, the type names are
* available everywhere and user code can hold pointers to these structs.
***************************************************************************/
/**
 * This structure represents an a hash table.
 * Use "HashTable" instead when you are creating a new variable. [See top comments]
 */
struct _HashTable {
  /** The array of pointers to the head of a singly linked list, whose nodes
      are HashTableEntry objects */
  HashTableEntry** buckets;

  /** The hash function pointer */
  HashFunction hash;

  /** The number of buckets in the hash table */
  unsigned int num_buckets;
};

/**
 * This structure represents a hash table entry.
 * Use "HashTableEntry" instead when you are creating a new variable. [See top comments]
 */
struct _HashTableEntry {
  /** The key for the hash table entry */
  unsigned int key;

  /** The value associated with this hash table entry */
  void* value;

  /**
  * A pointer pointing to the next hash table entry
  * NULL means there is no next entry (i.e. this is the tail)
  */
  HashTableEntry* next;
};


/****************************************************************************
* Private Functions
*
* These functions are not available outside of this file, since they are not
* declared in hash_table.h.
***************************************************************************/
/**
* createHashTableEntry
*
* Helper function that creates a hash table entry by allocating memory for it on
* the heap. It initializes the entry with key and value, initialize pointer to
* the next entry as NULL, and return the pointer to this hash table entry.
*
* @param key The key corresponds to the hash table entry
* @param value The value stored in the hash table entry
* @return The pointer to the hash table entry
*/
static HashTableEntry* createHashTableEntry(unsigned int key, void* value) {
struct _HashTableEntry *HTE = (HashTableEntry*) malloc(sizeof(HashTableEntry)); //Create a new HashTableEntry and allocate the space in the heap for it
    if (HTE == NULL) {
        printf("Not enough Space");
        return HTE;
    }
    HTE->key = key;                                                                 //set the key of the new HashTableEntry to the one provided
    HTE->value = value;                                                             //set the value of the new HashTableEntry to the one provided
    HTE->next = NULL;                                                               //set the next of the new HashTableEntry to NULL (this should be set in the calling function)
    return HTE;                                                                     //return the newly created HashTableEntry
}

/**
* findItem
*
* Helper function that checks whether there exists the hash table entry that
* contains a specific key.
*
* @param hashTable The pointer to the hash table.
* @param key The key corresponds to the hash table entry
* @return The pointer to the hash table entry, or NULL if key does not exist
*/
static HashTableEntry* findItem(HashTable* hashTable, unsigned int key) {
    unsigned int num = hashTable->hash(key);         //get the correct hash code
    HashTableEntry* temp = hashTable->buckets[num];  //get the head of the correct list which could contain the key
    //Go through the list until we find the item or reach the end
    while (temp != NULL) {                           //while the current entry we're check is not NULL
        if (temp->key == key) {                      //check if the key of the current entry's key and the key we're searching for match
            return temp;                             //If they do, then return the current entry
        }
        temp = temp->next;                           //If the keys don't match, go to the next entry and check it (until we reach the end of the list)
    }
    return NULL;                                     //If the key wasn't in that list which we found via the hashcode, it wasn't in the hash table
}

/****************************************************************************
* Public Interface Functions
*
* These functions implement the public interface as specified in the header
* file, and make use of the private functions and hidden definitions in the
* above sections.
****************************************************************************/
// The createHashTable is provided for you as a starting point.
HashTable* createHashTable(HashFunction hashFunction, unsigned int numBuckets) {
  // The hash table has to contain at least one bucket. Exit gracefully if
  // this condition is not met.
  if (numBuckets==0) {
    printf("Hash table has to contain at least 1 bucket...\n");
    exit(1);
  }

  // Allocate memory for the new HashTable struct on heap.
  HashTable* newTable = (HashTable*)malloc(sizeof(HashTable));

  // Initialize the components of the new HashTable struct.
  newTable->hash = hashFunction;
  newTable->num_buckets = numBuckets;
  newTable->buckets = (HashTableEntry**)malloc(numBuckets*sizeof(HashTableEntry*));

  // As the new buckets are empty, init each bucket as NULL.
  unsigned int i;
  for (i=0; i<numBuckets; ++i) {
    newTable->buckets[i] = NULL;
  }

  // Return the new HashTable struct.
  return newTable;
}

void destroyHashTable(HashTable* hashTable) {
    //To destroy the hash table, We need to free up all the things in the list, the lists, and the hash table itself
    for (int i = 0; i < hashTable->num_buckets; i++) {  //iterate through all the lists and free up all the entries in the lists 
        if (hashTable->buckets[i] != NULL) {                    //Check if the current list is NULL (if it exists)
            HashTableEntry* HTE = hashTable->buckets[i];//If it does exist, there are entries in it, so get the head
            HashTableEntry* tmp;                        //Set up a new pointer for later use
            while (HTE != NULL) {                       //Check if the current entry is not NULL
                tmp = HTE->next;                        //If it does exist, get the next entry to check it later
                free(HTE);                              //Free up the space taken up by the current entry
                HTE = tmp;                              //set the current entry to the next entry
            }
        }
    }
    free(hashTable->buckets);                           //Free up the space of the lists
    free(hashTable);                                    //Free up the space of the hash table itself
    return;                                             //return 
}

void* insertItem(HashTable* hashTable, unsigned int key, void* value) {
    unsigned int num = hashTable->hash(key);                 //Get the hash value of the key
    HashTableEntry *found = findItem(hashTable, key);        //Check if the key is already in the table
    //If it is in the Hash Table, we can just update the existing HashTableEntry's value instead of having to create a new entry
    if (found != NULL) {
        if (found->key == key) {             //Make sure the key matches with the one we want
            void* foundV = found->value;    //Save the value of the old Entry for returning later
            found->value = value;            //Set the Entry's value to the new value
            return foundV;                   //Return the old value that was stored in the Entry before the insert
        }
    }
    //If it is not already in the Hash Table, add at the head
    HashTableEntry *newh = createHashTableEntry(key,value);  //Create a new HashTableEntry
    if (newh == NULL) {                                      //If there isn't enough space for a new entry it will return NULL
        return NULL;
    }
    newh->next = hashTable->buckets[num];   //set the new HashTableEntry's next to the head
    hashTable->buckets[num] = newh;         //set the head of the list at bucket[hash] to the new node
    return NULL;                           //return NULL to show it didn't replace a value

}

void* getItem(HashTable* hashTable, unsigned int key) {
//To get the item, we go to the findItem helper method and check if that finds something
    HashTableEntry* found = findItem(hashTable, key);    //Check if the key is in the hash table
    //if it isn't in the hash tabel, return NULL
    if (found == NULL) {
        return NULL;
    //if it is in the hash table, return its value
    } else {
        return found->value;
    }
}

void* removeItem(HashTable* hashTable, unsigned int key) {
    //To remove, first we check if it's in the table, then check if its at the head of the list or if its in the middle of the list
    //We have different things to do if it's at any of the previous possibiilties
    unsigned int num = hashTable->hash(key);        //Get the hashed key from the hash function
    //Check if the key is in the hash table
    if (findItem(hashTable, key) == NULL) {
        return NULL; //if the key is not in the hash table, return NULL
    }   
    //If the key is in the hash table, we still need to check if its at the front of the list or in the middle of it
    HashTableEntry *HTE = hashTable->buckets[num]; //get the head of the list at the correct position in the hash table
    //Check if the head is not null (which it shouldn't be, if there isn't an error in the find method) and if the key matches the head's key
    if (HTE != NULL && HTE->key == key) {     //if the entry which needs to be removed is the head:
        void* removed = HTE->value;           //get the head's value for returning later
        hashTable->buckets[num] = HTE->next;  //set the head to the head's next entry
        free(HTE);                            //free the old head
        return removed;                       //return the old head's value
    }
    //If the Entry is not a the head, then it must be in the middle of the list and we will need to get the entry right before the entry we need to remove
    HashTableEntry* next = HTE->next;         //get the next entry in the list for checking
    //Check if the current entry and the next entry are not NULL
    while (HTE != NULL && next != NULL) {
        if (next->key == key) {                   //Check if the next key is the key we're looking for
            HashTableEntry* removed = next;       //If it is the next entry, save it for freeing
            void* removedV = removed->value;      //Save the value of the removed entry 
            HTE->next = removed->next;           //set the current entry's next to the removed entry's next to skip over the removed (this essentially removes it)
            free(removed);                        //free the removed entry's space
            return removedV;                      //return the value from the removed entry
        }
        HTE = HTE->next;                          //if it is not the next key, go to the next set of entries
        next = next->next;                        //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    }
    return NULL;                                  //Just in case the remove function doesn't work, return NULL
}

void deleteItem(HashTable* hashTable, unsigned int key) {
    //Just like the remove function, first check where the key is (or isn't) and use different things for where it is 
    unsigned int num = hashTable->hash(key);   //Find the correct hash code
    //Check if the key is in the hash table
    if (findItem(hashTable, key) == NULL) {
        return; //if it isn't return now
    }       
    //if the key is in the hash table, check if it's at the head or in the middle
    HashTableEntry *HTE = hashTable->buckets[num];  //get the head of the list in the correct hash position
    //check if the head exist and if the key which needs to be removed matches the head key
    if (HTE != NULL && HTE->key == key) {
        hashTable->buckets[num] = HTE->next;   //if it does, set the head to the next entry in the list
        free(HTE);                             //and free the space the old head was taking up
        return;                                //then return
    }
    //if the head key and the removal key does not match, the key must be in the middle of the list
    HashTableEntry* next = HTE->next;  //get the next entry in the list
    //iteratre through the list until you find the entry before the entry we need to remove
    while (HTE != NULL && next != NULL) {
        if (next->key == key) { //Check if the next entry is the one we need to remove
            HashTableEntry* removed = next; //If so, get this next entry for freeing up later
            HTE->next = removed->next;      //set the next entry of entry right before the removed entry to the removed entry's next (essentially skipping ovet the removed entry)
            free(removed);                  //free the space the removed entry was taking up
            return;                         //and return
        }
        HTE = HTE->next;                    //if we didn't find the key, go to the next set of entries
        next = HTE->next;
    }
    return;                                 //if the delete function fails in some way just return
}
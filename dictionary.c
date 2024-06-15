#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"

table_t *create_table() { // Creates a Hash Table that is filled with NULLs in order for words to be added later
    table_t *hashTable = (table_t*) malloc(sizeof(table_t));
    if (hashTable == NULL) {
        free(hashTable);
        return NULL;
    }
    hashTable->length = INITIAL_HASH_TABLE_SIZE;
    hashTable->array = (list_node_t**) malloc(sizeof(list_node_t*) * hashTable->length);
    for(int i = 0; i < INITIAL_HASH_TABLE_SIZE; i++) {
        hashTable->array[i] = NULL;
    }
    return hashTable;
}

dictionary_t *create_dictionary() { // Creates dictonary with associated table
    dictionary_t *dict = malloc(sizeof(dictionary_t));
    if (dict == NULL) {
        return NULL;
    }
    dict->table = create_table();
    if (dict->table == NULL) {
        return NULL;
    }
    dict->size = 0;
    return dict;
}

int hash_code(const char* word) { // Converts a number into a hash code accessible to an index within the hash table.
    int hashNum = 0;
    for(int index = 0; index < strlen(word); index++) {
        hashNum += word[index];
    }
    return hashNum % INITIAL_HASH_TABLE_SIZE;
}

table_t* resize_table(table_t* orig) { // Resizes current table by a factor of original table * 2 + 1 when load factor reaches 0.8 called in dict_insert
    if (orig == NULL) {
        return NULL;
    } 
    table_t *newTable = malloc(sizeof(table_t));
    if (newTable == NULL) {
        return NULL;
    }
    newTable->length = orig->length * 2 + 1;
    newTable->array = malloc(sizeof(list_node_t*) * newTable->length);
    if (newTable->array == NULL) {
        return NULL;
    }
    for (int i = 0; i < newTable->length; i++) {
        newTable->array[i] = NULL;
    }
    for (int i = 0; i < orig->length; i++) {
        list_node_t *p = orig->array[i];
        while (p != NULL) {
            list_node_t *newP = malloc(sizeof(list_node_t));
            if (newP == NULL) {
                free(newTable->array);
                return NULL;
            }
            strcpy(newP->word, p->word);
            int newIndex = hash_code(p->word) % newTable->length;
            newP->next = newTable->array[newIndex];
            newTable->array[newIndex] = newP;
            list_node_t *origTableTemp = p;
            p = p->next;
            free(origTableTemp);
        }
    }
    free(orig->array);
    free(orig);
    return newTable;
}
    

 
int dict_insert(dictionary_t *dict, const char *word) { // Inserts a word in form list_node_t and puts in into a specific hash code index in dictionary
    if (dict == NULL) {
        return -1;
    }
    int wordIndex = hash_code(word);
    if ((dict->size/dict->table->length) > 0.8) {
        table_t *newDictTable = resize_table(dict->table);
        if (newDictTable == NULL) {
            return -1;
        }
        dict->table = newDictTable;
    }
    list_node_t *newWord =  (list_node_t*)malloc(sizeof(list_node_t)); 
    if (newWord == NULL) {
        free(newWord);
        return -1;
    }
    strcpy(newWord->word, word);
    newWord->next = NULL;
    if (dict->table->array[wordIndex] == NULL) {
        dict->table->array[wordIndex] = newWord;
        dict->size++;
        return 0;
    } else {
        while (newWord->next != NULL) {
            newWord = newWord->next;
        }
        newWord->next = dict->table->array[wordIndex];
        dict->table->array[wordIndex] = newWord;
        dict->size++;
        return 0;
    }
    return -1;
}

int dict_find(const dictionary_t *dict, const char *query) { // finds a particular query and compares a word within the dictionary and returns 0 or 1 if its found or not.
    if (dict == NULL || dict->table == NULL) {
        return 0;
    }
    for(int i = 0; i < dict->table->length; i++) {
        list_node_t *currNode = dict->table->array[i];
        while (currNode != NULL) {
            if (strcmp(currNode->word, query) == 0) {
                return 1;
            }
            currNode = currNode->next;
        }
    }
    return 0;
}

void dict_print(const dictionary_t *dict) { // Prints out all contents of the dictionary
    if (dict != NULL && dict->table != NULL && dict->table->array != NULL) {
        for(int i = 0; i < dict->table->length; i++) {
            list_node_t *currArray = dict->table->array[i];
            // list_node_t *currDictNode = currArray;
            while (currArray != NULL) {
                    printf("%s\n", currArray->word);
                    currArray = currArray->next;
            }
        }
    }
}

void dict_free(dictionary_t *dict) { // Free contents of the dictionary
   if (dict != NULL) {
        for (int i = 0; i < dict->table->length; i++) {
            list_node_t *freeNode = dict->table->array[i];
            while (freeNode != NULL) {
                list_node_t *currFreeNode = freeNode;
                freeNode = freeNode->next;
                free(currFreeNode);
            }
        }
        free(dict->table->array);
        free(dict->table);
    } 
    free(dict);
}

dictionary_t *read_dict_from_text_file(const char *file_name) { // Scans in line by line of text file and inserts words into the dictionary.
    FILE *newFile = fopen(file_name, "r");
    dictionary_t *newDictFile = create_dictionary();
    if (newFile == NULL) {
        dict_free(newDictFile);
        return NULL;
     }
    else {
         if (newDictFile == NULL || newDictFile->table == NULL || newDictFile->table->array == NULL) {
            return NULL;
        }
        char scanWord[MAX_WORD_LEN];
        while (fscanf(newFile, "%s", scanWord) != EOF) {
            dict_insert(newDictFile, scanWord);
        }
        fclose(newFile);
    }
    return newDictFile;
}

int write_dict_to_text_file(const dictionary_t *dict, const char *file_name) { // Writes all contents of a dictionary to a new text file.
    FILE *newDictFile = fopen(file_name, "w");
    if (newDictFile == NULL) {
        return -1;
    }
    if (dict == NULL || dict-> table == NULL || dict->table->array == NULL) {
        fclose(newDictFile);
        return -1;
    }
    for (int i = 0; i< dict->table->length; i++) {
        list_node_t *writNode = dict->table->array[i];
        while (writNode != NULL) {
            fprintf(newDictFile, "%s\n", writNode->word);
            writNode = writNode->next;
        }
    }
    fclose(newDictFile);
    return 0;
}









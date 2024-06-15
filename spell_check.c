#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "dictionary.h"

#define MAX_CMD_LEN 128

// A helper function to spell check a specific file
// 'file_name': Name of the file to spell check
// 'dict': A dictionary containing correct words
int spell_check_file(const char *file_name, const dictionary_t *dict) { // Checks if the word within the text file is correct (word is in the dictionary).
    FILE *checkFile = fopen(file_name, "r");
    if (checkFile == NULL) {
        printf("Spell check failed\n");
        return 1;
    }
     if (dict == NULL || dict->table == NULL || dict->table->array == NULL) {
        fclose(checkFile);
        return 1;
    }
    int notSpelledCorrectly = 0;
    char *wordToAdd;
    char numLines[MAX_WORD_LEN];
    while (fgets(numLines, sizeof(numLines), checkFile) != NULL) {
            wordToAdd = strtok(numLines, " \n");
            while (wordToAdd != NULL) {
                if (dict_find(dict, wordToAdd)) {
                    printf("%s ", wordToAdd);
                } else {
                    printf("%s[X] ", wordToAdd);
                    notSpelledCorrectly = 1;
                }
                wordToAdd = strtok(NULL, " \n");
            }
            printf("\n");
    }
    fclose(checkFile);
    return notSpelledCorrectly;
}

/*
 * This is in general *very* similar to the list_main file seen in lab
 */
int main(int argc, char **argv) { // Determines number of arguments so that it outputs either a dictionary, dictionary or txt file, or Spell Check System. 
    char cmd[MAX_CMD_LEN];
    dictionary_t *dict = create_dictionary();
    if (argc == 2) {
        dictionary_t* success = read_dict_from_text_file(argv[1]);
        if (success) {
            printf("Dictionary successfully read from text file\n");
            dict_free(dict);
            dict = success;
        } else {
            printf("Failed to read dictionary from text file\n");
            dict_free(dict);
            return 0;
        }
    }
    if (argc == 3) {
        dictionary_t* success = read_dict_from_text_file(argv[1]);
        if (success) {
            printf("Dictionary successfully read from text file\n");
        } else {
            printf("Failed to read dictionary from text file\n");
            dict_free(dict);
            return 0;
        }
        if (!spell_check_file(argv[2], success)) {
            printf("Spell check failed");
            dict_free(success);
            dict_free(dict);
            return 0;
        } else {
            dict_free(success);
            dict_free(dict);
            return 1;
        }
    }
       

    printf("CSCI 2021 Spell Check System\n");
    printf("Commands:\n");
    printf("  add <word>:              adds a new word to dictionary\n");
    printf("  lookup <word>:           searches for a word\n");
    printf("  print:                   shows all words currently in the dictionary\n");
    printf("  load <file_name>:        reads in dictionary from a file\n");
    printf("  save <file_name>:        writes dictionary to a file\n");
    printf("  check <file_name>: spell checks the specified file\n");
    printf("  exit:                    exits the program\n");

    while (1) {
        printf("spell_check> ");
        if (scanf("%s", cmd) == EOF) {
            printf("\n");
            break;
        }

        if (strcmp("exit", cmd) == 0) {
            break;
        }
        else if (strcmp("print", cmd) == 0) {
            if (dict != NULL && dict->table != NULL && dict->table->array != NULL) {
                dict_print(dict);
            } 
        }
        else if (strcmp("add", cmd) == 0) {
            scanf("%s", cmd);
            if (dict != NULL) {
                dict_insert(dict, cmd);
            }
        }
        else if (strcmp("lookup", cmd) == 0) {
            scanf("%s", cmd);
            int successfulLookup = dict_find(dict, cmd);
            if (successfulLookup == 1) {
                printf ("'%s' present in dictionary\n", cmd);
            } else {
                printf("'%s' not found\n", cmd);
            }
        }
        else if (strcmp("load", cmd) == 0) {
            scanf("%s", cmd);
            if (cmd != NULL) {
                dictionary_t* success = read_dict_from_text_file(cmd);
                if (success) {
                    printf("Dictionary successfully read from text file\n");
                    dict_free(dict);
                    dict = success;
                } else {
                    printf("Failed to read dictionary from text file\n");
                }
            }
            else {
                printf("FILE NOT FOUND\n");
            }
        }
        else if (strcmp("save", cmd) == 0) {
            scanf("%s", cmd);
            if (cmd != NULL) {
                int success = write_dict_to_text_file(dict, cmd);
                if (success == 0) {
                    printf("Dictionary successfully written to text file\n");
                }
                else {
                    printf("NOT FOUND");
                    printf("%d\n", success);
                }
            }
        }
        else if (strcmp("check", cmd) == 0) {
            scanf("%s", cmd);
            if (cmd != NULL) {
                if (dict != NULL) {
                    spell_check_file(cmd, dict);
                } else {
                    printf("Tried to check from an Empty Dictonary");
                }
            } else {
                printf("FILE NOT FOUND");
            }
        }


        // TODO Add cases for the other commands
        // Read in the command and any additional arguments (where needed)

        else {
            printf("Unknown command %s\n", cmd);
        }
    }

    dict_free(dict);
    return 0;
}


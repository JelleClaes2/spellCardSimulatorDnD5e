#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_LIST_SIZE (100)

enum bool{false,true};

struct anyKey{
    char* name;
    char* value;
};

enum components{V,S,M};

enum type{sphere,cone,cylinder,line,cube};

struct area_of_effect{
    int size;
    enum type type;
};

struct damage_type {
    char *index;
    char *name;
    char *url;
};

enum damage_group{damage_at_character_level,damage_at_slot_level};

struct damage_struct {
    struct annyKey *keyValues;
    struct damage_type damageType;
};

union damage {
    struct damage_struct damage_at_character_level;
    struct damage_struct damage_at_slot_level;
};

struct school{
    char* index;
    char* name;
    char* url;
};

struct classes{
    char* index;
    char* name;
    char* url;
};

struct subclasses{
    char* index;
    char* name;
    char* url;
};

struct spell{
    char* index;
    char* name;
    char* url;
    char* desc;
    char* higher_level;
    char* range;
    enum components components[MAX_LIST_SIZE];
    char* material;
    struct area_of_effect area_of_effect;
    enum bool ritual;
    char* duration;
    enum bool concentration;
    char* casting_time;
    int level;
    char* attack_type;
    enum damage_group damage_group;
    union damage damage;
    struct school school;
    struct classes classes[MAX_LIST_SIZE];
    struct subclasses subclasses[MAX_LIST_SIZE];
};

int main(int argc , char* argv[]){

    struct spell blur;

    FILE *filePointer = fopen("spells/blur.json","r");//Open blur.json TODO NEED TO CHECK ARG TO MAIN
    if (filePointer == NULL) // Check if file is succesfully opened
    {
        perror("File opening failed"); // Print error
        return -1;                     // Stop program
    }
    char buffer[1024];    // Prepare a line buffer
    char *parsing = NULL; // Prepare helper pointer for strsep
    int counter = 0;      // Prepare helper counter for printing
    char check[6] = "count";
    while (!feof(filePointer)) // Keep reading file till EndOfFile is reached
    {
        if (fgets(buffer, sizeof(buffer), filePointer) == NULL) // Read one line (stops on newline or eof), will return NULL on eof or fail
        {
            fclose(filePointer);
            break; // Stop reading
        }

        parsing = buffer; // Point to buffer (reset)
        char *token = strtok(&parsing, "\"");
        while (token) // If token exists
        {

            token = strtok(&parsing, "\""); // Find next token
            //printf("%s\n", buffer);
            if (token != NULL) {
                if (strcmp(token, "index") == 0) {
                    char *index = strtok(&parsing[2], ",");
                    blur.index = index;
                    printf("index=%s",blur.index);
                }
            }

        }
    }



}
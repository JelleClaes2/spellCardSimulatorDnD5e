#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LIST_SIZE (100)

char *strsep(char **stringp, const char *delim) {
    char *rv = *stringp;
    if (rv) {
        *stringp += strcspn(*stringp, delim);
        if (**stringp)
            *(*stringp)++ = '\0';
        else
            *stringp = 0; }
    return rv;
}

enum bool{false,true};

struct anyKey{
    uint8_t level;
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


struct damage {
    enum damage_group damage_group;
    struct anyKey *keyValues;
};

struct resource{
    char* index;
    char* name;
    char* url;
};

struct spell{
    char* index;
    char* name;
    char* url;
    char** desc;
    char** higher_level;
    char* range;
    uint8_t componentsCounter;
    enum components* components;
    char* material;
    struct area_of_effect area_of_effect;
    enum bool ritual;
    char* duration;
    enum bool concentration;
    char* casting_time;
    int level;
    char* attack_type;
    struct damage damage;
    struct damage_type damage_type;
    struct resource school;
    uint8_t classesCounter;
    struct resource* classes;
    uint8_t subclassesCounter;
    struct resource* subclasses;
    //TODO ADD DC SEE JSON FILES
};

void jsonParser(FILE* filePointer, struct spell* spell);
uint8_t parseArray (FILE* filePointer, char*** element);
uint8_t parseEnumComponentsArray (FILE* filePointer, enum components** element);
void parseNameUrlIndex (FILE* filePointer, char** name, char** url, char** index);
uint8_t parseAnyKeyArray (FILE* filePointer, struct anyKey** anyKey);

int main(int argc , char* argv[]){

    uint8_t characterLevel = 0;
    char* nameHistoryFile;
    uint8_t amountOfSpellSlots[9]; //highest slot level == 9 and never higher than 9 slots of a certain spell
    char* fileName;


    for(int i=1;i<argc;i++){
        if(argv[i][0] == '-'){
            printf("argv[%d] = %s \n",i,argv[i]);
            if(argv[i][1] == 's'){
                i++;
                for(int j=0;j<9;j++){
                    printf("argv[%d] = %s \n",i,argv[i]);
                    if(isdigit(*argv[i]) != 0){
                        amountOfSpellSlots[j] = atoi(argv[i]);
                        i++;
                        printf("spell slots [%d] = %d\n",j,amountOfSpellSlots[j]);
                    }else{
                        break;
                    }
                }
            } if(argv[i][1] == 'l'){
                characterLevel = atoi(argv[i+1]);
                i++;
                printf("character level = %d\n",characterLevel);
            } else if(argv[i][1] == 'h'){
                nameHistoryFile = argv[i+1];
                i++;
                printf("name history file = %s",nameHistoryFile);
            }
        } else{
            struct spell* currentSpel = (struct spell*)calloc(1,sizeof(struct spell));
            strcpy(fileName,"spells/");
            strcat(fileName,argv[i]);
            FILE *filePointerSpell = fopen(fileName,"r");
            if (filePointerSpell == NULL) // Check if file is succesfully opened
            {
                perror("File opening failed"); // Print error
                //return -1;                     // Stop program
            }
            jsonParser(filePointerSpell, currentSpel);
            fclose(filePointerSpell);
            printf("file location = %s\n",fileName);
        }
    }

    FILE* filePointerHistory = fopen(nameHistoryFile,"w");
    if(filePointerHistory == NULL){
        perror("File opening failed");
        return -1;
    }

    fclose(filePointerHistory);
    return 0;
}

uint8_t parseArray (FILE* filePointer, char*** element) {
    char buffer[1024];    // Prepare a line buffer
    char *parsing = NULL; // Prepare helper pointer for strsep
    char *string;
    char *token;

    (*element) = (char **) calloc(1,sizeof(char**)); // only allocate 1 char pointer place
    fgets(buffer, sizeof(buffer), filePointer);
    parsing=buffer;
    uint8_t count=0;

    while (strchr(buffer,']')== NULL) {
        count++;
        strsep(&parsing, "\"");
        token = strsep(&parsing, "\"");

        string = (char *) calloc(strlen(token) + 1, sizeof(char));
        strcpy(string, token);
        if (count != 1) {
            (*element) = (char **) realloc(*element, count * sizeof(char **)); // add one more entry
        }
        (*element)[count-1] = string;
        fgets(buffer, sizeof(buffer), filePointer);
        parsing = buffer;
    }
    return count;
}

uint8_t parseEnumComponentsArray (FILE* filePointer, enum components** element) {
    char buffer[1024];    // Prepare a line buffer
    char *parsing = NULL; // Prepare helper pointer for strsep
    char *token;

    (*element) = (enum components *) calloc(1,sizeof(enum components*)); // only allocate 1 char pointer place
    fgets(buffer, sizeof(buffer), filePointer);
    parsing=buffer;
    uint8_t count=0;

    while (strchr(buffer,']')== NULL) {
        count++;
        strsep(&parsing, "\"");
        token = strsep(&parsing, "\"");
        printf("comp token %s\n",token);

        if (count != 1) {
            (*element) = (enum components *) realloc(*element, count * sizeof(enum components *)); // add one more entry
        }
        if(strcmp(token, "V")==0){
            (*element)[count-1] = V;
        }else if(strcmp(token, "S")==0){
            (*element)[count-1] = S;
        }else if(strcmp(token, "M")==0){
            (*element)[count-1] = M;
        }

        fgets(buffer, sizeof(buffer), filePointer);
        parsing = buffer;
    }
    return count;
}

void parseNameUrlIndex (FILE* filePointer, char** name, char** url, char** index) {
    char buffer[1024];    // Prepare a line buffer
    char *parsing = NULL; // Prepare helper pointer for strsep
    char *token;

    for (int i=0;i<3;i++) {
        fgets(buffer, sizeof(buffer), filePointer);
        parsing = buffer;
        strsep(&parsing, "\"");
        token = strsep(&parsing, "\"");
        if (token != NULL) {
            if (strcmp(token, "index") == 0) {
                parsing += 3;
                token = strsep(&parsing, "\"");
                *index = (char *) calloc(strlen(token) + 1, sizeof(char));
                strcpy(*index, token);
            } else if (strcmp("name", token) == 0) {
                parsing += 3;
                token = strsep(&parsing, "\"");
                *name = (char *) calloc(strlen(token) + 1, sizeof(char));
                strcpy(*name, token);
            } else if (strcmp("url", token) == 0) {
                parsing += 3;
                token = strsep(&parsing, "\"");
                *url = (char *) calloc(strlen(token) + 1, sizeof(char));
                strcpy(*url, token);
            }
        }
    }
}

uint8_t parseAnyKeyArray (FILE* filePointer, struct anyKey** anyKey) {
    char buffer[1024];    // Prepare a line buffer
    char *parsing = NULL; // Prepare helper pointer for strsep
    char *token;

    (*anyKey) = ( struct anyKey*) calloc(1,sizeof(struct anyKey)); // only allocate 1 char pointer place
    fgets(buffer, sizeof(buffer), filePointer);
    parsing=buffer;
    uint8_t count=0;

    while (strchr(buffer,'}')== NULL) {
        count++;
        if (count != 1) {
            (*anyKey) = (struct anyKey*) realloc(*anyKey, count * sizeof(struct anyKey)); // add one more entry
        }
        strsep(&parsing, "\"");
        token = strsep(&parsing, "\"");
        (*anyKey)[count-1].level = atoi(token);

        strsep(&parsing, "\"");
        token = strsep(&parsing, "\"");
        (*anyKey)[count-1].value = (char *) calloc(strlen(token) + 1, sizeof(char));
        strcpy((*anyKey)[count-1].value, token);

        fgets(buffer, sizeof(buffer), filePointer);
        parsing = buffer;
    }
    return count;
}

void jsonParser(FILE* filePointer, struct spell* spell){
    enum damage_group damageGroup;

    char *string;

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
        char *token = strsep(&parsing, "\"");
        while (token) // If token exists
        {

            token = strsep(&parsing, "\""); // Find next token
            //printf("buffer = %s\n", buffer);
            if (token != NULL) {
                printf("token = '%s'\n",token);
                if (strcmp(token, "index") == 0) {
                    parsing+=3;
                    token = strsep(&parsing, "\"");
                    string = (char *)calloc(strlen(token)+1, sizeof(char));
                    strcpy(string,token);
                    spell->index = string;
                    printf("index = %s\n",spell->index);
                } else if(strcmp("name",token)==0){
                    parsing+=3;
                    token = strsep(&parsing, "\"");
                    string = (char *)calloc(strlen(token)+1, sizeof(char));
                    strcpy(string,token);
                    spell->name = string;
                    printf("name =%s\n",spell->name);
                }else if(strcmp("url",token)==0) {
                    parsing += 3;
                    token = strsep(&parsing, "\"");
                    string = (char *) calloc(strlen(token) + 1, sizeof(char));
                    strcpy(string,token);
                    spell->url = string;
                    printf("url = %s\n", spell->url);
                } else if (strcmp("desc",token)==0){
                    uint8_t count=parseArray(filePointer,&(spell->desc));
                    printf("size %d\n", count);
                    for (int i=0; i<count; i++) {
                        printf("desc[%d] =%s\n",i, spell->desc[i]);
                    }
                } else if(strcmp("higher_level",token)==0){
                    uint8_t count=parseArray(filePointer,&(spell->higher_level));
                    printf("size %d\n", count);
                    for (int i=0; i<count; i++) {
                        printf("higher lvl[%d] =%s\n",i, spell->higher_level[i]);
                    }
                } else if(strcmp("range",token)==0){
                    parsing += 3;
                    token = strsep(&parsing, "\"");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    spell->range = string;
                    printf("range =%s\n",spell->range);
                } else if(strcmp("components",token)==0){
                    uint8_t count= parseEnumComponentsArray(filePointer,&(spell->components));
                    printf("size %d\n", count);
                    for (int i=0; i<count; i++) {
                        printf("components[%d] =%d\n",i, spell->components[i]);
                    }
                } else if(strcmp("material",token)==0){
                    parsing += 3;
                    token = strsep(&parsing, "\"");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    spell->material = string;
                    printf("material =%s\n",spell->material);
                } else if(strcmp("area_of_effect",token)==0){
                    for (int i=0;i<2;i++){
                        fgets(buffer, sizeof(buffer), filePointer);
                        parsing = buffer;
                        strsep(&parsing, "\"");
                        token = strsep(&parsing, "\"");
                        if (strcmp(token,"type")==0){
                            parsing += 3;
                            token = strsep(&parsing, "\"");
                            if (strcmp("sphere", token) == 0) {
                                spell->area_of_effect.type = sphere;
                            } else if (strcmp("cone", token) == 0) {
                                spell->area_of_effect.type = cone;
                            } else if (strcmp("cylinder", token) == 0) {
                                spell->area_of_effect.type = cylinder;
                            } else if (strcmp("line", token) == 0) {
                                spell->area_of_effect.type = line;
                            } else if (strcmp("cube", token) == 0) {
                                spell->area_of_effect.type = cube;
                            }
                            printf("area_of_effect_type =%d\n",spell->area_of_effect.type);
                        } else if(strcmp(token , "size")==0){
                            parsing += 2;
                            token = strsep(&parsing, "\n");
                            spell->area_of_effect.size = atoi(token);
                            printf("area_of_effect_size = %d\n",spell->area_of_effect.size);
                        }
                    }

                }

                   /* while (strcmp("}",token)!=0) {
                        token = strsep(&parsing, "\"");
                        if (strcmp("type", token) == 0) {
                            parsing += 3;
                            token = strsep(&parsing, ",");
                            string = (char *) calloc(strlen(token) + 1, sizeof(char));
                            strcpy(string, token);
                            if (strcmp("sphere", token) == 0) {
                                spell->area_of_effect.type = sphere;
                            } else if (strcmp("cone", token) == 0) {
                                spell->area_of_effect.type = cone;
                            } else if (strcmp("cylinder", token) == 0) {
                                spell->area_of_effect.type = cylinder;
                            } else if (strcmp("line", token) == 0) {
                                spell->area_of_effect.type = line;
                            } else if (strcmp("cube", token) == 0) {
                                spell->area_of_effect.type = cube;
                            }
                            printf("area_of_effect_type =%d\n",spell->area_of_effect.type);
                        } else if (strcmp("size", token) == 0) {
                            spell->area_of_effect.size = atoi(string);
                            printf("area_of_effect_size = %d\n",spell->area_of_effect.size);
                        }
                    }
                } */else if(strcmp("ritual",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    printf ("token %s\n",token);
                    if(strcmp("true",token)==0){
                        spell->ritual = true;
                    } else if(strcmp("false",token)==0){
                        spell->ritual = false;
                    }
                    printf("ritual =%d\n",spell->ritual);
                } else if(strcmp("duration",token)==0){
                    parsing += 3;
                    token = strsep(&parsing, "\"");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    spell->duration = string;
                    printf("duration =%s\n",spell->duration);
                } else if(strcmp("concentration",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    if(strcmp("true",token)==0){
                        spell->concentration = true;
                    } else if(strcmp("false",token)==0){
                        spell->concentration = false;
                    }
                    printf("concentration =%d\n",spell->concentration);
                } else if(strcmp("casting_time",token)==0){
                    parsing += 3;
                    token = strsep(&parsing, "\"");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    spell->casting_time = string;
                    printf("casting_time =%s\n",spell->casting_time);
                } else if(strcmp("level",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    spell->level = atoi(token);
                    printf("level =%d\n",spell->level);
                } else if(strcmp("attack_type",token)==0){
                    parsing += 3;
                    token = strsep(&parsing, "\"");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    spell->attack_type = string;
                    printf("attack_type =%s\n",spell->attack_type);
                } else if (strcmp("damage",token)==0){

                    fgets(buffer, sizeof(buffer), filePointer);
                    while(strchr(buffer,'}')==NULL){
                        parsing = buffer; // Point to buffer (reset)
                        strsep(&parsing, "\"");
                        token = strsep(&parsing, "\"");
                        if (strcmp("damage_at_character_level",token)==0){
                            damageGroup = damage_at_character_level;
                            uint8_t count=parseAnyKeyArray(filePointer,&(spell->damage.keyValues));
                            printf("size %d\n", count);
                            for (int i=0; i<count; i++) {
                                printf("key val [%d] =%d , %s\n",i, spell->damage.keyValues[i].level,spell->damage.keyValues[i].value);
                            }
                            fgets(buffer, sizeof(buffer), filePointer);
                        } else if(strcmp("damage_at_slot_level",token)==0) {
                            damageGroup = damage_at_slot_level;
                            uint8_t count=parseAnyKeyArray(filePointer,&(spell->damage.keyValues));
                            printf("size %d\n", count);
                            for (int i=0; i<count; i++) {
                                printf("key val [%d] =%d , %s\n",i, spell->damage.keyValues[i].level,spell->damage.keyValues[i].value);
                            }
                            fgets(buffer, sizeof(buffer), filePointer);
                        } else if(strcmp("damage_type",token)==0) {
                            parseNameUrlIndex(filePointer,&(spell->damage_type.name), &(spell->damage_type.url), &(spell->damage_type.index));
                            printf("damage type name =%s\n",spell->damage_type.name);
                            printf("damage type url =%s\n",spell->damage_type.url);
                            printf("damage type index =%s\n",spell->damage_type.index);
                            fgets(buffer, sizeof(buffer), filePointer);
                            fgets(buffer, sizeof(buffer), filePointer);
                        }
                    }

                } else if(strcmp("school",token)==0){
                    parseNameUrlIndex(filePointer,&(spell->school.name), &(spell->school.url), &(spell->school.index));
                    printf("school name =%s\n",spell->school.name);
                    printf("school url =%s\n",spell->school.url);
                    printf("school index =%s\n",spell->school.index);
                }/* else if(strcmp("classes",token)==0){ //TODO DOESNT WORK
                    uint8_t classesCounter = 0;
                    while (1){
                        token = strsep(&parsing, "\"");
                        if(strcmp("]",token)!=0)//find
                        {
                            break;
                        }
                        char* key_token = strsep(&parsing, "\"");
                        parsing += 2;
                        token = strsep(&parsing, "\"");
                        //token = strsep(&parsing, ",");
                        string = (char *) calloc(strlen(token)+1,sizeof(char));
                        strcpy(string,token);
                        if(strcmp("index",key_token)==0){
                            strcpy(string,token);
                            spell.classes->index = string;
                            printf("classes index =%s\n",spell.classes->index);
                        } else if(strcmp("name",key_token)==0){
                            spell.classes->name = string;
                            printf("classes name =%s\n",spell.classes->name);
                        } else if(strcmp("url",key_token)==0){
                            spell.classes->url = string;
                            printf("classes url =%s\n",spell.classes->url);
                        }
                    }
                } *//*else if(strcmp("subclasses",token)==0){ //TODO DOESNT WORK
                    while (strcmp("]",token)!=0){
                        token = strsep(&parsing, "\"");
                        parsing += 2;
                        token = strsep(&parsing, ",");
                        string = (char *) calloc(strlen(token)+1,sizeof(char));
                        strcpy(string,token);
                        if(strcmp("index",token)==0){
                            spell.subclasses->index = string;
                            printf("subclasses index =%s\n",spell.subclasses->index);
                        } else if(strcmp("name",token)==0){
                            spell.subclasses->name = string;
                            printf("subclasses name =%s\n",spell.subclasses->name);
                        } else if(strcmp("url",token)==0){
                            spell.subclasses->url = string;
                            printf("subclasses url =%s\n",spell.subclasses->url);
                        }
                    }*/
                }
            }
        }
    }

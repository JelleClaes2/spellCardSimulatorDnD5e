#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

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

struct resource{
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
    uint8_t componentsCounter;
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
    struct resource school;
    uint8_t classesCounter;
    struct resource classes[MAX_LIST_SIZE];
    uint8_t subclassesCounter;
    struct resource subclasses[MAX_LIST_SIZE];
};

struct spell jsonParser(FILE* filePointer);

int main(int argc , char* argv[]){

    struct spell sacred_flame;

    FILE *filePointer = fopen("spells/sacred-flame.json","r");//Open blur.json TODO NEED TO CHECK ARG TO MAIN
    if (filePointer == NULL) // Check if file is succesfully opened
    {
        perror("File opening failed"); // Print error
        return -1;                     // Stop program
    }
    sacred_flame = jsonParser(filePointer);

    return 0;
}

struct spell jsonParser(FILE* filePointer){
    struct spell spell;
    enum damage_group damageGroup;

    char *string;

    uint8_t indexCount = 0;

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
                //TODO DELETE INDEX COUNTER
                //printf("token = '%s'\n",token);
                if (strcmp(token, "index") == 0) {
                    parsing+=2;
                    token = strsep(&parsing, ",");
                    string = (char *)calloc(strlen(token)+1, sizeof(char));
                    strcpy(string,token);
                    if(indexCount == 0){
                        spell.index = string;
                        printf("index =%s\n",spell.index);
                        indexCount++;
                    } /*else if(indexCount == 1){
                        if(damageGroup == damage_at_character_level){
                            string = spell.damage.damage_at_character_level.damageType.index;
                            printf("character damage index = %s\n",spell.damage.damage_at_character_level.damageType.index);
                        } else if(damageGroup == damage_at_slot_level){
                            string = spell.damage.damage_at_slot_level.damageType.index;
                            printf("slot damage index = %s\n",spell.damage.damage_at_slot_level.damageType.index);
                        }
                        indexCount ++;
                    }*/

                } else if(strcmp("name",token)==0){
                    parsing+=2;
                    token = strsep(&parsing, ",");
                    string = (char *)calloc(strlen(token)+1, sizeof(char));
                    strcpy(string,token);
                    spell.name = string;
                    printf("name =%s\n",spell.name);
                }else if(strcmp("url",token)==0) {
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token) + 1, sizeof(char));
                    strcpy(string,token);
                    spell.url = string;
                    printf("url = %s\n", spell.url);
                } /*else if (strcmp("desc",token)==0){ //TODO DOESNT WORK
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    spell.desc = string;
                    printf("desc =%s\n",spell.desc);
                } */else if(strcmp("higher_level",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    spell.higher_level = string;
                    printf("Higher level =%s\n",spell.higher_level);
                } else if(strcmp("range",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    spell.range = string;
                    printf("range =%s\n",spell.range);
                } /*else if(strcmp("components",token)==0){ //TODO DOESNT WORK
                    int i = 0;
                    while (strcmp("]",token)!=0){
                        token = strsep(&parsing, "\"");
                        parsing += 2;
                        token = strsep(&parsing, ",");
                        string = (char *) calloc(strlen(token)+1,sizeof(char));
                        strcpy(string,token);
                        if(strcmp("V",token)==0){
                            spell.components[i] = V;
                        }else if(strcmp("S",token)==0){
                            spell.components[i] = S;
                        }else if(strcmp("M",token)==0){
                            spell.components[i] = M;
                        }
                        i++;
                    }
                    for ( int j=0;j<i;j++){
                        printf("component[%d] = %s",j,spell.components);
                    }
                }*/ else if(strcmp("material",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    spell.material = string;
                    printf("material =%s\n",spell.material);
                } else if(strcmp("area_of_effect",token)==0){
                    while (strcmp("}",token)!=0) {
                        token = strsep(&parsing, "\"");
                        if (strcmp("type", token) == 0) {
                            parsing += 2;
                            token = strsep(&parsing, ",");
                            string = (char *) calloc(strlen(token) + 1, sizeof(char));
                            strcpy(string, token);
                            if (strcmp("sphere", token) == 0) {
                                spell.area_of_effect.type = sphere;
                            } else if (strcmp("cone", token) == 0) {
                                spell.area_of_effect.type = cone;
                            } else if (strcmp("cylinder", token) == 0) {
                                spell.area_of_effect.type = cylinder;
                            } else if (strcmp("line", token) == 0) {
                                spell.area_of_effect.type = line;
                            } else if (strcmp("cube", token) == 0) {
                                spell.area_of_effect.type = cube;
                            }
                            printf("area_of_effect_type =%s\n",spell.area_of_effect.type);
                        } else if (strcmp("size", token) == 0) {
                            spell.area_of_effect.size = atoi(string);
                            printf("area_of_effect_size = %d\n",spell.area_of_effect.size);
                        }
                    }
                } else if(strcmp("ritual",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    if(strcmp("true",token)==0){
                        spell.ritual = true;
                    } else if(strcmp("false",token)==0){
                        spell.ritual = false;
                    }
                    printf("ritual =%s\n",spell.ritual);
                } else if(strcmp("duration",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    spell.duration = string;
                    printf("duration =%s\n",spell.duration);
                } else if(strcmp("concentration",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    if(strcmp("true",token)==0){
                        spell.concentration = true;
                    } else if(strcmp("false",token)==0){
                        spell.concentration = false;
                    }
                    printf("concentration =%s\n",spell.concentration);
                } else if(strcmp("casting_time",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    spell.casting_time = string;
                    printf("casting_time =%s\n",spell.casting_time);
                } else if(strcmp("level",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    spell.level = atoi(string);
                    printf("level =%d\n",spell.level);
                } else if(strcmp("attack_type",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    spell.attack_type = string;
                    printf("attack_type =%s\n",spell.attack_type);
                } else if (strcmp("damage_at_character_level",token)==0){//TODO FILL IN THIS PART OF STRUCT
                    damageGroup = damage_at_character_level;
                } else if(strcmp("damage_at_slot_level",token)==0){
                    damageGroup = damage_at_slot_level;
                } /*else if(strcmp("school",token)==0){//TODO DOESNT WORK
                    while (strcmp("}",token)!=0) {
                        token = strsep(&parsing, "\"");
                        parsing += 2;
                        token = strsep(&parsing, ",");
                        string = (char *) calloc(strlen(token)+1,sizeof(char));
                        strcpy(string,token);
                        if(strcmp("index",token)==0){
                            spell.school.index = string;
                            printf("school index =%s\n",spell.school.index);
                        } else if(strcmp("name",token)==0){
                            spell.school.name = string;
                            printf("school name =%s\n",spell.school.name);
                        } else if(strcmp("url",token)==0){
                            spell.school.url = string;
                            printf("school url =%s\n",spell.school.url);
                        }
                    }
                }*//* else if(strcmp("classes",token)==0){ //TODO DOESNT WORK
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
    return spell;
    }

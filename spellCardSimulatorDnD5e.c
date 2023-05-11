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
    struct resource classes[MAX_LIST_SIZE];
    struct resource subclasses[MAX_LIST_SIZE];
};

int main(int argc , char* argv[]){

    struct spell sarced_flame;
    enum damage_group damageGroup;

    char *string;

    uint8_t indexCount = 0;

    FILE *filePointer = fopen("spells/sacred_flame_test.json","r");//Open blur.json TODO NEED TO CHECK ARG TO MAIN
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
        char *token = strsep(&parsing, "\"");
        while (token) // If token exists
        {

            token = strsep(&parsing, "\""); // Find next token
            //printf("buffer = %s\n", buffer);
                if (token != NULL) {

                //printf("token = '%s'\n",token);
                if (strcmp(token, "index") == 0) {
                    parsing+=2;
                    token = strsep(&parsing, ",");
                    string = (char *)calloc(strlen(token)+1, sizeof(char));
                    strcpy(string,token);
                    if(indexCount == 0){
                        sarced_flame.index = string;
                        printf("index =%s\n",sarced_flame.index);
                        indexCount++;
                    } /*else if(indexCount == 1){
                        if(damageGroup == damage_at_character_level){
                            string = sarced_flame.damage.damage_at_character_level.damageType.index;
                            printf("character damage index = %s\n",sarced_flame.damage.damage_at_character_level.damageType.index);
                        } else if(damageGroup == damage_at_slot_level){
                            string = sarced_flame.damage.damage_at_slot_level.damageType.index;
                            printf("slot damage index = %s\n",sarced_flame.damage.damage_at_slot_level.damageType.index);
                        }
                        indexCount ++;
                    }*/

                } else if(strcmp("name",token)==0){
                    parsing+=2;
                    token = strsep(&parsing, ",");
                    string = (char *)calloc(strlen(token)+1, sizeof(char));
                    strcpy(string,token);
                    sarced_flame.name = string;
                    printf("name =%s\n",sarced_flame.name);
                }else if(strcmp("url",token)==0) {
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token) + 1, sizeof(char));
                    strcpy(string,token);
                    sarced_flame.url = string;
                    printf("url = %s\n", sarced_flame.url);
                } else if (strcmp("desc",token)==0){ //TODO DOESNT WORK
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    sarced_flame.desc = string;
                    printf("desc =%s\n",sarced_flame.desc);
                } else if(strcmp("higher_level",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    sarced_flame.higher_level = string;
                    printf("Higer level =%s\n",sarced_flame.higher_level);
                } else if(strcmp("range",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    sarced_flame.range = string;
                    printf("range =%s\n",sarced_flame.range);
                } else if(strcmp("components",token)==0){ //TODO DOESNT WORK
                    int i = 0;
                    while (strcmp("]",token)!=0){
                        token = strsep(&parsing, "\"");
                        parsing += 2;
                        token = strsep(&parsing, ",");
                        string = (char *) calloc(strlen(token)+1,sizeof(char));
                        strcpy(string,token);
                        if(strcmp("V",token)==0){
                            sarced_flame.components[i] = V;
                        }else if(strcmp("S",token)==0){
                            sarced_flame.components[i] = S;
                        }else if(strcmp("M",token)==0){
                            sarced_flame.components[i] = M;
                        }
                        i++;
                    }
                    for ( int j=0;j<i;j++){
                        printf("component[%d] = %s",j,sarced_flame.components);
                    }
                } else if(strcmp("material",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    sarced_flame.material = string;
                    printf("material =%s\n",sarced_flame.material);
                } else if(strcmp("area_of_effect",token)==0){
                    while (strcmp("}",token)!=0) {
                        token = strsep(&parsing, "\"");
                        if (strcmp("type", token) == 0) {
                            parsing += 2;
                            token = strsep(&parsing, ",");
                            string = (char *) calloc(strlen(token) + 1, sizeof(char));
                            strcpy(string, token);
                            if (strcmp("sphere", token) == 0) {
                                sarced_flame.area_of_effect.type = sphere;
                            } else if (strcmp("cone", token) == 0) {
                                sarced_flame.area_of_effect.type = cone;
                            } else if (strcmp("cylinder", token) == 0) {
                                sarced_flame.area_of_effect.type = cylinder;
                            } else if (strcmp("line", token) == 0) {
                                sarced_flame.area_of_effect.type = line;
                            } else if (strcmp("cube", token) == 0) {
                                sarced_flame.area_of_effect.type = cube;
                            }
                            printf("area_of_effect_type =%s\n",sarced_flame.area_of_effect.type);
                        } else if (strcmp("size", token) == 0) {
                                sarced_flame.area_of_effect.size = atoi(string);
                            printf("area_of_effect_size = %d\n",sarced_flame.area_of_effect.size);
                        }
                    }
                } else if(strcmp("ritual",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    if(strcmp("true",token)==0){
                        sarced_flame.ritual = true;
                    } else if(strcmp("false",token)==0){
                        sarced_flame.ritual = false;
                    }
                    printf("ritual =%s\n",sarced_flame.ritual);
                } else if(strcmp("duration",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    sarced_flame.duration = string;
                    printf("duration =%s\n",sarced_flame.duration);
                } else if(strcmp("concentration",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    if(strcmp("true",token)==0){
                        sarced_flame.concentration = true;
                    } else if(strcmp("false",token)==0){
                        sarced_flame.concentration = false;
                    }
                    printf("concentration =%s\n",sarced_flame.concentration);
                } else if(strcmp("casting_time",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    sarced_flame.casting_time = string;
                    printf("casting_time =%s\n",sarced_flame.casting_time);
                } else if(strcmp("level",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    sarced_flame.level = atoi(string);
                    printf("level =%d\n",sarced_flame.level);
                } else if(strcmp("attack_type",token)==0){
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    string = (char *) calloc(strlen(token)+1,sizeof(char));
                    strcpy(string,token);
                    sarced_flame.attack_type = string;
                    printf("attack_type =%s\n",sarced_flame.attack_type);
                } else if (strcmp("damage_at_character_level",token)==0){//TODO FILL IN THIS PART OF STRUCT
                    damageGroup = damage_at_character_level;
                } else if(strcmp("damage_at_slot_level",token)==0){
                    damageGroup = damage_at_slot_level;
                } else if(strcmp("school",token)==0){//TODO DOESNT WORK
                    while (strcmp("}",token)!=0) {
                        token = strsep(&parsing, "\"");
                        parsing += 2;
                        token = strsep(&parsing, ",");
                        string = (char *) calloc(strlen(token)+1,sizeof(char));
                        strcpy(string,token);
                        if(strcmp("index",token)==0){
                            sarced_flame.school.index = string;
                            printf("school index =%s\n",sarced_flame.school.index);
                        } else if(strcmp("name",token)==0){
                            sarced_flame.school.name = string;
                            printf("school name =%s\n",sarced_flame.school.name);
                        } else if(strcmp("url",token)==0){
                            sarced_flame.school.url = string;
                            printf("school url =%s\n",sarced_flame.school.url);
                        }
                    }
                } else if(strcmp("classes",token)==0){ //TODO DOESNT WORK
                    while (strcmp("]",token)!=0){
                        token = strsep(&parsing, "\"");
                        parsing += 2;
                        token = strsep(&parsing, ",");
                        string = (char *) calloc(strlen(token)+1,sizeof(char));
                        strcpy(string,token);
                        if(strcmp("index",token)==0){
                            sarced_flame.classes->index = string;
                            printf("classes index =%s\n",sarced_flame.classes->index);
                        } else if(strcmp("name",token)==0){
                            sarced_flame.classes->name = string;
                            printf("classes name =%s\n",sarced_flame.classes->name);
                        } else if(strcmp("url",token)==0){
                            sarced_flame.classes->url = string;
                            printf("classes url =%s\n",sarced_flame.classes->url);
                        }
                    }
                }else if(strcmp("subclasses",token)==0){ //TODO DOESNT WORK
                    while (strcmp("]",token)!=0){
                        token = strsep(&parsing, "\"");
                        parsing += 2;
                        token = strsep(&parsing, ",");
                        string = (char *) calloc(strlen(token)+1,sizeof(char));
                        strcpy(string,token);
                        if(strcmp("index",token)==0){
                            sarced_flame.subclasses->index = string;
                            printf("subclasses index =%s\n",sarced_flame.subclasses->index);
                        } else if(strcmp("name",token)==0){
                            sarced_flame.subclasses->name = string;
                            printf("subclasses name =%s\n",sarced_flame.subclasses->name);
                        } else if(strcmp("url",token)==0){
                            sarced_flame.subclasses->url = string;
                            printf("subclasses url =%s\n",sarced_flame.subclasses->url);
                        }
                    }
                }
            }
        }
    }



}
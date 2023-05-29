#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

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

struct dc{
    struct resource dc_type;
    char* dc_succes;
};

struct spell{
    char* index;
    char* name;
    char* url;
    char** desc;
    uint8_t descCounter;
    char** higher_level;
    uint8_t higher_levelCounter;
    char* range;
    uint8_t componentsCounter;
    enum components* components;
    char* material;
    struct area_of_effect* area_of_effect;
    enum bool ritual;
    char* duration;
    enum bool concentration;
    char* casting_time;
    int level;
    char* attack_type;
    struct damage* damage;
    uint8_t damageKeyValueCounter;
    struct damage_type damage_type;
    struct resource school;
    uint8_t classesCounter;
    struct resource* classes;
    uint8_t subclassesCounter;
    struct resource* subclasses;
    struct dc dc;
};

struct spellNode {
    struct spell* data;
    struct spellNode* next;
};


void jsonParser(FILE* filePointer, struct spell* spell);
uint8_t parseArray (FILE* filePointer, char*** element);
uint8_t parseEnumComponentsArray (FILE* filePointer, enum components** element);
void parseNameUrlIndex (FILE* filePointer, char** name, char** url, char** index);
uint8_t parseAnyKeyArray (FILE* filePointer, struct anyKey** anyKey);
uint8_t parseNameUrlIndexArray(FILE* filePointer, struct resource** classes);
void push(struct spellNode** head, struct spell* spell);
void freeSpellList(struct spellNode* head);
void freeSpell(struct spell* spell);
struct spell* pop(struct spellNode** head);
void cycle(struct spellNode** head);
void printSpell(struct spellNode* head);
void printSpellOption(struct spellNode* head,char* optionstring);


int main(int argc , char* argv[]){

    struct spellNode* spellList = NULL;
    uint8_t characterLevel = 0;
    char* nameHistoryFile;
    uint8_t amountOfSpellSlots[9] = {0}; //highest slot level == 9 and never higher than 9 slots of a certain spell
    char* fileName;

    for(int i=1;i<argc;i++){
        if(argv[i][0] == '-'){
            if(argv[i][1] == 's'){
                i++;
                for(int j=0;j<9;j++){
                    if(isdigit(*argv[i]) != 0){
                        amountOfSpellSlots[j] = atoi(argv[i]);
                        i++;
                    }else{
                        break;
                    }
                }
            } if(argv[i][1] == 'l'){
                characterLevel = atoi(argv[i+1]);
                i++;
            } else if(argv[i][1] == 'h'){
                nameHistoryFile = argv[i+1];
                i++;
            }
        } else{
            struct spell* currentSpel = (struct spell*)calloc(1,sizeof(struct spell));
            char* fileName = calloc(sizeof(argv[i])/sizeof(char)+7,sizeof(char));
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
            free(fileName);
            push(&spellList, currentSpel);// PUSH == add spell POP==delete first AND CYCLE == cycle in deck
        }
    }

    FILE* filePointerHistory = fopen(nameHistoryFile,"w");
    if(filePointerHistory == NULL){
        perror("File opening failed");
        return -1;
    }

    //printSpell(spellList);
    char userInput[100];
    char stringWrite[100];
    uint8_t castingLevel =0;
    char* parsing;

    printf("The following queries are available:\n");
    printf("View spell list: Returns a list of all the available spells\n");
    printf("spell-name  [-at attack_type] [-ct casting_time] [-cl classes] [-co components] \n");
    printf("[-de description] [-du duration] [-r range] ...\n");
    printf("stop : stops the program\n");
    fgets(userInput, sizeof(userInput),stdin);
    userInput[strcspn(userInput, "\n")] = '\0';

    while (strcmp(userInput,"stop")!=0){
        if(strcmp(userInput,"View spell list")==0) {
            printf ("Available spells are :");
            struct spellNode* temp = spellList;
            while (temp != NULL) {
                printf (" (%s)",temp->data->index);
                temp = temp->next;
            }
            printf("\n");
        } else if (strstr(userInput,"cast")!=0) {
            parsing = userInput;
            strsep(&parsing, " "); // remove word cast
            char* spellName = strsep(&parsing, " ");
            struct spellNode *viewSpell = spellList;
            while (viewSpell != NULL && strcmp(viewSpell->data->index, spellName) != 0) {
                viewSpell = viewSpell->next;
            }
            char* option = strsep(&parsing, " ");
            uint8_t level=0;
            if (strcmp (option,"-l") ==0) {
                level = atoi(strsep(&parsing, " "));

                if (viewSpell->data->damage == NULL) {
                    printf("This spell can only be casted at the level defined in the spell\n");
                    level=viewSpell->data->level;
                } else {
                    if((viewSpell->data->damage->damage_group) == 0){ //damage at character level
                        printf("This spell can't be casted as a higher level. This spells damage dependants on character level\n");
                    }
                }


                if (amountOfSpellSlots[level-1]==0){

                    printf ("A %dth level spell slot is not available. Do you want to use a %dth spell slot?\n", level, level-1);
                    printf ("The list of available spell slots are:\n");
                    for (int j=0; j<level; j++) {
                        printf("* %dst level: %d\n",j+1,amountOfSpellSlots[j]);
                    }
                }  else {

                    printf ("Casting succeeded at level %d\n", level);
                    sprintf(stringWrite,"used spell %s at level %d\n",viewSpell->data->name,level);
                    fputs(stringWrite,filePointerHistory);
                    amountOfSpellSlots[level-1]--;
                }
            } else {
                if (viewSpell->data->damage == NULL) {
                    printf("This spell can only be casted at the level defined in the spell\n");
                    level=viewSpell->data->level;
                } else {
                    for (int j=8;j>0;j--) {
                        if (amountOfSpellSlots[j]!=0){
                            level = j+1;
                            break;
                        }
                    }
                }

                printf ("Casting succeeded at level %d\n", level);
                sprintf(stringWrite,"used spell %s at level %d\n",viewSpell->data->name,level);
                fputs(stringWrite,filePointerHistory);
                amountOfSpellSlots[level-1]--;
            }
        } else {
            parsing = userInput;
            char* spellName = strsep(&parsing, " ");
            struct spellNode *viewSpell = spellList;
            while (viewSpell != NULL && strcmp(viewSpell->data->index, spellName) != 0) {
                viewSpell = viewSpell->next;
            }
            printSpellOption(viewSpell,parsing);
        }
        printf("The following queries are available:\n");
        printf("View spell list: Returns a list of all the available spells\n");
        printf("spell-name  [-at attack_type] [-ct casting_time] [-cl classes] [-co components] \n");
        printf("[-de description] [-du duration] [-r range] ...\n");
        printf("stop : stops the program\n");
        fgets(userInput, sizeof(userInput),stdin);
        userInput[strcspn(userInput, "\n")] = '\0';
    }

    fclose(filePointerHistory);
    freeSpellList(spellList);
    return 0;


}

uint8_t parseArray (FILE* filePointer, char*** element) {
    char buffer[1024];    // Prepare a line buffer
    char *parsing = NULL; // Prepare helper pointer for strsep
    char *string;
    char *token;

    (*element) = (char **) calloc(1,sizeof(char*)); // only allocate 1 char pointer place
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
            (*element) = (char **) realloc(*element, count * sizeof(char *)); // add one more entry
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

    (*element) = (enum components *) calloc(1,sizeof(enum components)); // only allocate 1 char pointer place
    fgets(buffer, sizeof(buffer), filePointer);
    parsing=buffer;
    uint8_t count=0;

    while (strchr(buffer,']')== NULL) {
        count++;
        strsep(&parsing, "\"");
        token = strsep(&parsing, "\"");

        if (count != 1) {
            (*element) = (enum components *) realloc(*element, count * sizeof(enum components )); // add one more entry
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

uint8_t parseNameUrlIndexArray(FILE* filePointer, struct resource** classes) {
    char buffer[1024] = " ";    // Prepare a line buffer
    char *parsing = NULL; // Prepare helper pointer for strsep
    char *token;
    uint8_t amountOfStructs = 0;
    fgets(buffer, sizeof(buffer), filePointer);
    (*classes) = ( struct resource*) calloc(1,sizeof(struct resource)); // only allocate 1 char pointer place

    while (strchr(buffer,']')== NULL){

        if (strchr(buffer,'{')!=NULL) {
            amountOfStructs++;
            if (amountOfStructs == 1) {
                (*classes) = (struct resource *) calloc(1, sizeof(struct resource)); // only allocate 1 char pointer place
            } else {
                (*classes) = ( struct resource*) realloc(*classes, amountOfStructs * sizeof(struct resource)); // add one more entry
            }
            parseNameUrlIndex (filePointer,&((*classes)[amountOfStructs-1].name),&((*classes)[amountOfStructs-1].url),&((*classes)[amountOfStructs-1].index));
            fgets(buffer, sizeof(buffer), filePointer);
            fgets(buffer, sizeof(buffer), filePointer);
        }
    }
    return amountOfStructs;
}


void jsonParser(FILE* filePointer, struct spell* spell) {
    enum damage_group damageGroup;

    char *string;

    char buffer[1024];    // Prepare a line buffer
    char *parsing = NULL; // Prepare helper pointer for strsep
    int counter = 0;      // Prepare helper counter for printing
    char check[6] = "count";
    while (!feof(filePointer)) // Keep reading file till EndOfFile is reached
    {
        if (fgets(buffer, sizeof(buffer), filePointer) ==
            NULL) // Read one line (stops on newline or eof), will return NULL on eof or fail
        {
            fclose(filePointer);
            break; // Stop reading
        }

        parsing = buffer; // Point to buffer (reset)
        char *token = strsep(&parsing, "\"");
        while (token) // If token exists
        {

            token = strsep(&parsing, "\""); // Find next token
            if (token != NULL) {
                if (strcmp(token, "index") == 0) {
                    parsing += 3;
                    token = strsep(&parsing, "\"");
                    string = (char *) calloc(strlen(token) + 1, sizeof(char));
                    strcpy(string, token);
                    spell->index = string;
                } else if (strcmp("name", token) == 0) {
                    parsing += 3;
                    token = strsep(&parsing, "\"");
                    string = (char *) calloc(strlen(token) + 1, sizeof(char));
                    strcpy(string, token);
                    spell->name = string;
                } else if (strcmp("url", token) == 0) {
                    parsing += 3;
                    token = strsep(&parsing, "\"");
                    string = (char *) calloc(strlen(token) + 1, sizeof(char));
                    strcpy(string, token);
                    spell->url = string;
                } else if (strcmp("desc", token) == 0) {
                    spell->descCounter = parseArray(filePointer, &(spell->desc));
                } else if (strcmp("higher_level", token) == 0) {
                    if(strchr(parsing,']')==NULL){
                        spell->higher_levelCounter = parseArray(filePointer, &(spell->higher_level));
                    }
                } else if (strcmp("range", token) == 0) {
                    parsing += 3;
                    token = strsep(&parsing, "\"");
                    string = (char *) calloc(strlen(token) + 1, sizeof(char));
                    strcpy(string, token);
                    spell->range = string;
                } else if (strcmp("components", token) == 0) {
                    spell->componentsCounter = parseEnumComponentsArray(filePointer, &(spell->components));
                } else if (strcmp("material", token) == 0) {
                    parsing += 3;
                    token = strsep(&parsing, "\"");
                    string = (char *) calloc(strlen(token) + 1, sizeof(char));
                    strcpy(string, token);
                    spell->material = string;
                } else if (strcmp("area_of_effect", token) == 0) {
                    spell->area_of_effect = (struct area_of_effect*) calloc(1,sizeof(struct area_of_effect));
                    for (int i = 0; i < 2; i++) {
                        fgets(buffer, sizeof(buffer), filePointer);
                        parsing = buffer;
                        strsep(&parsing, "\"");
                        token = strsep(&parsing, "\"");
                        if (strcmp(token, "type") == 0) {
                            parsing += 3;
                            token = strsep(&parsing, "\"");
                            if (strcmp("sphere", token) == 0) {
                                spell->area_of_effect->type = sphere;
                            } else if (strcmp("cone", token) == 0) {
                                spell->area_of_effect->type = cone;
                            } else if (strcmp("cylinder", token) == 0) {
                                spell->area_of_effect->type = cylinder;
                            } else if (strcmp("line", token) == 0) {
                                spell->area_of_effect->type = line;
                            } else if (strcmp("cube", token) == 0) {
                                spell->area_of_effect->type = cube;
                            }
                        } else if (strcmp(token, "size") == 0) {
                            parsing += 2;
                            token = strsep(&parsing, "\n");
                            spell->area_of_effect->size = atoi(token);
                        }
                    }

                } else if (strcmp("ritual", token) == 0) {
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    if (strcmp("true", token) == 0) {
                        spell->ritual = true;
                    } else if (strcmp("false", token) == 0) {
                        spell->ritual = false;
                    }
                } else if (strcmp("duration", token) == 0) {
                    parsing += 3;
                    token = strsep(&parsing, "\"");
                    string = (char *) calloc(strlen(token) + 1, sizeof(char));
                    strcpy(string, token);
                    spell->duration = string;
                } else if (strcmp("concentration", token) == 0) {
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    if (strcmp("true", token) == 0) {
                        spell->concentration = true;
                    } else if (strcmp("false", token) == 0) {
                        spell->concentration = false;
                    }
                } else if (strcmp("casting_time", token) == 0) {
                    parsing += 3;
                    token = strsep(&parsing, "\"");
                    string = (char *) calloc(strlen(token) + 1, sizeof(char));
                    strcpy(string, token);
                    spell->casting_time = string;
                } else if (strcmp("level", token) == 0) {
                    parsing += 2;
                    token = strsep(&parsing, ",");
                    spell->level = atoi(token);
                } else if (strcmp("attack_type", token) == 0) {
                    parsing += 3;
                    token = strsep(&parsing, "\"");
                    string = (char *) calloc(strlen(token) + 1, sizeof(char));
                    strcpy(string, token);
                    spell->attack_type = string;
                } else if (strcmp("damage", token) == 0) {
                    spell->damage = (struct damage*) calloc(1,sizeof(struct damage));
                    fgets(buffer, sizeof(buffer), filePointer);
                    while (strchr(buffer, '}') == NULL) {
                        parsing = buffer; // Point to buffer (reset)
                        strsep(&parsing, "\"");
                        token = strsep(&parsing, "\"");
                        if (strcmp("damage_at_character_level", token) == 0) {
                            spell->damage->damage_group = damage_at_character_level;
                            spell->damageKeyValueCounter = parseAnyKeyArray(filePointer, &(spell->damage->keyValues));
                            fgets(buffer, sizeof(buffer), filePointer);
                        } else if (strcmp("damage_at_slot_level", token) == 0) {
                            spell->damage->damage_group = damage_at_slot_level;
                            spell->damageKeyValueCounter = parseAnyKeyArray(filePointer, &(spell->damage->keyValues));
                            fgets(buffer, sizeof(buffer), filePointer);
                        } else if (strcmp("damage_type", token) == 0) {
                            parseNameUrlIndex(filePointer, &(spell->damage_type.name), &(spell->damage_type.url),
                                              &(spell->damage_type.index));
                            fgets(buffer, sizeof(buffer), filePointer);
                            fgets(buffer, sizeof(buffer), filePointer);
                        }
                    }

                } else if (strcmp("school", token) == 0) {
                    parseNameUrlIndex(filePointer, &(spell->school.name), &(spell->school.url), &(spell->school.index));

                } else if (strcmp("classes", token) == 0) {
                    spell->classesCounter = parseNameUrlIndexArray(filePointer, &(spell->classes));
                } else if (strcmp("subclasses", token) == 0) {
                    if(strchr(parsing,']')==NULL){
                        spell->subclassesCounter = parseNameUrlIndexArray(filePointer, &(spell->subclasses));
                    }

                } else if(strcmp(token,"dc")==0){
                    fgets(buffer, sizeof(buffer), filePointer);
                    while (strchr(buffer, '}') == NULL) {
                        parsing = buffer; // Point to buffer (reset)
                        strsep(&parsing, "\"");
                        token = strsep(&parsing, "\"");
                        if (strcmp("dc_type", token) == 0) {
                            parseNameUrlIndex(filePointer, &(spell->dc.dc_type.name), &(spell->dc.dc_type.url),
                                              &(spell->dc.dc_type.index));
                            fgets(buffer, sizeof(buffer), filePointer);
                            fgets(buffer, sizeof(buffer), filePointer);
                        } else if (strcmp("dc_success", token) == 0) {
                            parsing += 3;
                            token = strsep(&parsing, "\"");
                            string = (char *) calloc(strlen(token) + 1, sizeof(char));
                            strcpy(string, token);
                            spell->dc.dc_succes = string;
                            fgets(buffer, sizeof(buffer), filePointer);
                        }
                    }
                }
            }
        }
    }
}

void push(struct spellNode** head, struct spell* spell) {
    struct spellNode* newNode = (struct spellNode*)malloc(sizeof(struct spellNode));
    newNode->data = spell;
    newNode->next = NULL;
    if (*head == NULL) {
        *head = newNode;
    } else {
        struct spellNode* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

// Function to free the linked list memory
void freeSpellList(struct spellNode* head) {
    struct spellNode* current = head;
    while (current != NULL) {
        struct spellNode* temp = current;
        current = current->next;
        freeSpell(temp->data);
        free(temp);
    }
}

// Function to free a spell
void freeSpell(struct spell* spell) {
    if (spell->name!=NULL){
        free(spell->name);
    }
    if (spell->index!=NULL){
        free(spell->index);
    }
    if (spell->url!=NULL){
        free(spell->url);
    }
    if (spell->range!=NULL){
        free(spell->range);
    }
    if (spell->material!=NULL){
        free(spell->material);
    }
    for (int i=0;i<spell->descCounter;i++){
        free(spell->desc[i]);
    }
    if (spell->desc!=NULL) {
        free(spell->desc);
    }
    for (int i=0;i<spell->higher_levelCounter;i++){
        free(spell->higher_level[i]);
    }
    if (spell->higher_level!=NULL) {
        free(spell->higher_level);
    }
    if (spell->components!=NULL) {
        free(spell->components);
    }
    if (spell->area_of_effect!=NULL) {
        free(spell->area_of_effect);
    }
    if (spell->duration!=NULL) {
        free(spell->duration);
    }
    if (spell->casting_time!=NULL) {
        free(spell->casting_time);
    }
    if (spell->attack_type!=NULL) {
        free(spell->attack_type);
    }

    for (int i=0;i<spell->damageKeyValueCounter;i++){
        free(spell->damage->keyValues[i].value);
    }

    if (spell->damage!=NULL && spell->damage->keyValues!=NULL) {
        free(spell->damage->keyValues);
    }

    if (spell->damage!=NULL) {
        free(spell->damage);
    }

    if (spell->school.name != NULL){
        free(spell->school.name);
    }

    if (spell->school.url != NULL) {
        free(spell->school.url);
    }
    if (spell->school.index != NULL) {
        free(spell->school.index);
    }

    if (spell->damage_type.name != NULL) {
        free(spell->damage_type.name);
    }

    if (spell->damage_type.url != NULL) {
        free(spell->damage_type.url);
    }

    if (spell->damage_type.index != NULL) {
        free(spell->damage_type.index);
    }

    for (int i=0;i<spell->classesCounter;i++){
        if (spell->classes[i].name!=NULL) {
            free(spell->classes[i].name);
        }
        if (spell->classes[i].url!=NULL) {
            free(spell->classes[i].url);
        }
        if (spell->classes[i].index!=NULL) {
            free(spell->classes[i].index);
        }
    }
    if (spell->classes!=NULL){
        free(spell->classes);
    }
    for (int i=0;i<spell->subclassesCounter;i++){
        if (spell->subclasses[i].name!=NULL) {
            free(spell->subclasses[i].name);
        }
        if (spell->subclasses[i].url!=NULL) {
            free(spell->subclasses[i].url);
        }
        if (spell->subclasses[i].index!=NULL) {
            free(spell->subclasses[i].index);
        }
    }
    if (spell->subclasses!=NULL){
        free(spell->subclasses);
    }
    if (spell->dc.dc_succes!=NULL){
        free(spell->dc.dc_succes);
    }
    if (spell->dc.dc_type.name!=NULL){
        free(spell->dc.dc_type.name);
    }
    if (spell->dc.dc_type.url!=NULL){
        free(spell->dc.dc_type.url);
    }
    if (spell->dc.dc_type.name!=NULL){
        free(spell->dc.dc_type.index);
    }

}

struct spell* pop(struct spellNode** head) {
    if (*head == NULL) {
        return NULL; // Empty list
    }
    struct spellNode* temp = *head;
    struct spell* spell = temp->data;
    *head = (*head)->next;
    free(temp);
    return spell;
}

void cycle(struct spellNode** head) {
    if (*head == NULL) {
        return; // Empty list
    }
    struct spellNode* last = *head;
    while (last->next != NULL) {
        last = last->next;
    }
    last->next = *head;
    *head = (*head)->next;
    last->next->next = NULL;
}

void printSpell(struct spellNode* head){
    uint8_t count =0;
    printf("index : %s\n",(*head).data->index);

    printf("name : %s\n",(*head).data->name);

    printf("url : %s\n",(*head).data->url);

    printf("description : ");

    printf("count %d\n", (*head).data->descCounter);
    for(int i=0;i<(*head).data->descCounter;i++){
        printf("%s",(*head).data->desc[i]);
    }
    printf("\n");

    printf("higher level: ");
    if ((*head).data->higher_level != NULL) {
        printf("count higher level %d\n",(*head).data->higher_levelCounter);
        for(int i=0;i<(*head).data->higher_levelCounter;i++){
            printf("%s",(*head).data->higher_level[i]);
        }
    }
    printf("\n");

    printf("range : %s\n",(*head).data->range);

    printf("components : ");
    if ((*head).data->components!=NULL){
        for(int i=0;i<(*head).data->componentsCounter;i++){
            switch((*head).data->components[i]){
                case 0:
                    printf("V\n");
                    break;
                case 1:
                    printf("S\n");
                    break;
                case 2:
                    printf("M\n");
            }
        }
    }


    printf("material : %s\n",(*head).data->material);

    printf("area of effect : \n");
    if ((*head).data->area_of_effect!=NULL) {
        printf("size : %d\n",(*head).data->area_of_effect->size);
    }

    if ((*head).data->area_of_effect!=NULL) {
        printf("type : ");
        switch ((*head).data->area_of_effect->type) {
            case 0:
                printf("sphere\n");
                break;
            case 1:
                printf("cone\n");
                break;
            case 2:
                printf("cylinder\n");
                break;
            case 3:
                printf("line\n");
                break;
            case 4:
                printf("cube\n");
                break;
        }
    }


    printf("ritual : ");
    switch ((*head).data->ritual) {
        case 0:
            printf("false\n");
            break;
        case 1:
            printf("true\n");
            break;
    }

    printf("duration : %s\n",(*head).data->duration);

    printf("concentration : ");
    switch ((*head).data->concentration) {
        case 0:
            printf("false\n");
            break;
        case 1:
            printf("true\n");
            break;
    }

    printf("casting time : %s\n",(*head).data->casting_time);

    printf("level : %d\n",(*head).data->level);

    printf("attack type : %s\n",(*head).data->attack_type);


    if ((*head).data->damage!=NULL) {
        printf("damage at levels : ");
        switch ((*head).data->damage->damage_group) {
            case 0:
                printf("damage at character level\n");
                break;
            case 1:
                printf("damage at slot level\n");
                break;
        }
    }

    if ((*head).data->damage!=NULL) {
        printf ("here\n");
        printf("%d\n",(*head).data->damageKeyValueCounter);
        for(int i=0; i<(*head).data->damageKeyValueCounter;i++){
            printf("level %d , %s\n", (*head).data->damage->keyValues[i].level, (*head).data->damage->keyValues[i].value);
        }
    }


    printf("damage type :\n");
    printf("index : %s\n",(*head).data->damage_type.index);
    printf("name : %s\n",(*head).data->damage_type.name);
    printf("url : %s\n",(*head).data->damage_type.url);

    printf("school : \n");
    printf("index : %s\n",(*head).data->school.index);
    printf("name : %s\n",(*head).data->school.name);
    printf("url : %s\n",(*head).data->school.url);

    printf("classes : ");
    for(int i=0;i<(*head).data->classesCounter;i++){
        printf("class : %d\n",i);
        printf("index : %s\n",(*head).data->classes[i].index);
        printf("name : %s\n",(*head).data->classes[i].name);
        printf("url : %s\n",head->data->classes[i].url);
    }

    printf("subclasses : ");
    for(int i=0;i<(*head).data->subclassesCounter;i++){
        printf("subclass : %d\n",i);
        printf("index : %s\n",(*head).data->subclasses[i].index);
        printf("name : %s\n",(*head).data->subclasses[i].name);
        printf("url : %s\n",head->data->subclasses[i].url);
    }
}

void printSpellOption(struct spellNode* head, char* optionString){
    char *options = strsep(&optionString, " ");
    while (options != 0) {
        if (strcmp(options, "-at") == 0) {
            printf("attack type : %s\n", head->data->attack_type);
        } else if (strcmp(options, "-de") == 0) {
            printf("description : ");
            printf("count %d\n", (*head).data->descCounter);
            for(int i=0;i<(*head).data->descCounter;i++){
                printf("%s",(*head).data->desc[i]);
            }
            printf("\n");
        } else if (strcmp(options, "-i") == 0) {
            printf("index : %s\n",head->data->index);
        } else if (strcmp(options, "-u") == 0) {
            printf("url : %s\n",head->data->url);
        } else if (strcmp(options, "-hl") == 0) {
            printf("higher level: ");
            if ((*head).data->higher_level != NULL) {
                printf("count higher level %d\n",(*head).data->higher_levelCounter);
                for(int i=0;i<(*head).data->higher_levelCounter;i++){
                    printf("%s",(*head).data->higher_level[i]);
                }
            }
            printf("\n");
        } else if (strcmp(options, "-r") == 0) {
            printf("range : %s\n",(*head).data->range);
        } else if (strcmp(options, "-co") == 0) {
            printf("components : ");
            if ((*head).data->components!=NULL){
                for(int i=0;i<(*head).data->componentsCounter;i++){
                    switch((*head).data->components[i]){
                        case 0:
                            printf("V\n");
                            break;
                        case 1:
                            printf("S\n");
                            break;
                        case 2:
                            printf("M\n");
                    }
                }
            }
        } else if (strcmp(options, "-ma") == 0) {
            printf("material : %s\n",(*head).data->material);
        } else if (strcmp(options, "-ae") == 0) {
            printf("area of effect : \n");
            if ((*head).data->area_of_effect!=NULL) {
                printf("size : %d\n",(*head).data->area_of_effect->size);
            }

            if ((*head).data->area_of_effect!=NULL) {
                printf("type : ");
                switch ((*head).data->area_of_effect->type) {
                    case 0:
                        printf("sphere\n");
                        break;
                    case 1:
                        printf("cone\n");
                        break;
                    case 2:
                        printf("cylinder\n");
                        break;
                    case 3:
                        printf("line\n");
                        break;
                    case 4:
                        printf("cube\n");
                        break;
                }
            }
        } else if (strcmp(options, "-ri") == 0) {
            printf("ritual : ");
            switch ((*head).data->ritual) {
                case 0:
                    printf("false\n");
                    break;
                case 1:
                    printf("true\n");
                    break;
            }
        } else if (strcmp(options, "-du") == 0) {
            printf("duration : %s\n",(*head).data->duration);
        } else if (strcmp(options, "-c") == 0) {
            printf("concentration : ");
            switch ((*head).data->concentration) {
                case 0:
                    printf("false\n");
                    break;
                case 1:
                    printf("true\n");
                    break;
            }
        } else if (strcmp(options, "-ct") == 0) {
            printf("casting time : %s\n",(*head).data->casting_time);
        } else if (strcmp(options, "-l") == 0) {
            printf("level : %d\n",(*head).data->level);
        } else if (strcmp(options, "-d") == 0) {
            if ((*head).data->damage != NULL) {
                printf("damage at levels : ");
                switch ((*head).data->damage->damage_group) {
                    case 0:
                        printf("damage at character level\n");
                        break;
                    case 1:
                        printf("damage at slot level\n");
                        break;
                }
            }

            if ((*head).data->damage != NULL) {
                printf("here\n");
                printf("%d\n", (*head).data->damageKeyValueCounter);
                for (int i = 0; i < (*head).data->damageKeyValueCounter; i++) {
                    printf("level %d , %s\n", (*head).data->damage->keyValues[i].level,
                           (*head).data->damage->keyValues[i].value);
                }
            }
        } else if (strcmp(options, "-dt") == 0) {
            printf("damage type :\n");
            printf("index : %s\n",(*head).data->damage_type.index);
            printf("name : %s\n",(*head).data->damage_type.name);
            printf("url : %s\n",(*head).data->damage_type.url);
        } else if (strcmp(options, "-s") == 0) {
            printf("school : \n");
            printf("index : %s\n",(*head).data->school.index);
            printf("name : %s\n",(*head).data->school.name);
            printf("url : %s\n",(*head).data->school.url);
        } else if (strcmp(options, "-cl") == 0) {
            printf("classes : ");
            for(int i=0;i<(*head).data->classesCounter;i++){
                printf("class : %d\n",i);
                printf("index : %s\n",(*head).data->classes[i].index);
                printf("name : %s\n",(*head).data->classes[i].name);
                printf("url : %s\n",head->data->classes[i].url);
            }
        } else if (strcmp(options, "-sc") == 0) {
            printf("subclasses : ");
            for (int i = 0; i < (*head).data->subclassesCounter; i++) {
                printf("subclass : %d\n", i);
                printf("index : %s\n", (*head).data->subclasses[i].index);
                printf("name : %s\n", (*head).data->subclasses[i].name);
                printf("url : %s\n", head->data->subclasses[i].url);
            }
        }
        options = strsep(&optionString, " ");
    }

}

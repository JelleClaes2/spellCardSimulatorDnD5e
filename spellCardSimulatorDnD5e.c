#include <stdio.h>
#include <stdint.h>


enum bool{false,true};

struct anyKey{
    char* name;
    char* value;
};

struct damage_type {
    char *index;
    char *name;
    char *url;
};

struct spell{
    char* index;
    char* name;
    char* url;
    char* desc;
    char* higher_level;
    char* range;
    enum components{V,S,M};
    char* material;
    struct area_of_effect{
        int size;
        enum type{sphere,cone,cylinder,line,cube};
    };
    enum bool ritual;
    char* duration;
    enum bool concentration;
    char* casting_time;
    int level;
    char* attack_type;
    enum damage_group{damage_at_character_level,damage_at_slot_level};
    union damage{
        struct damage_at_character_level{
            struct annyKey* keyValues;
            struct damage_type damageType;
        };
        struct damage_at_slot_level{
            struct annyKey* keyValues;
            struct damage_type damageType;
        };
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
};

int main(int argc , char* argv[]){

    FILE *filePointer = fopen("blur.json","r");//Open blur.json TODO NEED TO CHECK ARG TO MAIN

}
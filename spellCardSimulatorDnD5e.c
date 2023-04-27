#include <stdio.h>
#include <stdint.h>


int main(int argc , char* argv[]){

    enum bool{false,true};
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
        union damage{
            struct damage_at_character_level{
                //TODO
            };
            struct damage_at_slot_level{
                //TODO
            };
        };
        struct damage_type{
            char* index;
            char* name;
            char* url;
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
}
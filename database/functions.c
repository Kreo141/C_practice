#include <stdio.h>
#include <string.h>
#include "functions.h"

char directorys[] = "db/";

void createDatabase(char *dbName){
    FILE *dbFile;
    char filePath[165];

    snprintf(filePath, sizeof(filePath), "%s%s.txt", directorys, dbName);

    /*
        Checks if database name already exist
    */
    dbFile = fopen(filePath, "r");
    if(dbFile != NULL){
        printf("Database %s already exists.\n", dbName);
    }

    /*
        Creating database
    */
    dbFile = fopen(filePath, "w");
    




    
}

void modifyDatabase(){

}

void deleteDatabase(){

}

void displayDatabase(){

}
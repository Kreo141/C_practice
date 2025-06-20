#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <direct.h>
#include <stdlib.h>
#include "database.h"

struct column_properties{
    bool not_null;
    bool auto_increment;
    bool if_primary_key;
};

struct column{
    char *column_name;
    char *datatype;
    char *datas;
    struct column_properties properties;
};

struct table{
    char *table_name;
    char *table_file_path;
    int column_count;
    struct column columns[10];
};

struct database{
    char *database_name;
    char *database_file_path;
    int tableCount;
    struct table tables[10];
};

char *directory = "db/";
char tableDIR[128];

bool isInitialized = false;

struct database db;

void initDatabase(char *databaseName){
    
    FILE *file;
    char filepath[128];

    /*
        Checks if database already exists
    */

    snprintf(filepath, sizeof(filepath), "%s%s.txt", directory, databaseName);
    snprintf(tableDIR, sizeof(tableDIR), "%s%s/", directory, databaseName);

    file = fopen(filepath, "r");
    if(file == NULL){
        printf("Database %s does not exists.", databaseName);
    }

    char temp[255];
    
    /*
        INITIALIZE DATABASE STRUCT
    */
    while(fgets(temp, 255, file)){
        if(strstr(temp, "{") != NULL || strstr(temp, "}") != NULL || temp[0] == '\n' || temp[0] == '\r'){
            continue;
        }

        if(strstr(temp, "name=")){
            char *token = strtok(temp, "=");
            token = strtok(NULL, "=");
            if(token != NULL){
                token[strcspn(token, "\r\n")] = 0;
                db.database_name = strdup(token);
            }
        }

        if(strstr(temp, "tables=")){
            char *token = strtok(temp, "=");
            token = strtok(NULL, "=");

            if(token != NULL){
                token[strcspn(token, "\r\n")] = 0;
                char *tableName = strtok(token, ",");

                int tableIndex = 0;
                while(token != NULL && tableIndex < 10){
                    token[strcspn(token, "\r\n")] = 0;
                    db.tables[tableIndex].table_name = strdup(token);
                    token = strtok(NULL, ",");
                    db.tableCount++;
                    tableIndex++;
                }
            }

        }
        fclose(file);
    }

    /*
        INITIALIZE TABLE STRUCT
    */

    // Table file path
    for(int x = 0; x < db.tableCount; x++){
        int totalAllocation = strlen(tableDIR) + 1 + strlen(db.tables[x].table_name)+5;

        char temp__table_File_Path[totalAllocation];
        snprintf(temp__table_File_Path, sizeof(temp__table_File_Path), "%s%s.txt", tableDIR, db.tables[x].table_name);

        db.tables[x].table_file_path = malloc(strlen(temp__table_File_Path)+1);
        strcpy(db.tables[x].table_file_path, temp__table_File_Path);
    }

    /*
        INITIALIZE TABLE COLUMNS
    */
    for(int x = 0; x < db.tableCount; x++){
        file = fopen(db.tables[x].table_file_path, "r");

        int columnCount = 0;
        
        char temp[512];
        while(fgets(temp, sizeof(temp), file)){
            if(strstr(temp, "#") || temp[0] == '\n' || temp[0] == '\r'){
                continue;
            }

            if(strstr(temp, "column:")){    
                char *token;
                token = strtok(temp, ":");

                token = strtok(NULL, ":");
                db.tables[x].columns[columnCount].column_name = malloc(strlen(token) + 1);
                strcpy(db.tables[x].columns[columnCount].column_name, token);

                token = strtok(NULL, ":");
                db.tables[x].columns[columnCount].datatype = malloc(strlen(token) + 1);
                strcpy(db.tables[x].columns[columnCount].datatype, token);

                
                bool value; 

                token = strtok(NULL, ":");
                value = (strstr(token, "true")) ? true : false;
                db.tables[x].columns[columnCount].properties.not_null = value;

                token = strtok(NULL, ":");
                value = (strstr(token, "true")) ? true : false;
                db.tables[x].columns[columnCount].properties.auto_increment = value;

                token = strtok(NULL, ":");
                value = (strstr(token, "true")) ? true : false;
                db.tables[x].columns[columnCount].properties.if_primary_key = value;

                db.tables[x].columns[columnCount].datas = NULL;
            }

            if(strstr(temp, "{")){
                while(fgets(temp, sizeof(temp), file) != NULL){
                    if(strstr(temp, "}")){
                        break;
                    }
                    
                    if(db.tables[x].columns[columnCount].datas == NULL){
                        int len = strlen(temp) + 2;
                        char container[len];

                        // format "data1," the text and put it into a temporary {container}
                        snprintf(container, len, "%s,", temp);

                        
                        // allocate column data
                        db.tables[x].columns[columnCount].datas = malloc(len);

                        if(db.tables[x].columns[columnCount].datas != NULL){
                            strcpy(db.tables[x].columns[columnCount].datas, container);
                        } else {
                            printf("Something went wrong while allocating memory in line 179");
                        }

                        continue;
                    }

                    // get the memory size of the new data and the memory allocation of current datas
                    int newDataMemory = strlen(temp) + 1;
                    int currentDatasAllocationMemory = strlen(db.tables[x].columns[columnCount].datas);
                    int newAllocationMemory = newDataMemory + currentDatasAllocationMemory;

                    char currentData[1024];

                    // now, allocate it to the current data variable
                    db.tables[x].columns[columnCount].datas = malloc(newAllocationMemory);

                    // now, concatenate the new data to the datas;
                    snprintf(db.tables[x].columns[columnCount].datas, newAllocationMemory, "%s%s,", currentData, temp); 
                }
            }

            columnCount++;
        }
    }





    
    fclose(file);

    isInitialized = true;

    printf("Database %s initialized.\n", db.database_name);

    
    for(int x = 0; x < db.tableCount; x++){
        printf("Table %i Name: %s\n", x+1, db.tables[x].table_name);
        printf("\tfilePath: %s\n", db.tables[x].table_file_path);
        
        printf("\t\tColumn 1: %s%s%i%i%i\n", db.tables[x].columns[0].column_name, db.tables[x].columns[0].datatype, db.tables[x].columns[0].properties.not_null, db.tables[x].columns[0].properties.auto_increment, db.tables[x].columns[0].properties.if_primary_key);
        printf("\t\tColumn 2: %s%s%i%i%i\n\n", db.tables[x].columns[1].column_name, db.tables[x].columns[1].datatype, db.tables[x].columns[1].properties.not_null, db.tables[x].columns[1].properties.auto_increment, db.tables[x].columns[1].properties.if_primary_key);

    }

}
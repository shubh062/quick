#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include "include.h"

//flags
extern checkLinks;
extern deleteFlag;
extern verbose;

//Copying Information
extern copiedEntities;
extern bytesCopied;
extern totalEntities;

#define MAX_PATH_SIZE 256
extern char origDirName[MAX_PATH_SIZE];
extern char destDirName[MAX_PATH_SIZE];


void copyfile(char *name1, char *name2){
    struct stat mybuf;
    struct stat mybuf2;
    char type;

    //step1: get the file type of the source path
    if(lstat(name1,&mybuf)==0){
        switch (S_IFMT & mybuf.st_mode){
        case S_IFDIR :
            type="d";
            break;
        case S_ISLNK:
            type="l";
            break;
        case S_IFREG:
            type="-";
            break;
        
        default:
            type="?";
            break;
        }
    }
    else return;

    if(strcmp(type,"d")==0){
        traversedir(name1,name2);
    }
    else if(strcmp(type,"l")==0){
        if(checkLinks) printf("call for copy of symbolic function");
        return;
    }
    
    






}

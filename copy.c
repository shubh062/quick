#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "include.h"

//flags
extern char checkLinks;
extern char deleteFlag;
extern char verbose;

//Copying Information
extern int copiedEntities;
extern int bytesCopied;
extern int totalEntities;

#define MAX_PATH_SIZE 256
extern char origDirName[MAX_PATH_SIZE];
extern char destDirName[MAX_PATH_SIZE];


int deepcopy(char *file1, char* file2){
    int sr, dt; //file descriptor
    ssize_t readed;
    char buffer[BUFFSIZE];

    //file descriptors source and destination
    if(sr=open(file1,O_RDONLY)<0){ // here file is opend with read only permissions
        return -1;
    }
    if(dt=open(file2,O_WRONLY|O_TRUNC|O_CREAT)<0){ //here file2 is opened with write,create if not existed and truncate if exist permission
        close(sr);
        return -2;
    } 



    //block size for reading and writing at one iteration is BUFFSIZE
    while( ( readed= read(sr,buffer,BUFFSIZE) ) > 0){ //if read operation is successfull
            //the read fxn returns the bytes readed.   
        if(write(dt,buffer,BUFFSIZE)<readed){ //if the written size is less than readed size, the process terminates
            close(sr); 
            close(dt);
            return -3;
        }
    }

    close(sr);
    close(dt);

    if(readed<0){
        return -4;
    }

    return 0;
}


void copyfile(char *name1, char *name2){
    struct stat mybuf;
    struct stat mybuf2;
    char *type; //can use char type also, then to compare and assign;

    //step1: get the file type of the source path
    if(lstat(name1,&mybuf)==0){
        switch (S_IFMT & mybuf.st_mode){
        case S_IFDIR :
            type="d";
            break;
        case S_IFLNK:
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
        traverseDir(name1,name2);
        return;
    }
     if(strcmp(type,"l")==0){
        if(checkLinks) printf("call for copy of symbolic function");
        return;
    }//Case: Hardlink to explicitly handled
    //free(type);
    //type=NULL;
    //if the file is regular 

    //case1 if the corresponding dest file dont't exist 
    if(stat(name2,&mybuf2)<0){
        if(errno!=ENOENT) {
            perror("stat");
            return;
        }
       if(deepcopy(name1,name2)<0){
        perror("error while copying file: ");
        return;
       }
       chmod(name2,mybuf.st_mode);

        if(stat(name2,&mybuf2)<0){
            perror("stat");
            return;
        }

        bytesCopied+=(int)mybuf2.st_size;
        copiedEntities++;

        if(verbose) {
            printf("copied %s :\n",name2);
        }
    }
    //case2 : if the corresponding destination file exists
    else{
        // if(stat(name2,&mybuf2)<0){
        //     if(errno!=ENOENT) {
        //         perror("stat");
        //         return;
        //     }   
        // }

        //case1: if the file status/info of source and dest are different
        if(mybuf2.st_mode!=mybuf.st_mode){
            
            if((mybuf.st_mode & S_IFMT) == S_IFDIR){
                rmdir(name2); //deleteDirectory(name2) can used if customized
            }
            else if((mybuf.st_mode & S_IFMT) == S_IFLNK){
                printf("explicitly going to be handled");
            }
            else{
                remove(name2);
            }

            if(deepcopy(name1,name2)<0){
                printf("error while copying");
                return;
            }

            chmod(name2,mybuf.st_mode);
            copiedEntities++;

            if(stat(name2,&mybuf2)<0){
                perror("stat");
                return;
            }
            bytesCopied+=(int)mybuf2.st_size;
            if(verbose) {
                printf("copied %s :\n",name2);
            }

        }  
        //case2: if the file size is different 
        else if(mybuf2.st_size!=mybuf.st_size){

            if(deepcopy(name1,name2)<0){
                printf("error while copying");
                return;
            }

            chmod(name2,mybuf.st_mode);
            copiedEntities++;
            
            if(stat(name2,&mybuf2)<0){
                perror("stat");
                return;
            }            
            bytesCopied+=(int)mybuf2.st_size;
            if(verbose) {
                printf("copied %s :\n",name2);
            }

        }

        else if(mybuf.st_mtime>mybuf2.st_mtime){

            if(deepcopy(name1,name2)<0){
                printf("error while copying");
                return;
            }

            chmod(name2,mybuf.st_mode);
            if(stat(name2,&mybuf2)<0){
                perror("stat");
                return;
            }            
            copiedEntities++;
            bytesCopied+=(int)mybuf2.st_size;
            if(verbose) {
                printf("copied %s :\n",name2);
            }

        }

        }
    }

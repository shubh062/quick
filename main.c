#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include "include.h"

//flags
int verbose;
int checkLinks;
int deleteFlag;

//Copying Information
int copiedEntities;
int bytesCopied;
int totalEntities;

#define MAX_PATH_SIZE 256
char origDirName[MAX_PATH_SIZE];
char destDirName[MAX_PATH_SIZE];



void fixPath(char *, char []);

void traverseDir(char orig[],char destName[]){
    //to do 
    char *source; //pointer to origin
    struct stat sr; //dont use poinnter cuz we want it to be locally available for this function and then vanish

    char * destin;//pointer to desination
    struct stat dt;
    struct dirent *direntp;
    DIR *dp; //directory pointer for traversal of directory

    
    //1. check if source directory exist if not exit;
    if(stat(orig,&sr)<0) return;

    //2. opening the directory if(exists)
    if((dp=opendir(orig))==NULL){ //this makes sure that the oriign is a directory that why we can open it, so now we proceed to make corresponding directory to destination
        perror("opendir");
        return ;
    }
    
    //3. check if dest directory(specifically PATH) exist;
    if(stat(destName,&dt)<0){ //if Path(can be a diretory/or a file) dont exists;
        if(mkdir(destName,sr.st_mode)==-1){//made the directory
            perror("mkdir");
            return;
        }
        // else {
        //     chmod(destName,sr.st_mode); //change modes (permissions) as respect to the source directory
        // }
        copiedEntities++;

        if(verbose) printf("created a directory: %s \n", destName);
    }
    else{ // if the directory already exists;
        if(deleteFlag) {
            //checkfordeletedfiles(orig,destNaME);
            printf("Above Function Intitiated: checks and deletes -> the file that dest has and source files hasnt");
        }
        else{ //if d flag was not given;
            if((S_IFMT & dt.st_mode)!=S_IFDIR){//IF the path not corresponds to a directory
                remove(destDirName);
                mkdir(destDirName,dt.st_mode);
                //chmod(orig,dt.st_mode);
                copiedEntities++;
                if(verbose){
                    printf("replaced : %s with a new directory ", destDirName);
                }
            }
        }

    }

    //4. Before copying we need to avoid any case of looping a cycle while copying 
    char *name1=realpath(orig,NULL);
    char *name2=realpath(destName,NULL);
    if(strcmp(name1,name2)==0){
        if(verbose) printf("By copying %s, a loop is created so it is skipped copying",orig);
        free(name1);
        free(name2);
        closedir(dp);
        return;
    }
    //if not looping then continue;
    free(name1);
    free(name2);

    //5.now we will recursively traverse directry and copy files of each directory entry if it is regular file or if it is a directory then call traversedir again
    while ((direntp=readdir(dp))!=NULL)
    {   
        //getting the source directory entry
        char *temp1=malloc(strlen(orig)+strlen(direntp->d_name)+2);
        strcat(temp1,orig);
            if(temp1[strlen(orig)-1]!="/") strcat(temp1,"/");
        strcat(temp1,direntp->d_name);

        //framing the exact path for corresponding directory entry
        char *temp2=malloc(strlen(destName)+strlen(direntp->d_name)+2);
        strcat(temp2,destName);
            if(temp2[strlen(destName)-1]!="/") strcat(temp2,"/");
        strcat(temp2,direntp->d_name);  

        //check if the entry corresponds to the current parent entry or not
        int flag= strcmp(direntp->d_name,".") && strcmp(direntp->d_name,"..");

        if(flag!=0){
            totalEntities++;
            copyfile(temp1,temp2);
        }    

        free(temp1);
        temp1=NULL;
        free(temp2);
        temp2=NULL;

    }

    closedir(dp);
    
}



int main(int argc, char* argv[]){
    //we are expecting atleast 3 arg counts, (outputfilename , source and destination)
    if(argc<3){
        printf("Invalid Arguments");
        return -1;
    }

    clock_t start = clock();//starting time;

    //reading flags and storing in 'global variable' because they need to be paased to other function
    for(int i=1;i<(argc-2);i++){            //leaving last 2 inputs that are source and dest
        if(strcmp(argv[i],"-v")==0){        //(v->verbose, which provides addtional info about copying )
            verbose=1;
            printf("the verbose flag is set %d \n",verbose);
        }
        else if(strcmp(argv[i],"-l")==0){   //(-l which checks for symbolic links and )
            checkLinks=1;
            printf("the checklink is set %d \n",checkLinks);
        }
        else if(strcmp(argv[i],"-d")==0){   //(-d flag, which means we are required to delete extrafiles that are present excess in dest folder, but not in source)
            deleteFlag=1;
            printf("the delete flag is set %d \n",deleteFlag);
        }
        else {
            printf("wrong input \n");
            return -1;
        }
    }


    struct stat mybuf; //structure for containing file status of the source directory, below we check source file existence
    if(stat(argv[argc-2],&mybuf)<0){//gives 0 on success: means succesfully retrives the file attribute in  mybuf named stat structure
        perror(argv[argc-2]);       //if failed, means source file not exist since returned -1 and throws error
        return -1;
    }//now we have file attributes of source(argv[argc-2]) stored in mybuf stat structure

    if((mybuf.st_mode & S_IFMT) == S_IFDIR){ //if the source file is a directory
        //before copying we need to canonicate path(convert to full path) //canonized path to be store in var origDirName, destDirName
        fixPath(argv[argc-2],"s");
        fixPath(argv[argc-1],"d");
        traverseDir( origDirName,destDirName);
    }
    else { //if the source path is any other file
        printf("the current file is regular so copy function is to be initiated \n");
        copyfile(origDirName,destDirName);
    }

    clock_t end=clock(); //ending time;
    double time_spent=(double)(end-start) / CLOCKS_PER_SEC;

    printf("there are %d files/directories in the heirarchy\n",totalEntities);
    printf("Number of Entities copied: %d\n",copiedEntities);
    printf("Copied %d bytes in %f seconds at %f bytes/sec\n",bytesCopied,time_spent,bytesCopied/time_spent);

    return 0;
}


//here manual conversion to absolute path can be done as well 
void fixPath(char * oldPath, char which[]){
	 if (which=='s' && (realpath(oldPath, origDirName) != NULL)) {
		printf("origin canonized ");
    } 
	else if (which=='d' && (realpath(oldPath, destDirName) != NULL)) {
		printf("dest canonized ");
	} 
	else {
        perror("realpath");
        // Handle the error as needed
    }
	printf("\n");
}
//manual conversion to absolute path must be preferred here since 2 different directory names can have same content
//and same file structure inside them and then using realpath function can make conflicts to canonicize the paths
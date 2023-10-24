#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

//flags
int verbose;
int checkLinks;
int deleteFlag;

#define MAX_PATH_SIZE 256
char origDirName[MAX_PATH_SIZE];
char destDirName[MAX_PATH_SIZE];



void fixPath(char *, char []);
void traverseDir(char origDirName[],char destDirName[]){
    printf("hello");
    printf("source and dest dir :start\n %s \n %s \n end:",origDirName,destDirName);
    
}



int main(int argc, char* argv[]){
    //we are expecting atleast 3 arg counts, (outputfilename , source and destination)
    if(argc<3){
        printf("Invalid Arguments");
        return -1;
    }

//reading flags and storing in 'global variable' because they need to be paased to other function
    for(int i=1;i<(argc-2);i++){//leaving last 2 inputs that are source and dest
        if(strcmp(argv[i],"-v")==0){ //(v->verbose, which provides addtional info about copying )
            verbose=1;
            printf("the verbose flag is set %d \n",verbose);
        }
        else if(strcmp(argv[i],"-l")==0){ //(-l which checks for sybolic links and )
            checkLinks=1;
            printf("the checklink is set %d \n",checkLinks);
        }
        else if(strcmp(argv[i],"-d")==0){    //(-d flag, which means we are required to delete extrafiles that are present excess in dest folder, but not in source)
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
        perror(argv[argc-2]); //if failed, means source file not exist since returned -1 and throws error
        return -1;
    }//now we have file attributes of source(argv[argc-2]) stored in mybuf stat structure

    if((mybuf.st_mode & S_IFMT) == S_IFDIR){ //if the source file is a directory
        //before copying we need to canonicate path(convert to full path) //canonized path to be store in var origDirName, destDirName
        fixPath(argv[argc-2],"source");
        fixPath(argv[argc-1],"dest");
        traverseDir( origDirName,destDirName);

    }
    else { //if the source path is any other file
        printf("the current file is regular so copy function is to be initiated \n");
    }

    return 0;
}



void fixPath(char * oldPath, char which[]){
    if(oldPath[0]=='.' && oldPath[1]=='/'){    //if the given path is relative path 
        if(which=="source"){
            strcpy(origDirName,&oldPath[2]); //we removed the first 2 characters ./ and and copied to dir name
        }
        else if(which=="dest"){
            strcpy(destDirName,&oldPath[2]);
        }
    }
    else{ //if the file is starts with / it is full path
        if(which=="source"){
            strcpy(origDirName,oldPath); //we directly copied the path
            printf("origin directory name changed to %s \n",origDirName);
        }
        else if(which=="dest"){
            strcpy(destDirName,oldPath);
        }    
    }
}
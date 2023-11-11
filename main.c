#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>

//flags
int verbose;
int checkLinks;
int deleteFlag;

//Copying Information
int copiedEntities;

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
        if(mkdir(destName)==-1){//made the directory
            perror("mkdir");
            return;
        }
        else {
            chmod(destName,sr.st_mode); //change modes (permissions) as respect to the source directory
        }
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
                mkdir(destDirName);
                chmod(orig,dt.st_mode);
                copiedEntities++;
                if(verbose){
                    printf("replaced : %s with a new directory ", destDirName);
                }
            }
        }

    }

  //Before copying we need to avoid any case of looping a cycle while copying 
  
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
        fixPath(argv[argc-2],"s");
        fixPath(argv[argc-1],"d");
        traverseDir( origDirName,destDirName);
    }
    else { //if the source path is any other file
        printf("the current file is regular so copy function is to be initiated \n");
    }

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

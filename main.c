#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

//flags
int verbose;
int checkLinks;
int deleteFlag;

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
    }
//now we have file attributes of source(argv[argc-2]) stored in mybuf stat structure
    




    return 0;
}
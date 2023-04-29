#include "HackerEnrollment.h"
#include <string.h>

int main(int argc, char** argv){
    if(argc < 6 || argc > 7){
        fprintf(stderr,"Check arguments");
        return 0;
    }
    char *check = argv[1];
    int offset=0;
    if(strcmp(check,"-i") == 0){
        offset=1;
    }
    if((argc == 7 && strcmp(check,"-i") == 0) || (argc == 6 && strcmp(check,"-i") != 0)){
        FILE* studentsFile = fopen(argv[1+offset],"r");
        if(studentsFile == NULL) {
            return 0;
        }
        FILE* coursesFile = fopen(argv[2+offset],"r");
        if(coursesFile == NULL) {
            return 0;
        }
        FILE* hackersFile = fopen(argv[3+offset],"r");
        if(hackersFile == NULL) {
            return 0;
        }
        FILE* queueFile = fopen(argv[4+offset],"r");
        if(queueFile == NULL) {
            return 0;
        }
        FILE* targetFile = fopen(argv[5+offset],"a");
        if(targetFile == NULL) {
            return 0;
        }
        EnrollmentSystem newSystem = createEnrollment(studentsFile,coursesFile,hackersFile);
        if(newSystem==NULL){
            return 0;
        }
        newSystem = readEnrollment(newSystem,queueFile);
        if(newSystem==NULL){
            return 0;
        }
        addFriendshipFunctions(newSystem,offset);
        hackEnrollment(newSystem,targetFile);
        fclose(studentsFile);
        fclose(hackersFile);
        fclose(coursesFile);
        fclose(queueFile);
        fclose(targetFile);
        return 0;
    }
    return 0;
}


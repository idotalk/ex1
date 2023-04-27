#include "HackerEnrollment.h"
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv){
    if(argc < 6 || argc > 7){
        fprintf(stderr,"Check arguments");
        return 0;
    }
    char *check = argv[1];
    int o=0;
    if(strcmp(check,"-i") == 0){
        o=1;
    }
    if((argc == 7 && strcmp(check,"-i") == 0) || (argc == 6 && strcmp(check,"-i") != 0)){
        FILE* studentsFile = fopen(argv[1+o],"r");
        if(studentsFile == NULL) {
            return 0;
        }
        FILE* coursesFile = fopen(argv[2+o],"r");
        if(coursesFile == NULL) {
            return 0;
        }
        FILE* hackersFile = fopen(argv[3+o],"r");
        if(hackersFile == NULL) {
            return 0;
        }
        FILE* queueFile = fopen(argv[4+o],"r");
        if(queueFile == NULL) {
            return 0;
        }
        FILE* targetFile = fopen(argv[5+o],"a");
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
        FriendshipFunction* functionsArray = malloc(sizeof(functionsArray)*4);
        if(!functionsArray){
            return 0;
        }
        Course course = newSystem->m_coursesArray[0];
        int i=1;
        while(course != NULL){
            if(course->m_ilQueue != NULL){
                IsraeliQueue courseQueue = course->m_ilQueue;
                IsraeliQueueAddFriendshipMeasure(courseQueue,(FriendshipFunction)compareIds);
                IsraeliQueueAddFriendshipMeasure(courseQueue,(FriendshipFunction)checkFileConnection);
                if (o==1){
                    IsraeliQueueAddFriendshipMeasure(courseQueue,(FriendshipFunction)nameCompareCaseInsensitive);
                }
                else{
                    IsraeliQueueAddFriendshipMeasure(courseQueue,(FriendshipFunction)nameCompareCaseSensitive);
                }

            }
            course = newSystem->m_coursesArray[i++];
        }
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
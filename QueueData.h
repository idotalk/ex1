#ifndef QUEUEDATA_H
#define QUEUEDATA_H
#include "IsraeliQueue.h";

typedef struct Student
{
    unsigned int id;
    int totalCredits;
    int gpa;
    char *name;
    char *surname;
    char *city;
    char *department;

} Student;

typedef struct Data
{
    FriendshipFunction*  friendshipFunctions;
    ComparisonFunction*  compareFunction;
    int friendshipThreshold;
    int rivalryThreshold;
    int remainedFriends=FRIEND_QUOTA;
    int reminedRivals=RIVAL_QUOTA;
    Student student;
}* pData;

#endif //QUEUEDATA_H
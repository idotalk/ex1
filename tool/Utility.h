#ifndef UTILITY_H
#define UTILITY_H
#include <stdio.h>
#include "IsraeliQueue.h"

typedef struct Student{
    unsigned int m_studentID;
    int m_studentGPA;
    int m_totalCredits;
    char* m_name;
    char* m_surname;
    char* m_city;
    char* m_department;
    unsigned int* m_friendsStudents; // Exist only if student is Hacker
    unsigned int* m_rivalsStudents;  // Exist only if student is Hacker
}Student, *pStudent;

typedef struct Hacker{
    unsigned int m_hackerID;
    int* m_desiredCourses; // -1 terminated array
    int m_amountDesired;
    unsigned int* m_friendsStudents; // 0 terminated array
    unsigned int* m_rivalsStudents;  // 0 terminated array
    pStudent m_studentData;

}Hacker, *pHacker;

typedef struct Course{
    int m_courseID;
    int m_courseSize;
    IsraeliQueue m_ilQueue;
}* Course;

Course createCourse(int,int, IsraeliQueue);

pStudent createStudent(unsigned int, int, int, char*, char*, char*, char*,unsigned int*,unsigned int*);

pHacker createHacker(unsigned int, int*,int, unsigned int*, unsigned int*, pStudent);

int countLinesInFile(FILE*);

int compareIds(void* ,void*);
int nameCompareCaseSensitive(void* ,void*);
int nameCompareCaseInsensitive(void* ,void*);
int valueCaseSensitive(char*);
int valueCaseInsensitive(char*);
int checkFileConnection(void* firstItem, void* secondItem);
int compare(void*, void*);


#endif //UTILITY_H

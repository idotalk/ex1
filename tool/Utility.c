#include "Utility.h"
#include <stdlib.h>
#include <string.h>


// Local functions declaration.
/** NOTICE! this function can be called only from HackerEnrollment.c file.
 *  Return ASCII value of string - case sensitive */
static int valueCaseSensitive(char*);
/** Return ASCII value of string - case insensitive */
static int valueCaseInsensitive(char*);

/** NOTICE! this function can be called only from HackerEnrollment.c file.
 *  In case one of the students passed to the function is hacker:
 *  Returns if the students are friends by the Hackers file provided in Create Enrollment.
 *  Return value: 20 if friends, 0 otherwise. */
static int isFriendsByFile(pStudent student1, pStudent student2);

/** NOTICE! this function can be called only from HackerEnrollment.c file.
 *  In case one of the students passed to the function is hacker:
 *  Returns if the students are rivals by the Hackers file provided in Create Enrollment.
 *  Return value: -20 if rivals, 0 otherwise. */
static int isRivalsByFile(pStudent student1, pStudent student2);

Course createCourse(int courseID,int size, IsraeliQueue ilQueue){
    Course newCourse = malloc(sizeof(*newCourse));
    if(!newCourse){
        return NULL;
    }
    newCourse->m_courseID = courseID;
    newCourse->m_courseSize = size;
    newCourse->m_ilQueue = ilQueue;
    return newCourse;
}

pStudent createStudent(unsigned int studentID,
                       int totalCredits,
                       int studentGPA,
                       char* name,
                       char* surname,
                       char* city,
                       char* department,
                       unsigned int* friendsStudents,
                       unsigned int* rivalsStudents){
    pStudent newStudent = malloc(sizeof(*newStudent));
    if(!newStudent){
        return NULL;
    }
    newStudent->m_studentID = studentID;
    newStudent->m_studentGPA = studentGPA;
    newStudent->m_totalCredits = totalCredits;
    char *studentName = malloc(strlen(name)+1);
    if(!studentName){
        return NULL;
    }
    char *cityName = malloc(strlen(city)+1);
    if(!cityName){
        return NULL;
    }
    char *departmentName = malloc(strlen(department)+1);
    if(!departmentName){
        return NULL;
    }
    char *studentSurname = malloc(strlen(surname)+1);
    if(!studentSurname){
        return NULL;
    }
    strcpy(studentName,name);
    strcpy(cityName,city);
    strcpy(departmentName,department);
    strcpy(studentSurname,surname);
    newStudent->m_department = departmentName;
    newStudent->m_name = studentName;
    newStudent->m_surname = studentSurname;
    newStudent->m_city = cityName;

    newStudent->m_friendsStudents = friendsStudents;
    newStudent->m_rivalsStudents = rivalsStudents;
    return newStudent;
}

pHacker createHacker(unsigned int hackerID, int* coursesArray,int coursesAmount, unsigned int* friends,
                     unsigned int* rivals, pStudent studentPointer){
    pHacker newHacker = malloc(sizeof(*newHacker));
    if(!newHacker){
        return NULL;
    }
    newHacker->m_amountDesired = coursesAmount;
    newHacker->m_hackerID = hackerID;
    newHacker->m_desiredCourses = coursesArray;
    newHacker->m_friendsStudents = friends;
    newHacker->m_rivalsStudents = rivals;
    newHacker->m_studentData = studentPointer;
    return newHacker;
}

int countLinesInFile(FILE* file){
    if(file==NULL){
        return 0;
    }
    int lines = 0;
    int i;
    while((i=fgetc(file))!=EOF) {
        if(i=='\n'){
            lines++;
        }
    }
    rewind(file);
    return lines;
}

int longestLineInFile(FILE* file){
    if(file==NULL){
        return 0;
    }
    int longest = 0, count=0;
    int i;
    while((i=fgetc(file))!=EOF) {
        count++;
        if(i=='\n'){
            longest = count>longest? count: longest;
            count = 0;
        }
    }
    rewind(file);
    return longest;
}

int compareIds(void* firstItem, void* secondItem){
    if(firstItem == NULL || secondItem == NULL){
        return 0;
    }
    pStudent student1 = (pStudent)firstItem;
    pStudent student2 = (pStudent)secondItem;
    unsigned int a = student1->m_studentID, b = student2->m_studentID;
    return a > b ? (int) (a - b) : (int)(b - a);
}

int nameCompareCaseSensitive(void* firstItem, void* secondItem){
    if(firstItem == NULL || secondItem == NULL){
        return 0;
    }
    pStudent student1 = (pStudent)firstItem;
    pStudent student2 = (pStudent)secondItem;
    int sumFirstStudent = valueCaseSensitive(student1->m_name) + valueCaseSensitive(student1->m_surname);
    int sumSecondStudent = valueCaseSensitive(student2->m_name) + valueCaseSensitive(student2->m_surname);
    return abs(sumFirstStudent - sumSecondStudent);
}

int nameCompareCaseInsensitive(void* firstItem, void* secondItem){
    if(firstItem == NULL || secondItem == NULL){
        return 0;
    }
    pStudent student1 = (pStudent)firstItem;
    pStudent student2 = (pStudent)secondItem;
    int sumFirstStudent = valueCaseInsensitive(student1->m_name) + valueCaseInsensitive(student1->m_surname);
    int sumSecondStudent = valueCaseInsensitive(student2->m_name) + valueCaseInsensitive(student2->m_surname);
    return abs(sumFirstStudent - sumSecondStudent);

}

int checkFileConnection(void* firstItem, void* secondItem){
    if(firstItem == NULL || secondItem == NULL){
        return 0;
    }
    pStudent student1 = (pStudent)firstItem;
    pStudent student2 = (pStudent)secondItem;
    return isFriendsByFile(student1,student2) + isRivalsByFile(student1,student2);
}

int compare(void* firstItem, void* secondItem){
    pStudent student1 = (pStudent)firstItem;
    pStudent student2 = (pStudent)secondItem;
    if(student2->m_studentID == student1->m_studentID){
        return 1;
    } else return 0;
}



static int valueCaseSensitive(char* str){
    int sum = 0;
    while(*str++){
        sum+= *str;
    }
    return sum;
}

static int valueCaseInsensitive(char* str){
    int sum = 0;
    while(*str++){
        if(*str >= 'a' && *str <='z'){
            sum+= *str;
        }
        else {
            sum+= *str- 'A' + 'a';
        }
    }
    return sum;
}

static int isFriendsByFile(pStudent student1, pStudent student2){
    printf("%u %u\n",student1->m_studentID,student2->m_studentID);
    if(student1->m_friendsStudents == NULL && student2->m_friendsStudents == NULL){
        printf("0");
        return 0;
    }
    int i=0;
    while (student1->m_friendsStudents[i] != 0 && student1->m_friendsStudents!=NULL){
        if(student1->m_friendsStudents[i] == student2->m_studentID){
            printf("20");
            return 20;
        }
        i++;
    }
    i=0;
    while (student2->m_friendsStudents[i] != 0 && student2->m_friendsStudents!=NULL){
        if(student2->m_friendsStudents[i] == student1->m_studentID){
            printf("20");
            return 20;
        }
        i++;
    }
    return 0;
}

static int isRivalsByFile(pStudent student1, pStudent student2){
    printf("%u %u\n",student1->m_studentID,student2->m_studentID);
    if(student1->m_rivalsStudents == NULL && student2->m_rivalsStudents == NULL){
        printf("0");
        return 0;
    }
    int i=0;
    while (student1->m_rivalsStudents[i] != 0 && student1->m_rivalsStudents!=NULL){
        if(student1->m_rivalsStudents[i] == student2->m_studentID){
            printf("-20");
            return -20;
        }
        i++;
    }
    i=0;
    while (student2->m_rivalsStudents[i] != 0 && student2->m_rivalsStudents!=NULL){
        if(student2->m_rivalsStudents[i] == student1->m_studentID){
            printf("-20");
            return -20;
        }
        i++;
    }
    return 0;
}

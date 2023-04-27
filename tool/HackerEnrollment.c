#include <stdlib.h>
#include <string.h>

#include "HackerEnrollment.h"
#include "IsraeliQueue.h"

#define CHARBUFFER 10240

/** NOTICE! this function can be called only from HackerEnrollment.c file.
 *  Create a students array for Enrollment System using the data in a file provided.
 *  In case of failure of any kind return false. */
static bool readStudentData(EnrollmentSystem system, FILE* studentsFile);

/** NOTICE! this function can be called only from HackerEnrollment.c file.
 *  Create a courses array for Enrollment System using the data in a file provided.
 *  In case of failure of any kind return false. */
static bool readCoursesData(EnrollmentSystem system, FILE* coursesFile);

/** NOTICE! this function can be called only from HackerEnrollment.c file.
 *  Create a hackers array for Enrollment System using the data in a file provided.
 *  In case of failure of any kind return false. */
static bool readHackersData(EnrollmentSystem system, FILE* hackersFile,int countCourses, int countStudents);

/** NOTICE! this function can be called only from HackerEnrollment.c file.
 *  Return a pointer to a course struct in EnrollmentSystem.
 *  If course isn't in the system NULL is returned */
static Course searchCourse (int courseID, EnrollmentSystem system);

/** NOTICE! this function can be called only from HackerEnrollment.c file.
 *  Return a pointer to a Student struct in EnrollmentSystem.
 *  If student isn't in the system NULL is returned */
static pStudent searchStudent (unsigned int studentID, EnrollmentSystem system);

static void printNotMatched(unsigned int arr[], FILE *out,int index);

static void printToFile(pHacker hacker, EnrollmentSystem system, FILE* out);

static int positionInArray(IsraeliQueue ilQueue, pStudent student);

static void printQueue(IsraeliQueue ilQueue, FILE* out);


EnrollmentSystem createEnrollment(FILE* students, FILE* courses, FILE* hackers){
    EnrollmentSystem newSystem = malloc(sizeof(*newSystem));
    if(!newSystem){
        return NULL;
    }
    bool readSuccess = readStudentData(newSystem,students);
    if(readSuccess == false){
        return NULL;
    }

    readSuccess = readCoursesData(newSystem, courses);
    if(readSuccess == false) {
        return NULL;
    }
    readSuccess = readHackersData(newSystem,hackers,
                                  newSystem->m_courseAmount,newSystem->m_studentsAmount);
    if(readSuccess == false) {
        return NULL;
    }
    int i=0;
    pHacker connectStudentToHacker = newSystem->m_hackerArray[i];
    while(connectStudentToHacker){
        int j=0;
        pStudent checkStudent = newSystem->m_studentsArray[j];
        while (checkStudent){
            if(connectStudentToHacker->m_hackerID == checkStudent->m_studentID){
                connectStudentToHacker->m_studentData = checkStudent;
                checkStudent->m_friendsStudents = connectStudentToHacker->m_friendsStudents;
                checkStudent->m_rivalsStudents = connectStudentToHacker->m_rivalsStudents;
            }
            j++;
            checkStudent = newSystem->m_studentsArray[j];
        }
        i++;
        connectStudentToHacker = newSystem->m_hackerArray[i];
    }

    return newSystem;
}

EnrollmentSystem readEnrollment(EnrollmentSystem system, FILE* queues){
    int courseID;
    unsigned int ID;
    char buffer[CHARBUFFER];
    while (fscanf(queues, "%d", &courseID)==1){
        Course currentCourse = searchCourse(courseID,system);
        ComparisonFunction compareFunction = compare;
        IsraeliQueue enrollmentQueue = IsraeliQueueCreate(NULL,compareFunction,FRIENDSHIPTHRESHOLD,RIVALYTHRESHOLD);
        fgets(buffer, CHARBUFFER, queues);
        char *token = strtok(buffer, " ");
        while (token){
            sscanf(token, "%u", &ID);
            pStudent currentStudent = searchStudent(ID,system);
            IsraeliQueueError errorCheck = IsraeliQueueEnqueue(enrollmentQueue,currentStudent);
            if(errorCheck!=ISRAELIQUEUE_SUCCESS){
                return NULL;
            }
            token = strtok(NULL, " ");
        }
        currentCourse->m_ilQueue = enrollmentQueue;
    }
    return system;
}


void hackEnrollment(EnrollmentSystem system, FILE* out) {
    if (system == NULL || out == NULL) {
        return;
    }
    unsigned int *notMatched = malloc(sizeof (unsigned int) * system->m_hackersAmount);
    if(!notMatched){
        return;
    }
    for (int i = 0; i < system->m_hackersAmount; i++){
        notMatched[i]=0;
    }
    int notMatchedIndex = 0;
    int constraintsMatched = 0;
    for (int i = 0; i<system->m_hackersAmount; i++){
        pHacker hacker = system->m_hackerArray[i];
        for(int j=0;j<hacker->m_amountDesired; j++) {
            Course desiredCourse = searchCourse(hacker->m_desiredCourses[j], system);
            IsraeliQueue courseQueue = desiredCourse->m_ilQueue;
            if (courseQueue == NULL && desiredCourse->m_courseSize > 1) {
                constraintsMatched++;
            } else {
                if (IsraeliQueueContains(courseQueue, hacker->m_studentData)) {
                    IsraeliQueueError errorCheck = IsraeliQueueImprovePositions(courseQueue);
                    if (errorCheck != ISRAELIQUEUE_SUCCESS) {
                        return;
                    }
                } else {
                    IsraeliQueueError errorCheck = IsraeliQueueEnqueue(courseQueue, hacker->m_studentData);
                    if (errorCheck != ISRAELIQUEUE_SUCCESS) {
                        return;
                    }

                }
                int position = positionInArray(courseQueue, hacker->m_studentData);
                if (position == -1) {
                    return;
                }
                if (position < desiredCourse->m_courseSize) {
                    constraintsMatched++;
                }
            }
        }
        if (constraintsMatched < 2 && hacker->m_amountDesired >= 2) {
            notMatched[notMatchedIndex++] = hacker->m_hackerID;
        } else {
            printToFile(hacker,system,out);
        }
    }
    printNotMatched(notMatched,out,notMatchedIndex);
}

static void printNotMatched(unsigned int arr[], FILE *out, int index){
    if(arr[0] == 0 || out == NULL){
        return;
    }
    fprintf(out,"Cannot satisfy constraints for ");
    for (int i=0; i < index; i++){
        fprintf(out,"%u ",arr[i]);
    }
}

static void printToFile(pHacker hacker, EnrollmentSystem system, FILE* out) {
    if(system == NULL || out==NULL || hacker== NULL){
        return;
    }
    for (int i=0; i<hacker->m_amountDesired; i++){
        Course desiredCourse = searchCourse(hacker->m_desiredCourses[i], system);
        IsraeliQueue courseQueue = desiredCourse->m_ilQueue;
        if (courseQueue == NULL && desiredCourse->m_courseSize > 1) {
            fprintf(out,"%d %u\n",desiredCourse->m_courseID,hacker->m_hackerID);
        } else {
            int position = positionInArray(courseQueue, hacker->m_studentData);
            if (position < desiredCourse->m_courseSize && position != -1){
                fprintf(out,"%d ", desiredCourse->m_courseID);
                printQueue(courseQueue,out);
            }
        }
    }
}

static void printQueue(IsraeliQueue ilQueue, FILE* out){
    int size = IsraeliQueueSize(ilQueue);
    for (int i=0; i<size; i++){
        pStudent student = (pStudent) IsraeliQueueDequeue(ilQueue);
        fprintf(out,"%u ", student->m_studentID);
    }
}

static int positionInArray(IsraeliQueue ilQueue, pStudent student){
    IsraeliQueue clonedQueue = IsraeliQueueClone(ilQueue);
    if (!clonedQueue){
        return -1;
    }
    int count = 0;
    while (IsraeliQueueContains(clonedQueue,student)){
        IsraeliQueueDequeue(clonedQueue);
        count++;
    }
    IsraeliQueueDestroy(clonedQueue);
    return count;
}


static bool readStudentData(EnrollmentSystem system, FILE* studentsFile){
    int countStudents = countLinesInFile(studentsFile);
    system->m_studentsAmount = countStudents;
    pStudent *studentsArray = malloc(sizeof(pStudent) * (countStudents+1));
    if (!studentsArray){
        return false;
    }
    studentsArray[countStudents] = NULL;
    unsigned int studentID;
    int studentGPA,totalCredits;
    char name[CHARBUFFER], surname[CHARBUFFER], city[CHARBUFFER], department[CHARBUFFER];
    for (int i = 0; i < countStudents; i++) {
        fscanf(studentsFile,"%u %d %d %s %s %s %s",&studentID,&totalCredits,
               &studentGPA,name,surname,city,department);
        pStudent newStudent = createStudent(studentID,totalCredits,studentGPA,name,surname,city,department,NULL ,NULL);
        studentsArray[i] = newStudent;
    }
    system->m_studentsArray = studentsArray;
    rewind(studentsFile);
    return true;
}

static bool readCoursesData(EnrollmentSystem system, FILE* coursesFile){
    int countCourses = countLinesInFile(coursesFile);
    system->m_courseAmount = countCourses;
    Course *coursesArray = malloc(sizeof(Course) * (countCourses+1));
    if (!coursesArray){
        return false;
    }
    coursesArray[countCourses] = NULL;
    int courseID, courseSize;
    for (int i = 0; i < countCourses; i++) {
        fscanf(coursesFile,"%d %d",&courseID,&courseSize);
        Course newCourse = createCourse(courseID,courseSize,NULL);
        coursesArray[i] = newCourse;
    }
    rewind(coursesFile);
    system->m_coursesArray = coursesArray;
    return true;
}

static bool readHackersData(EnrollmentSystem system, FILE* hackersFile,int countCourses, int countStudents){
    int countHackers = countLinesInFile(hackersFile)/4;
    system->m_hackersAmount = countHackers;
    pHacker* hackersArray = malloc(sizeof(pHacker) * (countHackers+1));
    if(!hackersArray){
        return false;
    }
    int hackerIndex = 0;
    int *coursesDesired = malloc(sizeof(int) * countCourses);
    if(!coursesDesired){
        return false;
    }
    for(int i=0;i<countCourses;i++){
        coursesDesired[i]=0;
    }
    unsigned int *rivalsStudents = malloc(sizeof(unsigned int)*countStudents);
    if(!rivalsStudents){
        return false;
    }
    unsigned int *friendsStudents = malloc(sizeof(unsigned int)*countStudents);
    if(!friendsStudents){
        return false;
    }
    for(int i=0;i<countStudents;i++){
        friendsStudents[i]=0;
        rivalsStudents[i]=0;
    }
    char buffer[CHARBUFFER];
    unsigned int hackerId, helperBuffer;
    int course, a=0, b=0, c=0; //a - index for courses array b - for rivals c - for friends
    for(int i=0; i<countHackers; i++) {
        for (int j = 0; j < 4; j++) {
            fgets(buffer, CHARBUFFER, hackersFile);
            char *token = strtok(buffer, " ");
            while (token) {
                switch (j) {
                    case 0:
                        sscanf(token, "%u", &hackerId);
                        break;
                    case 1:
                        sscanf(token, "%d", &course);
                        coursesDesired[a++] = course;
                        break;
                    case 2:
                        sscanf(token, "%u", &helperBuffer);
                        friendsStudents[c++] = helperBuffer;
                        break;
                    case 3:
                        sscanf(token, "%u", &helperBuffer);
                        rivalsStudents[b++] = helperBuffer;
                        break;
                }
                token = strtok(NULL, " ");
            }
        }
        coursesDesired[a] = -1;
        friendsStudents[c] = 0;
        rivalsStudents[b] = 0;
        pHacker newHacker = createHacker(hackerId,coursesDesired,a,friendsStudents,rivalsStudents,NULL);
        hackersArray[hackerIndex++] = newHacker;
    }
    hackersArray[hackerIndex] = NULL;
    system->m_hackerArray = hackersArray;
    return true;
}

static Course searchCourse (int courseID, EnrollmentSystem system){
    Course currentCourse = system->m_coursesArray[0];
    int index = 1;
    while (currentCourse->m_courseID != courseID && currentCourse != NULL){
        currentCourse = system->m_coursesArray[index++];
    }
    return currentCourse;
}

static pStudent searchStudent (unsigned int studentID, EnrollmentSystem system){
    for(int i=0; i < system->m_studentsAmount; i++){
        pStudent student =system->m_studentsArray[i];
        if(studentID == student->m_studentID){
            return student;
        }
    }
    return NULL;
}

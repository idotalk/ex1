#include <stdlib.h>
#include <string.h>

#include "HackerEnrollment.h"
#include "IsraeliQueue.h"


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

/** NOTICE! this function can be called only from HackerEnrollment.c file.
 *  Print to file if couldn't match at least one hacker demands. */
static void printNotMatched(unsigned int arr[], FILE *out,int index);

/** NOTICE! this function can be called only from HackerEnrollment.c file.
 *  Returns the foremost position of the student in a queue by ID.
 *  In case of failure return -1 */
static int positionInQueue(IsraeliQueue ilQueue, pStudent student);

/** NOTICE! this functions can be called only from HackerEnrollment.c file.
 *  In case of success in Hack enrollment, print to file all the courses enrollment queues. */
static void printToFile(EnrollmentSystem system, FILE* out);
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
    int courseID, longestLine = longestLineInFile(queues);
    unsigned int ID;
    char *buffer = malloc(longestLine + 1);
    if(!buffer){
        return NULL;
    }
    while (fscanf(queues, "%d", &courseID)==1){
        Course currentCourse = searchCourse(courseID,system);
        ComparisonFunction compareFunction = compare;
        IsraeliQueue enrollmentQueue = IsraeliQueueCreate(NULL,compareFunction,FRIENDSHIPTHRESHOLD,RIVALYTHRESHOLD);
        fgets(buffer, longestLine + 1, queues);
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
    free(buffer);
    return system;
}


void hackEnrollment(EnrollmentSystem system, FILE* out) {
    if (system == NULL || out == NULL) {
        return;
    }
    unsigned int *notMatched = malloc(sizeof (unsigned int) * system->m_hackersAmount);
    if(!notMatched){
        exit(1); //Out of memory - void function at the end of main.
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
                IsraeliQueue newQueue = IsraeliQueueCreate(NULL,NULL,FRIENDSHIPTHRESHOLD,RIVALYTHRESHOLD);
                if(!newQueue){
                    exit(1); //Out of memory - void function at the end of main.
                }
                pStudent currentStudent = searchStudent(hacker->m_hackerID,system);
                IsraeliQueueError errorCheck = IsraeliQueueEnqueue(newQueue,currentStudent);
                if(errorCheck!=ISRAELIQUEUE_SUCCESS){
                    exit(1); //Failure occurred - void function at the end of main.
                }
                desiredCourse->m_ilQueue = newQueue;
                constraintsMatched++;
            } else {
                IsraeliQueueError errorCheck = IsraeliQueueEnqueue(courseQueue, hacker->m_studentData);
                if (errorCheck != ISRAELIQUEUE_SUCCESS) {
                    exit(1); //Out of memory - void function at the end of main.
                }
                int position = positionInQueue(courseQueue, hacker->m_studentData);
                if (position == -1) {
                    exit(1); //Out of memory - void function at the end of main.
                }
                if (position < desiredCourse->m_courseSize) {
                    constraintsMatched++;
                }
            }
        }
        if ((constraintsMatched < 2 && hacker->m_amountDesired >= 2) ||
        (constraintsMatched == 0 && hacker->m_amountDesired == 1)) {
            notMatched[notMatchedIndex++] = hacker->m_hackerID;
        }
    }
    if(notMatchedIndex > 0){
        printNotMatched(notMatched,out,notMatchedIndex);
    } else {
        printToFile(system,out);
    }
    free(notMatched);
}

void addFriendshipFunctions(EnrollmentSystem system, int offset){
    Course course = system->m_coursesArray[0];
    int i=1;
    while(course != NULL){
        if(course->m_ilQueue != NULL){
            IsraeliQueue courseQueue = course->m_ilQueue;
            IsraeliQueueAddFriendshipMeasure(courseQueue,(FriendshipFunction)compareIds);
            IsraeliQueueAddFriendshipMeasure(courseQueue,(FriendshipFunction)checkFileConnection);
            if (offset == 1){
                IsraeliQueueAddFriendshipMeasure(courseQueue,(FriendshipFunction)nameCompareCaseInsensitive);
            }
            else{
                IsraeliQueueAddFriendshipMeasure(courseQueue,(FriendshipFunction)nameCompareCaseSensitive);
            }

        }
        course = system->m_coursesArray[i++];
    }
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

static void printToFile(EnrollmentSystem system, FILE* out) {
    if(system == NULL || out==NULL){
        return;
    }
    for (int i=0; i< system->m_courseAmount ; i++){
        Course courseToPrint = system->m_coursesArray[i];
        IsraeliQueue courseQueue = courseToPrint->m_ilQueue;
        if(courseQueue != NULL){
            fprintf(out,"%d ", courseToPrint->m_courseID);
            printQueue(courseQueue,out);
        }
        fprintf(out,"\n");
    }
}

static void printQueue(IsraeliQueue ilQueue, FILE* out){
    int size = IsraeliQueueSize(ilQueue);
    for (int i=0; i<size; i++){
        pStudent student = (pStudent) IsraeliQueueDequeue(ilQueue);
        fprintf(out,"%u ", student->m_studentID);
    }
}

static int positionInQueue(IsraeliQueue ilQueue, pStudent student){
    IsraeliQueue clonedQueue = IsraeliQueueClone(ilQueue);
    if (!clonedQueue){
        return -1;
    }
    int count = 0;
    while (IsraeliQueueContains(clonedQueue,student)){
        pStudent helper = (pStudent)IsraeliQueueDequeue(clonedQueue);
        count++;
        if(helper->m_studentID == student->m_studentID) {
            break;
        }
    }
    IsraeliQueueDestroy(clonedQueue);
    return count;
}

static bool readStudentData(EnrollmentSystem system, FILE* studentsFile){
    int countStudents = countLinesInFile(studentsFile);
    int longestLine = longestLineInFile(studentsFile);
    system->m_studentsAmount = countStudents;
    pStudent *studentsArray = malloc(sizeof(pStudent) * (countStudents+1));
    if (!studentsArray){
        return false;
    }
    studentsArray[countStudents] = NULL;
    unsigned int studentID;
    int studentGPA,totalCredits;
    char *name = malloc(longestLine + 1);
    char *surname = malloc(longestLine + 1);
    char *city = malloc(longestLine + 1);
    char *department = malloc(longestLine + 1);
    if(!name || !surname || !city || !department){
        return false;
    }
    for (int i = 0; i < countStudents; i++) {
        fscanf(studentsFile,"%u %d %d %s %s %s %s",&studentID,&totalCredits,
               &studentGPA,name,surname,city,department);
        pStudent newStudent = createStudent(studentID,totalCredits,studentGPA,name,surname,city,department,NULL ,NULL);
        studentsArray[i] = newStudent;
    }
    free(name);
    free(surname);
    free(city);
    free(department);
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
    int longestLine = longestLineInFile(hackersFile);
    system->m_hackersAmount = countHackers;
    pHacker* hackersArray = malloc(sizeof(pHacker) * (countHackers+1));
    if(!hackersArray){
        return false;
    }
    char *buffer = malloc(longestLine + 1);
    if(!buffer){
        return false;
    }
    unsigned int hackerId, helperBuffer;
    int hackerIndex = 0 ,course, a=0, b=0, c=0; //a - index for courses array b - for rivals c - for friends
    for(int i=0; i<countHackers; i++) {
        int *coursesDesired = malloc(sizeof(int) * countCourses);
        unsigned int *rivalsStudents = malloc(sizeof(unsigned int)*countStudents);
        unsigned int *friendsStudents = malloc(sizeof(unsigned int)*countStudents);
        if(!rivalsStudents || !friendsStudents || !coursesDesired){
            return false;
        }
        for(int q=0; q<countCourses; q++){
            coursesDesired[q]=-1;
        }
        for(int k=0;k<countStudents;k++){
            friendsStudents[k]=0;
            rivalsStudents[k]=0;
        }
        for (int j = 0; j < 4; j++) {
            fgets(buffer, longestLine + 1 , hackersFile);
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
        pHacker newHacker = createHacker(hackerId,coursesDesired,a,friendsStudents,rivalsStudents,NULL);
        hackersArray[hackerIndex++] = newHacker;
    }
    hackersArray[hackerIndex] = NULL;
    system->m_hackerArray = hackersArray;
    free(buffer);
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

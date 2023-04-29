#ifndef HACKERENROLLMENT_H
#define HACKERENROLLMENT_H

#include <stdio.h>
#include "Utility.h"

#define FRIENDSHIPTHRESHOLD 20
#define RIVALYTHRESHOLD 0


/** Struct declaration - All fields are arrays of structs, each array ends with NULL.
 * @see: Utility.H for more information about students, hacker and courses structs.  */
typedef struct EnrollmentSystem {
    pStudent* m_studentsArray;
    Course* m_coursesArray;
    pHacker* m_hackerArray;
    int m_courseAmount;
    int m_studentsAmount;
    int m_hackersAmount;
}* EnrollmentSystem;

/** Creates a new EnrollmentSystem object based on the data provided by the files.
 *  @param Students, Courses, Hackers data files
 *  In case of failure, return NULL. */
EnrollmentSystem createEnrollment(FILE* , FILE*, FILE*);

/** Reads courses queues data and update a provided EnrollmentSystem object according to it.
 *  @param EnrollmentSystem: System object.
 *  @param Queues: Queues data file.
 *  In case of failure, return NULL. */
EnrollmentSystem readEnrollment(EnrollmentSystem, FILE*);

/**  Update the status of the queues in an EnrollmentSystem according to the Hackers demands.
 *   Copy the updated Status to a chosen stream.
 *  @param EnrollmentSystem: System object.
 *  @param Out: stream file.
 *  In case of failure an error message would be printed to stderr and the function will abort. */
void hackEnrollment(EnrollmentSystem, FILE*);

void addFriendshipFunctions(EnrollmentSystem system, int offset);

#endif //HACKERENROLLMENT_H

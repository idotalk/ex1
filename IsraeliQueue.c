#include <stdio.h>
#include "IsraeliQueue.h"
#include "Node.h"

typedef struct IsraeliQueue_t {
    FriendshipFunction*  friendshipFunctions;
    ComparisonFunction*  compareFunction;
    int friendshipThreshold;
    int rivalryThreshold;
    struct Node *head;
    struct Node *tail;
}* IsraeliQueue;

/**Creates a new IsraeliQueue_t object with the provided friendship functions, a NULL-terminated array,
 * comparison function, friendship threshold and rivalry threshold. Returns a pointer
 * to the new object. In case of failure, return NULL.*/
IsraeliQueue IsraeliQueueCreate(FriendshipFunction *functionsArray, ComparisonFunction cmpFunction, int friendship_th, int rivalry_th){
    IsraeliQueue newQueue = malloc(sizeof(*newQueue));
    if(!newQueue){
        return NULL;
    }
    newQueue->friendshipFunctions = functionsArray;
    newQueue->compareFunction = cmpFunction;
    newQueue->friendshipThreshold - friendship_th;
    newQueue->rivalryThreshold = rivalry_th;
    newQueue->head = NULL;
    newQueue->tail = NULL;
    return newQueue;
}

/**Returns a new queue with the same elements as the parameter. If the parameter is NULL,
 * NULL is returned.*/
IsraeliQueue IsraeliQueueClone(IsraeliQueue queueToClone){
    IsraeliQueue clonedQueue = malloc(sizeof(*clonedQueue));
    if(!clonedQueue){
        return NULL;
    }
    clonedQueue->friendshipFunctions = queueToClone->friendshipFunctions;
    clonedQueue->compareFunction = queueToClone->compareFunction;
    clonedQueue->friendshipThreshold - queueToClone->friendshipThreshold;
    clonedQueue->rivalryThreshold = queueToClone->rivalryThreshold;
    clonedQueue->head = queueToClone->head;;
    clonedQueue->tail = queueToClone->tail;;
    return clonedQueue;
}

/**@param IsraeliQueue: an IsraeliQueue created by IsraeliQueueCreate
 *
 * Deallocates all memory allocated by IsraeliQueueCreate for the object pointed to by
 * the parameter.*/
void IsraeliQueueDestroy(IsraeliQueue ilQueue){
    destroyFromStart(ilQueue->head);
}

/**@param IsraeliQueue: an IsraeliQueue in which to insert the item.
 * @param item: an item to enqueue
 *
 * Places the item in the foremost position accessible to it.*/
IsraeliQueueError IsraeliQueueEnqueue(IsraeliQueue, void *);

/**@param IsraeliQueue: an IsraeliQueue to which the function is to be added
 * @param FriendshipFunction: a FriendshipFunction to be recognized by the IsraeliQueue
 * going forward.
 *
 * Makes the IsraeliQueue provided recognize the FriendshipFunction provided.*/
IsraeliQueueError IsraeliQueueAddFriendshipMeasure(IsraeliQueue ilQueue, FriendshipFunction newFriendshipFunction){
    if(ilQueue == NULL) {
        return ISRAELIQUEUE_BAD_PARAM;
    }
    int currentFunctionsAmount = 0;
    while (ilQueue->friendshipFunctions[currentFunctionsAmount] != NULL){
        currentFunctionsAmount++;
    }
    FriendshipFunction *newFunctionsArray = malloc(sizeof(FriendshipFunction) * (currentFunctionsAmount + 1) );
    if (!newFunctionsArray) {
        return ISRAELIQUEUE_ALLOC_FAILED;
    }
    newFunctionsArray[currentFunctionsAmount + 1] = newFriendshipFunction;
    int index = 0;
    while (ilQueue->friendshipFunctions[index] != NULL){
        newFunctionsArray[index] = ilQueue->friendshipFunctions[index];
        index++;
    }
    free (ilQueue->friendshipFunctions);
    ilQueue->friendshipFunctions = newFunctionsArray;
    return ISRAELIQUEUE_SUCCESS;
}

/**@param IsraeliQueue: an IsraeliQueue whose friendship threshold is to be modified
 * @param friendship_threshold: a new friendship threshold for the IsraeliQueue*/
IsraeliQueueError IsraeliQueueUpdateFriendshipThreshold(IsraeliQueue ilQueue, int newFriendshipTh){
    if(ilQueue == NULL) {
        return ISRAELIQUEUE_BAD_PARAM;
    }
    ilQueue->friendshipThreshold = newFriendshipTh;
    return ISRAELIQUEUE_SUCCESS;
}

/**@param IsraeliQueue: an IsraeliQueue whose rivalry threshold is to be modified
 * @param friendship_threshold: a new rivalry threshold for the IsraeliQueue*/
IsraeliQueueError IsraeliQueueUpdateRivalryThreshold(IsraeliQueue ilQueue, int newRivalryTh){
    if(ilQueue == NULL) {
        return ISRAELIQUEUE_BAD_PARAM;
    }
    ilQueue->rivalryThreshold = newRivalryTh;
    return ISRAELIQUEUE_SUCCESS;
}

/**Returns the number of elements of the given queue. If the parameter is NULL, 0
 * is returned.*/
int IsraeliQueueSize(IsraeliQueue ilQueue){
    if(ilQueue == NULL || (ilQueue->head == NULL && ilQueue->tail == NULL)) {
        return 0;
    }
    int count=0;
    Node temp = ilQueue->head;
    while (temp != NULL) {
        count++;
        temp = temp->m_nextNode;
    }
    return count;
}

/**Removes and returns the foremost element of the provided queue. If the parameter
 * is NULL or a pointer to an empty queue, NULL is returned.*/
void* IsraeliQueueDequeue(IsraeliQueue);

/**@param item: an object comparable to the objects in the IsraeliQueue
 *
 * Returns whether the queue contains an element equal to item. If either
 * parameter is NULL, false is returned.*/
bool IsraeliQueueContains(IsraeliQueue, void *);

/**Advances each item in the queue to the foremost position accessible to it,
 * from the back of the queue frontwards.*/
IsraeliQueueError IsraeliQueueImprovePositions(IsraeliQueue);

/**@param q_arr: a NULL-terminated array of IsraeliQueues
 * @param ComparisonFunction: a comparison function for the merged queue
 *
 * Merges all queues in q_arr into a single new queue, with parameters the parameters described
 * in the exercise. Each queue in q_arr enqueues its head in the merged queue, then lets the next
 * one enqueue an item, in the order defined by q_arr. In the event of any error during execution, return NULL.*/
IsraeliQueue IsraeliQueueMerge(IsraeliQueue*,ComparisonFunction);


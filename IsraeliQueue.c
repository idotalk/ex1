#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "IsraeliQueue.h"
#include "Node.h"


typedef struct IsraeliQueue_t {
    FriendshipFunction*  m_friendshipFunctions;
    ComparisonFunction  m_compareFunction;
    int m_friendshipThreshold;
    int m_rivalryThreshold;
    struct Node *m_head;
    struct Node *m_tail;
}* IsraeliQueue;

/**Creates a new IsraeliQueue_t object with the provided friendship functions, a NULL-terminated array,
 * comparison function, friendship threshold and rivalry threshold. Returns a pointer
 * to the new object. In case of failure, return NULL.*/
IsraeliQueue IsraeliQueueCreate(FriendshipFunction *functionsArray, ComparisonFunction cmpFunction, int friendship_th, int rivalry_th){
    if(functionsArray == NULL || cmpFunction == NULL){ //invalid parameter.
        return NULL;
    }
    IsraeliQueue newQueue = malloc(sizeof(*newQueue));
    if(!newQueue){
        return NULL;
    }
    newQueue->m_friendshipFunctions = functionsArray;
    newQueue->m_compareFunction = cmpFunction;
    newQueue->m_friendshipThreshold = friendship_th;
    newQueue->m_rivalryThreshold = rivalry_th;
    newQueue->m_head = NULL;
    newQueue->m_tail = NULL;
    return newQueue;
}

/** Cloning an Israeli Queue and returns pointer to a new similar queue.
 *  This is a deep copy which means the function allocate new memory for the new queue.
 *  Any change at the old queue won't affect the new one and vice versa.
 *  In case of any error return NULL */
IsraeliQueue IsraeliQueueClone(IsraeliQueue ilQueue){
    if (ilQueue == NULL){
        return NULL;
    }
    //* Allocating new queue.
    IsraeliQueue clonedQueue = malloc(sizeof(*clonedQueue));
    if(!clonedQueue){
        return NULL;
    }
    //* Calculate copy and allocate space for a new friendship functions array to the new queue.
    int FunctionsAmount = 0;
    while (ilQueue->m_friendshipFunctions[FunctionsAmount] != NULL){
        FunctionsAmount++;
    }
    FriendshipFunction *newFunctionsArray = malloc(sizeof(FriendshipFunction) * FunctionsAmount);
    if (!newFunctionsArray) {
        return NULL;
    }
    int index = 0;
    while (ilQueue->m_friendshipFunctions[index] != NULL){
        newFunctionsArray[index] = ilQueue->m_friendshipFunctions[index];
        index++;
    }   
    clonedQueue->m_friendshipFunctions = newFunctionsArray;
    clonedQueue->m_compareFunction = ilQueue->m_compareFunction;
    clonedQueue->m_friendshipThreshold = ilQueue->m_friendshipThreshold;
    clonedQueue->m_rivalryThreshold = ilQueue->m_rivalryThreshold;
    Node helperNode = ilQueue->m_head, temp = NULL;
    while(helperNode){
        Node clonedNode = createNode(helperNode->m_data, helperNode->m_remainedFriends, helperNode->m_reminedRivals);
        if(!clonedNode){
            return NULL;
        }
        clonedNode->m_prevNode = temp;
        if (clonedQueue->m_head == NULL){
            clonedQueue->m_head=clonedNode;
        }
        else {
        connectNodes(temp, clonedNode);
        }
        temp = clonedNode;
        helperNode = helperNode->m_nextNode;
    }
    clonedQueue->m_tail = temp;
    assert(clonedQueue->m_head->m_prevNode == NULL);
    assert(clonedQueue->m_tail->m_nextNode == NULL);
    return clonedQueue;
}

/**@param IsraeliQueue: an IsraeliQueue created by IsraeliQueueCreate
 *
 * Deallocates all memory allocated by IsraeliQueueCreate for the object pointed to by
 * the parameter.*/
void IsraeliQueueDestroy(IsraeliQueue ilQueue){
    destroyFromStart(ilQueue->m_head);
    free (ilQueue);
}

/**@param IsraeliQueue: an IsraeliQueue in which to insert the item.
 * @param item: an item to enqueue
 *
 * Places the item in the foremost position accessible to it.*/
IsraeliQueueError IsraeliQueueEnqueue(IsraeliQueue ilQueue, void * item){
    if(ilQueue == NULL || IsraeliQueueContains(item, ilQueue->m_compareFunction)){
        return ISRAELIQUEUE_BAD_PARAM;
    }
    Node newNode = createNode(item, FRIEND_QUOTA, RIVAL_QUOTA);
    if(!newNode) {
        return ISRAELIQUEUE_ALLOC_FAILED;
    }
    if(ilQueue->m_head == NULL){ //* Queue is empty, place the new node at the beginning of the queue.
        ilQueue->m_head = newNode;
        ilQueue->m_tail = newNode;
        return ISRAELIQUEUE_SUCCESS;
    }

    Node searchFriend = ilQueue->m_head;
    while (searchFriend){
        int i = 0;
        //* Searching for the first friend in the queue.
        while(ilQueue->m_friendshipFunctions[i]){
            FriendshipFunction function = ilQueue->m_friendshipFunctions[i];
            if(function(newNode, searchFriend->m_data) > ilQueue->m_friendshipThreshold){
                Node searchRival = searchFriend->m_nextNode;
                //* Searching for possible blocking rival.
                while (searchRival != NULL){
                    int j = 0, friendshipValue = 0, currentValue;
                    while(ilQueue->m_friendshipFunctions[j]){
                        FriendshipFunction function2 = ilQueue->m_friendshipFunctions[j];
                        currentValue = function2(newNode, searchRival);
                        if(currentValue > ilQueue->m_friendshipThreshold) {
                            break;
                        }
                        friendshipValue += 
                        j++;
                    }
                    if(currentValue > ilQueue->m_friendshipThreshold) {
                        searchRival = searchRival->m_nextNode;
                    }
                    else{
                        double average = (double)friendshipValue / (double)j;
                        if(average < ilQueue->m_rivalryThreshold){
                            searchRival->m_reminedRivals--;
                            break;
                        }
                    }
                    searchRival = searchRival->m_nextNode;
                }
                //* If we didnt find rivals we connect the new node after it's friend.
                if(searchRival == NULL) {
                    connectNodes(searchFriend,newNode);
                    if(ilQueue->m_tail == searchFriend){
                        ilQueue->m_tail = newNode;
                    }
                    searchFriend->m_remainedFriends--;
                }
            }
            i++;
        }
        searchFriend = searchFriend->m_nextNode;
    }
    if(searchFriend == NULL) {
        connectNodes(ilQueue->m_tail, newNode);
        ilQueue->m_tail = newNode;
    }
    return ISRAELIQUEUE_SUCCESS;
}

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
    while (ilQueue->m_friendshipFunctions[currentFunctionsAmount] != NULL){
        currentFunctionsAmount++;
    }
    FriendshipFunction *newFunctionsArray = malloc(sizeof(FriendshipFunction) * (currentFunctionsAmount + 1) );
    if (!newFunctionsArray) {
        return ISRAELIQUEUE_ALLOC_FAILED;
    }
    newFunctionsArray[currentFunctionsAmount + 1] = newFriendshipFunction;
    int index = 0;
    while (ilQueue->m_friendshipFunctions[index] != NULL){
        newFunctionsArray[index] = ilQueue->m_friendshipFunctions[index];
        index++;
    }
    free (ilQueue->m_friendshipFunctions);
    ilQueue->m_friendshipFunctions = newFunctionsArray;
    return ISRAELIQUEUE_SUCCESS;
}

/**@param IsraeliQueue: an IsraeliQueue whose friendship threshold is to be modified
 * @param friendship_threshold: a new friendship threshold for the IsraeliQueue*/
IsraeliQueueError IsraeliQueueUpdateFriendshipThreshold(IsraeliQueue ilQueue, int newFriendshipTh){
    if(ilQueue == NULL) {
        return ISRAELIQUEUE_BAD_PARAM;
    }
    ilQueue->m_friendshipThreshold = newFriendshipTh;
    return ISRAELIQUEUE_SUCCESS;
}

/**@param IsraeliQueue: an IsraeliQueue whose rivalry threshold is to be modified
 * @param friendship_threshold: a new rivalry threshold for the IsraeliQueue*/
IsraeliQueueError IsraeliQueueUpdateRivalryThreshold(IsraeliQueue ilQueue, int newRivalryTh){
    if(ilQueue == NULL) {
        return ISRAELIQUEUE_BAD_PARAM;
    }
    ilQueue->m_rivalryThreshold = newRivalryTh;
    return ISRAELIQUEUE_SUCCESS;
}

/**Returns the number of elements of the given queue. If the parameter is NULL, 0
 * is returned.*/
int IsraeliQueueSize(IsraeliQueue ilQueue){
    if(ilQueue == NULL || (ilQueue->m_head == NULL && ilQueue->m_tail == NULL)) {
        return 0;
    }
    int count=0;
    Node temp = ilQueue->m_head;
    while (temp != NULL) {
        count++;
        temp = temp->m_nextNode;
    }
    return count;
}

/**Removes and returns the foremost element of the provided queue. If the parameter
 * is NULL or a pointer to an empty queue, NULL is returned.*/
void* IsraeliQueueDequeue(IsraeliQueue ilQueue){
    if(ilQueue == NULL || (ilQueue->m_head == NULL && ilQueue->m_tail == NULL)) {
        return NULL;
    }
    Node nextInQueue = ilQueue->m_head->m_nextNode;
    void* item = ilQueue->m_head->m_data;
    free(ilQueue->m_head);
    ilQueue->m_head = nextInQueue;
    return item;
}

/**@param item: an object comparable to the objects in the IsraeliQueue
 *
 * Returns whether the queue contains an element equal to item. If either
 * parameter is NULL, false is returned.*/
bool IsraeliQueueContains(IsraeliQueue ilQueue, void * item){
    if(ilQueue == NULL || item == NULL){
        return false;
    }
    Node helper = ilQueue->m_head;
    ComparisonFunction cmpFunction = ilQueue->m_compareFunction;
    while (helper != NULL){
            if(cmpFunction(item,helper->m_data)){
                return 1;
            }
            helper = helper->m_nextNode;
    }
    return 0;
}

/**Advances each item in the queue to the foremost position accessible to it,
 * from the back of the queue frontwards.*/
IsraeliQueueError IsraeliQueueImprovePositions(IsraeliQueue ilQueue){
    if(ilQueue == NULL || (ilQueue->m_head == NULL && ilQueue->m_tail == NULL) || ilQueue->m_head->m_nextNode == NULL) {
        return ISRAELIQUEUE_SUCCESS;
    }
    IsraeliQueue updatedQueue = IsraeliQueueCreate(ilQueue->m_friendshipFunctions,
                                                    ilQueue->m_compareFunction ,
                                                    ilQueue->m_friendshipThreshold,
                                                    ilQueue->m_rivalryThreshold );
    if(!updatedQueue){
        return ISRAELIQUEUE_ALLOC_FAILED;
    }
    Node helperNode = ilQueue->m_head;
    while (helperNode){
        IsraeliQueueError error = IsraeliQueueEnqueue(updatedQueue, helperNode->m_data);
        if(error != ISRAELIQUEUE_SUCCESS){
            return error;
        }
        updatedQueue->m_tail->m_remainedFriends = helperNode->m_remainedFriends;
        updatedQueue->m_tail->m_reminedRivals = helperNode->m_reminedRivals;
        helperNode = helperNode->m_nextNode;
    }
    IsraeliQueue temp = ilQueue;
    ilQueue = updatedQueue;
    IsraeliQueueDestroy(temp);
    return ISRAELIQUEUE_SUCCESS;
}

/**@param q_arr: a NULL-terminated array of IsraeliQueues
 * @param ComparisonFunction: a comparison function for the merged queue
 *
 * Merges all queues in q_arr into a single new queue, with parameters the parameters described
 * in the exercise. Each queue in q_arr enqueues its m_head in the merged queue, then lets the next
 * one enqueue an item, in the order defined by q_arr. In the event of any error during execution, return NULL.*/
IsraeliQueue IsraeliQueueMerge(IsraeliQueue* ilQueueArray, ComparisonFunction cmpFunction){
    if (ilQueueArray == NULL || cmpFunction == NULL){
        return NULL;
    }

    IsraeliQueue mergedQueue = malloc(sizeof(*mergedQueue));
    if(!mergedQueue){
        return NULL;
    }
    mergedQueue->m_compareFunction = cmpFunction;
    //* Calculating new thresholds.
    int queuesAmount = 0, sumFriendshipThreshold = 0, multiplyRivalThreshold = 1, maxQueueLength = 0;
    while(ilQueueArray[queuesAmount] != NULL){
        IsraeliQueue currentQueue = ilQueueArray[queuesAmount];
        sumFriendshipThreshold += currentQueue->m_friendshipThreshold;
        multiplyRivalThreshold *= currentQueue->m_rivalryThreshold;
        queuesAmount++;
        int len = IsraeliQueueSize(ilQueueArray[queuesAmount]);
        maxQueueLength = len > maxQueueLength ? len : maxQueueLength;
    }
    double averageFriendshipThreshold = (double)sumFriendshipThreshold / (double)queuesAmount;
    int newFriendshipThreshold = (int)ceil(averageFriendshipThreshold);
    double geoAverageRivalThreshold = pow(multiplyRivalThreshold, 1/(double)queuesAmount);
    int newRivalThreshold = (int)ceil(geoAverageRivalThreshold);
    
    mergedQueue->m_friendshipThreshold = newFriendshipThreshold;
    mergedQueue->m_rivalryThreshold = newRivalThreshold;
    
    //* Building a new array of friendship functions.
    int countFunction = 0;
    for(int i = 0; i < queuesAmount; i++){
        IsraeliQueue temp = ilQueueArray[i];
        int j=0;
        while (temp->m_friendshipFunctions[j]){
            countFunction++;
            j++;
        }
    }
    FriendshipFunction* newArray = malloc(sizeof(FriendshipFunction) * countFunction);
    if(!newArray){
        return NULL;
    }
    int index = 0;
    for(int i = 0; i < queuesAmount; i++){
        IsraeliQueue temp = ilQueueArray[i];
        int j=0;
        while (temp->m_friendshipFunctions[j]){
            for (int k=0; k < countFunction; k++){
                if(newArray[k] == temp->m_friendshipFunctions[j]){
                    continue;
                }
                else{
                    newArray[index++] = temp->m_friendshipFunctions[j];
                }
            }
            j++;
        }
    }
    newArray[index] = NULL;
    mergedQueue->m_friendshipFunctions = newArray;

    // Cloning all Queues items to the merged queue.
    for (int posInQueue = 0; posInQueue<maxQueueLength; posInQueue++){
        for(int queue = 0; queue<queuesAmount; queue++){
            IsraeliQueue currentQueue = ilQueueArray[queue];
            Node currentNode = currentQueue->m_head;
            for(int i=0; i<posInQueue; i++){
                currentNode = currentNode->m_nextNode;
            }
            if(currentNode != NULL && IsraeliQueueContains(mergedQueue,currentNode) == 0){
                IsraeliQueueError error = IsraeliQueueEnqueue(mergedQueue, currentNode->m_data);
                if(error!=ISRAELIQUEUE_SUCCESS) {
                    return NULL;
                }
            }
        }
    }
    return mergedQueue;
}

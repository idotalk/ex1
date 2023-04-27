#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "IsraeliQueue.h"
#include "Node.h"

// Struct fields
typedef struct IsraeliQueue_t {
    FriendshipFunction *m_friendshipFunctions;
    ComparisonFunction  m_compareFunction;
    int m_friendshipThreshold;
    int m_rivalryThreshold;
    struct Node *m_head;
    struct Node *m_tail;
}* IsraeliQueue;

// Local Functions

/** NOTICE! this function can be called only from IsraeliQueue.c file.
 *  Returns a Node pointer to the foremost friend which is possible to enqueue a new Node after it.
 *  If friend didn't found, returns endSearch.
 *  @param ilQueue: IsraeliQueue object.
 *  @param toEnqueue: the new Node you want to insert to the Queue.
 *  @param startSearch: where to start the search for a valid friend (usually would be the queue head).
 *  @param endSearch: where to end the search for a valid friend. */
static Node findFriend (IsraeliQueue ilQueue ,Node toEnqueue,Node startSearch, Node endSearch);

/** NOTICE! this function can be called only from IsraeliQueue.c file.
 *  Return the current amount of friendship functions in the Queue that passed as parameter */
static int numberOfFriendshipFunctions (IsraeliQueue ilQueue, FriendshipFunction newFunction);

/** NOTICE! this function can be called only from IsraeliQueue.c file.
 *  Copy a passed Israeli Queue friendship function array to a new array
 *  @param newFunction Optional!:1. pass NULL to skip this part.
 *                               2. pass a new friendship function to determine if it's already exist.
 * return -1 if this function is already exist in the queue functions array. */
static void copyFunctionsArray (IsraeliQueue ilQueue, FriendshipFunction* newArray);

/** NOTICE! this function can be called only from IsraeliQueue.c file.
 *  Enqueue a given node to the Queue */
static IsraeliQueueError enQueue(IsraeliQueue ilQueue, Node newNode);

//Functions implementation

IsraeliQueue IsraeliQueueCreate(FriendshipFunction *functionsArray, ComparisonFunction compareFunction,
                                int friendship_th, int rivalry_th){
    IsraeliQueue newQueue = malloc(sizeof(*newQueue));
    if(!newQueue){
        return NULL;
    }
    newQueue->m_friendshipFunctions = functionsArray;
    newQueue->m_compareFunction = compareFunction;
    newQueue->m_friendshipThreshold = friendship_th;
    newQueue->m_rivalryThreshold = rivalry_th;
    newQueue->m_head = NULL;
    newQueue->m_tail = NULL;
    return newQueue;
}

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
    int functionsAmount = numberOfFriendshipFunctions(ilQueue,NULL);
    FriendshipFunction *newFunctionsArray = malloc(sizeof(FriendshipFunction) * functionsAmount);
    if (!newFunctionsArray) {
        return NULL;
    }
    copyFunctionsArray(ilQueue, newFunctionsArray);
    clonedQueue->m_friendshipFunctions = newFunctionsArray;
    clonedQueue->m_compareFunction = ilQueue->m_compareFunction;
    clonedQueue->m_friendshipThreshold = ilQueue->m_friendshipThreshold;
    clonedQueue->m_rivalryThreshold = ilQueue->m_rivalryThreshold;
    Node helperNode = ilQueue->m_head, temp = NULL;
    Node clonedNode = createNode(helperNode->m_data, helperNode->m_remainedFriends, helperNode->m_reminedRivals);
    clonedQueue->m_head = clonedNode;
    helperNode = helperNode->m_nextNode;
    temp = clonedNode;
    while(helperNode != NULL){
        clonedNode = createNode(helperNode->m_data, helperNode->m_remainedFriends, helperNode->m_reminedRivals);
        connectNodes(temp, clonedNode);
        temp = clonedNode;
        helperNode = helperNode->m_nextNode;
    }
    clonedQueue->m_tail = clonedNode;
    assert(clonedQueue->m_tail->m_nextNode == NULL);
    assert(clonedQueue->m_head->m_prevNode == NULL);
    assert(clonedQueue->m_head->m_data == ilQueue->m_head->m_data);
    assert(clonedQueue->m_tail->m_data == ilQueue->m_tail->m_data);
    return clonedQueue;
}

void IsraeliQueueDestroy(IsraeliQueue ilQueue){
    if(ilQueue == NULL){
        return;
    }
    if(ilQueue->m_head == NULL){
        free (ilQueue);
        return;
    }
    destroyFromStart(ilQueue->m_head);
    free (ilQueue);
}

IsraeliQueueError IsraeliQueueEnqueue(IsraeliQueue ilQueue, void * item){
    if(ilQueue == NULL ||  item == NULL){
        return ISRAELIQUEUE_BAD_PARAM;
    }
    Node newNode = createNode(item, FRIEND_QUOTA, RIVAL_QUOTA);
    if(!newNode) {
        return ISRAELIQUEUE_ALLOC_FAILED;
    }
    return enQueue(ilQueue,newNode);
}

IsraeliQueueError IsraeliQueueAddFriendshipMeasure(IsraeliQueue ilQueue, FriendshipFunction newFriendshipFunction){
    if(ilQueue == NULL || newFriendshipFunction == NULL) {
        return ISRAELIQUEUE_BAD_PARAM;
    }
    if(ilQueue->m_friendshipFunctions == NULL){
        FriendshipFunction *newFunctionsArray = malloc(sizeof(FriendshipFunction) * 2);
        if (!newFunctionsArray) {
            return ISRAELIQUEUE_ALLOC_FAILED;
        }
        newFunctionsArray[0] = newFriendshipFunction;
        newFunctionsArray[1] = NULL;
        ilQueue->m_friendshipFunctions = newFunctionsArray;
        return ISRAELIQUEUE_SUCCESS;
    }
    int currentFunctionsAmount = numberOfFriendshipFunctions(ilQueue, newFriendshipFunction);
    if(currentFunctionsAmount == -1){
        return ISRAELIQUEUE_BAD_PARAM; // maybe success
    }
    FriendshipFunction *newFunctionsArray = malloc(sizeof(FriendshipFunction) * (currentFunctionsAmount+2));
    if (!newFunctionsArray) {
        return ISRAELIQUEUE_ALLOC_FAILED;
    }
    copyFunctionsArray(ilQueue, newFunctionsArray);
    newFunctionsArray[currentFunctionsAmount] = newFriendshipFunction;
    newFunctionsArray[currentFunctionsAmount + 1] = NULL;
    free(ilQueue->m_friendshipFunctions);
    ilQueue->m_friendshipFunctions = newFunctionsArray;
    return ISRAELIQUEUE_SUCCESS;
}

IsraeliQueueError IsraeliQueueUpdateFriendshipThreshold(IsraeliQueue ilQueue, int newFriendshipThreshold){
    if(ilQueue == NULL || newFriendshipThreshold <= ilQueue->m_rivalryThreshold) {
        return ISRAELIQUEUE_BAD_PARAM;
    }
    ilQueue->m_friendshipThreshold = newFriendshipThreshold;
    return ISRAELIQUEUE_SUCCESS;
}

IsraeliQueueError IsraeliQueueUpdateRivalryThreshold(IsraeliQueue ilQueue, int newRivalryThreshold){
    if(ilQueue == NULL || newRivalryThreshold >= ilQueue->m_friendshipThreshold) {
        return ISRAELIQUEUE_BAD_PARAM;
    }
    ilQueue->m_rivalryThreshold = newRivalryThreshold;
    return ISRAELIQUEUE_SUCCESS;
}

int IsraeliQueueSize(IsraeliQueue ilQueue){
    if(ilQueue == NULL || (ilQueue->m_head == NULL && ilQueue->m_tail == NULL)) {
        return 0;
    }
    int count = 0;
    Node temp = ilQueue->m_head;
    while (temp != NULL) {
        count++;
        temp = temp->m_nextNode;
    }
    return count;
}

void* IsraeliQueueDequeue(IsraeliQueue ilQueue){
    if(ilQueue == NULL || (ilQueue->m_head == NULL && ilQueue->m_tail == NULL)) {
        return NULL;
    }
    void* item = ilQueue->m_head->m_data;
    Node toFree = ilQueue->m_head;
    ilQueue->m_head = ilQueue->m_head->m_nextNode;
    if(IsraeliQueueSize(ilQueue) > 1){
        ilQueue->m_head->m_prevNode = NULL;
    }
    free(toFree);
    if(ilQueue->m_head==NULL){
        ilQueue->m_tail = NULL;
    }
    return item;
}

int IsraeliQueueContains(IsraeliQueue ilQueue, void * item){
    if(ilQueue == NULL || item == NULL){
        return 0;
    }
    Node helper = ilQueue->m_head;
    ComparisonFunction compareFunction = ilQueue->m_compareFunction;
    while (helper != NULL){
        if(compareFunction(item,helper->m_data) == 1){
            return 1;
        }
        helper = helper->m_nextNode;
    }
    return 0;
}

IsraeliQueueError IsraeliQueueImprovePositions(IsraeliQueue ilQueue){
    if(ilQueue == NULL){
        return ISRAELIQUEUE_BAD_PARAM;
    }
    if((ilQueue->m_head == NULL && ilQueue->m_tail == NULL) || ilQueue->m_head->m_nextNode == NULL) {
        return ISRAELIQUEUE_SUCCESS;
    }
    int queueLength = IsraeliQueueSize(ilQueue), i=0;
    Node* nodesArray = malloc(sizeof(Node)*queueLength);
    if(!nodesArray){
        return ISRAELIQUEUE_ALLOC_FAILED;
    }
    Node helper = ilQueue->m_head;
    while (helper){
        nodesArray[i++] = helper;
        helper = helper->m_nextNode;
    }
    for (int j = queueLength - 1; j >= 0; j--){
        Node currentNode = nodesArray[j];
        if(ilQueue->m_head == currentNode) {
            ilQueue->m_head = ilQueue->m_head->m_nextNode;
        }
        if(ilQueue->m_tail == currentNode) {
            ilQueue->m_tail = ilQueue->m_tail->m_prevNode;
        }
        disconnectNode(currentNode);
        IsraeliQueueError errorCheck = enQueue(ilQueue, currentNode);
        if(errorCheck != ISRAELIQUEUE_SUCCESS){
            return errorCheck;
        }
    }
    free(nodesArray);
    return ISRAELIQUEUE_SUCCESS;
}

IsraeliQueue IsraeliQueueMerge(IsraeliQueue* ilQueueArray, ComparisonFunction compareFunction){
    if(ilQueueArray == NULL || compareFunction == NULL){
        return NULL;
    }
    IsraeliQueue mergedQueue = malloc(sizeof(*mergedQueue));
    if(!mergedQueue){
        return NULL;
    }
    mergedQueue->m_compareFunction = compareFunction;
    //* Calculating new thresholds.
    int queuesAmount = 0, sumFriendshipThreshold = 0, multiplyRivalThreshold = 1, maxQueueLength = 0;
    while(ilQueueArray[queuesAmount++] != NULL){
        IsraeliQueue currentQueue = ilQueueArray[queuesAmount];
        sumFriendshipThreshold += currentQueue->m_friendshipThreshold;
        multiplyRivalThreshold *= currentQueue->m_rivalryThreshold;
        int len = IsraeliQueueSize(ilQueueArray[queuesAmount]);
        maxQueueLength = len > maxQueueLength ? len : maxQueueLength;
    }
    int newFriendshipThreshold = (int)ceil(((double)sumFriendshipThreshold / (double)queuesAmount));
    int newRivalThreshold = (int)ceil(pow(multiplyRivalThreshold, 1/(double)queuesAmount));
    mergedQueue->m_friendshipThreshold = newFriendshipThreshold;
    mergedQueue->m_rivalryThreshold = newRivalThreshold;

    //* Building a new array of friendship functions.
    int countFunction = 0;
    for(int i = 0; i < queuesAmount; i++){
        IsraeliQueue temp = ilQueueArray[i];
        countFunction += numberOfFriendshipFunctions(temp, NULL);
    }
    FriendshipFunction* newArray = malloc(sizeof(*newArray) * (countFunction + 1 ));
    if(!newArray){
        return NULL;
    }
    for(int i=0;i<countFunction;i++){
        newArray[i] = NULL;
    }
    // Do not use copyFunctionsArray here! it can duplicate functions in the new array.
    int index = 0;
    for(int i = 0; i < queuesAmount; i++){
        IsraeliQueue temp = ilQueueArray[i];
        int j=0;
        while (temp->m_friendshipFunctions[j++]){
            for (int k=0; k < countFunction; k++) {
                if (newArray[k] == temp->m_friendshipFunctions[j]) {
                    continue;
                } else {
                    newArray[index++] = temp->m_friendshipFunctions[j];
                }
            }
        }
    }
    newArray[index] = NULL;
    mergedQueue->m_friendshipFunctions = newArray;

    // Cloning all Queues items to the merged queue.
    for (int posInQueue = 0; posInQueue<maxQueueLength; posInQueue++){
        for(int queue = 0; queue<queuesAmount; queue++){
            IsraeliQueue currentQueue = ilQueueArray[queue];
            Node currentNode = currentQueue->m_head;
            for(int i=0; i<posInQueue && currentNode!= NULL; i++){
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


// @see: Declaration + documentation at the beginning of this file.
static Node findFriend (IsraeliQueue ilQueue ,Node toEnqueue,Node startSearch, Node endSearch){
    int countFunctions = numberOfFriendshipFunctions(ilQueue,NULL);
    Node searchFriend = startSearch;
    while(searchFriend != NULL){
        //* Searching for the first friend in the queue.
        for(int i=0;i<countFunctions;i++){
            FriendshipFunction function = ilQueue->m_friendshipFunctions[i];
            if(function(toEnqueue->m_data, searchFriend->m_data) > ilQueue->m_friendshipThreshold &&
               searchFriend->m_remainedFriends > 0) {
                Node searchRival = searchFriend->m_nextNode;
                //* Searching for possible blocking rival.
                while (searchRival != NULL){
                    int currentValue, friendshipValue=0;
                    for(int j=0; j<countFunctions; j++){
                        FriendshipFunction function2 = ilQueue->m_friendshipFunctions[j];
                        currentValue = function2(toEnqueue->m_data, searchRival->m_data);
                        if(currentValue > ilQueue->m_friendshipThreshold) {
                            break;
                        }
                        friendshipValue += currentValue;
                    }
                    if(currentValue > ilQueue->m_friendshipThreshold) {
                        searchRival = searchRival->m_nextNode;
                    }
                    else{
                        double average = (double)friendshipValue / (double)countFunctions;
                        if(average < ilQueue->m_rivalryThreshold && searchRival->m_reminedRivals > 0){
                            searchRival->m_reminedRivals--;
                            searchRival = searchRival->m_nextNode;
                            break;
                        }
                        else{
                            searchRival = searchRival->m_nextNode;
                        }
                    }
                }
                if(searchRival == NULL && searchFriend->m_remainedFriends > 0) {
                    searchFriend->m_remainedFriends--;
                    return searchFriend;
                }
            }
        }
        searchFriend = searchFriend->m_nextNode;
    }
    return searchFriend;
}

// @see: Declaration + documentation at the beginning of this file.
static IsraeliQueueError enQueue(IsraeliQueue ilQueue, Node newNode){
    if(ilQueue == NULL || newNode ==NULL){
        return ISRAELIQUEUE_BAD_PARAM;
    }
    if(ilQueue->m_head == NULL){ //* Queue is empty, place the new node at the beginning of the queue.
        ilQueue->m_head = newNode;
        ilQueue->m_tail = newNode;
        return ISRAELIQUEUE_SUCCESS;
    }
    if(ilQueue->m_friendshipFunctions == NULL || ilQueue->m_friendshipFunctions[0] == NULL){
        connectNodes(ilQueue->m_tail, newNode);
        ilQueue->m_tail = newNode;
        return ISRAELIQUEUE_SUCCESS;
    }
    Node searchFriend = findFriend(ilQueue, newNode, ilQueue->m_head, NULL);
    if(searchFriend == NULL){
        connectNodes(ilQueue->m_tail, newNode);
        ilQueue->m_tail = newNode;
    }
    else{
        connectNodes(searchFriend,newNode);
    }
    return ISRAELIQUEUE_SUCCESS;
}

// @see: Declaration + documentation at the beginning of this file.
static int numberOfFriendshipFunctions (IsraeliQueue ilQueue, FriendshipFunction newFunction){
    int functionsAmount = 0;
    while (ilQueue->m_friendshipFunctions[functionsAmount] != NULL){
        if(newFunction != NULL && ilQueue->m_friendshipFunctions[functionsAmount] == newFunction){
            return -1;
        }
        functionsAmount++;
    }
    return functionsAmount;
}

// @see: Declaration + documentation at the beginning of this file.
static void copyFunctionsArray (IsraeliQueue ilQueue, FriendshipFunction* newArray){
    int size = numberOfFriendshipFunctions(ilQueue,NULL);
    for (int i=0; i<size; i++){
        newArray[i] = ilQueue->m_friendshipFunctions[i];
    }
    newArray[size] = NULL;
}
